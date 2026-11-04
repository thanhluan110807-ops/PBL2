#pragma once
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "service/VisitSearchService.h"
#include <string>
#include <QMainWindow>
class QResizeEvent;
class QCloseEvent;
class QTimer;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLineEdit;
class QComboBox;
class QTableWidget;
class QLabel;
class QPushButton;
class QFrame;
class QScrollArea;
class QStackedWidget;

class DemoWindow : public QMainWindow {
    // Mượn dữ liệu; main giữ Registry và các service sống lâu hơn cửa sổ.
    PatientRegistry &Registry;
    TriageService &DichVu;
    VisitSearchService &TraCuu;
    QLineEdit *MaBacSi;
    QLineEdit *MaLanKhamLoc;
    QLineEdit *NgayLoc;
    QFrame *ThanhBen = nullptr;
    QPushButton *DieuHuong[5]{};
    QLabel *TieuDe;
    QLabel *MoTa;
    QLabel *KhuVuc;
    QLabel *DongHo;
    QLabel *SoLieu[4]{};
    QFrame *TheSoLieu[4]{};
    QGridLayout *LuoiSoLieu;
    QLineEdit *TimChung;
    QLineEdit *TimHang;
    QComboBox *LocMuc;
    QComboBox *LocTrangThai;
    QPushButton *LocLuong[3]{};
    QPushButton *GoiLuot;
    QTableWidget *Bang;
    QVBoxLayout *CacThe;
    QStackedWidget *VungDanhSach;
    QLabel *SoKetQua;
    QLabel *TenPanel;
    QLabel *HuongDan;
    int Trang = 0, Luong = 0, LocKpi = -1;
    bool Hep = false;
    bool DaThayDoi = false;
    std::string TepPatientTuDong, TepVisitTuDong;
    QTimer *ThuLaiLuu = nullptr;
    QLabel *TinhTrangLuu = nullptr;
    bool ThuLuuTuDong();
    bool CoTheThayDoi();
    QBoxLayout *HangDauTrang = nullptr, *HangBoLoc = nullptr;
    void TaoGiaoDien();
    void TaoThanhBen(QHBoxLayout *Khung);
    void TaoNoiDung(QVBoxLayout *Khung);
    void DoDuLieu();
    void ChuyenTrang(int ChiSo);
    void GoiTiep();

  protected:
    void resizeEvent(QResizeEvent *SuKien) override;
    void closeEvent(QCloseEvent *SuKien) override;

  public:
    DemoWindow(PatientRegistry &Registry, TriageService &DichVu,
               VisitSearchService &TraCuu, const std::string &TepPatient, const std::string &TepVisit, QWidget *Cha = nullptr);
    void LamMoi();
    // Gọi ngay sau thay đổi; chỉ trả true sau khi bản lưu đã được đồng bộ.
    bool DanhDauThayDoi();
    void MoTiepNhan();
    void MoDanhGia(const std::string &MaLanKham);
    void MoChiTiet(const std::string &MaLanKham);
    void MoHoSo(const std::string &MaBenhNhan);
    void MoCanLamSang(const std::string &MaLanKham, bool QuayLai);
    void XacNhanDong(const std::string &MaLanKham, bool Huy);
};
