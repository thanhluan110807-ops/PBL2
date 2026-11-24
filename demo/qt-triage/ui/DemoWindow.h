#pragma once
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "service/VisitSearchService.h"
#include <string>
#include <QMainWindow>
class AdminPanel;
class QResizeEvent;
class QCloseEvent;
class QTimer;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QComboBox;
class QTableWidget;
class QLabel;
class QPushButton;
class QFrame;
class QScrollArea;
class QStackedWidget;

class DemoWindow : public QMainWindow {
    enum class VaiTro { ChuaChon, BacSi, DieuDuong, QuanLy };
    enum TrangChucNang {
        TrangTiepNhan = 1, TrangHangCho, TrangLuotKham, TrangHoSo,
        TrangNhanSu, TrangCaTruc, TrangQuanLyLich, TrangLichCaNhan, TrangKham, SoTrang
    };
    VaiTro VaiTroHienTai = VaiTro::ChuaChon;
    std::string MaNhanVien;
    AdminPanel* QuanTri = nullptr;
    // Mượn dữ liệu; main giữ Registry và các service sống lâu hơn cửa sổ.
    PatientRegistry &Registry;
    TriageService &DichVu;
    VisitSearchService &TraCuu;
    QLineEdit *MaLanKhamLoc;
    QLineEdit *NgayLoc;
    QFrame *ThanhBen = nullptr;
    QPushButton *DieuHuong[SoTrang]{};
    QLabel *TieuDe;
    QLabel *MoTa;
    QLabel *KhuVuc;
    QStackedWidget *CacTrang = nullptr;
    QLabel *NhanVaiTro;
    QPushButton *NutTiepNhan;
    QScrollArea *TrangCaNhan;
    QVBoxLayout *NoiDungCaNhan;
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
    int Trang = 0, Luong = 0;
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
    void TaoTrangCaNhan();
    void HienLichCaNhan();
    void HienBenhNhanDangKham();
    bool DuocMoTrang(int ChiSo) const;
    bool LaDieuDuongThaoTac() const;
    bool LaBacSiPhuTrach(const Visit& LanKham) const;
    bool CoTheHuyLuot(const Visit& LanKham) const;
    const Visit* TimLuotDangKham() const;
    void DoDuLieu();
    void ChuyenTrang(int ChiSo);
    void GoiTiep();

  protected:
    void resizeEvent(QResizeEvent *SuKien) override;
    void closeEvent(QCloseEvent *SuKien) override;

  public:
    DemoWindow(PatientRegistry &Registry, TriageService &DichVu,
               VisitSearchService &TraCuu, const std::string &TepPatient, const std::string &TepVisit, QWidget *Cha = nullptr);
    bool ChonVaiTro();
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
