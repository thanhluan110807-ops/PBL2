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

// Widget chỉ thu thập đầu vào và hiển thị; mọi quyết định lịch nằm trong service.
class AdminPanel : public QDialog {
    bool TrongTrangChinh = false;
    AdminData DuLieu;
    QString DuongDan;
    bool DaSua = false;
    bool LoiTepLienKet = false;
    QPushButton* NutLuu;
    QLabel *ThongBao;
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
    void TaoNhanSu(class QTabWidget* Tab);
    void TaoCa(class QTabWidget* Tab);
    void TaoLich(class QTabWidget* Tab);
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
    bool ChoPhepDong();
    bool LuuNhanSu(const QString& Tep);
    bool NapNhanSu(const QString& Tep);
    bool LuuTep(const QString& Tep);
    bool NapTep(const QString& Tep);
    const AdminData& LayDuLieu() const { return DuLieu; }
};
