// Phần B — Chức năng chính: Giao diện Qt quản lý nhân sự, ca trực và nhập/xuất file.
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "ui/AdminPanel.h"
#include "ui/StaffPresenter.h"
#include "service/SchedulerService.h"
#include "repository/ScheduleFileManager.h"
#include "repository/StaffFileManager.h"
#include <QInputDialog>
#include <QTimeZone>
#include "util/My_stdexcept.h"
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QScrollArea>
#include <QTableWidget>
#include <QTabWidget>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <exception>

namespace {
// Adapter: GUI cũ dùng Schedule; mọi thao tác mới đi qua Registry kiểm tra nghiệp vụ.
std::unique_ptr<ScheduleRegistry> TaoRegistry(const AdminData& D) {
    auto R=std::make_unique<ScheduleRegistry>();
    for (int I=0; I<D.Lich.CacCa.KichThuoc(); ++I) R->ThemCaTruc(D.Lich.CacCa[I]);
    for (int I=0; I<D.Lich.PhanCong.KichThuoc(); ++I) R->ThemPhanCong(D.Lich.PhanCong[I],*D.NhanSu);
    return R;
}
Schedule ChupLich(const ScheduleRegistry& R) {
    Schedule L;
    for (int I=0; I<R.SoCa(); ++I) L.CacCa.ThemCuoi(R.LayCa(I));
    for (int I=0; I<R.SoPhanCong(); ++I) L.PhanCong.ThemCuoi(R.LayPhanCong(I));
    return L;
}
bool CoLichSu(const Schedule& L) {
    for (int I=0; I<L.PhanCong.KichThuoc(); ++I)
        if (L.PhanCong[I].LayTrangThai()!=AssignmentStatus::DuKien) return true;
    return false;
}
Shift GanNgay(const Shift& C, const std::string& Tuan) {
    const auto Ngay=QDate::fromString(QString::fromStdString(Tuan),Qt::ISODate);
    const auto Moc=QDateTime(Ngay,QTime(0,0),QTimeZone(7*3600)).toSecsSinceEpoch();
    return Shift::TuThoiDiem(C.LayMaCaTruc(),Moc+C.LayPhutBatDau()*60,Moc+C.LayPhutKetThuc()*60,C.SoBacSi,C.SoDieuDuong,C.CanTruongCa);
}
QPushButton* Nut(QLayout* Cot, const QString& Ten, const char* Ma) {
    auto* N = new QPushButton(Ten); N->setObjectName(Ma); Cot->addWidget(N); return N;
}
QTableWidget* Bang(QVBoxLayout* Cot, const QStringList& CotTen, const char* Ma) {
    auto* B = new QTableWidget(0, CotTen.size()); B->setObjectName(Ma);
    B->setHorizontalHeaderLabels(CotTen); B->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    B->setSelectionBehavior(QAbstractItemView::SelectRows); B->setSelectionMode(QAbstractItemView::SingleSelection);
    B->setEditTriggers(QAbstractItemView::NoEditTriggers); Cot->addWidget(B); return B;
}
void O(QTableWidget* B, int Hang, int Cot, const QString& S) { B->setItem(Hang,Cot,new QTableWidgetItem(S)); }
QString ThoiDiem(std::time_t T) {
    return QDateTime::fromSecsSinceEpoch(T,QTimeZone(7*3600)).toString("dd/MM/yyyy HH:mm");
}
QString VaiTro(AssignmentRole V) {
    return V == AssignmentRole::TruongCa ? "Trưởng ca" : V == AssignmentRole::BacSi ? "Bác sĩ" : "Điều dưỡng";
}
// Form chỉ sửa thông tin hành chính; giữ các số liệu đã ghi nhận khi tạo bản cập nhật.
void GiuTrangThai(const NhanVienYTe* Cu, NhanVienYTe& Moi) {
    if (!Cu) return;
    Moi.KhoiPhucTrangThai({Cu->LayDiemMetMoi(),Cu->LayGioLamTuan(),Cu->LaySoCaDemThang(),Cu->LaySoCaCuoiTuanThang(),Cu->DaVuaTrucDem()});
    if (Cu->CoNgaySinh()) Moi.CapNhatThongTinCaNhan(Moi.LayHoTen(),Cu->LayGioiTinh(),Cu->LayNgaySinh());
    else Moi.CapNhatThongTinCaNhan(Moi.LayHoTen(),Cu->LayGioiTinh());
    if (Cu->LayLoaiNhanVien()==StaffType::DieuDuong && Moi.LayLoaiNhanVien()==StaffType::DieuDuong) {
        const auto& DD=static_cast<const DieuDuong&>(*Cu);
        auto& MoiDD=static_cast<DieuDuong&>(Moi);
        MoiDD.KhoiPhucSangLoc(DD.DangSangLoc(),DD.LayGioSangLocLienTuc(),DD.DangTheoDongHo(),DD.LayBatDauSangLoc(),DD.LayMocDongHo());
    }
}

}
AdminPanel::AdminPanel(QWidget* Cha, const QString& TepMacDinh) : QDialog(Cha), DuongDan(TepMacDinh) {
    setWindowTitle("Nhân sự và lịch trực · Phần B"); setObjectName("adminPanel"); resize(1050,760);
    auto* Cot = new QVBoxLayout(this);
    auto* MoTa = new QLabel("Lập lịch một tuần · nghỉ giữa ca 12 giờ, sau ca đêm 24 giờ · tối đa 48 giờ/tuần.\n"
                           "Điểm mệt mỏi là mô phỏng. Dữ liệu nhân sự/lịch được lưu riêng với bệnh nhân demo.");
    MoTa->setWordWrap(true); Cot->addWidget(MoTa);
    auto* Hang = new QHBoxLayout; Cot->addLayout(Hang);
    Hang->addWidget(new QLabel("Tuần bắt đầu (thứ Hai):"));
    Tuan = new QDateEdit; Tuan->setObjectName("adminWeek"); Tuan->setCalendarPopup(true);
    Tuan->setDate(QDate::fromString(QString::fromStdString(DuLieu.TuanBatDau),Qt::ISODate));
    Hang->addWidget(Tuan);
    connect(Tuan,&QDateEdit::dateChanged,this,[this](QDate Ngay) {
        if (Ngay.dayOfWeek() != 1) { BaoLoi("Hay chon ngay thu Hai"); return; }
        if (CoLichSu(DuLieu.Lich)) {
            BaoLoi("Khong doi tuan khi da co lich su phan cong");
            Tuan->blockSignals(true); Tuan->setDate(QDate::fromString(QString::fromStdString(DuLieu.TuanBatDau),Qt::ISODate)); Tuan->blockSignals(false); return;
        }
        for (int I=0; I<DuLieu.Lich.CacCa.KichThuoc(); ++I)
            DuLieu.Lich.CacCa[I]=GanNgay(DuLieu.Lich.CacCa[I],Ngay.toString(Qt::ISODate).toStdString());
        DuLieu.TuanBatDau = Ngay.toString(Qt::ISODate).toStdString(); DaSua = true; LamMoi();
    });
    auto* Mau = Nut(Hang,"Nạp mẫu nhân sự + ca","adminSeed");
    connect(Mau,&QPushButton::clicked,this,[this] { NapMau(); });
    auto* Tab = new QTabWidget; Cot->addWidget(Tab,1);
    TaoNhanSu(Tab); TaoCa(Tab); TaoLich(Tab);
    ThongBao = new QLabel; ThongBao->setObjectName("adminMessage"); ThongBao->setWordWrap(true); Cot->addWidget(ThongBao);
    TrangThaiTep = new QLabel; TrangThaiTep->setWordWrap(true); Cot->addWidget(TrangThaiTep);
    auto* Tep = new QHBoxLayout; Cot->addLayout(Tep);
    auto* Luu = Nut(Tep,"Lưu file…","adminSave"); auto* Doc = Nut(Tep,"Mở file…","adminLoad");
    auto* Dong = Nut(Tep,"Đóng","adminClose");
    connect(Luu,&QPushButton::clicked,this,[this] {
        const auto P = QFileDialog::getSaveFileName(this,"Lưu nhân sự và lịch",DuongDan,"PBL2 (*.pbl2);;Tất cả (*)");
        if (!P.isEmpty()) LuuTep(P);
    });
    connect(Doc,&QPushButton::clicked,this,[this] {
        if (!ChoPhepBoThayDoi()) return;
        const auto P = QFileDialog::getOpenFileName(this,"Đọc nhân sự và lịch",DuongDan,"PBL2 (*.pbl2);;Tất cả (*)");
        if (!P.isEmpty()) NapTep(P);
    });
    connect(Dong,&QPushButton::clicked,this,&AdminPanel::close);
    LamMoi();
    if (!TepMacDinh.isEmpty() && QFileInfo::exists(TepMacDinh)) NapTep(TepMacDinh);
}
void AdminPanel::TaoNhanSu(QTabWidget* Tab) {
    auto* W = new QWidget; auto* Cot = new QVBoxLayout(W);
    auto* TruongNhap = new QWidget;
    auto* F = new QFormLayout(TruongNhap); F->setRowWrapPolicy(QFormLayout::WrapLongRows);
    auto* Cuon = new QScrollArea; Cuon->setWidgetResizable(true); Cuon->setWidget(TruongNhap);
    Cuon->setMaximumHeight(310); Cot->addWidget(Cuon);
    MaNV = new QLineEdit; MaNV->setObjectName("staffId"); F->addRow("Mã nhân viên",MaNV);
    TenNV = new QLineEdit; TenNV->setObjectName("staffName"); F->addRow("Họ tên",TenNV);
    LoaiNV = new QComboBox; LoaiNV->addItems({"Bác sĩ","Điều dưỡng"}); LoaiNV->setObjectName("staffType"); F->addRow("Loại",LoaiNV);
    CapNV = new QComboBox; CapNV->setObjectName("staffRank"); F->addRow("Cấp bậc",CapNV);
    auto CapNhatCap = [this] {
        CapNV->clear();
        if (LoaiNV->currentIndex() == 0) {
            CapNV->addItem("Bác sĩ nội trú",2); CapNV->addItem("Bác sĩ chính",3); CapNV->addItem("Trưởng khoa",4);
        } else { CapNV->addItem("Điều dưỡng trẻ",0); CapNV->addItem("Điều dưỡng chính",1); }
        ChuyenKhoa->setEnabled(LoaiNV->currentIndex() == 0);
    };
    KinhNghiem = new QSpinBox; KinhNghiem->setRange(0,80); F->addRow("Năm kinh nghiệm",KinhNghiem);
    ChuyenKhoa = new QLineEdit; ChuyenKhoa->setObjectName("staffSpecialty"); F->addRow("Chuyên khoa",ChuyenKhoa);
    DiemNen = new QDoubleSpinBox; DiemNen->setRange(0,10000); F->addRow("Điểm mệt mỏi nền",DiemNen);
    connect(LoaiNV,&QComboBox::currentIndexChanged,this,[CapNhatCap](int) { CapNhatCap(); }); CapNhatCap();
    GioMongDau = new QSpinBox; GioMongDau->setRange(0,23); GioMongDau->setValue(7);
    GioMongCuoi = new QSpinBox; GioMongCuoi->setRange(0,23); GioMongCuoi->setValue(15);
    MongCuoiTuan = new QCheckBox("Ưu tiên ca cuối tuần");
    F->addRow("Giờ bắt đầu mong muốn (ưu tiên mềm)",GioMongDau);
    F->addRow("Giờ kết thúc mong muốn",GioMongCuoi); F->addRow(MongCuoiTuan);
    DangKyDuPhong = new QCheckBox("Đăng ký vào danh sách dự phòng"); DangKyDuPhong->setChecked(true); F->addRow(DangKyDuPhong);
    auto* Them = Nut(Cot,"Thêm nhân viên","staffAdd"); connect(Them,&QPushButton::clicked,this,[this] { ThemNhanSu(); });
    auto* Sua = Nut(Cot,"Nạp dòng đã chọn vào biểu mẫu","staffEditSelect");
    auto* CapNhat = Nut(Cot,"Cập nhật nhân viên theo mã (cần xếp lại lịch)","staffUpdate");
    auto* Xoa = Nut(Cot,"Xóa nhân viên đã chọn (cần xếp lại lịch)","staffDelete");
    connect(CapNhat,&QPushButton::clicked,this,[this] { ThemNhanSu(true); });
    connect(Sua,&QPushButton::clicked,this,[this] {
        const int I = BangNhanSu->currentRow();
        if (I < 0) { BaoLoi("Hay chon nhan vien"); return; }
        const auto& N = DuLieu.NhanSu->Lay(I);
        DangKyDuPhong->setChecked(DuLieu.DuPhong.Co(N.LayMaDinhDanh()));
        MaNV->setText(QString::fromStdString(N.LayMaDinhDanh()));
        TenNV->setText(QString::fromStdString(N.LayHoTen()));
        LoaiNV->setCurrentIndex(N.LayLoaiNhanVien()==StaffType::BacSi ? 0 : 1);
        CapNV->setCurrentIndex(CapNV->findData(static_cast<int>(N.LayCapBac())));
        GioMongDau->setValue(N.LayCaMongMuon().GioBatDau);
        GioMongCuoi->setValue(N.LayCaMongMuon().GioKetThuc);
        MongCuoiTuan->setChecked(N.LayCaMongMuon().MongMuonCuoiTuan);
        KinhNghiem->setValue(N.LaySoNamKinhNghiem()); DiemNen->setValue(N.LayDiemMetMoiNen());
        ChuyenKhoa->setText(N.LayLoaiNhanVien()==StaffType::BacSi ? QString::fromStdString(static_cast<const BacSi&>(N).LayChuyenKhoa()) : "");
    });
    connect(Xoa,&QPushButton::clicked,this,[this] {
        const int I=BangNhanSu->currentRow();
        if (I<0) { BaoLoi("Hay chon nhan vien"); return; }
        if (CoLichSu(DuLieu.Lich)) { BaoLoi("Khong xoa nhan su khi da co lich su phan cong"); return; }
        const auto Ma=DuLieu.NhanSu->Lay(I).LayMaDinhDanh();
        if (QMessageBox::question(this,"Xóa nhân viên", "Xóa " + QString::fromStdString(Ma) + " và xóa phân công hiện tại?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes) return;
        DuLieu.NhanSu->Xoa(Ma); DuLieu.DuPhong.Xoa(Ma); DuLieu.Lich.PhanCong.XoaHet(); DaSua=true; LamMoi();
        ThongBao->setText("Đã xóa nhân viên. Hãy xếp lại lịch.");
    });
    BangNhanSu = Bang(Cot,{"Mã","Họ tên","Loại","Giờ đã xếp","Điểm dự kiến","Dự phòng"},"staffTable");
    auto* Tim = new QLineEdit; Tim->setObjectName("staffSearchDoctor");
    Tim->setPlaceholderText("Tìm bác sĩ theo tên (phân biệt hoa/thường và dấu)"); Cot->addWidget(Tim);
    connect(Tim,&QLineEdit::textChanged,this,[this](const QString& Ten) {
        const auto DS=DuLieu.NhanSu->TimBacSiTheoTen(Ten.toStdString());
        for (int I=0;I<DuLieu.NhanSu->KichThuoc();++I) {
            bool Hien=Ten.isEmpty();
            for (int J=0;J<DS.KichThuoc();++J) if (DS[J]->LayMaDinhDanh()==DuLieu.NhanSu->Lay(I).LayMaDinhDanh()) Hien=true;
            BangNhanSu->setRowHidden(I,!Hien);
        }
    });
    auto* HangSL=new QHBoxLayout; Cot->addLayout(HangSL);
    auto* BatDau=Nut(HangSL,"Bắt đầu sàng lọc","staffTriageStart");
    auto* CapGio=Nut(HangSL,"Cập nhật giờ / cảnh báo","staffTriageRefresh");
    auto* KetThuc=Nut(HangSL,"Kết thúc sàng lọc","staffTriageEnd");
    connect(BatDau,&QPushButton::clicked,this,[this] { CapNhatSangLoc(0); });
    connect(CapGio,&QPushButton::clicked,this,[this] { CapNhatSangLoc(1); });
    connect(KetThuc,&QPushButton::clicked,this,[this] { CapNhatSangLoc(2); });
    auto* TepNhanSu=new QHBoxLayout; Cot->addLayout(TepNhanSu);
    auto* Xuat=Nut(TepNhanSu,"Xuất file nhân viên…","staffExport");
    auto* Nhap=Nut(TepNhanSu,"Mở file nhân viên…","staffImport");
    connect(Xuat,&QPushButton::clicked,this,[this] {
        const auto P=QFileDialog::getSaveFileName(this,"Xuất nhân viên",{},"PBL2 (*.pbl2)");
        if (!P.isEmpty()) LuuNhanSu(P);
    });
    connect(Nhap,&QPushButton::clicked,this,[this] {
        if (!ChoPhepBoThayDoi()) return;
        const auto P=QFileDialog::getOpenFileName(this,"Mở nhân viên",{},"PBL2 (*.pbl2)");
        if (!P.isEmpty()) NapNhanSu(P);
    });
    Tab->addTab(W,"1. Nhân sự");
}
void AdminPanel::TaoCa(QTabWidget* Tab) {
    auto* W = new QWidget; auto* Cot = new QVBoxLayout(W); auto* F = new QFormLayout; Cot->addLayout(F);
    MaCa = new QLineEdit; MaCa->setObjectName("shiftId"); F->addRow("Mã ca",MaCa);
    NgayCa = new QComboBox; NgayCa->addItems({"Thứ Hai","Thứ Ba","Thứ Tư","Thứ Năm","Thứ Sáu","Thứ Bảy","Chủ nhật"}); F->addRow("Ngày bắt đầu",NgayCa);
    GioDau = new QTimeEdit(QTime(7,0)); GioCuoi = new QTimeEdit(QTime(15,0));
    F->addRow("Giờ bắt đầu",GioDau); F->addRow("Giờ kết thúc (nhỏ hơn giờ đầu: sang ngày)",GioCuoi);
    SoBS = new QSpinBox; SoBS->setRange(0,10); SoBS->setValue(1); F->addRow("Số bác sĩ",SoBS);
    SoDD = new QSpinBox; SoDD->setRange(0,10); SoDD->setValue(1); F->addRow("Số điều dưỡng",SoDD);
    TruongCa = new QCheckBox("Cần một bác sĩ chính/trưởng khoa làm trưởng ca"); TruongCa->setChecked(true); F->addRow(TruongCa);
    auto* Them = Nut(Cot,"Thêm ca (lịch hiện có sẽ cần xếp lại)","shiftAdd"); connect(Them,&QPushButton::clicked,this,[this] { ThemCa(); });
    BangCa = Bang(Cot,{"Mã ca","Bắt đầu","Kết thúc","Bác sĩ","Điều dưỡng"},"shiftTable");
    auto* Xoa = Nut(Cot,"Xóa ca đã chọn (cần xếp lại lịch)","shiftDelete");
    connect(Xoa,&QPushButton::clicked,this,[this] {
        const int I=BangCa->currentRow();
        if (I<0) { BaoLoi("Hay chon ca"); return; }
        if (CoLichSu(DuLieu.Lich)) { BaoLoi("Khong xoa ca da co lich su"); return; }
        if (QMessageBox::question(this,"Xóa ca","Xóa ca đã chọn và phân công hiện tại?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes) return;
        Vector<Shift> ConLai;
        for(int J=0;J<DuLieu.Lich.CacCa.KichThuoc();++J) if(J!=I) ConLai.ThemCuoi(DuLieu.Lich.CacCa[J]);
        DuLieu.Lich.CacCa=std::move(ConLai); DuLieu.Lich.PhanCong.XoaHet(); DaSua=true; LamMoi();
    });
    Tab->addTab(W,"2. Ca trực");
}
void AdminPanel::TaoLich(QTabWidget* Tab) {
    auto* W = new QWidget; auto* Cot = new QVBoxLayout(W);
    auto* Giai = Nut(Cot,"Xếp lịch hợp lệ bằng Backtracking","scheduleGenerate");
    connect(Giai,&QPushButton::clicked,this,[this] { LapLich(); });
    BangLich = Bang(Cot,{"Ca","Nhân viên","Họ tên","Vai trò","Trạng thái","Mã phân công"},"scheduleTable");
    auto* Hang = new QHBoxLayout; Cot->addLayout(Hang);
    auto* Tim = Nut(Hang,"Tìm người thay cho dòng đã chọn","replacementFind");
    ChonNguoiThay = new QComboBox; ChonNguoiThay->setObjectName("replacementCandidates"); Hang->addWidget(ChonNguoiThay);
    auto* Thay = Nut(Hang,"Xác nhận thay","replacementApply");
    connect(Tim,&QPushButton::clicked,this,[this] { TimThay(); });
    connect(Thay,&QPushButton::clicked,this,[this] { ThayNguoi(); });
    connect(BangLich,&QTableWidget::itemSelectionChanged,this,[this] { ChonNguoiThay->clear(); });
    auto* TrangThai = new QHBoxLayout; Cot->addLayout(TrangThai);
    const QString TenNut[]={"Xác nhận phân công","Hủy phân công…","Ghi nhận bắt đầu","Đề xuất tăng cường…"};
    const char* MaNut[]={"assignmentConfirm","assignmentCancel","assignmentStart","scheduleReinforce"};
    for (int I=0; I<4; ++I) {
        auto* N=Nut(TrangThai,TenNut[I],MaNut[I]);
        connect(N,&QPushButton::clicked,this,[this,I] { ThaoTacPhanCong(I); });
    }
    auto* TepLich=new QHBoxLayout; Cot->addLayout(TepLich);
    auto* Luu=Nut(TepLich,"Xuất file lịch riêng…","scheduleExport");
    auto* Doc=Nut(TepLich,"Mở file lịch riêng…","scheduleImport");
    connect(Luu,&QPushButton::clicked,this,[this] {
        const auto P=QFileDialog::getSaveFileName(this,"Xuất lịch",{},"PBL2 (*.pbl2)");
        if (P.isEmpty()) return;
        try { const auto R=TaoRegistry(DuLieu); ScheduleFileManager::LuuLichTruc(P.toStdString(),*R); ThongBao->setText("Đã xuất lịch; file này cần nhân sự tương ứng khi đọc."); }
        catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
    });
    connect(Doc,&QPushButton::clicked,this,[this] {
        if (!ChoPhepBoThayDoi()) return;
        const auto P=QFileDialog::getOpenFileName(this,"Mở lịch",{},"PBL2 (*.pbl2)");
        if (P.isEmpty()) return;
        try { auto R=ScheduleFileManager::DocLichTruc(P.toStdString(),*DuLieu.NhanSu); DuLieu.Lich=ChupLich(*R); DaSua=true; LamMoi(); }
        catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
    });
    Tab->addTab(W,"3. Lịch và thay ca");
}
void AdminPanel::BaoLoi(const char* Loi) { ThongBao->setText("Lỗi: " + QString::fromUtf8(Loi)); }
void AdminPanel::LamMoi() {
    if (!TrangThaiTep) return;
    BangNhanSu->setRowCount(DuLieu.NhanSu->KichThuoc());
    for (int I = 0; I < DuLieu.NhanSu->KichThuoc(); ++I) {
        const auto& N = DuLieu.NhanSu->Lay(I);
        O(BangNhanSu,I,0,QString::fromStdString(N.LayMaDinhDanh())); O(BangNhanSu,I,1,QString::fromStdString(N.LayHoTen()));
        BangNhanSu->item(I,1)->setToolTip(StaffPresenter::MoTa(N));
        O(BangNhanSu,I,2,N.LayLoaiNhanVien() == StaffType::BacSi ? "Bác sĩ" : "Điều dưỡng");
        O(BangNhanSu,I,3,QString::number(FatigueService::TinhGio(N.LayMaDinhDanh(),DuLieu.Lich)));
        O(BangNhanSu,I,4,QString::number(FatigueService::TinhDiem(N,DuLieu.Lich)));
        O(BangNhanSu,I,5,DuLieu.DuPhong.Co(N.LayMaDinhDanh()) ? "Có" : "Không");
    }
    BangCa->setRowCount(DuLieu.Lich.CacCa.KichThuoc());
    for (int I = 0; I < DuLieu.Lich.CacCa.KichThuoc(); ++I) {
        const auto& C = DuLieu.Lich.CacCa[I];
        O(BangCa,I,0,QString::fromStdString(C.LayMaCaTruc())); O(BangCa,I,1,ThoiDiem(C.LayBatDau())); O(BangCa,I,2,ThoiDiem(C.LayKetThuc()));
        O(BangCa,I,3,QString::number(C.SoBacSi)); O(BangCa,I,4,QString::number(C.SoDieuDuong));
    }
    BangLich->setRowCount(DuLieu.Lich.PhanCong.KichThuoc());
    for (int I = 0; I < DuLieu.Lich.PhanCong.KichThuoc(); ++I) {
        const auto& P = DuLieu.Lich.PhanCong[I]; const auto* N = DuLieu.NhanSu->Tim(P.MaNhanVien);
        O(BangLich,I,0,QString::fromStdString(P.MaCa)); O(BangLich,I,1,QString::fromStdString(P.MaNhanVien));
        const QString TrangThai[]={"Dự kiến","Đã xác nhận","Đang trực","Đã hủy"};
        O(BangLich,I,4,TrangThai[static_cast<int>(P.LayTrangThai())]);
        O(BangLich,I,5,QString::fromStdString(P.LayMaPhanCong()));
        BangLich->item(I,4)->setToolTip(QString::fromStdString(P.LayLyDoHuy()));
        O(BangLich,I,2,N ? QString::fromStdString(N->LayHoTen()) : "Không tìm thấy"); O(BangLich,I,3,VaiTro(P.VaiTro));
    }
    if (auto* Tim=findChild<QLineEdit*>("staffSearchDoctor")) {
        Tim->clear();
        for (int I=0;I<BangNhanSu->rowCount();++I) BangNhanSu->setRowHidden(I,false);
    }
    ChonNguoiThay->clear();
    TrangThaiTep->setText((DaSua ? "Chưa lưu · " : "Đã đồng bộ · ") + (DuongDan.isEmpty() ? "Chưa chọn file" : DuongDan));
}
void AdminPanel::ThemNhanSu(bool CapNhat) {
    try {
        if (CapNhat && CoLichSu(DuLieu.Lich)) { BaoLoi("Khong sua nhan su lam mat lich su phan cong"); return; }
        if (!CapNhat && DuLieu.NhanSu->KichThuoc() >= 100) { BaoLoi("Toi da 100 nhan vien"); return; }
        const auto Ma = MaNV->text().trimmed().toStdString(), Ten = TenNV->text().trimmed().toStdString();
        const auto Cap = static_cast<CapBac>(CapNV->currentData().toInt());
        if (CapNhat && !DuLieu.Lich.PhanCong.Rong() && QMessageBox::question(this,"Cập nhật nhân viên","Cập nhật sẽ xóa phân công hiện tại để xếp lại. Tiếp tục?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes) return;
        const CaTrucMongMuon MongMuon{GioMongDau->value(),GioMongCuoi->value(),MongCuoiTuan->isChecked()};
        const auto* Cu = CapNhat ? DuLieu.NhanSu->Tim(Ma) : nullptr;
        if (LoaiNV->currentIndex() == 0) {
            const bool NoiTru = Cu && Cu->LayLoaiNhanVien()==StaffType::BacSi && static_cast<const BacSi*>(Cu)->LaBacSiNoiTru();
            BacSi N(Ma,Ten,Cap,KinhNghiem->value(),ChuyenKhoa->text().trimmed().toStdString(),NoiTru,DiemNen->value(),MongMuon);
            GiuTrangThai(Cu,N);
            if (CapNhat) DuLieu.NhanSu->CapNhat(N); else DuLieu.NhanSu->Them(N);
        } else {
            DieuDuong N(Ma,Ten,Cap,KinhNghiem->value(),DiemNen->value(),MongMuon);
            GiuTrangThai(Cu,N);
            if (CapNhat) DuLieu.NhanSu->CapNhat(N); else DuLieu.NhanSu->Them(N);
        }
        if (DangKyDuPhong->isChecked()) DuLieu.DuPhong.Them(Ma); else DuLieu.DuPhong.Xoa(Ma);
        if (CapNhat) DuLieu.Lich.PhanCong.XoaHet();
        DaSua = true; LamMoi(); ThongBao->setText("Đã thêm nhân viên.");
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
}
void AdminPanel::ThemCa() {
    try {
        if (CoLichSu(DuLieu.Lich)) { BaoLoi("Khong thay doi ca khi da co lich su phan cong"); return; }
        if (DuLieu.Lich.CacCa.KichThuoc() >= 32) { BaoLoi("Toi da 32 ca"); return; }
        const int Dau = GioDau->time().hour()*60 + GioDau->time().minute();
        int Cuoi = GioCuoi->time().hour()*60 + GioCuoi->time().minute();
        if (Cuoi == Dau) { BaoLoi("Gio dau va cuoi phai khac nhau"); return; }
        if (Cuoi < Dau) Cuoi += 1440;
        Shift C{MaCa->text().trimmed().toStdString(),NgayCa->currentIndex()*1440+Dau,NgayCa->currentIndex()*1440+Cuoi,SoBS->value(),SoDD->value(),TruongCa->isChecked()};
        if (!C.HopLe()) { BaoLoi("Ca khong hop le"); return; }
        for (int I=0;I<DuLieu.Lich.CacCa.KichThuoc();++I) if (DuLieu.Lich.CacCa[I].LayMaCaTruc()==C.LayMaCaTruc()) { BaoLoi("Trung ma ca"); return; }
        if (!DuLieu.Lich.PhanCong.Rong() && QMessageBox::question(this,"Xếp lại lịch","Thêm ca sẽ xóa phân công hiện tại để xếp lại. Tiếp tục?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes) return;
        C=GanNgay(C,DuLieu.TuanBatDau);
        DuLieu.Lich.CacCa.ThemCuoi(C); DuLieu.Lich.PhanCong.XoaHet();
        DaSua = true; LamMoi(); ThongBao->setText("Đã thêm ca. Hãy xếp lịch trước khi lưu.");
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
}
void AdminPanel::LapLich() {
    try {
        auto R=TaoRegistry(DuLieu);
        const auto DS=SchedulerService(*DuLieu.NhanSu,*R).LapLichDuKien(20000);
        for (int I=0; I<DS.KichThuoc(); ++I) R->ThemPhanCong(DS[I],*DuLieu.NhanSu);
        DuLieu.Lich=ChupLich(*R); DaSua=true; LamMoi();
        ThongBao->setText(QString("Đã bổ sung %1 phân công dự kiến; giữ lại các bản cũ. Chọn dòng để xác nhận.").arg(DS.KichThuoc()));
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
}
void AdminPanel::NapMau() {
    if (DuLieu.NhanSu->KichThuoc() || !DuLieu.Lich.CacCa.Rong()) { BaoLoi("Chi nap mau khi chua co du lieu; mo lai ung dung hoac mo file khac"); return; }
    DuLieu.NhanSu->Them(BacSi("BS01","Nguyễn An",CapBac::BacSiChinh,8,"Cấp cứu"));
    DuLieu.NhanSu->Them(BacSi("BS02","Trần Bình",CapBac::BacSiChinh,6,"Cấp cứu"));
    DuLieu.NhanSu->Them(DieuDuong("DD01","Lê Lan",CapBac::DieuDuongChinh,5));
    DuLieu.NhanSu->Them(DieuDuong("DD02","Phạm Mai",CapBac::DieuDuongTre,1));
    for (int I=0;I<DuLieu.NhanSu->KichThuoc();++I) DuLieu.DuPhong.Them(DuLieu.NhanSu->Lay(I).LayMaDinhDanh());
    DuLieu.Lich.CacCa.ThemCuoi({"T2-SANG",420,900,1,1,true});
    DuLieu.Lich.CacCa.ThemCuoi({"T2-DEM",1320,1800,1,1,true});
    for (int I=0; I<DuLieu.Lich.CacCa.KichThuoc(); ++I) DuLieu.Lich.CacCa[I]=GanNgay(DuLieu.Lich.CacCa[I],DuLieu.TuanBatDau);
    DaSua = true; LamMoi(); ThongBao->setText("Đã nạp 4 nhân viên, 2 ca. Chọn tab Lịch để xếp lịch.");
}
void AdminPanel::TimThay() {
    try {
        ChonNguoiThay->clear();
        const int I = BangLich->currentRow();
        if (I < 0) { BaoLoi("Hay chon mot dong phan cong"); return; }
        const auto R=TaoRegistry(DuLieu);
        const auto DS=SchedulerService(*DuLieu.NhanSu,*R).TimNguoiThayCa(DuLieu.Lich.PhanCong[I].LayMaPhanCong(),&DuLieu.DuPhong);
        for (int J=0;J<DS.KichThuoc();++J) ChonNguoiThay->addItem(QString::fromStdString(DS[J]->LayMaDinhDanh()));
        ThongBao->setText(QString("Có %1 người cùng cấp bậc/chuyên khoa và đủ điều kiện.").arg(DS.KichThuoc()));
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
}
void AdminPanel::ThayNguoi() {
    const int I = BangLich->currentRow();
    if (I < 0 || ChonNguoiThay->currentIndex() < 0) { BaoLoi("Chon dong va tim nguoi thay truoc"); return; }
    try {
        auto R=TaoRegistry(DuLieu);
        const auto Cu=DuLieu.Lich.PhanCong[I];
        const auto DS=SchedulerService(*DuLieu.NhanSu,*R).TimNguoiThayCa(Cu.LayMaPhanCong(),&DuLieu.DuPhong);
        const auto Ma=ChonNguoiThay->currentText().toStdString(); bool Co=false;
        for (int J=0; J<DS.KichThuoc(); ++J) if (DS[J]->LayMaDinhDanh()==Ma) Co=true;
        if (!Co) { BaoLoi("Nguoi thay khong con du dieu kien"); return; }
        R->HuyPhanCong(Cu.LayMaPhanCong(),"Thay boi "+Ma);
        std::string M=Cu.LayMaPhanCong()+"-thay";
        while (R->TimPhanCong(M)) M+="-1";
        R->ThemPhanCong(ShiftAssignment(M,Ma,Cu.MaCa,Cu.VaiTro),*DuLieu.NhanSu);
        R->XacNhanPhanCong(M,*DuLieu.NhanSu);
        DuLieu.Lich=ChupLich(*R); DaSua=true; LamMoi(); ThongBao->setText("Đã xác nhận người thay; giữ bản cũ cùng lý do hủy.");
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }

}
bool AdminPanel::LuuTep(const QString& Tep) {
    try {
        if (Tuan->date().dayOfWeek()!=1) { BaoLoi("Ngay bat dau phai la thu Hai"); return false; }
        AdminFileManager::Luu(Tep.toStdString(),DuLieu); DuongDan = Tep; DaSua = false; LamMoi();
        ThongBao->setText("Đã lưu đầy đủ nhân sự và lịch."); return true;
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
    return false;
}
bool AdminPanel::NapTep(const QString& Tep) {
    try {
        AdminData Moi = AdminFileManager::Doc(Tep.toStdString());
        DuLieu = std::move(Moi); DuongDan = Tep;
        Tuan->blockSignals(true); Tuan->setDate(QDate::fromString(QString::fromStdString(DuLieu.TuanBatDau),Qt::ISODate)); Tuan->blockSignals(false);
        DaSua = false; LamMoi(); ThongBao->setText("Đã nạp và kiểm tra toàn bộ dữ liệu."); return true;
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
    return false;
}
bool AdminPanel::ChoPhepBoThayDoi() {
    return !DaSua || QMessageBox::question(this,"Dữ liệu chưa lưu","Bỏ các thay đổi chưa lưu? Chọn Không để quay lại lưu file.",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes;
}
void AdminPanel::closeEvent(QCloseEvent* E) { if (ChoPhepBoThayDoi()) E->accept(); else E->ignore(); }
void AdminPanel::reject() { if (ChoPhepBoThayDoi()) QDialog::reject(); }

void AdminPanel::CapNhatSangLoc(int Lenh) {
    try {
        const int I=BangNhanSu->currentRow();
        if (I<0) { BaoLoi("Chon dieu duong truoc"); return; }
        const auto& NV=DuLieu.NhanSu->Lay(I);
        if (NV.LayLoaiNhanVien()!=StaffType::DieuDuong) { BaoLoi("Chuc nang nay danh cho dieu duong"); return; }
        DieuDuong Moi(static_cast<const DieuDuong&>(NV));
        const auto BayGio=static_cast<std::time_t>(QDateTime::currentSecsSinceEpoch());
        if (Lenh==0) Moi.BatDauTrucSangLoc(BayGio);
        else if (Lenh==1) Moi.CapNhatThoiGianSangLoc(BayGio);
        else Moi.KetThucTrucSangLoc(BayGio);
        const bool CanXoay=Moi.CanXoayTuaSangLoc();
        const double Gio=Moi.LayGioSangLocLienTuc();
        DuLieu.NhanSu->CapNhat(Moi); DaSua=true; LamMoi();
        ThongBao->setText(Lenh==2 ? "Đã kết thúc sàng lọc." :
            QString("Sàng lọc liên tục %1 giờ. %2").arg(Gio,0,'f',2).arg(CanXoay ? "Cần tìm người xoay tua (đã đạt 4 giờ)." : "Chưa đạt mốc 4 giờ."));
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
}

void AdminPanel::ThaoTacPhanCong(int Lenh) {
    const int I=BangLich->currentRow();
    if (I<0) { BaoLoi("Chon mot dong phan cong truoc"); return; }
    try {
        auto R=TaoRegistry(DuLieu); const auto P=DuLieu.Lich.PhanCong[I];
        if (Lenh==0) R->XacNhanPhanCong(P.LayMaPhanCong(),*DuLieu.NhanSu);
        else if (Lenh==1) {
            bool OK=false; const auto LyDo=QInputDialog::getText(this,"Hủy phân công","Lý do:",QLineEdit::Normal,{},&OK);
            if (!OK) return;
            R->HuyPhanCong(P.LayMaPhanCong(),LyDo.trimmed().toStdString());
        } else if (Lenh==2) R->GhiNhanBatDau(P.LayMaPhanCong(),QDateTime::currentSecsSinceEpoch());
        else {
            bool OK=false;
            const int BS=QInputDialog::getInt(this,"Tăng cường","Số bác sĩ bổ sung:",0,0,10,1,&OK); if (!OK) return;
            const int DD=QInputDialog::getInt(this,"Tăng cường","Số điều dưỡng bổ sung:",0,0,10,1,&OK); if (!OK) return;
            const auto DS=SchedulerService(*DuLieu.NhanSu,*R).DeXuatTangCuong(P.MaCa,BS,DD,20000);
            for (int J=0; J<DS.KichThuoc(); ++J) R->ThemPhanCong(DS[J],*DuLieu.NhanSu);
        }
        DuLieu.Lich=ChupLich(*R); DaSua=true; LamMoi(); ThongBao->setText("Đã cập nhật phân công. Hãy lưu file để giữ thay đổi.");
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
}

bool AdminPanel::LuuNhanSu(const QString& Tep) {
    try {
        StaffFileManager::LuuNhanVien(Tep.toStdString(),*DuLieu.NhanSu);
        // Xuất riêng không đồng nghĩa snapshot lịch/dự phòng đã được lưu.
        ThongBao->setText("Đã xuất file nhân viên. Lưu file chung để giữ cả lịch và danh sách dự phòng.");
        return true;
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
    return false;
}
bool AdminPanel::NapNhanSu(const QString& Tep) {
    try {
        auto Moi=StaffFileManager::DocNhanVien(Tep.toStdString());
        // Kiểm tra trước khi thay nguồn dữ liệu: không xóa lịch để lách lỗi tham chiếu.
        ScheduleRegistry KiemTra;
        for (int I=0; I<DuLieu.Lich.CacCa.KichThuoc(); ++I) KiemTra.ThemCaTruc(DuLieu.Lich.CacCa[I]);
        for (int I=0; I<DuLieu.Lich.PhanCong.KichThuoc(); ++I) KiemTra.ThemPhanCong(DuLieu.Lich.PhanCong[I],*Moi);
        DuLieu.DuPhong.Duyet([&](const std::string& Ma) {
            if (!Moi->Tim(Ma)) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"File nhan vien thieu ma trong danh sach du phong");
        });
        DuLieu.NhanSu=std::move(Moi); DaSua=true; LamMoi();
        ThongBao->setText("Đã nạp nhân viên và kiểm tra liên kết lịch/dự phòng. Hãy lưu file chung.");
        return true;
    } catch (const My_stdexcept& E) { BaoLoi(E.LayThongBao()); }
      catch (const std::exception& E) { BaoLoi(E.what()); }
    return false;
}
