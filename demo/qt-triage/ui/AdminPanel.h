// Phần B — Chức năng chính: Giao diện Qt quản lý nhân sự, ca trực và lưu dữ liệu liên kết.
#pragma once
#include "repository/AdminFileManager.h"
#include <QDialog>
class QLineEdit;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QTableWidget;
class QLabel;
class QDateEdit;
class QTimeEdit;
class QCheckBox;
class QCloseEvent;
class QFormLayout;
class QResizeEvent;
class QPushButton;
class QStackedWidget;
class QTabWidget;
class QBoxLayout;

// Widget chỉ thu thập đầu vào và hiển thị; mọi quyết định lịch nằm trong service.
class AdminPanel : public QDialog {
    bool TrongTrangChinh = false;
    AdminData DuLieu;
    QString DuongDan;
    bool DaSua = false;
    bool LoiTepLienKet = false;
    QPushButton* NutLuu;
    QLabel *ThongBao;
    QLabel *TieuDe;
    QWidget *ChonTuan;
    QPushButton *NutNapMau;
    QStackedWidget *CacTrang = nullptr;
    QTabWidget *CacTab = nullptr;
    QBoxLayout *BoCucTieuDe = nullptr, *BoCucNhanSu = nullptr, *BoCucCa = nullptr;
    QLabel *TrangThaiTep;
    QLabel *SoNhanSu, *SoCa, *SoPhanCong, *BuocTiep;
    QLabel *ChiTietNhanSu, *ChiTietCa, *ChiTietPhanCong;
    QDateEdit *Tuan;
    QTableWidget *BangNhanSu, *BangCa, *BangLich;
    QLineEdit *MaNV, *TenNV, *ChuyenKhoa, *MaCa;
    QComboBox *LoaiNV, *CapNV, *NgayCa, *ChonNguoiThay;
    QSpinBox *KinhNghiem, *SoBS, *SoDD, *GioMongDau, *GioMongCuoi;
    QDoubleSpinBox *DiemNen;
    QTimeEdit *GioDau, *GioCuoi;
    QCheckBox *TruongCa, *MongCuoiTuan, *DangKyDuPhong;
    QFormLayout *BieuMauNhanSu = nullptr, *BieuMauCa = nullptr;
    void ThemTrang(QWidget* NoiDung, const QString& Ten);
    void TaoNhanSu();
    void TaoCa();
    void TaoLich();
    void LamMoi();
    void BaoLoi(const char* Loi);
    void ThemNhanSu(bool CapNhat = false);
    void CapNhatSangLoc(int Lenh);
    void ThemCa();
    void LapLich();
    void ThaoTacPhanCong(int Lenh);
    void NapMau();
    void TimThay();
    void ThayNguoi();
    bool ChoPhepBoThayDoi();
    void reject() override;
protected:
    void closeEvent(QCloseEvent* SuKien) override;
    void resizeEvent(QResizeEvent* SuKien) override;
public:
    explicit AdminPanel(QWidget* Cha = nullptr, const QString& TepMacDinh = QString(), bool TrongTrangChinh = false);
    void ChonTrang(int ChiSo, const QString& Ten);
    bool ChoPhepDong();
    bool LuuNhanSu(const QString& Tep);
    bool NapNhanSu(const QString& Tep);
    bool LuuTep(const QString& Tep);
    bool NapTep(const QString& Tep);
    const AdminData& LayDuLieu() const { return DuLieu; }
};
