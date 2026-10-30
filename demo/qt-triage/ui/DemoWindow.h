#pragma once
#include "model/DemoTriageService.h"
#include <QMainWindow>
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
    Demo::DemoTriageService &DichVu;
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
    QBoxLayout *HangDauTrang = nullptr, *HangBoLoc = nullptr, *HangDemo = nullptr;
    void TaoGiaoDien();
    void TaoThanhBen(QHBoxLayout *Khung);
    void TaoNoiDung(QVBoxLayout *Khung);
    void DoDuLieu();
    void ChuyenTrang(int ChiSo);
    void GoiTiep();
    void DatLai(bool BaMot);

  protected:
    void resizeEvent(QResizeEvent *SuKien) override;

  public:
    explicit DemoWindow(Demo::DemoTriageService &DichVu, QWidget *Cha = nullptr);
    void LamMoi();
    void MoTiepNhan();
    void MoDanhGia(int ChiSo);
    void MoChiTiet(int ChiSo);
    void MoHoSo(int ChiSo, int Luot = -1);
    void MoCanLamSang(int ChiSo, bool QuayLai);
    void XacNhanDong(int ChiSo, bool Huy);
};
