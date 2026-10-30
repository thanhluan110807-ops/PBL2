#include "ui/DemoWindow.h"
#include "ui/DemoStyle.h"
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

DemoWindow::DemoWindow(Demo::DemoTriageService &DichVu, QWidget *Cha) : QMainWindow(Cha), DichVu(DichVu) {
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
    Cot->addWidget(Nhan("Ca sáng · 07:00 – 15:00", "muted"));
    Cot->addSpacing(24);
    Cot->addWidget(Nhan("Nguyễn An"));
    Cot->addWidget(Nhan("Điều dưỡng điều phối", "muted"));
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
    TimChung->setPlaceholderText("Tìm bệnh nhân hoặc lượt khám…");
    TimChung->setAccessibleName("Tìm bệnh nhân hoặc lượt khám");
    TimChung->setMaximumWidth(330);
    HangDau->addWidget(TimChung, 1);
    HangDau->addWidget(Nhan("● Dữ liệu demo", "status"));
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
    const char *CacNhan[] = {"Chờ sàng lọc", "Chờ khám lần đầu", "Chờ khám quay lại", "Cần tái đánh giá"};
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
        auto *Xem = Nut(ViTri == 3 ? "Cần chú ý" : "Xem lượt", qPrintable(QString("xemKpi%1").arg(ViTri)));
        Xem->setProperty("link", true);
        Hang->addWidget(Xem);
        Cot->addWidget(Duoi);
        TheSoLieu[ViTri] = The;
        LuoiSoLieu->addWidget(The, 0, ViTri);
        connect(Xem, &QPushButton::clicked, this, [this, ViTri] {
            Trang = 3;
            LocKpi = ViTri;
            TimChung->clear();
            LamMoi();
        });
    }
    Khung->addLayout(LuoiSoLieu);
    Khung->addWidget(Nhan("Nhóm cần tái đánh giá có thể trùng với các nhóm đang chờ.", "muted"));
    auto *Panel = new QFrame;
    Panel->setObjectName("panel");
    auto *CotPanel = new QVBoxLayout(Panel);
    CotPanel->setContentsMargins(18, 18, 18, 14);
    CotPanel->setSpacing(14);
    auto *HangTieuDe = new QHBoxLayout;
    TenPanel = Nhan("Hàng chờ khám", "section");
    HangTieuDe->addWidget(TenPanel);
    HangTieuDe->addStretch();
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
    for (int Loai = 0; Loai < 7; ++Loai)
        LocTrangThai->addItem(Demo::DemoTriageService::TenTrangThai(static_cast<Demo::TrangThai>(Loai)));
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
    VungDanhSach = new QStackedWidget;
    VungDanhSach->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    Bang = new QTableWidget;
    Bang->setObjectName("hangCho");
    Bang->setColumnCount(7);
    Bang->setHorizontalHeaderLabels({"STT", "Bệnh nhân / Lượt khám", "Cấp cứu", "Luồng / Trạng thái",
                                     "Đã chờ", "Đánh giá gần nhất", "Thao tác"});
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
    CotPanel->addWidget(SoKetQua);
    Khung->addWidget(Panel, 1);
    auto *GhiChu = Nhan(
        "Mức 1 ưu tiên cao nhất · Cùng mức: tối đa 3 lượt quay lại rồi 1 lượt lần đầu khi có cạnh tranh.",
        "muted");
    GhiChu->setWordWrap(true);
    Khung->addWidget(GhiChu);
    auto *DemoHang = new QHBoxLayout;
    HangDemo = DemoHang;
    DemoHang->addWidget(Nhan("Điều khiển demo", "section"));
    DemoHang->addStretch();
    auto *DatLaiNut = Nut("Đặt lại dữ liệu", "datLai");
    auto *BaMot = Nut("Kịch bản 3:1", "baMot");
    auto *Tien = Nut("+10 phút", "tienGio");
    DemoHang->addWidget(DatLaiNut);
    DemoHang->addWidget(BaMot);
    DemoHang->addWidget(Tien);
    Khung->addLayout(DemoHang);
    connect(DatLaiNut, &QPushButton::clicked, this, [this] { DatLai(false); });
    connect(BaMot, &QPushButton::clicked, this, [this] { DatLai(true); });
    connect(Tien, &QPushButton::clicked, this, [this] {
        DichVu.TienThoiGian();
        LamMoi();
        statusBar()->showMessage("Đã tiến 10 phút và cập nhật các cảnh báo.", 6000);
    });
    DongHo = Nhan("", "muted");
    DongHo->setWordWrap(true);
    Khung->addWidget(DongHo);
}
void DemoWindow::ChuyenTrang(int ChiSo) {
    Trang = ChiSo;
    LocKpi = -1;
    TimChung->clear();
    TimHang->clear();
    LamMoi();
}
void DemoWindow::LamMoi() {
    const auto So = DichVu.LayThongKe();
    const int Mang[] = {So.SangLoc, So.LanDau, So.QuayLai, So.CanDanhGia};
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
    GoiLuot->setEnabled(DichVu.LayHangCho().SoLuong > 0);
    HuongDan->setText(LaHang       ? "Gọi theo toàn bộ hàng chờ hiệu lực, không theo bộ lọc."
                      : Trang == 1 ? "Xác nhận phân loại trước khi đưa lượt vào hàng chờ khám."
                      : Trang == 4 ? "Hồ sơ lâu dài giữ mã ổn định, liên kết với các lượt khám."
                                   : "Xem diễn biến và thực hiện thao tác hợp lệ theo trạng thái.");
    const Date Ngay = DichVu.LayNgay();
    DongHo->setText(QString("Đồng hồ mô phỏng: %1 · %2/%3/%4 (UTC+7) · Chỉ chạy khi bấm +10 phút")
                        .arg(Gio(DichVu.LayBayGio()))
                        .arg(Ngay.Ngay)
                        .arg(Ngay.Thang)
                        .arg(Ngay.Nam));
    DoDuLieu();
}
void DemoWindow::DoDuLieu() {
    Bang->setRowCount(0);
    while (auto *Muc = CacThe->takeAt(0)) {
        delete Muc->widget();
        delete Muc;
    }
    const bool LaHang = Trang == 0 || Trang == 2;
    const auto Hang = DichVu.LayHangCho();
    int SoHien = 0;
    const int Tong = LaHang ? Hang.SoLuong : Trang == 4 ? DichVu.LaySoHoSo() : DichVu.LaySoLuot();
    for (int Thu = 0; Thu < Tong; ++Thu) {
        const int So = LaHang ? Hang.CacLuot[Thu] : Thu;
        const Demo::LuotKham *Luot = Trang == 4 ? nullptr : &DichVu.LayLuot(So);
        const int HoSoSo = Luot ? Luot->BenhNhan : So;
        const auto &HoSo = DichVu.LayHoSo(HoSoSo);
        const QString Ten = HoSo.ChuaRo ? "Chưa rõ danh tính" : Chu(HoSo.HoTen);
        const QString Tim = ChuanHoa(Ten + " " + Chu(HoSo.Ma) + (Luot ? " " + Chu(Luot->Ma) : ""));
        if (!Tim.contains(ChuanHoa(TimHang->text())) || !Tim.contains(ChuanHoa(TimChung->text())))
            continue;
        if (Luot) {
            if (Trang == 1 && Luot->TinhTrang != Demo::TrangThai::SangLoc)
                continue;
            if (LaHang && ((Luong == 1 && Luot->TinhTrang != Demo::TrangThai::LanDau) ||
                           (Luong == 2 && Luot->TinhTrang != Demo::TrangThai::QuayLai) ||
                           (LocMuc->currentIndex() > 0 && Luot->Muc != LocMuc->currentIndex())))
                continue;
            if (Trang == 3 && LocKpi >= 0) {
                if (LocKpi == 3 ? !DichVu.CanDanhGia(So) : static_cast<int>(Luot->TinhTrang) != LocKpi)
                    continue;
            } else if (Trang == 3 && LocTrangThai->currentIndex() > 0 &&
                       static_cast<int>(Luot->TinhTrang) != LocTrangThai->currentIndex() - 1)
                continue;
        }
        const int Dong = SoHien++;
        Bang->insertRow(Dong);
        Bang->setRowHeight(Dong, 74);
        const QString MaLuot = Luot ? Chu(Luot->Ma) : "Hồ sơ lâu dài";
        auto *TenO = new QTableWidgetItem(Ten + "\n" + Chu(HoSo.Ma) + " · " + MaLuot);
        TenO->setData(Qt::UserRole, So);
        Bang->setItem(Dong, 1, TenO);
        Bang->setItem(Dong, 0, new QTableWidgetItem(QString::number(Thu + 1)));
        auto *MucO = new QWidget;
        auto *BoCuc = new QHBoxLayout(MucO);
        BoCuc->setContentsMargins(6, 16, 6, 16);
        BoCuc->addWidget(HuyHieu(Luot ? Luot->Muc : 0));
        Bang->setCellWidget(Dong, 2, MucO);
        const QString Loai =
            Luot ? (LaHang ? (Luot->TinhTrang == Demo::TrangThai::QuayLai ? "Quay lại" : "Lần đầu")
                           : Demo::DemoTriageService::TenTrangThai(Luot->TinhTrang))
                 : QString("%1").arg(DichVu.LayTuoi(HoSo) < 0
                                         ? "Chưa rõ tuổi"
                                         : QString::number(DichVu.LayTuoi(HoSo)) + " tuổi");
        const QString Cho = Luot && Demo::DemoTriageService::DangCho(*Luot)
                                ? QString("%1 phút").arg(DichVu.LayBayGio() - Luot->VaoHangLuc)
                                : "—";
        const bool Can = Luot && DichVu.CanDanhGia(So);
        const QString DanhGia = Luot && Luot->SoDanhGia ? (Can ? "Cần tái đánh giá\n" : "Đã xác nhận\n") +
                                                              Gio(Luot->LichSu[Luot->SoDanhGia - 1].Luc)
                                                        : "Chưa đánh giá";
        Bang->setItem(Dong, 3, new QTableWidgetItem(Loai));
        Bang->setItem(Dong, 4, new QTableWidgetItem(Cho));
        auto *DanhGiaO = new QTableWidgetItem(DanhGia);
        if (Can)
            DanhGiaO->setForeground(QColor("#9A5E0D"));
        Bang->setItem(Dong, 5, DanhGiaO);
        const auto Mo = [this, So, HoSoSo] {
            if (Trang == 4)
                MoHoSo(HoSoSo);
            else
                MoChiTiet(So);
        };
        auto *ChiTiet = Nut("Chi tiết", qPrintable(QString("chiTiet%1").arg(So)));
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
            Dau->addWidget(HuyHieu(Luot->Muc));
        Cot->addLayout(Dau);
        Cot->addWidget(Nhan(Chu(HoSo.Ma) + " · " + MaLuot, "muted"));
        Cot->addWidget(Nhan(Luot ? Loai + " · " + Cho : Loai));
        auto *Duoi = new QHBoxLayout;
        auto *N = Nhan(Luot ? DanhGia : "Dị ứng: " + Chu(HoSo.DiUng.empty() ? "Chưa ghi nhận" : HoSo.DiUng),
                       "muted");
        N->setWordWrap(true);
        Duoi->addWidget(N, 1);
        auto *Xem = Nut("Chi tiết", qPrintable(QString("theChiTiet%1").arg(So)));
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
    HangDemo->setDirection(Huong);
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
    const auto K = DichVu.GoiTiep();
    if (!K.ThanhCong) {
        statusBar()->showMessage(Chu(K.Loi), 7000);
        return;
    }
    const auto &L = DichVu.LayLuot(K.ChiSo);
    const auto &H = DichVu.LayHoSo(L.BenhNhan);
    LamMoi();
    statusBar()->showMessage("Đã gọi " + Chu(H.ChuaRo ? "Chưa rõ danh tính" : H.HoTen) + " · " + Chu(L.Ma) +
                                 " → Đang khám",
                             10000);
}
void DemoWindow::DatLai(bool BaMot) {
    if (QMessageBox::question(this, "Điều khiển demo",
                              BaMot ? "Nạp 5 lượt quay lại và 2 lượt lần đầu cùng mức 3? Các thay đổi thử "
                                      "nghiệm sẽ được thay thế."
                                    : "Đặt lại 14 hồ sơ mẫu? Các thay đổi thử nghiệm sẽ được thay thế.",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;
    DichVu.NapMau(BaMot);
    Luong = 0;
    LocLuong[0]->setChecked(true);
    LocMuc->setCurrentIndex(0);
    ChuyenTrang(0);
    statusBar()->showMessage("Đã nạp dữ liệu mô phỏng.", 6000);
}
