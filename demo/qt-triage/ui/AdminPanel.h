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

// Widget chỉ thu thập đầu vào và hiển thị; mọi quyết định lịch nằm trong service.
class AdminPanel : public QDialog {
    AdminData DuLieu;
    QString DuongDan;
    bool DaSua = false;
    QLabel *ThongBao;
    QLabel *TrangThaiTep;
    QDateEdit *Tuan;
    QTableWidget *BangNhanSu, *BangCa, *BangLich;
    QLineEdit *MaNV, *TenNV, *ChuyenKhoa, *MaCa;
    QComboBox *LoaiNV, *CapNV, *NgayCa, *ChonNguoiThay;
    QSpinBox *KinhNghiem, *SoBS, *SoDD, *GioMongDau, *GioMongCuoi;
    QDoubleSpinBox *DiemNen;
    QTimeEdit *GioDau, *GioCuoi;
    QCheckBox *TruongCa, *MongCuoiTuan, *DangKyDuPhong;
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
public:
    explicit AdminPanel(QWidget* Cha = nullptr, const QString& TepMacDinh = QString());
    bool LuuTep(const QString& Tep);
    bool NapTep(const QString& Tep);
    const AdminData& LayDuLieu() const { return DuLieu; }
};
