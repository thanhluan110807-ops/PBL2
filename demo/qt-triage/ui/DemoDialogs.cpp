#include <QWidget>
#include <QString>
#include <QObject>
#include <QListWidgetItem>
#include <QVariant>
#include "ui/DemoWindow.h"
#include "ui/DemoStyle.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "dsa/Vector.h"
#include "util/My_stdexcept.h"
#include <ctime>
#include <string>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStatusBar>
#include <QVBoxLayout>
using namespace DemoUi;
namespace {
QLabel* ThemVanBan(QVBoxLayout* Cot, const QString& Chuoi, const char* Ma = "") {
    auto* N=Nhan(Chuoi); N->setObjectName(Ma);
    N->setWordWrap(true); N->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    N->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
    Cot->addWidget(N); return N;
}
QLabel* ThemThongTin(QVBoxLayout* Cot, const QString& Ten, const QString& GiaTri, const char* Ma = "") {
    return ThemVanBan(Cot,Ten+": "+GiaTri,Ma);
}
QVBoxLayout* ThemThe(QVBoxLayout* Cot) {
    auto* The=new QFrame; The->setObjectName("card");
    auto* NoiDung=new QVBoxLayout(The);
    NoiDung->setContentsMargins(16,14,16,14); NoiDung->setSpacing(8);
    NoiDung->setAlignment(Qt::AlignTop); Cot->addWidget(The); return NoiDung;
}
// QObject hủy widget con; các biến mượn widget chỉ sống trong lúc exec().
struct KhungHop {
    QDialog Hop;
    QVBoxLayout *NoiDung;
    QWidget *VungNhap;
    QFormLayout *Form;
    QHBoxLayout *Chan;
    QLabel *Loi;
    void ChuLon() {
        Hop.setStyleSheet(R"(
QWidget { font-size:18px; }
QLabel[role="section"] { font-size:24px; }
QLabel[role="muted"], QLabel[role="error"] { font-size:16px; }
QLabel[role="inputHint"] { font-size:14px; color:#686571; }
QPushButton, QLineEdit, QComboBox { font-size:18px; }
)");
    }
    KhungHop(QWidget *Cha, const QString &Ten, const QString &MoTa, bool DocChiTiet = false) : Hop(Cha) {
        Hop.setObjectName("demoDialog"); Hop.setWindowTitle(Ten);
        Hop.resize(qMin(760, Cha->width() - 24), qMin(DocChiTiet ? 760 : 850, Cha->height() - 48));
        if (DocChiTiet) ChuLon();
        auto *Goc = new QVBoxLayout(&Hop);
        if (DocChiTiet) { Goc->setContentsMargins(20,20,20,16); Goc->setSpacing(12); }
        Goc->addWidget(Nhan(Ten, "section"));
        auto *Phu = Nhan(MoTa, "muted"); Phu->setWordWrap(true); Goc->addWidget(Phu);
        auto *Cuon = new QScrollArea;
        Cuon->setObjectName("dialogScroll");
        Cuon->setWidgetResizable(true);
        auto *Than = new QWidget;
        Than->setObjectName("dialogContent");
        NoiDung = new QVBoxLayout(Than);
        // Dồn thông tin về đầu trang, không giãn các dòng theo chiều cao hộp thoại.
        NoiDung->setAlignment(Qt::AlignTop); NoiDung->setSpacing(12);
        VungNhap = new QWidget; VungNhap->setObjectName("dialogForm");
        Form = new QFormLayout(VungNhap); Form->setContentsMargins(0,0,0,0);
        Form->setRowWrapPolicy(DocChiTiet ? QFormLayout::WrapAllRows : QFormLayout::WrapLongRows);
        Form->setVerticalSpacing(10);
        Form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        NoiDung->addWidget(VungNhap); VungNhap->setVisible(!DocChiTiet);
        Loi = Nhan("", "error"); Loi->setObjectName("loiTong"); Loi->setWordWrap(true);
        NoiDung->addWidget(Loi); Loi->hide();
        Cuon->setWidget(Than); Goc->addWidget(Cuon,1);
        Chan = new QHBoxLayout; Goc->addLayout(Chan);
        auto *Dong = Nut("Đóng", "dongHop"); Chan->addWidget(Dong);
        QObject::connect(Dong, &QPushButton::clicked, &Hop, &QDialog::reject);
    }
    QLineEdit *Truong(const char *Ma, const QString &Ten, const QString &GiaTri = "", const QString &GoiY = "") {
        auto *E = new QLineEdit(GiaTri); E->setObjectName(Ma); E->setAccessibleName(Ten);
        Form->addRow(Ten,E);
        if (!GoiY.isEmpty()) {
            auto* GoiYTruong=Nhan(GoiY,"inputHint"); GoiYTruong->setObjectName(QString(Ma)+"Hint");
            GoiYTruong->setWordWrap(true); Form->addRow(GoiYTruong);
            E->setAccessibleDescription(GoiY);
        }
        return E;
    }
    QPushButton *Luu(const QString &Ten) {
        auto *N = Nut(Ten, "luu", true); Chan->addWidget(N); return N;
    }
    void BaoLoi(const My_stdexcept &E) { Loi->setText(QString::fromUtf8(E.LayThongBao())); Loi->show(); }
    void VanBan(const QString &Chuoi) {
        ThemVanBan(NoiDung,Chuoi);
    }
};
std::string Doc(QLineEdit *E) { return E->text().trimmed().toStdString(); }
void BaoLoi(const char *Chuoi) { throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, Chuoi); }
Date DocNgay(QLineEdit *E) {
    const QDate D = QDate::fromString(E->text().trimmed(), "dd/MM/yyyy");
    if (!D.isValid()) BaoLoi("Ngay sinh khong hop le: nhap dd/MM/yyyy");
    return {D.day(), D.month(), D.year()};
}
int DocSo(QLineEdit *E) {
    bool HopLe = false; const int So = E->text().trimmed().toInt(&HopLe);
    if (!HopLe) BaoLoi("Can nhap so nguyen hop le vao cac truong so do / tuoi");
    return So;
}
QComboBox *TaoGioiTinh(KhungHop &K, Gender GiaTri = Gender::ChuaXacDinh) {
    auto *E = new QComboBox;
    E->setObjectName("GioiTinh"); E->setAccessibleName("Giới tính");
    E->addItems({"Nam", "Nữ", "Khác", "Chưa xác định"});
    E->setCurrentIndex(static_cast<int>(GiaTri)); K.Form->addRow("Giới tính", E); return E;
}
bool DaDong(const Visit &V) {
    return V.LayTrangThai() == VisitStatus::DaKetThuc || V.LayTrangThai() == VisitStatus::DaHuy;
}
QString NgaySinh(const Patient &P) {
    if (!P.CoNgaySinh()) return "";
    const Date D = P.LayNgaySinh(); return QDate(D.Nam, D.Thang, D.Ngay).toString("dd/MM/yyyy");
}
} // namespace

void DemoWindow::MoTiepNhan() {
    if (!CoTheThayDoi()) return;
    KhungHop K(this, "Tiếp nhận bệnh nhân", "Mã bệnh nhân và mã lần khám là hai mã riêng. Chọn đúng hồ sơ cũ trước khi tiếp nhận.");
    K.ChuLon();
    K.Hop.setStyleSheet(K.Hop.styleSheet()+R"(
QLineEdit:read-only, QLineEdit:disabled, QComboBox:disabled { background:#ECEBF0; color:#514B61; border-color:#DCD7E6; }
QListWidget#chonHoSo { background:white; border:1px solid #D6D1DE; border-radius:8px; padding:4px; }
QListWidget#chonHoSo::item { padding:6px; }
QListWidget#chonHoSo::item:selected { background:#EEE9FA; color:#593AB8; }
)");
    auto *CheDo = new QComboBox;
    CheDo->setObjectName("cheDo"); CheDo->addItems({"Tạo hồ sơ mới", "Chọn hồ sơ đã có"});
    K.Form->addRow("Cách tiếp nhận", CheDo);
    auto* VungChon=new QWidget; VungChon->setObjectName("vungChonHoSoCu");
    auto* CotChon=new QVBoxLayout(VungChon); CotChon->setContentsMargins(0,0,0,0); CotChon->setSpacing(8);
    auto* Tim=new QLineEdit; Tim->setObjectName("timHoSo");
    Tim->setAccessibleName("Tìm hồ sơ cũ theo tên / CCCD / mã");
    auto* NhanTim=Nhan("Tìm hồ sơ cũ theo tên / CCCD / mã"); NhanTim->setBuddy(Tim);
    CotChon->addWidget(NhanTim); CotChon->addWidget(Tim);
    auto *DanhSach = new QListWidget; DanhSach->setObjectName("chonHoSo");
    DanhSach->setAccessibleName("Danh sách hồ sơ bệnh nhân cũ");
    DanhSach->setMaximumHeight(160); CotChon->addWidget(DanhSach);
    K.Form->addRow(VungChon);
    auto* HuongDan=Nhan("", "muted"); HuongDan->setWordWrap(true); K.Form->addRow(HuongDan);
    auto *Ma = K.Truong("MaBenhNhan", "Mã bệnh nhân mới *");
    auto *Ten = K.Truong("HoTen", "Họ tên (để trống nếu chưa rõ)");
    auto *Gioi = TaoGioiTinh(K);
    auto *Sinh = K.Truong("NgaySinh", "Ngày sinh dd/MM/yyyy");
    auto *Tuoi = K.Truong("Tuoi", "Tuổi ước lượng nếu chưa có ngày sinh");
    auto *Cccd = K.Truong("CanCuoc", "CCCD");
    auto *DiUng = K.Truong("DiUng", "Dị ứng");
    auto *TienSu = K.Truong("TienSu", "Tiền sử bệnh");
    auto *MaLuot = K.Truong("MaLanKham", "Mã lần khám *");
    auto *TrieuChung = K.Truong("TrieuChung", "Triệu chứng / lý do khám *");
    K.VanBan("Trước khi đánh giá cần có ngày sinh hoặc tuổi ước lượng.");
    auto *Luu = K.Luu("Tiếp nhận và mở lần khám");
    const auto DienThongTin=[&] {
        const bool Moi=CheDo->currentIndex()==0;
        const Patient* P=!Moi && DanhSach->currentItem()
            ? Registry.TimBenhNhan(DanhSach->currentItem()->data(Qt::UserRole).toString().toStdString()) : nullptr;
        const auto Dat=[&](QLineEdit* O, const QString& GiaTri, bool DaBiet) {
            O->setText(GiaTri); O->setEnabled(Moi || P!=nullptr);
            O->setReadOnly(!Moi && DaBiet);
            O->setToolTip(!Moi && DaBiet ? "Thông tin đã có trong hồ sơ; được giữ nguyên khi tiếp nhận lại." : "");
        };
        Dat(Ma,P ? Chu(P->LayMaDinhDanh()) : "",P!=nullptr);
        Dat(Ten,P ? Chu(P->LayHoTen()) : "",P && !P->LayHoTen().empty());
        Dat(Sinh,P ? NgaySinh(*P) : "",P && P->CoNgaySinh());
        Dat(Tuoi,P && P->CoTuoiUocLuong() ? QString::number(P->LayTuoiUocLuong()) : "",
            P && (P->CoTuoiUocLuong() || P->CoNgaySinh()));
        Tuoi->setPlaceholderText(P && P->CoNgaySinh() && !P->CoTuoiUocLuong() ? "Đã có ngày sinh" : "");
        Dat(Cccd,P ? Chu(P->LaySoCanCuoc()) : "",P && !P->LaySoCanCuoc().empty());
        Dat(DiUng,P ? Chu(P->LayThongTinDiUng()) : "",P && !P->LayThongTinDiUng().empty());
        Dat(TienSu,P ? Chu(P->LayTienSuBenh()) : "",P && !P->LayTienSuBenh().empty());
        Gioi->setCurrentIndex(static_cast<int>(P ? P->LayGioiTinh() : Gender::ChuaXacDinh));
        Gioi->setEnabled(Moi || (P && P->LayGioiTinh()==Gender::ChuaXacDinh));
        Gioi->setToolTip(!Moi && P && !Gioi->isEnabled() ? "Giới tính đã có trong hồ sơ." : "");
        static_cast<QLabel*>(K.Form->labelForField(Ma))->setText(Moi ? "Mã bệnh nhân mới *" : "Mã bệnh nhân");
        Luu->setEnabled(Moi || P!=nullptr);
        K.Loi->clear(); K.Loi->hide();
    };
    const auto TimLai = [&] {
        DanhSach->clear();
        const auto CacHoSo = Registry.LayDanhSachBenhNhan();
        for (int I = 0; I < CacHoSo.KichThuoc(); ++I) {
            const Patient &P = *CacHoSo[I];
            QString NoiDung=Chu(P.LayMaDinhDanh());
            if (!P.LayHoTen().empty()) NoiDung+=" · "+Chu(P.LayHoTen());
            if (!P.LaySoCanCuoc().empty()) NoiDung+=" · "+Chu(P.LaySoCanCuoc());
            if (!ChuanHoa(NoiDung).contains(ChuanHoa(Tim->text()))) continue;
            auto *Dong = new QListWidgetItem(NoiDung);
            Dong->setData(Qt::UserRole, Chu(P.LayMaDinhDanh())); DanhSach->addItem(Dong);
        }
        // Không tự chọn kết quả, nhất là khi có người trùng tên.
        DienThongTin();
    };
    connect(Tim, &QLineEdit::textChanged, &K.Hop, TimLai);
    connect(DanhSach,&QListWidget::currentItemChanged,&K.Hop,[&] { DienThongTin(); });
    const auto DoiCheDo = [&] {
        const bool Moi = CheDo->currentIndex() == 0;
        VungChon->setVisible(!Moi);
        HuongDan->setText(Moi ? "Nhập thông tin người bệnh và lần khám mới." :
            "Chọn đúng người từ danh sách. Ô xám là thông tin đã có; ô trắng có thể bổ sung.");
        if (Moi) DienThongTin(); else TimLai();
    };
    connect(CheDo, qOverload<int>(&QComboBox::currentIndexChanged), &K.Hop, DoiCheDo); DoiCheDo();
    const auto DocHoSo=[&](const std::string& MaHoSo) {
        const Gender G=static_cast<Gender>(Gioi->currentIndex());
        Patient P=Doc(Sinh).empty() ? Patient(MaHoSo,Doc(Ten),G,Doc(Cccd))
            : Patient(MaHoSo,Doc(Ten),G,DocNgay(Sinh),Doc(Cccd));
        if (!Doc(Tuoi).empty()) P.DatTuoiUocLuong(DocSo(Tuoi));
        P.CapNhatThongTinDiUng(Doc(DiUng)); P.CapNhatTienSuBenh(Doc(TienSu));
        return P;
    };
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            if (Doc(MaLuot).empty() || Doc(TrieuChung).empty()) BaoLoi("Nhap ma lan kham va trieu chung");
            if (Registry.TimLanKham(Doc(MaLuot))) BaoLoi("Ma lan kham da ton tai");
            std::string MaChon;
            if (CheDo->currentIndex() == 1) {
                if (!DanhSach->currentItem()) BaoLoi("Chon dung ho so benh nhan truoc");
                MaChon = DanhSach->currentItem()->data(Qt::UserRole).toString().toStdString();
                Registry.BoSungThongTinConThieu(MaChon,DocHoSo(MaChon));
                if (!DanhDauThayDoi()) { K.Hop.accept(); return; }
                DienThongTin();
            } else {
                Patient P=DocHoSo(Doc(Ma));
                Registry.ThemBenhNhan(P); MaChon = P.LayMaDinhDanh();
                if (!DanhDauThayDoi()) { K.Hop.accept(); return; }
                // Nếu bước tạo Visit thất bại, hồ sơ vẫn còn và lần thử lại dùng hồ sơ đó.
                Tim->clear(); CheDo->setCurrentIndex(1);
                for (int I = 0; I < DanhSach->count(); ++I)
                    if (DanhSach->item(I)->data(Qt::UserRole).toString() == Chu(MaChon))
                        DanhSach->setCurrentRow(I);
            }
            DichVu.TiepNhan(Doc(MaLuot), MaChon, std::time(nullptr), Doc(TrieuChung));
            DanhDauThayDoi();
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    K.Hop.exec(); LamMoi();
}

void DemoWindow::MoDanhGia(const std::string &MaLanKham) {
    if (!CoTheThayDoi()) return;
    const Visit *V = Registry.TimLanKham(MaLanKham); if (!V) return;
    KhungHop K(this, "Đánh giá sinh hiệu", Chu(MaLanKham) + " · Mức cấp cứu được tính từ sinh hiệu và tuổi.");
    K.ChuLon();
    K.Form->setRowWrapPolicy(QFormLayout::WrapAllRows);
    K.Form->setLabelAlignment(Qt::AlignLeft|Qt::AlignTop);
    K.Form->setVerticalSpacing(6);
    auto *Mach = K.Truong("Mach", "Mạch (lần/phút)", "", "Hợp lệ: 0–300 lần/phút, số nguyên.");
    auto *Ha = K.Truong("HuyetAp", "Huyết áp tâm thu (mmHg)", "", "Hợp lệ: 0–300 mmHg, số nguyên.");
    auto *Tho = K.Truong("NhipTho", "Nhịp thở (lần/phút)", "", "Hợp lệ: 0–120 lần/phút, số nguyên.");
    auto *Spo = K.Truong("SpO2", "SpO₂ (%)", "", "Hợp lệ: 0–100% (ví dụ 98,5 hoặc 98.5).");
    auto *Gcs = K.Truong("GCS", "GCS (3–15)", "", "Hợp lệ: 3–15 điểm, số nguyên.");
    auto *Dd = K.Truong("MaDieuDuong", "Mã điều dưỡng *", "", "Bắt buộc nhập mã điều dưỡng đánh giá.");
    const Patient *P = Registry.TimBenhNhan(V->LayMaBenhNhan());
    if (!P->CoNgaySinh() && !P->CoTuoiUocLuong())
        K.VanBan("Hồ sơ chưa có tuổi. Hãy bổ sung danh tính với ngày sinh trước khi đánh giá.");
    auto *Luu = K.Luu("Lưu đánh giá");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            bool HopLe = false; const double S = QString(Spo->text()).replace(',', '.').toDouble(&HopLe);
            if (!HopLe) BaoLoi("SpO2 phai la so hop le");
            const VitalSigns SoDo(DocSo(Mach), DocSo(Ha), DocSo(Tho), S, DocSo(Gcs));
            DichVu.DanhGia(MaLanKham, SoDo, std::time(nullptr), Doc(Dd));
            DanhDauThayDoi();
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    if (K.Hop.exec() == QDialog::Accepted) {
        LamMoi(); if (DaThayDoi) return;
        statusBar()->showMessage("Đã đánh giá: mức " + QString::number(V->LayDanhGiaGanNhat().LayMuc()), 8000);
    }
}

void DemoWindow::MoChiTiet(const std::string &MaLanKham) {
    // Sau mỗi thao tác, dựng lại nội dung từ Registry để tránh hiển thị bản cũ.
    while (const Visit *V = Registry.TimLanKham(MaLanKham)) {
        const Patient *P = Registry.TimBenhNhan(V->LayMaBenhNhan());
        KhungHop K(this, "Chi tiết lần khám", "Thông tin tiếp nhận và diễn biến lần khám.", true);
        K.Hop.setObjectName("chiTietDialog");
        ThemThongTin(K.NoiDung,"Mã lần khám",Chu(MaLanKham),"chiTietMaLanKham");
        ThemThongTin(K.NoiDung,"Mã bệnh nhân",Chu(P->LayMaDinhDanh()),"chiTietMaBenhNhan");
        ThemThongTin(K.NoiDung,"Họ tên",Chu(P->LayHoTen()),"chiTietHoTen");
        ThemThongTin(K.NoiDung,"CCCD",Chu(P->LaySoCanCuoc()),"chiTietCanCuoc");
        ThemThongTin(K.NoiDung,"Trạng thái",TenTrangThai(*V),"chiTietTrangThai");
        ThemThongTin(K.NoiDung,"Triệu chứng",Chu(V->LayTrieuChungChinh()),"chiTietTrieuChung");
        ThemThongTin(K.NoiDung,"Tiếp nhận",ThoiDiem(V->LayThoiDiemTiepNhan())+" (UTC+7)","chiTietTiepNhan");
        K.NoiDung->addWidget(Nhan("Thao tác", "section"));
        int HanhDong = 0;
        const auto Them = [&](const QString &Ten, const char *Ma, int So) {
            auto *N = Nut(Ten, Ma); K.NoiDung->addWidget(N);
            connect(N, &QPushButton::clicked, &K.Hop, [&, So] { HanhDong = So; K.Hop.accept(); });
        };
        Them("Hồ sơ bệnh nhân", "suaHoSo", 1);
        const VisitStatus T = V->LayTrangThai();
        if (T == VisitStatus::ChoKhamLanDau || T == VisitStatus::ChoKhamQuayLai)
            Them(V->SoDanhGia() ? "Đánh giá lại" : "Đánh giá ban đầu", "danhGia", 2);
        if (T == VisitStatus::DangKham) {
            Them("Chuyển cận lâm sàng", "chuyenCls", 3); Them("Kết thúc lần khám", "ketThuc", 5);
        }
        if (T == VisitStatus::ChoCanLamSang) Them("Ghi kết quả & quay lại", "quayLai", 4);
        if (!DaDong(*V)) Them("Hủy lần khám", "huyLuot", 6);
        if (V->SoLanBacSiThamGiaKham()) K.NoiDung->addWidget(Nhan("Bác sĩ đã tham gia", "section"));
        for (int I = 0; I < V->SoLanBacSiThamGiaKham(); ++I)
            ThemThongTin(K.NoiDung,"Mã bác sĩ",Chu(V->LayBacSiThamGiaKhamTai(I)));
        if (V->SoLanCanLamSang()) K.NoiDung->addWidget(Nhan("Cận lâm sàng", "section"));
        for (int I = 0; I < V->SoLanCanLamSang(); ++I) {
            const auto &C = V->LayCanLamSangTai(I);
            auto* The=ThemThe(K.NoiDung);
            ThemThongTin(The,"Chỉ định",Chu(C.LayChiDinh()),"chiTietChiDinh");
            ThemThongTin(The,"Bác sĩ chỉ định",Chu(C.LayMaBacSiChiDinh()));
            ThemThongTin(The,"Kết quả",C.DaCoKetQua() ? Chu(C.LayKetQua()) : "");
        }
        if (V->SoDanhGia()) K.NoiDung->addWidget(Nhan("Lịch sử đánh giá", "section"));
        for (int I = V->SoDanhGia() - 1; I >= 0; --I) {
            const auto &D = V->LayDanhGiaTai(I); const auto &S = D.LaySinhHieu();
            auto* The=ThemThe(K.NoiDung);
            ThemThongTin(The,"Thời điểm",ThoiDiem(D.LayThoiDiemDanhGia())+" (UTC+7)");
            ThemThongTin(The,"Điều dưỡng",Chu(D.LayMaDieuDuong()));
            ThemThongTin(The,"Mức cấp cứu",QString::number(D.LayMuc()));
            ThemThongTin(The,"Tuổi khi đánh giá",QString::number(D.LayTuoi())+
                (D.LaTuoiUocLuong() ? " (ước lượng)" : " (theo ngày sinh)"));
            ThemThongTin(The,"Mạch",QString::number(S.LayNhipTim())+" lần/phút","danhGiaNhipTim");
            ThemThongTin(The,"Huyết áp tâm thu",QString::number(S.LayHuyetApTamThu())+" mmHg");
            ThemThongTin(The,"Nhịp thở",QString::number(S.LayNhipTho())+" lần/phút");
            ThemThongTin(The,"SpO₂",QString::number(S.LaySpo2())+"%");
            ThemThongTin(The,"GCS",QString::number(S.LayDiemGcs()));
        }
        if (K.Hop.exec() != QDialog::Accepted) break;
        if (HanhDong == 1) MoHoSo(P->LayMaDinhDanh());
        else if (HanhDong == 2) MoDanhGia(MaLanKham);
        else if (HanhDong == 3 || HanhDong == 4) MoCanLamSang(MaLanKham, HanhDong == 4);
        else if (HanhDong == 5 || HanhDong == 6) XacNhanDong(MaLanKham, HanhDong == 6);
        else break;
    }
}

void DemoWindow::MoHoSo(const std::string &MaBenhNhan) {
    if (!CoTheThayDoi()) return;
    const Patient *P = Registry.TimBenhNhan(MaBenhNhan); if (!P) return;
    KhungHop K(this, "Hồ sơ bệnh nhân", "Mã bệnh nhân: " + Chu(MaBenhNhan), true);
    K.Hop.setObjectName("hoSoDialog");
    auto *Ten = K.Truong("HoTen", "Họ tên *", Chu(P->LayHoTen()));
    auto *Cccd = K.Truong("CanCuoc", "CCCD", Chu(P->LaySoCanCuoc()));
    auto *Sinh = K.Truong("NgaySinh", "Ngày sinh dd/MM/yyyy *", NgaySinh(*P));
    auto *Gioi = TaoGioiTinh(K, P->LayGioiTinh());
    auto* PhanHoi=Nhan("", "notice"); PhanHoi->setObjectName("hoSoPhanHoi"); PhanHoi->setWordWrap(true);
    K.NoiDung->addWidget(PhanHoi); PhanHoi->hide();
    auto* Xem=new QWidget; Xem->setObjectName("hoSoTomTat");
    auto* CotXem=new QVBoxLayout(Xem); CotXem->setContentsMargins(0,0,0,0);
    CotXem->setSpacing(10); CotXem->setAlignment(Qt::AlignTop); K.NoiDung->addWidget(Xem);
    const char* GioiTinh[]={"Nam","Nữ","Khác",""};
    const char* Nhom[]={"A","B","AB","O",""};
    auto* TenXem=ThemThongTin(CotXem,"Họ tên",Chu(P->LayHoTen()),"hoSoHoTen");
    auto* CccdXem=ThemThongTin(CotXem,"CCCD",Chu(P->LaySoCanCuoc()),"hoSoCanCuoc");
    auto* SinhXem=ThemThongTin(CotXem,"Ngày sinh",NgaySinh(*P),"hoSoNgaySinh");
    auto* GioiXem=ThemThongTin(CotXem,"Giới tính",GioiTinh[static_cast<int>(P->LayGioiTinh())],"hoSoGioiTinh");
    ThemThongTin(CotXem,"Nhóm máu",Nhom[static_cast<int>(P->LayNhomMau())],"hoSoNhomMau");
    ThemThongTin(CotXem,"Dị ứng",Chu(P->LayThongTinDiUng()),"hoSoDiUng");
    ThemThongTin(CotXem,"Tiền sử bệnh",Chu(P->LayTienSuBenh()),"hoSoTienSu");
    if (P->CoTuoiUocLuong()) ThemThongTin(CotXem,"Tuổi ước lượng",QString::number(P->LayTuoiUocLuong()));
    std::string MoLuot;
    const auto CacLuot = static_cast<const PatientRegistry&>(Registry).LayLichSuLanKham(MaBenhNhan);
    if (!CacLuot.Rong()) CotXem->addWidget(Nhan("Lịch sử lần khám", "section"));
    for (int I = 0; I < CacLuot.KichThuoc(); ++I) {
        const std::string Ma = CacLuot[I]->LayMaLanKham();
        auto *N = Nut(Chu(Ma) + " · " + TenTrangThai(*CacLuot[I]), "hoSoLuot"); CotXem->addWidget(N);
        connect(N, &QPushButton::clicked, &K.Hop, [&, Ma] { MoLuot = Ma; K.Hop.accept(); });
    }
    auto* Sua=Nut("Bổ sung / cập nhật thông tin","suaThongTin",true); K.Chan->addWidget(Sua);
    auto* Luu=K.Luu("Lưu thay đổi"); Luu->hide();
    auto* Dong=K.Hop.findChild<QPushButton*>("dongHop");
    bool DangSua=false;
    const auto CheDoSua=[&](bool SuaThongTin) {
        DangSua=SuaThongTin;
        K.VungNhap->setVisible(DangSua); Xem->setVisible(!DangSua);
        Sua->setVisible(!DangSua); Luu->setVisible(DangSua);
        Dong->setText(DangSua ? "Hủy chỉnh sửa" : "Đóng");
        K.Loi->clear(); K.Loi->hide(); PhanHoi->hide();
        if (DangSua) {
            Ten->setText(Chu(P->LayHoTen())); Cccd->setText(Chu(P->LaySoCanCuoc()));
            Sinh->setText(NgaySinh(*P)); Gioi->setCurrentIndex(static_cast<int>(P->LayGioiTinh()));
            Ten->setFocus();
        }
        K.Hop.findChild<QScrollArea*>("dialogScroll")->verticalScrollBar()->setValue(0);
    };
    connect(Sua,&QPushButton::clicked,&K.Hop,[&] { if (CoTheThayDoi()) CheDoSua(true); });
    disconnect(Dong,&QPushButton::clicked,&K.Hop,&QDialog::reject);
    connect(Dong,&QPushButton::clicked,&K.Hop,[&] { if (DangSua) CheDoSua(false); else K.Hop.reject(); });
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            Registry.BoSungDanhTinh(MaBenhNhan, Doc(Cccd), Doc(Ten), static_cast<Gender>(Gioi->currentIndex()), DocNgay(Sinh));
            const bool DaLuu=DanhDauThayDoi();
            TenXem->setText("Họ tên: "+Chu(P->LayHoTen())); CccdXem->setText("CCCD: "+Chu(P->LaySoCanCuoc()));
            SinhXem->setText("Ngày sinh: "+NgaySinh(*P)); GioiXem->setText("Giới tính: "+QString(GioiTinh[static_cast<int>(P->LayGioiTinh())]));
            CheDoSua(false);
            PhanHoi->setText(DaLuu ? "Đã cập nhật thông tin." : "Đã cập nhật thông tin. Chương trình đang thử lưu lại.");
            PhanHoi->show();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    K.Hop.exec(); LamMoi();
    if (!MoLuot.empty()) MoChiTiet(MoLuot);
}

void DemoWindow::MoCanLamSang(const std::string &MaLanKham, bool QuayLai) {
    if (!CoTheThayDoi()) return;
    KhungHop K(this, QuayLai ? "Ghi kết quả & quay lại" : "Chỉ định cận lâm sàng", Chu(MaLanKham));
    auto *NoiDung = K.Truong("NoiDung", QuayLai ? "Kết quả *" : "Chỉ định *");
    QLineEdit *BacSi = nullptr;
    if (!QuayLai) BacSi = K.Truong("MaBacSi", "Mã bác sĩ đã tham gia khám *", MaBacSi->text());
    auto *Luu = K.Luu(QuayLai ? "Lưu & vào hàng quay lại" : "Chuyển cận lâm sàng");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            if (QuayLai) DichVu.QuayLai(MaLanKham, Doc(NoiDung), std::time(nullptr));
            else DichVu.ChuyenCanLamSang(MaLanKham, Doc(NoiDung), Doc(BacSi));
            DanhDauThayDoi();
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    K.Hop.exec(); LamMoi();
}

void DemoWindow::XacNhanDong(const std::string &MaLanKham, bool Huy) {
    if (!CoTheThayDoi()) return;
    QMessageBox Hop(this);
    Hop.setWindowTitle(Huy ? "Hủy lần khám" : "Kết thúc lần khám");
    Hop.setTextFormat(Qt::PlainText); Hop.setText("Xác nhận đóng lần khám " + Chu(MaLanKham) + "?");
    auto *Dong = Hop.addButton("Xác nhận", QMessageBox::AcceptRole); Dong->setObjectName("xacNhanDong");
    auto *Quay = Hop.addButton("Quay lại", QMessageBox::RejectRole); Hop.setDefaultButton(Quay);
    Hop.exec(); if (Hop.clickedButton() != Dong) return;
    try {
        if (Huy) DichVu.Huy(MaLanKham); else DichVu.KetThuc(MaLanKham);
        DanhDauThayDoi();
        LamMoi();
    } catch (const My_stdexcept &E) { statusBar()->showMessage(QString::fromUtf8(E.LayThongBao()), 10000); }
}
