#include "ui/DemoWindow.h"
#include "ui/AdminPanel.h"
#include "ui/DemoStyle.h"
#include "model/DoctorParticipation.h"
#include "model/NhanVienYTe.h"
#include "model/Patient.h"
#include "model/Shift.h"
#include "model/ShiftAssignment.h"
#include "model/Visit.h"
#include "repository/AdminFileManager.h"
#include "repository/StaffRegistry.h"
#include <QButtonGroup>
#include <QComboBox>
#include <QCompleter>
#include <QDate>
#include <QDateTime>
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QStackedWidget>
#include <QStatusBar>
#include <QString>
#include <QTime>
#include <QTimeZone>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

using namespace DemoUi;
namespace {
QLabel* ThemNhan(QVBoxLayout* Cot, const QString& NoiDung, const char* Kieu = "", const char* Ma = "") {
    auto* NhanMoi = Nhan(NoiDung, Kieu);
    NhanMoi->setObjectName(Ma);
    NhanMoi->setWordWrap(true);
    Cot->addWidget(NhanMoi);
    return NhanMoi;
}

QVBoxLayout* ThemThe(QVBoxLayout* Cot) {
    auto* The = new QFrame;
    The->setObjectName("card");
    auto* NoiDung = new QVBoxLayout(The);
    NoiDung->setContentsMargins(16, 14, 16, 14);
    Cot->addWidget(The);
    return NoiDung;
}

int TimNhanVienDaNhap(const QComboBox& DanhSach) {
    const QString TuKhoa = DanhSach.currentText().trimmed();
    const int DongDangChon = DanhSach.currentIndex();
    if (DongDangChon >= 0 && TuKhoa == DanhSach.itemText(DongDangChon)) return DongDangChon;

    const int DongDungMa = DanhSach.findData(TuKhoa);
    if (DongDungMa >= 0) return DongDungMa;
    const int DongGoiY = DanhSach.findText(TuKhoa);
    if (DongGoiY >= 0) return DongGoiY;

    // Cho phép nhập thường hoặc hoa nếu chỉ có một mã khớp trong vai trò này.
    int DongKhop = -1;
    for (int I = 0; I < DanhSach.count(); ++I) {
        if (DanhSach.itemData(I).toString().compare(TuKhoa, Qt::CaseInsensitive) != 0) continue;
        if (DongKhop >= 0) return -1;
        DongKhop = I;
    }
    return DongKhop;
}
}

bool DemoWindow::ChonVaiTro() {
    QDialog Hop(this);
    Hop.setObjectName("chonVaiTroDialog");
    Hop.setWindowTitle("Chọn vai trò");
    Hop.setStyleSheet(Qss());
    Hop.setMinimumWidth(390);
    auto* Cot = new QVBoxLayout(&Hop);
    Cot->setContentsMargins(22, 20, 22, 20);
    ThemNhan(Cot, "Chọn vai trò", "section");
    ThemNhan(Cot, "Chọn vai trò mô phỏng để mở không gian làm việc tương ứng.", "muted");

    auto* HangVaiTro = new QHBoxLayout;
    Cot->addLayout(HangVaiTro);
    QButtonGroup NhomVaiTro(&Hop);
    const VaiTro CacVaiTro[] = {VaiTro::BacSi, VaiTro::DieuDuong, VaiTro::QuanLy};
    const char* TenVaiTro[] = {"Bác sĩ", "Điều dưỡng", "Quản lý"};
    const char* MaNut[] = {"vaiTroBacSi", "vaiTroDieuDuong", "vaiTroQuanLy"};
    for (int I = 0; I < 3; ++I) {
        auto* NutVaiTro = Nut(TenVaiTro[I], MaNut[I]);
        NutVaiTro->setProperty("tab", true);
        NutVaiTro->setCheckable(true);
        NhomVaiTro.addButton(NutVaiTro, static_cast<int>(CacVaiTro[I]));
        HangVaiTro->addWidget(NutVaiTro, 1);
    }

    auto* NhanNhanVien = ThemNhan(Cot, "Mã nhân viên", "section");
    auto* ChonNhanVien = new QComboBox;
    ChonNhanVien->setObjectName("chonNhanVienVaiTro");
    ChonNhanVien->setEditable(true);
    ChonNhanVien->setInsertPolicy(QComboBox::NoInsert);
    ChonNhanVien->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ChonNhanVien->completer()->setCompletionMode(QCompleter::PopupCompletion);
    auto* NhapMa = ChonNhanVien->lineEdit();
    NhapMa->setPlaceholderText("Nhập mã hoặc chọn trong danh sách");
    NhapMa->setAccessibleName("Mã nhân viên");
    NhanNhanVien->setBuddy(NhapMa);
    Cot->addWidget(ChonNhanVien);
    auto* HuongDan = ThemNhan(Cot, "", "muted");
    auto* Chan = new QHBoxLayout;
    Cot->addLayout(Chan);
    auto* Dong = Nut("Hủy", "huyChonVaiTro");
    auto* Vao = Nut(VaiTroHienTai == VaiTro::ChuaChon ? "Vào hệ thống" : "Xác nhận", "xacNhanVaiTro", true);
    Chan->addStretch();
    Chan->addWidget(Dong);
    Chan->addWidget(Vao);
    Vao->setDefault(true);
    connect(Dong, &QPushButton::clicked, &Hop, &QDialog::reject);
    connect(Vao, &QPushButton::clicked, &Hop, [&] { if (Vao->isEnabled()) Hop.accept(); });

    const auto CapNhatNutVao = [&] {
        const auto VaiTroChon = static_cast<VaiTro>(NhomVaiTro.checkedId());
        const bool CanChonNguoi = VaiTroChon == VaiTro::BacSi || VaiTroChon == VaiTro::DieuDuong;
        const int ViTri = TimNhanVienDaNhap(*ChonNhanVien);
        Vao->setEnabled(VaiTroChon == VaiTro::QuanLy || (CanChonNguoi && ViTri >= 0));
        QString ThongBao;
        if (CanChonNguoi) {
            if (ChonNhanVien->count() == 0)
                ThongBao = "Chưa có nhân viên cho vai trò này. Hãy vào Quản lý để thêm nhân sự.";
            else if (ViTri >= 0 && ChonNhanVien->currentText() != ChonNhanVien->itemText(ViTri))
                ThongBao = ChonNhanVien->itemText(ViTri);
            else if (ViTri < 0 && !ChonNhanVien->currentText().trimmed().isEmpty())
                ThongBao = "Hãy nhập đầy đủ mã nhân viên thuộc vai trò đang chọn.";
        }
        HuongDan->setText(ThongBao);
        HuongDan->setVisible(!ThongBao.isEmpty());
    };
    // Chỉ dùng mã nhân viên thật trong dữ liệu hiện tại để gắn đúng lịch và lượt khám.
    const auto NapNhanVien = [&] {
        const QSignalBlocker ChanTinHieu(ChonNhanVien);
        const int MaVaiTro = NhomVaiTro.checkedId();
        const auto VaiTroChon = static_cast<VaiTro>(MaVaiTro);
        const bool CanChonNguoi = VaiTroChon == VaiTro::BacSi || VaiTroChon == VaiTro::DieuDuong;
        ChonNhanVien->clear();
        if (CanChonNguoi) {
            const auto Loai = VaiTroChon == VaiTro::BacSi ? StaffType::BacSi : StaffType::DieuDuong;
            const auto& NhanSu = *QuanTri->LayDuLieu().NhanSu;
            for (int I = 0; I < NhanSu.KichThuoc(); ++I) {
                const auto& Nguoi = NhanSu.Lay(I);
                if (Nguoi.LayLoaiNhanVien() != Loai) continue;
                const QString Ma = Chu(Nguoi.LayMaDinhDanh());
                ChonNhanVien->addItem(Ma + " · " + Chu(Nguoi.LayHoTen()), Ma);
            }
            const int ViTriCu = VaiTroChon == VaiTroHienTai ? ChonNhanVien->findData(Chu(MaNhanVien)) : -1;
            ChonNhanVien->setCurrentIndex(ViTriCu);
        }
        NhanNhanVien->setVisible(CanChonNguoi);
        ChonNhanVien->setVisible(CanChonNguoi);
        ChonNhanVien->setEnabled(ChonNhanVien->count() > 0);
        if (ChonNhanVien->isEnabled()) {
            NhapMa->setFocus();
            NhapMa->selectAll();
        }
        CapNhatNutVao();
    };
    for (auto* NutVaiTro : NhomVaiTro.buttons())
        connect(NutVaiTro, &QPushButton::clicked, &Hop, NapNhanVien);
    connect(ChonNhanVien, &QComboBox::editTextChanged, &Hop, CapNhatNutVao);
    if (auto* NutCu = NhomVaiTro.button(static_cast<int>(VaiTroHienTai))) NutCu->setChecked(true);
    NapNhanVien();
    if (Hop.exec() != QDialog::Accepted) return false;

    const auto VaiTroMoi = static_cast<VaiTro>(NhomVaiTro.checkedId());
    const int ViTriMoi = TimNhanVienDaNhap(*ChonNhanVien);
    const auto MaMoi = ChonNhanVien->itemData(ViTriMoi).toString().toStdString();
    if (VaiTroMoi == VaiTroHienTai && MaMoi == MaNhanVien) return true;
    if (VaiTroHienTai == VaiTro::QuanLy && !QuanTri->LuuTruocKhiDoiVaiTro()) return false;

    VaiTroHienTai = VaiTroMoi;
    MaNhanVien = MaMoi;
    const QString Ten = NhomVaiTro.checkedButton()->text();
    NhanVaiTro->setText(MaNhanVien.empty() ? Ten : Ten + " · " + ChonNhanVien->itemText(ViTriMoi));
    QuanTri->setEnabled(VaiTroHienTai == VaiTro::QuanLy);
    for (int I = TrangTiepNhan; I < SoTrang; ++I) {
        const bool DuocMo = DuocMoTrang(I);
        DieuHuong[I]->setVisible(DuocMo);
        DieuHuong[I]->setEnabled(DuocMo);
    }
    Luong = 0;
    LocLuong[0]->setChecked(true);
    LocMuc->setCurrentIndex(0);
    statusBar()->clearMessage();
    ChuyenTrang(VaiTroMoi == VaiTro::DieuDuong ? TrangTiepNhan
                : VaiTroMoi == VaiTro::BacSi ? TrangHangCho : TrangLuotKham);
    return true;
}

bool DemoWindow::DuocMoTrang(int ChiSo) const {
    if (VaiTroHienTai == VaiTro::ChuaChon || ChiSo < TrangTiepNhan || ChiSo >= SoTrang) return false;
    switch (ChiSo) {
    case TrangTiepNhan: return VaiTroHienTai == VaiTro::DieuDuong;
    case TrangHangCho:
    case TrangLichCaNhan: return VaiTroHienTai == VaiTro::DieuDuong || VaiTroHienTai == VaiTro::BacSi;
    case TrangKham: return VaiTroHienTai == VaiTro::BacSi;
    case TrangLuotKham:
    case TrangHoSo: return true;
    default: return VaiTroHienTai == VaiTro::QuanLy;
    }
}

bool DemoWindow::LaDieuDuongThaoTac() const {
    // Từ Hàng chờ, kể cả các cửa sổ chi tiết liên kết cũng chỉ để xem.
    return VaiTroHienTai == VaiTro::DieuDuong && Trang != TrangHangCho;
}

bool DemoWindow::LaBacSiPhuTrach(const Visit& LanKham) const {
    const int SoLan = LanKham.SoLanBacSiThamGiaKham();
    return VaiTroHienTai == VaiTro::BacSi && SoLan > 0 &&
           LanKham.LayBacSiThamGiaKhamTai(SoLan - 1) == MaNhanVien;
}

bool DemoWindow::CoTheHuyLuot(const Visit& LanKham) const {
    const auto TrangThai = LanKham.LayTrangThai();
    return (LaDieuDuongThaoTac() &&
            (TrangThai == VisitStatus::ChoKhamLanDau || TrangThai == VisitStatus::ChoKhamQuayLai)) ||
           (LaBacSiPhuTrach(LanKham) &&
            (TrangThai == VisitStatus::DangKham || TrangThai == VisitStatus::ChoCanLamSang));
}

const Visit* DemoWindow::TimLuotDangKham() const {
    const Visit* KetQua = nullptr;
    const auto CacLuot = TraCuu.TimTheoBenhNhan();
    for (int I = 0; I < CacLuot.KichThuoc(); ++I) {
        const Visit* Luot = CacLuot[I].LanKham;
        if (Luot->LayTrangThai() != VisitStatus::DangKham || !LaBacSiPhuTrach(*Luot)) continue;
        // Các lần gọi được lưu cùng lượt khám, nên đổi vai trò hoặc mở lại vẫn tìm được.
        if (!KetQua || Luot->LayThamGiaTai(Luot->SoLanBacSiThamGiaKham() - 1).LayThoiDiemThamGia() >
                      KetQua->LayThamGiaTai(KetQua->SoLanBacSiThamGiaKham() - 1).LayThoiDiemThamGia())
            KetQua = Luot;
    }
    return KetQua;
}

void DemoWindow::TaoTrangCaNhan() {
    TrangCaNhan = new QScrollArea;
    TrangCaNhan->setObjectName("trangCaNhan");
    TrangCaNhan->setWidgetResizable(true);
    auto* NoiDung = new QWidget;
    NoiDung->setObjectName("content");
    NoiDungCaNhan = new QVBoxLayout(NoiDung);
    NoiDungCaNhan->setContentsMargins(24, 24, 24, 20);
    NoiDungCaNhan->setSpacing(12);
    TrangCaNhan->setWidget(NoiDung);
    CacTrang->addWidget(TrangCaNhan);
}

void DemoWindow::HienLichCaNhan() {
    ThemNhan(NoiDungCaNhan, "Lịch trực của tôi", "title");
    const auto& DuLieu = QuanTri->LayDuLieu();
    const QDate DauTuan = QDate::fromString(Chu(DuLieu.TuanBatDau), Qt::ISODate);
    const qint64 MocDau = QDateTime(DauTuan, QTime(0, 0), QTimeZone(7 * 3600)).toSecsSinceEpoch();
    const qint64 MocCuoi = MocDau + 7 * 24 * 3600;
    ThemNhan(NoiDungCaNhan, DauTuan.toString("dd/MM/yyyy") + " – " +
             DauTuan.addDays(6).toString("dd/MM/yyyy") + " · UTC+7", "muted", "tuanLichCaNhan");
    const QString TrangThai[] = {"Dự kiến", "Đã xác nhận", "Đang trực", "Đã hủy"};
    int SoCa = 0;
    for (int I = 0; I < DuLieu.Lich.CacCa.KichThuoc(); ++I) {
        const auto& Ca = DuLieu.Lich.CacCa[I];
        if (Ca.LayBatDau() < MocDau || Ca.LayBatDau() >= MocCuoi) continue;
        for (int J = 0; J < DuLieu.Lich.PhanCong.KichThuoc(); ++J) {
            const auto& PhanCong = DuLieu.Lich.PhanCong[J];
            if (!PhanCong.DangHieuLuc() || PhanCong.MaNhanVien != MaNhanVien ||
                PhanCong.MaCa != Ca.LayMaCaTruc()) continue;
            auto* The = ThemThe(NoiDungCaNhan);
            QString TenCa = Chu(Ca.LayMaCaTruc());
            if (PhanCong.VaiTro == AssignmentRole::TruongCa) TenCa += " · Trưởng ca";
            ThemNhan(The, TenCa, "section", "caTrucCaNhan");
            ThemNhan(The, ThoiDiem(Ca.LayBatDau()) + " → " + ThoiDiem(Ca.LayKetThuc()));
            ThemNhan(The, TrangThai[static_cast<int>(PhanCong.LayTrangThai())], "muted");
            ++SoCa;
        }
    }
    if (!SoCa) ThemNhan(NoiDungCaNhan, "Bạn chưa có ca trực trong tuần này.", "muted", "lichCaNhanRong");
}

void DemoWindow::HienBenhNhanDangKham() {
    ThemNhan(NoiDungCaNhan, "Khám", "title");
    const Visit* Luot = TimLuotDangKham();
    if (!Luot) {
        ThemNhan(NoiDungCaNhan, "Chưa có bệnh nhân đang khám. Mở Hàng chờ để gọi lượt tiếp theo.", "muted", "chuaGoiKham");
        auto* VeHang = Nut("Mở hàng chờ", "moHangCho");
        NoiDungCaNhan->addWidget(VeHang, 0, Qt::AlignLeft);
        connect(VeHang, &QPushButton::clicked, this, [this] { ChuyenTrang(TrangHangCho); });
        return;
    }
    const auto* BenhNhan = Registry.TimBenhNhan(Luot->LayMaBenhNhan());
    const std::string Ma = Luot->LayMaLanKham();
    auto* The = ThemThe(NoiDungCaNhan);
    const QString Ten = BenhNhan && !BenhNhan->LayHoTen().empty() ? Chu(BenhNhan->LayHoTen()) : "Chưa rõ danh tính";
    ThemNhan(The, Ten, "section", "benhNhanDangKham");
    ThemNhan(The, Chu(Luot->LayMaBenhNhan()) + " · " + Chu(Ma), "muted", "maLuotDangKham");
    ThemNhan(The, "Triệu chứng: " + Chu(Luot->LayTrieuChungChinh()));
    The->addWidget(HuyHieu(Luot->SoDanhGia() ? Luot->LayDanhGiaGanNhat().LayMuc() : 0), 0, Qt::AlignLeft);
    auto* ChiTiet = Nut("Mở chi tiết khám", "moChiTietKham", true);
    The->addWidget(ChiTiet, 0, Qt::AlignLeft);
    connect(ChiTiet, &QPushButton::clicked, this, [this, Ma] { MoChiTiet(Ma); });
}
