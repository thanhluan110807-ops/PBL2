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
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStatusBar>
#include <QVBoxLayout>
using namespace DemoUi;
namespace {
// QObject hủy widget con; các biến mượn widget chỉ sống trong lúc exec().
struct KhungHop {
    QDialog Hop;
    QVBoxLayout *NoiDung;
    QFormLayout *Form;
    QHBoxLayout *Chan;
    QLabel *Loi;
    KhungHop(QWidget *Cha, const QString &Ten, const QString &MoTa) : Hop(Cha) {
        Hop.setObjectName("demoDialog"); Hop.setWindowTitle(Ten);
        Hop.resize(qMin(760, Cha->width() - 24), qMin(850, Cha->height() - 48));
        auto *Goc = new QVBoxLayout(&Hop);
        Goc->addWidget(Nhan(Ten, "section"));
        auto *Phu = Nhan(MoTa, "muted"); Phu->setWordWrap(true); Goc->addWidget(Phu);
        auto *Cuon = new QScrollArea;
        Cuon->setWidgetResizable(true);
        auto *Than = new QWidget;
        Than->setObjectName("dialogContent");
        NoiDung = new QVBoxLayout(Than);
        Form = new QFormLayout;
        Form->setRowWrapPolicy(QFormLayout::WrapLongRows);
        Form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        NoiDung->addLayout(Form);
        Loi = Nhan("", "error"); Loi->setObjectName("loiTong"); Loi->setWordWrap(true);
        NoiDung->addWidget(Loi);
        Cuon->setWidget(Than); Goc->addWidget(Cuon);
        Chan = new QHBoxLayout; Goc->addLayout(Chan);
        auto *Dong = Nut("Đóng", "dongHop"); Chan->addWidget(Dong);
        QObject::connect(Dong, &QPushButton::clicked, &Hop, &QDialog::reject);
    }
    QLineEdit *Truong(const char *Ma, const QString &Ten, const QString &GiaTri = "") {
        auto *E = new QLineEdit(GiaTri); E->setObjectName(Ma); E->setAccessibleName(Ten);
        Form->addRow(Ten, E); return E;
    }
    QPushButton *Luu(const QString &Ten) {
        auto *N = Nut(Ten, "luu", true); Chan->addWidget(N); return N;
    }
    void BaoLoi(const My_stdexcept &E) { Loi->setText(QString::fromUtf8(E.LayThongBao())); }
    void VanBan(const QString &Chuoi) {
        auto *N = Nhan(Chuoi); N->setWordWrap(true); NoiDung->addWidget(N);
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
    KhungHop K(this, "Tiếp nhận bệnh nhân", "Mã bệnh nhân và mã lần khám là hai mã riêng. Chọn đúng hồ sơ cũ trước khi tiếp nhận.");
    auto *CheDo = new QComboBox;
    CheDo->setObjectName("cheDo"); CheDo->addItems({"Tạo hồ sơ mới", "Chọn hồ sơ đã có"});
    K.Form->addRow("Cách tiếp nhận", CheDo);
    auto *Tim = K.Truong("timHoSo", "Tìm hồ sơ cũ theo tên / CCCD / mã");
    auto *DanhSach = new QListWidget; DanhSach->setObjectName("chonHoSo");
    K.Form->addRow(DanhSach);
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
    K.VanBan("Trước khi đánh giá cần có ngày sinh hoặc tuổi ước lượng. Hồ sơ đã tạo chỉ bổ sung danh tính; thông tin y tế hiện chưa có chức năng sửa.");
    const auto TimLai = [&] {
        DanhSach->clear();
        const auto CacHoSo = Registry.LayDanhSachBenhNhan();
        for (int I = 0; I < CacHoSo.KichThuoc(); ++I) {
            const Patient &P = *CacHoSo[I];
            const QString NoiDung = Chu(P.LayMaDinhDanh()) + " · " + Chu(P.LayHoTen()) + " · " + Chu(P.LaySoCanCuoc());
            if (!ChuanHoa(NoiDung).contains(ChuanHoa(Tim->text()))) continue;
            auto *Dong = new QListWidgetItem(NoiDung);
            Dong->setData(Qt::UserRole, Chu(P.LayMaDinhDanh())); DanhSach->addItem(Dong);
        }
        // Không tự chọn kết quả, nhất là khi có người trùng tên.
    };
    TimLai(); connect(Tim, &QLineEdit::textChanged, &K.Hop, TimLai);
    const auto DoiCheDo = [&] {
        const bool Moi = CheDo->currentIndex() == 0;
        Tim->setEnabled(!Moi); DanhSach->setEnabled(!Moi);
        for (auto *E : {Ma, Ten, Sinh, Tuoi, Cccd, DiUng, TienSu}) E->setEnabled(Moi);
        Gioi->setEnabled(Moi);
    };
    connect(CheDo, qOverload<int>(&QComboBox::currentIndexChanged), &K.Hop, DoiCheDo); DoiCheDo();
    auto *Luu = K.Luu("Tiếp nhận & mở lần khám");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            if (Doc(MaLuot).empty() || Doc(TrieuChung).empty()) BaoLoi("Nhap ma lan kham va trieu chung");
            if (Registry.TimLanKham(Doc(MaLuot))) BaoLoi("Ma lan kham da ton tai");
            std::string MaChon;
            if (CheDo->currentIndex() == 1) {
                if (!DanhSach->currentItem()) BaoLoi("Chon dung ho so benh nhan truoc");
                MaChon = DanhSach->currentItem()->data(Qt::UserRole).toString().toStdString();
            } else {
                const Gender G = static_cast<Gender>(Gioi->currentIndex());
                Patient P = Doc(Sinh).empty() ? Patient(Doc(Ma), Doc(Ten), G, Doc(Cccd))
                    : Patient(Doc(Ma), Doc(Ten), G, DocNgay(Sinh), Doc(Cccd));
                if (!Doc(Tuoi).empty()) P.DatTuoiUocLuong(DocSo(Tuoi));
                P.CapNhatThongTinDiUng(Doc(DiUng)); P.CapNhatTienSuBenh(Doc(TienSu));
                Registry.ThemBenhNhan(P); MaChon = P.LayMaDinhDanh();
                // Nếu bước tạo Visit thất bại, hồ sơ vẫn còn và lần thử lại dùng hồ sơ đó.
                Tim->clear(); TimLai(); CheDo->setCurrentIndex(1);
                for (int I = 0; I < DanhSach->count(); ++I)
                    if (DanhSach->item(I)->data(Qt::UserRole).toString() == Chu(MaChon))
                        DanhSach->setCurrentRow(I);
            }
            DichVu.TiepNhan(Doc(MaLuot), MaChon, std::time(nullptr), Doc(TrieuChung));
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    K.Hop.exec(); LamMoi();
}

void DemoWindow::MoDanhGia(const std::string &MaLanKham) {
    const Visit *V = Registry.TimLanKham(MaLanKham); if (!V) return;
    KhungHop K(this, "Đánh giá sinh hiệu", Chu(MaLanKham) + " · Mức cấp cứu được tính từ sinh hiệu và tuổi.");
    auto *Mach = K.Truong("Mach", "Mạch (lần/phút)");
    auto *Ha = K.Truong("HuyetAp", "Huyết áp tâm thu (mmHg)");
    auto *Tho = K.Truong("NhipTho", "Nhịp thở (lần/phút)");
    auto *Spo = K.Truong("SpO2", "SpO₂ (%)");
    auto *Gcs = K.Truong("GCS", "GCS (3–15)");
    auto *Dd = K.Truong("MaDieuDuong", "Mã điều dưỡng *");
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
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    if (K.Hop.exec() == QDialog::Accepted) {
        LamMoi(); statusBar()->showMessage("Đã đánh giá: mức " + QString::number(V->LayDanhGiaGanNhat().LayMuc()), 8000);
    }
}

void DemoWindow::MoChiTiet(const std::string &MaLanKham) {
    // Sau mỗi thao tác, dựng lại nội dung từ Registry để tránh hiển thị bản cũ.
    while (const Visit *V = Registry.TimLanKham(MaLanKham)) {
        const Patient *P = Registry.TimBenhNhan(V->LayMaBenhNhan());
        KhungHop K(this, "Chi tiết lần khám", Chu(MaLanKham) + " · " + TenTrangThai(*V));
        K.Hop.setObjectName("chiTietDialog");
        K.VanBan(Chu(P->LayMaDinhDanh()) + " · " + Chu(P->LayHoTen()) + "\nTriệu chứng: " + Chu(V->LayTrieuChungChinh()));
        K.VanBan("Tiếp nhận: " + ThoiDiem(V->LayThoiDiemTiepNhan()) + " (UTC+7)");
        int HanhDong = 0;
        const auto Them = [&](const QString &Ten, const char *Ma, int So) {
            auto *N = Nut(Ten, Ma); K.NoiDung->addWidget(N);
            connect(N, &QPushButton::clicked, &K.Hop, [&, So] { HanhDong = So; K.Hop.accept(); });
        };
        Them("Hồ sơ / bổ sung danh tính", "suaHoSo", 1);
        const VisitStatus T = V->LayTrangThai();
        if (T == VisitStatus::ChoKhamLanDau || T == VisitStatus::ChoKhamQuayLai)
            Them(V->SoDanhGia() ? "Đánh giá lại" : "Đánh giá ban đầu", "danhGia", 2);
        if (T == VisitStatus::DangKham) {
            Them("Chuyển cận lâm sàng", "chuyenCls", 3); Them("Kết thúc lần khám", "ketThuc", 5);
        }
        if (T == VisitStatus::ChoCanLamSang) Them("Ghi kết quả & quay lại", "quayLai", 4);
        if (!DaDong(*V)) Them("Hủy lần khám", "huyLuot", 6);
        for (int I = 0; I < V->SoLanBacSiThamGiaKham(); ++I)
            K.VanBan("Bác sĩ đã tham gia: " + Chu(V->LayBacSiThamGiaKhamTai(I)));
        for (int I = 0; I < V->SoLanCanLamSang(); ++I) {
            const auto &C = V->LayCanLamSangTai(I);
            K.VanBan("Cận lâm sàng: " + Chu(C.LayChiDinh()) + " · BS " + Chu(C.LayMaBacSiChiDinh()) +
                     "\nKết quả: " + (C.DaCoKetQua() ? Chu(C.LayKetQua()) : "Chưa có"));
        }
        for (int I = V->SoDanhGia() - 1; I >= 0; --I) {
            const auto &D = V->LayDanhGiaTai(I); const auto &S = D.LaySinhHieu();
            K.VanBan(ThoiDiem(D.LayThoiDiemDanhGia()) + " · Điều dưỡng " + Chu(D.LayMaDieuDuong()) +
                QString("\nMức %1 · Tuổi %2 (%3)\nMạch %4 · HA %5 · Thở %6 · SpO₂ %7% · GCS %8")
                .arg(D.LayMuc()).arg(D.LayTuoi()).arg(D.LaTuoiUocLuong() ? "ước lượng" : "ngày sinh")
                .arg(S.LayNhipTim()).arg(S.LayHuyetApTamThu()).arg(S.LayNhipTho()).arg(S.LaySpo2()).arg(S.LayDiemGcs()));
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
    const Patient *P = Registry.TimBenhNhan(MaBenhNhan); if (!P) return;
    KhungHop K(this, "Hồ sơ bệnh nhân", Chu(MaBenhNhan) + " · Bổ sung danh tính giữ nguyên mã và lịch sử khám.");
    K.Hop.setObjectName("hoSoDialog");
    auto *Ten = K.Truong("HoTen", "Họ tên *", Chu(P->LayHoTen()));
    auto *Cccd = K.Truong("CanCuoc", "CCCD", Chu(P->LaySoCanCuoc()));
    auto *Sinh = K.Truong("NgaySinh", "Ngày sinh dd/MM/yyyy *", NgaySinh(*P));
    auto *Gioi = TaoGioiTinh(K, P->LayGioiTinh());
    const char *Nhom[] = {"A", "B", "AB", "O", "Chưa xác định"};
    K.VanBan("Nhóm máu: " + QString(Nhom[static_cast<int>(P->LayNhomMau())]) +
             "\nDị ứng: " + Chu(P->LayThongTinDiUng()) + "\nTiền sử: " + Chu(P->LayTienSuBenh()));
    if (P->CoTuoiUocLuong()) K.VanBan("Tuổi ước lượng: " + QString::number(P->LayTuoiUocLuong()));
    std::string MoLuot;
    const auto CacLuot = static_cast<const PatientRegistry&>(Registry).LayLichSuLanKham(MaBenhNhan);
    for (int I = 0; I < CacLuot.KichThuoc(); ++I) {
        const std::string Ma = CacLuot[I]->LayMaLanKham();
        auto *N = Nut(Chu(Ma) + " · " + TenTrangThai(*CacLuot[I]), "hoSoLuot"); K.NoiDung->addWidget(N);
        connect(N, &QPushButton::clicked, &K.Hop, [&, Ma] { MoLuot = Ma; K.Hop.accept(); });
    }
    auto *Luu = K.Luu("Bổ sung danh tính");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            Registry.BoSungDanhTinh(MaBenhNhan, Doc(Cccd), Doc(Ten), static_cast<Gender>(Gioi->currentIndex()), DocNgay(Sinh));
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    K.Hop.exec(); LamMoi();
    if (!MoLuot.empty()) MoChiTiet(MoLuot);
}

void DemoWindow::MoCanLamSang(const std::string &MaLanKham, bool QuayLai) {
    KhungHop K(this, QuayLai ? "Ghi kết quả & quay lại" : "Chỉ định cận lâm sàng", Chu(MaLanKham));
    auto *NoiDung = K.Truong("NoiDung", QuayLai ? "Kết quả *" : "Chỉ định *");
    QLineEdit *BacSi = nullptr;
    if (!QuayLai) BacSi = K.Truong("MaBacSi", "Mã bác sĩ đã tham gia khám *", MaBacSi->text());
    auto *Luu = K.Luu(QuayLai ? "Lưu & vào hàng quay lại" : "Chuyển cận lâm sàng");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        try {
            if (QuayLai) DichVu.QuayLai(MaLanKham, Doc(NoiDung), std::time(nullptr));
            else DichVu.ChuyenCanLamSang(MaLanKham, Doc(NoiDung), Doc(BacSi));
            K.Hop.accept();
        } catch (const My_stdexcept &E) { K.BaoLoi(E); }
    });
    K.Hop.exec(); LamMoi();
}

void DemoWindow::XacNhanDong(const std::string &MaLanKham, bool Huy) {
    QMessageBox Hop(this);
    Hop.setWindowTitle(Huy ? "Hủy lần khám" : "Kết thúc lần khám");
    Hop.setTextFormat(Qt::PlainText); Hop.setText("Xác nhận đóng lần khám " + Chu(MaLanKham) + "?");
    auto *Dong = Hop.addButton("Xác nhận", QMessageBox::AcceptRole); Dong->setObjectName("xacNhanDong");
    auto *Quay = Hop.addButton("Quay lại", QMessageBox::RejectRole); Hop.setDefaultButton(Quay);
    Hop.exec(); if (Hop.clickedButton() != Dong) return;
    try {
        if (Huy) DichVu.Huy(MaLanKham); else DichVu.KetThuc(MaLanKham);
        LamMoi();
    } catch (const My_stdexcept &E) { statusBar()->showMessage(QString::fromUtf8(E.LayThongBao()), 10000); }
}
