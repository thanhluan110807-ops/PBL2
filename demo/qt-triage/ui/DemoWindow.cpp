#include <QAbstractItemView>
#include <QBoxLayout>
#include <QWidget>
#include <QSizePolicy>
#include <QTableWidgetItem>
#include <QChar>
#include <QString>
#include <QVariant>
#include "ui/DemoWindow.h"
#include "ui/DemoStyle.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "model/Date.h"
#include "dsa/Vector.h"
#include "util/My_stdexcept.h"
#include <ctime>
#include <string>
#include <QDate>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QColor>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>
using namespace DemoUi;

DemoWindow::DemoWindow(PatientRegistry &Registry, TriageService &DichVu,
                       VisitSearchService &TraCuu, QWidget *Cha)
    : QMainWindow(Cha), Registry(Registry), DichVu(DichVu), TraCuu(TraCuu) {
    setWindowTitle("An Tâm · Điều phối cấp cứu · Demo Qt");
    setWindowIcon(BieuTuong("pulse", QColor("#7050F2")));
    setStyleSheet(Qss());
    resize(1440, 980);
    setMinimumSize(480, 600);
    TaoGiaoDien();
    LamMoi();
}
void DemoWindow::TaoThanhBen(QHBoxLayout *Khung) {
    ThanhBen = new QFrame;
    ThanhBen->setObjectName("sidebar");
    ThanhBen->setFixedWidth(212);
    auto *Cot = new QVBoxLayout(ThanhBen);
    Cot->setContentsMargins(16, 28, 16, 20);
    Cot->setSpacing(8);
    auto *Logo = Nhan("✚  An Tâm.", "brand");
    Logo->setStyleSheet("color:#6241C6;");
    Cot->addWidget(Logo);
    Cot->addWidget(Nhan("ĐIỀU PHỐI CẤP CỨU", "muted"));
    Cot->addSpacing(32);
    Cot->addWidget(Nhan("KHÔNG GIAN LÀM VIỆC", "muted"));
    Cot->addSpacing(8);
    const char *NhanNut[] = {"Tổng quan", "Tiếp nhận", "Hàng chờ", "Lượt khám", "Hồ sơ bệnh nhân"};
    const char *Icon[] = {"grid", "plus", "list", "list", "users"};
    auto *Nhom = new QButtonGroup(this);
    Nhom->setExclusive(true);
    for (int ViTri = 0; ViTri < 5; ++ViTri) {
        auto *NutMoi = Nut(NhanNut[ViTri], qPrintable(QString("nav%1").arg(ViTri)));
        NutMoi->setProperty("nav", true);
        NutMoi->setCheckable(true);
        NutMoi->setIcon(BieuTuong(Icon[ViTri]));
        Nhom->addButton(NutMoi);
        DieuHuong[ViTri] = NutMoi;
        Cot->addWidget(NutMoi);
        connect(NutMoi, &QPushButton::clicked, this, [this, ViTri] { ChuyenTrang(ViTri); });
    }
    Cot->addStretch();
    Cot->addWidget(Nhan("●  Khoa Cấp cứu", "section"));
    auto *Lich = Nut("Phân lịch trực · sắp có", "lichTruc");
    Lich->setEnabled(false);
    Cot->addWidget(Lich);
    Cot->addSpacing(24);


    Khung->addWidget(ThanhBen);
}
void DemoWindow::TaoGiaoDien() {
    auto *Goc = new QWidget;
    setCentralWidget(Goc);
    auto *Khung = new QHBoxLayout(Goc);
    Khung->setContentsMargins(0, 0, 0, 0);
    Khung->setSpacing(0);
    TaoThanhBen(Khung);
    auto *Cot = new QVBoxLayout;
    Cot->setContentsMargins(0, 0, 0, 0);
    Cot->setSpacing(0);
    Khung->addLayout(Cot, 1);
    auto *Dau = new QFrame;
    Dau->setObjectName("header");
    Dau->setMinimumHeight(72);
    auto *HangDau = new QHBoxLayout(Dau);
    HangDau->setContentsMargins(24, 12, 24, 12);
    auto *Menu = Nut("☰", "menu");
    Menu->setToolTip("Ẩn / hiện điều hướng");
    Menu->setAccessibleName("Ẩn hoặc hiện điều hướng");
    Menu->setFixedWidth(46);
    HangDau->addWidget(Menu);
    connect(Menu, &QPushButton::clicked, this, [this] {
        if (!Hep) {
            ThanhBen->setVisible(!ThanhBen->isVisible());
            return;
        }
        QDialog Hop(this);
        Hop.setWindowTitle("Điều hướng");
        auto *CotMenu = new QVBoxLayout(&Hop);
        int Dich = -1;
        for (int I = 0; I < 5; ++I) {
            auto *N = Nut(DieuHuong[I]->text(), "chonTrang");
            CotMenu->addWidget(N);
            connect(N, &QPushButton::clicked, &Hop, [&, I] {
                Dich = I;
                Hop.accept();
            });
        }
        Hop.exec();
        if (Dich >= 0)
            ChuyenTrang(Dich);
    });
    KhuVuc = Nhan("Tổng quan", "muted");
    HangDau->addWidget(KhuVuc);
    HangDau->addStretch();
    TimChung = new QLineEdit;
    TimChung->setObjectName("timChung");
    TimChung->setPlaceholderText("Tên bệnh nhân để tra cứu lượt khám…");
    TimChung->setAccessibleName("Tên bệnh nhân để tra cứu lượt khám");
    TimChung->setMaximumWidth(330);
    HangDau->addWidget(TimChung, 1);
    HangDau->addWidget(Nhan("● Dữ liệu phiên này", "status"));
    Cot->addWidget(Dau);
    auto *Cuon = new QScrollArea;
    Cuon->setObjectName("mainScroll");
    Cuon->setWidgetResizable(true);
    auto *NoiDung = new QWidget;
    NoiDung->setObjectName("content");
    auto *BoCuc = new QVBoxLayout(NoiDung);
    BoCuc->setContentsMargins(24, 24, 24, 20);
    BoCuc->setSpacing(16);
    TaoNoiDung(BoCuc);
    Cuon->setWidget(NoiDung);
    Cot->addWidget(Cuon, 1);
    connect(TimChung, &QLineEdit::textChanged, this, [this] {
        if (!TimChung->text().isEmpty()) {
            Trang = 3;
            LocKpi = -1;
        }
        LamMoi();
    });
}
void DemoWindow::TaoNoiDung(QVBoxLayout *Khung) {
    auto *Dau = new QHBoxLayout;
    HangDauTrang = Dau;
    auto *TieuDeCot = new QVBoxLayout;
    TieuDeCot->addWidget(Nhan("KHOA CẤP CỨU / KHÔNG GIAN ĐIỀU PHỐI", "muted"));
    TieuDe = Nhan("Điều phối cấp cứu", "title");
    TieuDeCot->addWidget(TieuDe);
    MoTa = Nhan("Theo dõi tiếp nhận, hàng chờ và diễn biến từng lượt khám.", "muted");
    MoTa->setWordWrap(true);
    TieuDeCot->addWidget(MoTa);
    Dau->addLayout(TieuDeCot, 1);
    auto *TiepNhan = Nut("Tiếp nhận bệnh nhân", "tiepNhan", true);
    TiepNhan->setIcon(BieuTuong("plus", Qt::white));
    Dau->addWidget(TiepNhan);
    connect(TiepNhan, &QPushButton::clicked, this, &DemoWindow::MoTiepNhan);
    Khung->addLayout(Dau);
    LuoiSoLieu = new QGridLayout;
    LuoiSoLieu->setSpacing(14);
    const char *CacNhan[] = {"Chờ đánh giá", "Chờ khám lần đầu", "Chờ khám quay lại", "Đang khám"};
    for (int ViTri = 0; ViTri < 4; ++ViTri) {
        auto *The = new QFrame;
        The->setObjectName("card");
        auto *Cot = new QVBoxLayout(The);
        Cot->setContentsMargins(16, 14, 16, 14);
        Cot->addWidget(Nhan(CacNhan[ViTri]));
        auto *Duoi = new QFrame;
        Duoi->setObjectName("subtle");
        auto *Hang = new QHBoxLayout(Duoi);
        SoLieu[ViTri] = Nhan("00", "number");
        SoLieu[ViTri]->setObjectName(QString("kpi%1").arg(ViTri));
        Hang->addWidget(SoLieu[ViTri]);
        Hang->addStretch();
        auto *Xem = Nut("Xem lượt", qPrintable(QString("xemKpi%1").arg(ViTri)));
        Xem->setProperty("link", true);
        Hang->addWidget(Xem);
        Cot->addWidget(Duoi);
        TheSoLieu[ViTri] = The;
        LuoiSoLieu->addWidget(The, 0, ViTri);
        connect(Xem, &QPushButton::clicked, this, [this, ViTri] {
            Trang = 3;
            LocKpi = ViTri;
            // KPI mở toàn bộ nhóm; xóa điều kiện của lần tra cứu trước.
            TimChung->clear();
            TimHang->clear();
            MaLanKhamLoc->clear();
            NgayLoc->clear();
            LamMoi();
        });
    }
    Khung->addLayout(LuoiSoLieu);
    auto *LuuY = Nhan("Dữ liệu chỉ ở trong phiên này. Chưa lưu thay đổi khi thoát; CSV gốc không bị sửa.", "muted");
    LuuY->setWordWrap(true);
    Khung->addWidget(LuuY);
    auto *Panel = new QFrame;
    Panel->setObjectName("panel");
    auto *CotPanel = new QVBoxLayout(Panel);
    CotPanel->setContentsMargins(18, 18, 18, 14);
    CotPanel->setSpacing(14);
    auto *HangTieuDe = new QHBoxLayout;
    TenPanel = Nhan("Hàng chờ khám", "section");
    HangTieuDe->addWidget(TenPanel);
    HangTieuDe->addStretch();
    MaBacSi = new QLineEdit;
    MaBacSi->setObjectName("maBacSi");
    MaBacSi->setPlaceholderText("Mã bác sĩ gọi khám");
    MaBacSi->setAccessibleName("Mã bác sĩ gọi khám");
    MaBacSi->setMaximumWidth(180);
    HangTieuDe->addWidget(MaBacSi);
    GoiLuot = Nut("Gọi lượt tiếp theo", "goiTiep", true);
    GoiLuot->setIcon(BieuTuong("arrow", Qt::white));
    HangTieuDe->addWidget(GoiLuot);
    connect(GoiLuot, &QPushButton::clicked, this, &DemoWindow::GoiTiep);
    CotPanel->addLayout(HangTieuDe);
    HuongDan = Nhan("Gọi theo toàn bộ hàng chờ hiệu lực, không theo bộ lọc.", "muted");
    HuongDan->setWordWrap(true);
    CotPanel->addWidget(HuongDan);
    auto *ThanhLoc = new QHBoxLayout;
    HangBoLoc = ThanhLoc;
    auto *NhomLuong = new QWidget;
    auto *HangLuong = new QHBoxLayout(NhomLuong);
    HangLuong->setContentsMargins(0, 0, 0, 0);
    ThanhLoc->addWidget(NhomLuong);
    auto *NhomLoc = new QWidget;
    auto *HangLoc = new QHBoxLayout(NhomLoc);
    HangLoc->setContentsMargins(0, 0, 0, 0);
    const char *NhanLoc[] = {"Tất cả", "Lần đầu", "Quay lại"};
    auto *Nhom = new QButtonGroup(this);
    Nhom->setExclusive(true);
    for (int ViTri = 0; ViTri < 3; ++ViTri) {
        LocLuong[ViTri] = Nut(NhanLoc[ViTri], qPrintable(QString("locLuong%1").arg(ViTri)));
        LocLuong[ViTri]->setCheckable(true);
        LocLuong[ViTri]->setProperty("tab", true);
        Nhom->addButton(LocLuong[ViTri]);
        HangLuong->addWidget(LocLuong[ViTri]);
        connect(LocLuong[ViTri], &QPushButton::clicked, this, [this, ViTri] {
            Luong = ViTri;
            DoDuLieu();
        });
    }
    LocLuong[0]->setChecked(true);
    ThanhLoc->addStretch();
    LocMuc = new QComboBox;
    LocMuc->setObjectName("locMuc");
    LocMuc->setAccessibleName("Lọc mức cấp cứu");
    LocMuc->addItem("Tất cả mức");
    for (int Muc = 1; Muc <= 5; ++Muc)
        LocMuc->addItem(QString("Mức %1").arg(Muc));
    HangLoc->addWidget(LocMuc);
    LocTrangThai = new QComboBox;
    LocTrangThai->setObjectName("locTrangThai");
    LocTrangThai->setAccessibleName("Lọc trạng thái lượt khám");
    LocTrangThai->addItem("Tất cả trạng thái");
    LocTrangThai->addItems({"Chờ đánh giá", "Chờ khám lần đầu", "Chờ khám quay lại",
                            "Đang khám", "Chờ cận lâm sàng", "Đã kết thúc", "Đã hủy"});
    HangLoc->addWidget(LocTrangThai);
    TimHang = new QLineEdit;
    TimHang->setObjectName("timHang");
    TimHang->setPlaceholderText("Tìm tên hoặc mã…");
    TimHang->setAccessibleName("Tìm trong danh sách");
    TimHang->setMaximumWidth(230);
    HangLoc->addWidget(TimHang, 1);
    ThanhLoc->addWidget(NhomLoc);
    CotPanel->addLayout(ThanhLoc);
    connect(TimHang, &QLineEdit::textChanged, this, [this] { DoDuLieu(); });
    connect(LocMuc, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] { DoDuLieu(); });
    connect(LocTrangThai, qOverload<int>(&QComboBox::currentIndexChanged), this, [this] {
        LocKpi = -1;
        DoDuLieu();
    });
    MaLanKhamLoc = new QLineEdit;
    MaLanKhamLoc->setObjectName("maLanKhamLoc");
    MaLanKhamLoc->setPlaceholderText("Mã lần khám (khớp chính xác)");
    MaLanKhamLoc->setAccessibleName("Mã lần khám để tra cứu");
    NgayLoc = new QLineEdit;
    NgayLoc->setObjectName("ngayLoc");
    NgayLoc->setPlaceholderText("Ngày tiếp nhận dd/MM/yyyy · UTC+7 (để trống: tất cả)");
    NgayLoc->setAccessibleName("Ngày tiếp nhận UTC+7");
    CotPanel->addWidget(MaLanKhamLoc);
    CotPanel->addWidget(NgayLoc);
    connect(MaLanKhamLoc, &QLineEdit::textChanged, this, [this] { DoDuLieu(); });
    connect(NgayLoc, &QLineEdit::textChanged, this, [this] { DoDuLieu(); });
    VungDanhSach = new QStackedWidget;
    VungDanhSach->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    Bang = new QTableWidget;
    Bang->setObjectName("hangCho");
    Bang->setColumnCount(7);
    Bang->setHorizontalHeaderLabels({"STT", "Bệnh nhân / Lượt khám", "Cấp cứu", "Luồng / Trạng thái",
                                     "Tiếp nhận (UTC+7)", "Đánh giá gần nhất", "Thao tác"});
    Bang->verticalHeader()->hide();
    Bang->setShowGrid(false);
    Bang->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Bang->setSelectionBehavior(QAbstractItemView::SelectRows);
    Bang->setSelectionMode(QAbstractItemView::SingleSelection);
    Bang->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    Bang->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    Bang->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    Bang->setColumnWidth(2, 105);
    Bang->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    Bang->setColumnWidth(6, 115);
    Bang->setMinimumHeight(390);
    VungDanhSach->addWidget(Bang);
    auto *DanhSachThe = new QWidget;
    CacThe = new QVBoxLayout(DanhSachThe);
    CacThe->setContentsMargins(0, 0, 0, 0);
    VungDanhSach->addWidget(DanhSachThe);
    CotPanel->addWidget(VungDanhSach, 1);
    SoKetQua = Nhan("", "muted");
    SoKetQua->setObjectName("soKetQua");
    SoKetQua->setWordWrap(true);
    CotPanel->addWidget(SoKetQua);
    Khung->addWidget(Panel, 1);
    auto *GhiChu = Nhan(
        "Mức 1 ưu tiên cao nhất · Hai đầu hàng cùng mức: luân phiên, lần hòa đầu ưu tiên quay lại. Mức cao hơn luôn được chọn trước.",
        "muted");
    GhiChu->setWordWrap(true);
    Khung->addWidget(GhiChu);
    DongHo = Nhan("Thời điểm hiển thị theo UTC+7 · Chưa liên kết danh sách nhân viên.", "muted");
    DongHo->setWordWrap(true);
    Khung->addWidget(DongHo);
}

void DemoWindow::ChuyenTrang(int ChiSo) {
    Trang = ChiSo;
    LocKpi = -1;
    TimChung->clear();
    TimHang->clear();
    MaLanKhamLoc->clear();
    NgayLoc->clear();
    LocTrangThai->setCurrentIndex(0);
    LamMoi();
}
void DemoWindow::LamMoi() {
    int Mang[4]{};
    const auto TatCa = TraCuu.TimTheoBenhNhan();
    for (int I = 0; I < TatCa.KichThuoc(); ++I) {
        const Visit &V = *TatCa[I].LanKham;
        if (V.LayTrangThai() == VisitStatus::ChoKhamLanDau) ++Mang[V.SoDanhGia() ? 1 : 0];
        if (V.LayTrangThai() == VisitStatus::ChoKhamQuayLai) ++Mang[2];
        if (V.LayTrangThai() == VisitStatus::DangKham) ++Mang[3];
    }
    for (int ViTri = 0; ViTri < 4; ++ViTri)
        SoLieu[ViTri]->setText(QString("%1").arg(Mang[ViTri], 2, 10, QChar('0')));
    const char *Ten[] = {"Tổng quan", "Tiếp nhận", "Hàng chờ", "Lượt khám", "Hồ sơ bệnh nhân"};
    KhuVuc->setText(Ten[Trang]);
    DieuHuong[Trang]->setChecked(true);
    TieuDe->setText(Trang == 0 || Trang == 2 ? "Điều phối cấp cứu" : Ten[Trang]);
    TenPanel->setText(Trang == 0 || Trang == 2 ? "Hàng chờ khám"
                      : Trang == 1             ? "Chờ sàng lọc"
                      : Trang == 3             ? "Danh sách lượt khám"
                                               : "Hồ sơ bệnh nhân");
    const bool LaHang = Trang == 0 || Trang == 2;
    for (auto *NutLoc : LocLuong)
        NutLoc->setVisible(LaHang);
    LocMuc->setVisible(LaHang);
    LocTrangThai->setVisible(Trang == 3);
    GoiLuot->setVisible(LaHang);
    GoiLuot->setEnabled(DichVu.SoDangCho() > 0);
    MaBacSi->setVisible(LaHang);
    MaLanKhamLoc->setVisible(Trang == 3);
    NgayLoc->setVisible(Trang == 3);
    HuongDan->setText(LaHang       ? "Gọi theo toàn bộ hàng chờ hiệu lực, không theo bộ lọc."
                      : Trang == 1 ? "Xác nhận phân loại trước khi đưa lượt vào hàng chờ khám."
                      : Trang == 4 ? "Hồ sơ lâu dài giữ mã ổn định, liên kết với các lượt khám."
                                   : "Xem diễn biến và thực hiện thao tác hợp lệ theo trạng thái.");
    DoDuLieu();
}
void DemoWindow::DoDuLieu() {
    Bang->setRowCount(0);
    while (auto *Muc = CacThe->takeAt(0)) {
        delete Muc->widget();
        delete Muc;
    }
    const bool LaHang = Trang == 0 || Trang == 2;
    const auto Hang = DichVu.LayDanhSachCho();
    const auto HoSo = Registry.LayDanhSachBenhNhan();
    Date Ngay{};
    const Date *NgayCanTim = nullptr;
    if (Trang == 3 && !NgayLoc->text().trimmed().isEmpty()) {
        const QDate D = QDate::fromString(NgayLoc->text().trimmed(), "dd/MM/yyyy");
        if (!D.isValid()) {
            SoKetQua->setText("Ngày không hợp lệ. Nhập dd/MM/yyyy hoặc để trống.");
            VungDanhSach->setCurrentIndex(0);
            return;
        }
        Ngay = {D.day(), D.month(), D.year()}; NgayCanTim = &Ngay;
    }
    const auto CacLuot = TraCuu.TimTheoBenhNhan(
        Trang == 3 ? TimChung->text().trimmed().toStdString() : "", NgayCanTim,
        Trang == 3 ? MaLanKhamLoc->text().trimmed().toStdString() : "");
    int SoHien = 0;
    const int Tong = LaHang ? Hang.KichThuoc() : Trang == 4 ? HoSo.KichThuoc() : CacLuot.KichThuoc();
    const QString CacTrangThai[] = {"Chờ đánh giá", "Chờ khám lần đầu", "Chờ khám quay lại",
                                    "Đang khám", "Chờ cận lâm sàng", "Đã kết thúc", "Đã hủy"};
    for (int Thu = 0; Thu < Tong; ++Thu) {
        const Visit *Luot = Trang == 4 ? nullptr : LaHang ? Hang[Thu] : CacLuot[Thu].LanKham;
        const Patient &BenhNhan = *(Luot ? Registry.TimBenhNhan(Luot->LayMaBenhNhan()) : HoSo[Thu]);
        const QString Ten = BenhNhan.LayHoTen().empty() ? "Chưa rõ danh tính" : Chu(BenhNhan.LayHoTen());
        const std::string MaBenhNhan = BenhNhan.LayMaDinhDanh();
        const std::string MaKham = Luot ? Luot->LayMaLanKham() : "";
        const QString Tim = ChuanHoa(Ten + " " + Chu(MaBenhNhan) + " " + Chu(MaKham) + " " + Chu(BenhNhan.LaySoCanCuoc()));
        if (!Tim.contains(ChuanHoa(TimHang->text()))) continue;
        const int Muc = Luot && Luot->SoDanhGia() ? Luot->LayDanhGiaGanNhat().LayMuc() : 0;
        if (Luot) {
            const auto TrangThai = Luot->LayTrangThai();
            if (Trang == 1 && (TrangThai != VisitStatus::ChoKhamLanDau || Luot->SoDanhGia())) continue;
            if (LaHang && ((Luong == 1 && TrangThai != VisitStatus::ChoKhamLanDau) ||
                           (Luong == 2 && TrangThai != VisitStatus::ChoKhamQuayLai) ||
                           (LocMuc->currentIndex() && Muc != LocMuc->currentIndex()))) continue;
            const int Loc = LocKpi >= 0 ? LocKpi : LocTrangThai->currentIndex() - 1;
            if (Trang == 3 && Loc >= 0 && TenTrangThai(*Luot) != CacTrangThai[Loc]) continue;
        }
        const int Dong = SoHien++;
        Bang->insertRow(Dong);
        Bang->setRowHeight(Dong, 74);
        const QString MaLuot = Luot ? Chu(MaKham) : "Hồ sơ lâu dài";
        auto *TenO = new QTableWidgetItem(Ten + "\n" + Chu(MaBenhNhan) + " · " + MaLuot);
        TenO->setData(Qt::UserRole, Luot ? Chu(MaKham) : Chu(MaBenhNhan));
        Bang->setItem(Dong, 1, TenO);
        Bang->setItem(Dong, 0, new QTableWidgetItem(QString::number(Thu + 1)));
        auto *MucO = new QWidget;
        auto *BoCuc = new QHBoxLayout(MucO);
        BoCuc->setContentsMargins(6, 16, 6, 16);
        BoCuc->addWidget(HuyHieu(Muc));
        Bang->setCellWidget(Dong, 2, MucO);
        const QString Loai = Luot ? TenTrangThai(*Luot) : "Hồ sơ bệnh nhân";
        const QString Cho = Luot ? ThoiDiem(Luot->LayThoiDiemTiepNhan()) : "—";
        const QString DanhGia = Luot && Luot->SoDanhGia()
            ? ThoiDiem(Luot->LayDanhGiaGanNhat().LayThoiDiemDanhGia()) : "Chưa đánh giá";
        Bang->setItem(Dong, 3, new QTableWidgetItem(Loai));
        Bang->setItem(Dong, 4, new QTableWidgetItem(Cho));
        Bang->setItem(Dong, 5, new QTableWidgetItem(DanhGia));
        // Giữ mã ổn định trong callback, không giữ chỉ số danh sách đang lọc.
        const auto Mo = [this, MaKham, MaBenhNhan] {
            if (MaKham.empty()) MoHoSo(MaBenhNhan);
            else MoChiTiet(MaKham);
        };
        auto *ChiTiet = Nut("Chi tiết", qPrintable(QString("chiTiet%1").arg(Thu)));
        ChiTiet->setProperty("link", true);
        ChiTiet->setAccessibleName("Chi tiết " + Ten + " " + MaLuot);
        connect(ChiTiet, &QPushButton::clicked, this, Mo);
        auto *NutO = new QWidget;
        auto *NutHang = new QHBoxLayout(NutO);
        NutHang->setContentsMargins(5, 14, 5, 14);
        NutHang->addWidget(ChiTiet);
        Bang->setCellWidget(Dong, 6, NutO);
        auto *The = new QFrame;
        The->setObjectName("card");
        auto *Cot = new QVBoxLayout(The);
        auto *Dau = new QHBoxLayout;
        auto *TenNhan = Nhan(Ten);
        TenNhan->setWordWrap(true);
        Dau->addWidget(TenNhan, 1);
        if (Luot)
            Dau->addWidget(HuyHieu(Muc));
        Cot->addLayout(Dau);
        Cot->addWidget(Nhan(Chu(MaBenhNhan) + " · " + MaLuot, "muted"));
        Cot->addWidget(Nhan(Luot ? Loai + " · " + Cho : Loai));
        auto *Duoi = new QHBoxLayout;
        auto *N = Nhan(Luot ? DanhGia : "Dị ứng: " + Chu(BenhNhan.LayThongTinDiUng().empty() ? "Chưa ghi nhận" : BenhNhan.LayThongTinDiUng()),
                       "muted");
        N->setWordWrap(true);
        Duoi->addWidget(N, 1);
        auto *Xem = Nut("Chi tiết", qPrintable(QString("theChiTiet%1").arg(Thu)));
        Xem->setProperty("link", true);
        connect(Xem, &QPushButton::clicked, this, Mo);
        Duoi->addWidget(Xem);
        Cot->addLayout(Duoi);
        CacThe->addWidget(The);
    }
    if (!SoHien) {
        auto *Rong = Nhan(Tong ? "Không tìm thấy kết quả phù hợp.\nThử đổi bộ lọc hoặc từ khóa."
                               : "Danh sách đang trống.\nLượt mới xuất hiện sau bước tiếp nhận / phân loại.",
                          "muted");
        Rong->setAlignment(Qt::AlignCenter);
        Rong->setMinimumHeight(160);
        CacThe->addWidget(Rong);
    }
    CacThe->addStretch();
    VungDanhSach->setCurrentIndex(Hep || !SoHien || Trang == 4 ? 1 : 0);
    VungDanhSach->setMinimumHeight(Hep || Trang == 4 ? qMax(220, CacThe->sizeHint().height()) : 390);
    SoKetQua->setText(
        QString("Hiển thị %1 / %2 · STT theo toàn bộ hàng chờ khi xem hàng chờ").arg(SoHien).arg(Tong));
}
void DemoWindow::resizeEvent(QResizeEvent *SuKien) {
    QMainWindow::resizeEvent(SuKien);
    if (!ThanhBen || !HangDauTrang)
        return;
    const auto Huong = width() < 750 ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;
    HangDauTrang->setDirection(Huong);
    HangBoLoc->setDirection(Huong);
    KhuVuc->setVisible(width() >= 750);
    const bool HepMoi = width() < 1050;
    if (HepMoi != Hep) {
        Hep = HepMoi;
        ThanhBen->setVisible(!Hep);
        for (int ViTri = 0; ViTri < 4; ++ViTri) {
            LuoiSoLieu->removeWidget(TheSoLieu[ViTri]);
            LuoiSoLieu->addWidget(TheSoLieu[ViTri], Hep ? ViTri / 2 : 0, Hep ? ViTri % 2 : ViTri);
        }
        DoDuLieu();
    }
}
void DemoWindow::GoiTiep() {
    try {
        const Visit &V = DichVu.GoiTiep(MaBacSi->text().trimmed().toStdString());
        LamMoi();
        statusBar()->showMessage("Đã gọi " + Chu(V.LayMaLanKham()) + " → Đang khám", 10000);
    } catch (const My_stdexcept &Loi) {
        statusBar()->showMessage(QString::fromUtf8(Loi.LayThongBao()), 10000);
    }
}
