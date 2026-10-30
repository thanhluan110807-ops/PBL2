#include "ui/DemoStyle.h"
#include "ui/DemoWindow.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QStatusBar>
#include <QStyle>
#include <QVBoxLayout>
using namespace DemoUi;
namespace {
// Dialog sở hữu widget qua cây QObject. Các helper không giữ con trỏ sau exec().
struct KhungHop {
    QDialog Hop;
    QVBoxLayout *Goc;
    QVBoxLayout *NoiDung;
    QHBoxLayout *Chan;
    QLabel *Loi;
    KhungHop(QWidget *Cha, const QString &TieuDe, const QString &MoTa) : Hop(Cha) {
        Hop.setObjectName("demoDialog");
        Hop.setWindowTitle(TieuDe);
        Hop.resize(qMin(760, Cha->width() - 24), qMin(850, Cha->height() - 48));
        Goc = new QVBoxLayout(&Hop);
        Goc->setContentsMargins(22, 20, 22, 18);
        Goc->setSpacing(14);
        auto *Dau = new QHBoxLayout;
        auto *ChuCot = new QVBoxLayout;
        ChuCot->addWidget(Nhan(TieuDe, "section"));
        auto *Phu = Nhan(MoTa, "muted");
        Phu->setWordWrap(true);
        ChuCot->addWidget(Phu);
        Dau->addLayout(ChuCot, 1);
        auto *Dong = Nut("×", "dongHop");
        Dong->setAccessibleName("Đóng hộp thoại");
        Dong->setToolTip("Đóng (Esc)");
        Dong->setFixedWidth(44);
        Dau->addWidget(Dong);
        QObject::connect(Dong, &QPushButton::clicked, &Hop, &QDialog::reject);
        Goc->addLayout(Dau);
        auto *Cuon = new QScrollArea;
        Cuon->setWidgetResizable(true);
        auto *Than = new QWidget;
        Than->setObjectName("dialogContent");
        NoiDung = new QVBoxLayout(Than);
        NoiDung->setSpacing(14);
        NoiDung->setContentsMargins(0, 0, 8, 0);
        Cuon->setWidget(Than);
        Goc->addWidget(Cuon, 1);
        Loi = Nhan("", "error");
        Loi->setObjectName("loiTong");
        Loi->setWordWrap(true);
        Loi->hide();
        NoiDung->addWidget(Loi);
        Chan = new QHBoxLayout;
        Chan->addStretch();
        Goc->addLayout(Chan);
    }
    QPushButton *ThemNutLuu(const QString &Ten) {
        auto *Huy = Nut("Hủy", "huy");
        Chan->addWidget(Huy);
        QObject::connect(Huy, &QPushButton::clicked, &Hop, &QDialog::reject);
        auto *Luu = Nut(Ten, "luu", true);
        Chan->addWidget(Luu);
        Luu->setDefault(true);
        return Luu;
    }
    void XoaLoi() {
        Loi->hide();
        for (auto *N : Hop.findChildren<QLabel *>())
            if (N->objectName().startsWith("err"))
                N->clear();
        for (auto *E : Hop.findChildren<QLineEdit *>()) {
            E->setProperty("invalid", false);
            E->style()->unpolish(E);
            E->style()->polish(E);
        }
    }
    void BaoLoi(const Demo::KetQua &KetQua) {
        Loi->setText(Chu(KetQua.Loi));
        Loi->show();
        auto *N = Hop.findChild<QLabel *>("err" + Chu(KetQua.Truong));
        if (N)
            N->setText(Chu(KetQua.Loi));
        auto *E = Hop.findChild<QWidget *>(Chu(KetQua.Truong));
        if (E) {
            E->setProperty("invalid", true);
            E->style()->unpolish(E);
            E->style()->polish(E);
            E->setFocus();
        }
    }
};
QLineEdit *Truong(QFormLayout *BoCuc, const char *Ma, const QString &Ten, const QString &GiaTri = "") {
    auto *Khung = new QWidget;
    auto *Cot = new QVBoxLayout(Khung);
    Cot->setContentsMargins(0, 0, 0, 0);
    Cot->setSpacing(4);
    auto *Nhap = new QLineEdit(GiaTri);
    Nhap->setObjectName(Ma);
    Nhap->setAccessibleName(Ten);
    Cot->addWidget(Nhap);
    auto *Loi = Nhan("", "error");
    Loi->setWordWrap(true);
    Loi->setObjectName(QString("err") + Ma);
    Cot->addWidget(Loi);
    auto *N = Nhan(Ten);
    N->setBuddy(Nhap);
    N->setWordWrap(true);
    BoCuc->addRow(N, Khung);
    return Nhap;
}
QFormLayout *TaoForm(QWidget *Cha) {
    auto *Form = new QFormLayout(Cha);
    Form->setVerticalSpacing(12);
    Form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    Form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    Form->setRowWrapPolicy(QFormLayout::WrapLongRows);
    return Form;
}
struct FormHoSo {
    QCheckBox *ChuaRo;
    QLineEdit *HoTen, *NgaySinh, *Tuoi, *CanCuoc, *DiUng, *TienSu;
    QComboBox *GioiTinh;
    QWidget *Khung;
    FormHoSo(const Demo::HoSo &HoSo) {
        Khung = new QWidget;
        auto *Form = TaoForm(Khung);
        ChuaRo = new QCheckBox("Chưa rõ danh tính");
        ChuaRo->setObjectName("ChuaRo");
        ChuaRo->setChecked(HoSo.ChuaRo);
        Form->addRow(ChuaRo);
        HoTen = Truong(Form, "HoTen", "Họ và tên", Chu(HoSo.HoTen));
        GioiTinh = new QComboBox;
        GioiTinh->setObjectName("GioiTinh");
        GioiTinh->setAccessibleName("Giới tính");
        GioiTinh->addItems({"Chưa xác định", "Nam", "Nữ", "Khác"});
        GioiTinh->setCurrentText(Chu(HoSo.GioiTinh));
        Form->addRow("Giới tính", GioiTinh);
        const auto D = HoSo.NgaySinh;
        NgaySinh = Truong(Form, "NgaySinh", "Ngày sinh (dd/MM/yyyy)",
                          HoSo.CoNgaySinh ? QDate(D.Nam, D.Thang, D.Ngay).toString("dd/MM/yyyy") : "");
        NgaySinh->setPlaceholderText("Để trống nếu chưa biết");
        Tuoi = Truong(Form, "Tuoi", "Tuổi ước lượng",
                      HoSo.TuoiUocLuong < 0 ? "" : QString::number(HoSo.TuoiUocLuong));
        Tuoi->setPlaceholderText("Dùng khi chưa rõ ngày sinh");
        CanCuoc = Truong(Form, "CanCuoc", "Căn cước (nếu có)", Chu(HoSo.CanCuoc));
        DiUng = Truong(Form, "DiUng", "Dị ứng", Chu(HoSo.DiUng));
        TienSu = Truong(Form, "TienSu", "Tiền sử", Chu(HoSo.TienSu));
    }
    Demo::KetQua Doc(Demo::HoSo &HoSo) const {
        HoSo.HoTen = HoTen->text().trimmed().toStdString();
        HoSo.ChuaRo = ChuaRo->isChecked();
        HoSo.GioiTinh = GioiTinh->currentText().toStdString();
        HoSo.CanCuoc = CanCuoc->text().trimmed().toStdString();
        HoSo.DiUng = DiUng->text().trimmed().toStdString();
        HoSo.TienSu = TienSu->text().trimmed().toStdString();
        HoSo.CoNgaySinh = !NgaySinh->text().trimmed().isEmpty();
        HoSo.TuoiUocLuong = -1;
        if (HoSo.CoNgaySinh) {
            const QDate D = QDate::fromString(NgaySinh->text().trimmed(), "dd/MM/yyyy");
            if (!D.isValid())
                return {false, "Nhập ngày hợp lệ theo dd/MM/yyyy.", "NgaySinh", -1};
            HoSo.NgaySinh = {D.day(), D.month(), D.year()};
        } else if (!Tuoi->text().trimmed().isEmpty()) {
            bool HopLe = false;
            HoSo.TuoiUocLuong = Tuoi->text().toInt(&HopLe);
            if (!HopLe || HoSo.TuoiUocLuong < 0)
                return {false, "Nhập tuổi nguyên từ 0 đến 150, hoặc để trống.", "Tuoi", -1};
        }
        return {};
    }
};
QString TenBenh(const Demo::HoSo &H) {
    return H.ChuaRo ? "Chưa rõ danh tính" : Chu(H.HoTen);
}
QString TuoiBenh(const Demo::DemoTriageService &D, const Demo::HoSo &H) {
    const int T = D.LayTuoi(H);
    return T < 0 ? "Chưa rõ tuổi"
                 : QString("%1 tuổi · %2").arg(T).arg(H.CoNgaySinh ? "ngày sinh" : "ước lượng");
}
QLabel *VanBan(const QString &Chuoi, const char *VaiTro = "") {
    auto *N = Nhan(Chuoi, VaiTro);
    N->setWordWrap(true);
    N->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return N;
}
QPlainTextEdit *NoiDungDai(KhungHop &K, const char *Ma, const QString &Ten) {
    K.NoiDung->addWidget(Nhan(Ten));
    auto *E = new QPlainTextEdit;
    E->setObjectName(Ma);
    E->setAccessibleName(Ten);
    E->setFixedHeight(96);
    K.NoiDung->addWidget(E);
    auto *L = Nhan("", "error");
    L->setObjectName(QString("err") + Ma);
    L->setWordWrap(true);
    K.NoiDung->addWidget(L);
    return E;
}
} // namespace
void DemoWindow::MoTiepNhan() {
    KhungHop K(this, "Tiếp nhận bệnh nhân", "Hồ sơ lâu dài và triệu chứng lượt khám được lưu riêng.");
    auto *CheDo = new QComboBox;
    CheDo->setObjectName("cheDo");
    CheDo->setAccessibleName("Cách tiếp nhận");
    CheDo->addItems({"Tạo hồ sơ mới", "Chọn hồ sơ đã có", "Người chưa rõ danh tính"});
    K.NoiDung->addWidget(CheDo);
    auto *ChuThich = VanBan("Có thể để trống tên, ngày sinh và căn cước khi chưa rõ danh tính. Mã hồ sơ được "
                            "giữ nguyên khi bổ sung thông tin.",
                            "notice");
    K.NoiDung->addWidget(ChuThich);
    FormHoSo Form(Demo::HoSo{});
    K.NoiDung->addWidget(Form.Khung);
    auto *Cu = new QWidget;
    auto *Cot = new QVBoxLayout(Cu);
    Cot->setContentsMargins(0, 0, 0, 0);
    auto *Tim = new QLineEdit;
    Tim->setObjectName("timHoSo");
    Tim->setAccessibleName("Tìm hồ sơ có sẵn");
    Tim->setPlaceholderText("Tìm mã hoặc tên bệnh nhân…");
    Cot->addWidget(Tim);
    auto *DanhSach = new QListWidget;
    DanhSach->setObjectName("chonHoSo");
    DanhSach->setAccessibleName("Danh sách hồ sơ");
    DanhSach->setMinimumHeight(180);
    Cot->addWidget(DanhSach);
    auto *MoLuot = Nut("Mở lượt đang hoạt động", "moLuotCu");
    Cot->addWidget(MoLuot);
    K.NoiDung->addWidget(Cu);
    Cu->hide();
    const auto TimLai = [&] {
        DanhSach->clear();
        for (int I = 0; I < DichVu.LaySoHoSo(); ++I) {
            const auto &H = DichVu.LayHoSo(I);
            if (!ChuanHoa(TenBenh(H) + " " + Chu(H.Ma)).contains(ChuanHoa(Tim->text())))
                continue;
            const int L = DichVu.TimLuotDangMo(I);
            auto *Dong = new QListWidgetItem(
                TenBenh(H) + " · " + Chu(H.Ma) +
                (L >= 0 ? " · đang có " + Chu(DichVu.LayLuot(L).Ma) : " · có thể tiếp nhận"));
            Dong->setData(Qt::UserRole, I);
            DanhSach->addItem(Dong);
        }
    };
    TimLai();
    connect(Tim, &QLineEdit::textChanged, &K.Hop, [&] { TimLai(); });
    int LuotCu = -1;
    connect(MoLuot, &QPushButton::clicked, &K.Hop, [&] {
        if (!DanhSach->currentItem()) {
            K.BaoLoi({false, "Chọn một hồ sơ trước.", "", -1});
            return;
        }
        LuotCu = DichVu.TimLuotDangMo(DanhSach->currentItem()->data(Qt::UserRole).toInt());
        if (LuotCu < 0) {
            K.BaoLoi({false, "Hồ sơ này không có lượt đang mở.", "", -1});
            return;
        }
        K.Hop.accept();
    });
    connect(CheDo, qOverload<int>(&QComboBox::currentIndexChanged), &K.Hop, [&](int ViTri) {
        Form.Khung->setVisible(ViTri != 1);
        Cu->setVisible(ViTri == 1);
        Form.ChuaRo->setChecked(ViTri == 2);
        K.XoaLoi();
    });
    auto *TrieuChung = NoiDungDai(K, "TrieuChung", "Triệu chứng / lý do đến khám *");
    auto *Luu = K.ThemNutLuu("Tiếp nhận & mở lượt");
    int LuotMoi = -1;
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        K.XoaLoi();
        Demo::HoSo H;
        int DaCo = -1;
        Demo::KetQua Q;
        if (CheDo->currentIndex() == 1) {
            if (!DanhSach->currentItem()) {
                K.BaoLoi({false, "Chọn một hồ sơ trước khi tiếp nhận.", "", -1});
                return;
            }
            DaCo = DanhSach->currentItem()->data(Qt::UserRole).toInt();
        } else {
            Q = Form.Doc(H);
            if (!Q.ThanhCong) {
                K.BaoLoi(Q);
                return;
            }
        }
        Q = DichVu.TiepNhan(H, TrieuChung->toPlainText().trimmed().toStdString(), DaCo);
        if (!Q.ThanhCong) {
            K.BaoLoi(Q);
            return;
        }
        LuotMoi = Q.ChiSo;
        K.Hop.accept();
    });
    if (K.Hop.exec() == QDialog::Accepted) {
        LamMoi();
        if (LuotMoi >= 0) {
            statusBar()->showMessage("Đã tiếp nhận · chờ sàng lọc.", 6000);
            MoChiTiet(LuotMoi);
        } else if (LuotCu >= 0)
            MoChiTiet(LuotCu);
    }
}
void DemoWindow::MoDanhGia(int ChiSo) {
    const auto &L = DichVu.LayLuot(ChiSo);
    const auto &H = DichVu.LayHoSo(L.BenhNhan);
    KhungHop K(this, L.SoDanhGia ? "Đánh giá lại" : "Phân loại ban đầu",
               TenBenh(H) + " · " + Chu(H.Ma) + " · " + Chu(L.Ma));
    K.NoiDung->addWidget(
        VanBan(Chu(L.TrieuChung) + "\nTuổi dùng để đánh giá: " + TuoiBenh(DichVu, H), "notice"));
    K.NoiDung->addWidget(
        VanBan("Nhập số đo của lần đánh giá này. Các trường sinh hiệu đều bắt buộc.", "muted"));
    auto *Vung = new QWidget;
    auto *Form = TaoForm(Vung);
    K.NoiDung->addWidget(Vung);
    QLineEdit *CacTruong[5];
    const char *Ma[] = {"Mach", "HuyetAp", "NhipTho", "SpO2", "GCS"};
    const char *NhanTruong[] = {"Mạch (lần/phút)", "Huyết áp tâm thu (mmHg)", "Nhịp thở (lần/phút)",
                                "SpO₂ (%)", "GCS (3–15 điểm)"};
    for (int I = 0; I < 5; ++I)
        CacTruong[I] = Truong(Form, Ma[I], NhanTruong[I]);
    auto *Muc = new QComboBox;
    Muc->setObjectName("Muc");
    Muc->setAccessibleName("Mức điều dưỡng xác nhận");
    Muc->addItem("Chọn mức điều dưỡng xác nhận");
    for (int I = 1; I <= 5; ++I)
        Muc->addItem(QString("Mức %1%2").arg(I).arg(I == 1 ? " · ưu tiên cao nhất" : ""));
    Form->addRow("Mức xác nhận *", Muc);
    auto *LoiMuc = Nhan("", "error");
    LoiMuc->setObjectName("errMuc");
    Form->addRow(LoiMuc);
    auto *DeXuat = VanBan("Mức đề xuất theo luật: chưa tính.\n" +
                              QString(DichVu.LayTuoi(H) < 0 ? "Chưa rõ tuổi · đề xuất theo tuổi chưa đầy đủ."
                                                            : "Nhập sinh hiệu rồi bấm xem đề xuất."),
                          "notice");
    DeXuat->setObjectName("deXuat");
    K.NoiDung->addWidget(DeXuat);
    auto *Xem = Nut("Xem mức đề xuất", "xemDeXuat");
    K.NoiDung->addWidget(Xem);
    K.NoiDung->addWidget(VanBan("Luật C++ từ patient-class @95e145f, phục vụ mô phỏng đồ án. Điều dưỡng xác "
                                "nhận mức trước khi đưa vào hàng.",
                                "muted"));
    const auto DocSoDo = [&](Demo::SinhHieu &S) -> Demo::KetQua {
        int *DiaChi[] = {&S.Mach, &S.HuyetAp, &S.NhipTho, nullptr, &S.GCS};
        for (int I = 0; I < 5; ++I) {
            bool HopLe = false;
            const QString Chuoi = CacTruong[I]->text().trimmed();
            if (I == 3)
                S.SpO2 = QString(Chuoi).replace(',', '.').toDouble(&HopLe);
            else
                *DiaChi[I] = Chuoi.toInt(&HopLe);
            if (!HopLe)
                return {false, I == 3 ? "Nhập SpO₂ từ 0 đến 100%." : "Nhập một số nguyên hợp lệ.", Ma[I], -1};
        }
        return Demo::DemoTriageService::KiemTraSinhHieu(S);
    };
    connect(Xem, &QPushButton::clicked, &K.Hop, [&] {
        K.XoaLoi();
        Demo::SinhHieu S;
        const auto Q = DocSoDo(S);
        if (!Q.ThanhCong) {
            K.BaoLoi(Q);
            return;
        }
        const auto GoiY = Demo::DemoTriageService::DeXuatMuc(S, DichVu.LayTuoi(H));
        DeXuat->setText("Mức đề xuất theo luật: " +
                        QString(GoiY.Muc ? "Mức " + QString::number(GoiY.Muc) : "chưa xác định") + "\n" +
                        (GoiY.DayDu ? "Đã áp dụng luật dự án theo tuổi."
                                    : "Chưa đủ tuổi; chỉ áp dụng điều kiện GCS/SpO₂ toàn cục."));
    });
    for (auto *E : CacTruong)
        connect(E, &QLineEdit::textChanged, &K.Hop,
                [=] { DeXuat->setText("Sinh hiệu đã thay đổi · bấm xem mức đề xuất để tính lại."); });
    auto *Luu = K.ThemNutLuu(L.TinhTrang == Demo::TrangThai::SangLoc ? "Xác nhận & đưa vào hàng"
                                                                     : "Lưu đánh giá mới");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        K.XoaLoi();
        Demo::SinhHieu S;
        auto Q = DocSoDo(S);
        if (Q.ThanhCong)
            Q = DichVu.XacNhan(ChiSo, S, Muc->currentIndex());
        if (!Q.ThanhCong) {
            K.BaoLoi(Q);
            return;
        }
        K.Hop.accept();
    });
    if (K.Hop.exec() == QDialog::Accepted) {
        LamMoi();
        statusBar()->showMessage("Đã lưu đánh giá và cập nhật thứ tự hàng chờ.", 6000);
    }
}
void DemoWindow::MoChiTiet(int ChiSo) {
    // Mở lại phần trình bày sau mỗi thao tác; không giữ widget cũ trỏ vào dữ liệu.
    bool TiepTuc = true;
    while (TiepTuc) {
        const auto &L = DichVu.LayLuot(ChiSo);
        const auto &H = DichVu.LayHoSo(L.BenhNhan);
        int HanhDong = 0;
        KhungHop K(this, "Chi tiết lượt khám", Chu(L.Ma) + " · tiếp nhận " + Gio(L.TiepNhanLuc));
        K.Hop.setObjectName("chiTietDialog");
        K.NoiDung->addWidget(
            VanBan(TenBenh(H) + " · " + Chu(H.Ma) + "\n" + Demo::DemoTriageService::TenTrangThai(L.TinhTrang),
                   "notice"));
        auto *HoSoNhom = new QGroupBox("Hồ sơ bệnh nhân · thông tin lâu dài");
        auto *HoSoCot = new QVBoxLayout(HoSoNhom);
        HoSoCot->addWidget(VanBan(
            "Mã: " + Chu(H.Ma) + "\nDanh tính: " + TenBenh(H) + "\nNgày sinh / tuổi: " +
            (H.CoNgaySinh
                 ? QDate(H.NgaySinh.Nam, H.NgaySinh.Thang, H.NgaySinh.Ngay).toString("dd/MM/yyyy") + " · "
                 : "") +
            TuoiBenh(DichVu, H) + "\nGiới tính: " + Chu(H.GioiTinh) +
            "\nCăn cước: " + Chu(H.CanCuoc.empty() ? "Chưa ghi nhận" : H.CanCuoc) +
            "\nDị ứng: " + Chu(H.DiUng.empty() ? "Chưa ghi nhận" : H.DiUng) +
            "\nTiền sử: " + Chu(H.TienSu.empty() ? "Chưa ghi nhận" : H.TienSu)));
        auto *Sua = Nut("Bổ sung / sửa danh tính", "suaHoSo");
        HoSoCot->addWidget(Sua);
        connect(Sua, &QPushButton::clicked, &K.Hop, [&] {
            HanhDong = 1;
            K.Hop.accept();
        });
        K.NoiDung->addWidget(HoSoNhom);
        auto *LuotNhom = new QGroupBox("Lượt khám · " + Chu(L.Ma));
        auto *LuotCot = new QVBoxLayout(LuotNhom);
        LuotCot->addWidget(VanBan("Triệu chứng: " + Chu(L.TrieuChung) +
                                  "\nTrạng thái: " + Demo::DemoTriageService::TenTrangThai(L.TinhTrang)));
        LuotCot->addWidget(HuyHieu(L.Muc));
        if (DichVu.CanDanhGia(ChiSo))
            LuotCot->addWidget(VanBan("Cần tái đánh giá · chờ lâu không tự nâng mức.", "error"));
        K.NoiDung->addWidget(LuotNhom);
        const auto ThemHanhDong = [&](const QString &Ten, const char *Ma, int MaHanhDong) {
            auto *N = Nut(Ten, Ma, MaHanhDong == 2);
            K.NoiDung->addWidget(N);
            connect(N, &QPushButton::clicked, &K.Hop, [&, MaHanhDong] {
                HanhDong = MaHanhDong;
                K.Hop.accept();
            });
        };
        if (!Demo::DemoTriageService::DaDong(L)) {
            ThemHanhDong(L.SoDanhGia ? "Đánh giá lại" : "Phân loại ban đầu", "danhGia", 2);
            if (L.TinhTrang == Demo::TrangThai::DangKham) {
                ThemHanhDong("Chuyển cận lâm sàng", "chuyenCls", 3);
                ThemHanhDong("Kết thúc lượt khám", "ketThuc", 5);
            }
            if (L.TinhTrang == Demo::TrangThai::CanLamSang)
                ThemHanhDong("Ghi nhận kết quả & quay lại", "quayLai", 4);
            ThemHanhDong("Hủy lượt khám", "huyLuot", 6);
        }
        if (L.SoCanLamSang) {
            K.NoiDung->addWidget(Nhan("Cận lâm sàng", "section"));
            for (int I = 0; I < L.SoCanLamSang; ++I) {
                const auto &C = L.CacLanCanLamSang[I];
                K.NoiDung->addWidget(
                    VanBan(Gio(C.Luc) + " · " + Chu(C.ChiDinh) + "\n" +
                               (C.TraLuc < 0 ? "Chờ kết quả" : Gio(C.TraLuc) + " · " + Chu(C.KetQua)),
                           "notice"));
            }
        }
        K.NoiDung->addWidget(Nhan("Sinh hiệu & lịch sử đánh giá", "section"));
        if (!L.SoDanhGia)
            K.NoiDung->addWidget(VanBan("Chưa có sinh hiệu. Nhập số đo khi phân loại ban đầu.", "muted"));
        for (int I = L.SoDanhGia - 1; I >= 0; --I) {
            const auto &B = L.LichSu[I];
            auto *Nhom = new QGroupBox(Gio(B.Luc) + " · " + Chu(B.Nguoi));
            auto *Cot = new QVBoxLayout(Nhom);
            const auto S = B.SoDo;
            Cot->addWidget(VanBan(QString("Mạch: %1 lần/phút  ·  HA tâm thu: %2 mmHg\nNhịp thở: %3 lần/phút  "
                                          "·  SpO₂: %4%  ·  GCS: %5")
                                      .arg(S.Mach)
                                      .arg(S.HuyetAp)
                                      .arg(S.NhipTho)
                                      .arg(S.SpO2)
                                      .arg(S.GCS)));
            Cot->addWidget(
                VanBan(QString("Tuổi đã dùng: %1 · nguồn: %2\nMức đề xuất: %3%4 · Mức xác nhận: %5")
                           .arg(B.Tuoi < 0 ? "Chưa rõ tuổi" : QString::number(B.Tuoi))
                           .arg(B.TuNgaySinh ? "ngày sinh"
                                : B.Tuoi < 0 ? "chưa biết"
                                             : "ước lượng")
                           .arg(B.GoiY.Muc ? QString::number(B.GoiY.Muc) : "chưa xác định")
                           .arg(B.GoiY.DayDu ? "" : " (chưa đầy đủ)")
                           .arg(B.Muc),
                       "muted"));
            if (B.DuLieuMau)
                Cot->addWidget(Nhan("Bản ghi dữ liệu hư cấu để thử luồng.", "muted"));
            K.NoiDung->addWidget(Nhom);
        }
        K.NoiDung->addWidget(Nhan("Diễn biến lượt khám", "section"));
        for (int I = L.SoMoc - 1; I >= 0; --I)
            K.NoiDung->addWidget(
                VanBan("●  " + Gio(L.CacMoc[I].Luc) + " · " + Chu(L.CacMoc[I].NoiDung), "muted"));
        auto *Dong = Nut("Đóng", "dongChiTiet");
        K.Chan->addWidget(Dong);
        connect(Dong, &QPushButton::clicked, &K.Hop, &QDialog::reject);
        if (K.Hop.exec() != QDialog::Accepted || !HanhDong)
            break;
        switch (HanhDong) {
        case 1:
            MoHoSo(L.BenhNhan, ChiSo);
            break;
        case 2:
            MoDanhGia(ChiSo);
            break;
        case 3:
            MoCanLamSang(ChiSo, false);
            break;
        case 4:
            MoCanLamSang(ChiSo, true);
            break;
        case 5:
            XacNhanDong(ChiSo, false);
            break;
        case 6:
            XacNhanDong(ChiSo, true);
            break;
        default:
            TiepTuc = false;
        }
    }
}
void DemoWindow::MoHoSo(int ChiSo, int Luot) {
    const auto &H = DichVu.LayHoSo(ChiSo);
    KhungHop K(this, "Hồ sơ bệnh nhân", Chu(H.Ma) + " · Mã và liên kết lượt khám được giữ nguyên.");
    K.Hop.setObjectName("hoSoDialog");
    FormHoSo Form(H);
    K.NoiDung->addWidget(Form.Khung);
    int MoLuot = -1;
    if (Luot < 0) {
        K.NoiDung->addWidget(Nhan("Lượt khám liên kết", "section"));
        for (int I = 0; I < DichVu.LaySoLuot(); ++I) {
            const auto &L = DichVu.LayLuot(I);
            if (L.BenhNhan != ChiSo)
                continue;
            auto *N = Nut(Chu(L.Ma) + " · " + Demo::DemoTriageService::TenTrangThai(L.TinhTrang),
                          qPrintable(QString("hoSoLuot%1").arg(I)));
            K.NoiDung->addWidget(N);
            connect(N, &QPushButton::clicked, &K.Hop, [&, I] {
                MoLuot = I;
                K.Hop.accept();
            });
        }
    }
    auto *Luu = K.ThemNutLuu("Lưu hồ sơ");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        K.XoaLoi();
        Demo::HoSo Moi;
        auto Q = Form.Doc(Moi);
        if (Q.ThanhCong)
            Q = DichVu.CapNhatHoSo(ChiSo, Moi);
        if (!Q.ThanhCong) {
            K.BaoLoi(Q);
            return;
        }
        K.Hop.accept();
        statusBar()->showMessage("Đã cập nhật hồ sơ, giữ nguyên mã bệnh nhân.", 6000);
    });
    if (K.Hop.exec() == QDialog::Accepted) {
        LamMoi();
        if (MoLuot >= 0)
            MoChiTiet(MoLuot);
    }
}
void DemoWindow::MoCanLamSang(int ChiSo, bool QuayLai) {
    const auto &L = DichVu.LayLuot(ChiSo);
    KhungHop K(this, QuayLai ? "Ghi nhận kết quả & quay lại" : "Chuyển cận lâm sàng",
               TenBenh(DichVu.LayHoSo(L.BenhNhan)) + " · " + Chu(L.Ma));
    K.Hop.resize(qMin(660, width() - 24), 460);
    if (QuayLai)
        K.NoiDung->addWidget(VanBan("Giữ cùng lượt " + Chu(L.Ma) +
                                        ". Lần vào hàng quay lại mới bắt đầu lúc " + Gio(DichVu.LayBayGio()) +
                                        ".",
                                    "notice"));
    auto *NoiDung = NoiDungDai(K, "NoiDung", QuayLai ? "Kết quả cận lâm sàng *" : "Nội dung chỉ định *");
    auto *Luu = K.ThemNutLuu(QuayLai ? "Lưu & vào hàng quay lại" : "Xác nhận chuyển");
    connect(Luu, &QPushButton::clicked, &K.Hop, [&] {
        K.XoaLoi();
        const auto Q = QuayLai
                           ? DichVu.QuayLai(ChiSo, NoiDung->toPlainText().trimmed().toStdString())
                           : DichVu.ChuyenCanLamSang(ChiSo, NoiDung->toPlainText().trimmed().toStdString());
        if (!Q.ThanhCong) {
            K.BaoLoi(Q);
            return;
        }
        K.Hop.accept();
    });
    if (K.Hop.exec() == QDialog::Accepted) {
        LamMoi();
        statusBar()->showMessage(QuayLai ? "Đã ghi kết quả và vào hàng quay lại." : "Đã chuyển cận lâm sàng.",
                                 6000);
    }
}
void DemoWindow::XacNhanDong(int ChiSo, bool Huy) {
    const auto &L = DichVu.LayLuot(ChiSo);
    QMessageBox Hop(this);
    Hop.setWindowTitle(Huy ? "Hủy lượt khám" : "Kết thúc lượt khám");
    Hop.setTextFormat(Qt::PlainText);
    Hop.setText(QString("Xác nhận %1 lượt %2 của %3?\nLượt sẽ đóng và rời hàng chờ.")
                    .arg(Huy ? "hủy" : "kết thúc", Chu(L.Ma), TenBenh(DichVu.LayHoSo(L.BenhNhan))));
    auto *XacNhan = Hop.addButton(Huy ? "Xác nhận hủy" : "Xác nhận kết thúc", QMessageBox::AcceptRole);
    auto *QuayLai = Hop.addButton("Quay lại", QMessageBox::RejectRole);
    Hop.setDefaultButton(QuayLai);
    Hop.exec();
    if (Hop.clickedButton() != XacNhan)
        return;
    const auto Q = DichVu.DongLuot(ChiSo, Huy);
    LamMoi();
    statusBar()->showMessage(Q.ThanhCong ? "Đã đóng lượt " + Chu(L.Ma) : Chu(Q.Loi), 6000);
}
