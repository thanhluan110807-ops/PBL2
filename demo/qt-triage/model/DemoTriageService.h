#pragma once
#include "model/Date.h"
#include <string>

// Mô phỏng độc lập Qt. Mảng cố định có kiểm tra sức chứa: không thay Vector/Heap của đồ án.
// Mọi dữ liệu sở hữu theo giá trị; UI chỉ được đọc const và gọi lệnh qua service.
namespace Demo {
constexpr int GioiHanHoSo = 64;
constexpr int GioiHanLuot = 96;
constexpr int GioiHanDanhGia = 32;
constexpr int GioiHanMoc = 128;
constexpr int GioiHanCanLamSang = 16;

enum class TrangThai { SangLoc, LanDau, QuayLai, DangKham, CanLamSang, KetThuc, Huy };
struct HoSo {
    std::string Ma, HoTen, GioiTinh = "Chưa xác định", CanCuoc, DiUng, TienSu;
    bool ChuaRo = false, CoNgaySinh = false;
    Date NgaySinh{1, 1, 2000};
    int TuoiUocLuong = -1; // -1 là chưa biết; 0 là tuổi thực dưới một năm.
};
struct SinhHieu {
    int Mach = -1, HuyetAp = -1, NhipTho = -1, GCS = -1;
    double SpO2 = -1;
};
struct DeXuat {
    int Muc = 0;
    bool DayDu = false;
};
struct DanhGia {
    SinhHieu SoDo;
    int Tuoi = -1, Muc = 0;
    bool TuNgaySinh = false, DuLieuMau = false;
    DeXuat GoiY;
    long long Luc = 0;
    std::string Nguoi = "Điều dưỡng Nguyễn An";
};
struct Moc {
    long long Luc = 0;
    std::string NoiDung;
};
struct CanLamSang {
    std::string ChiDinh, KetQua;
    long long Luc = 0, TraLuc = -1;
};
struct LuotKham {
    std::string Ma, TrieuChung;
    int BenhNhan = -1, Muc = 0, SoThuTu = 0;
    TrangThai TinhTrang = TrangThai::SangLoc;
    long long TiepNhanLuc = 0, VaoHangLuc = -1;
    DanhGia LichSu[GioiHanDanhGia];
    int SoDanhGia = 0;
    Moc CacMoc[GioiHanMoc];
    int SoMoc = 0;
    CanLamSang CacLanCanLamSang[GioiHanCanLamSang];
    int SoCanLamSang = 0;
};
struct KetQua {
    bool ThanhCong = true;
    std::string Loi, Truong;
    int ChiSo = -1;
};
struct HangCho {
    int CacLuot[GioiHanLuot]{};
    int SoLuong = 0;
};
struct ThongKe {
    int SangLoc = 0, LanDau = 0, QuayLai = 0, CanDanhGia = 0;
};

class DemoTriageService {
    HoSo BenhNhan[GioiHanHoSo];
    LuotKham CacLuot[GioiHanLuot];
    int SoHoSo = 0, SoLuot = 0, ThuTu = 0;
    int BoDem[6]{};
    long long BayGio = 570; // Phút tính từ 00:00 ngày 30/10/2026, UTC+7.
    void GhiMoc(LuotKham &Luot, const std::string &NoiDung);
    void VaoHang(LuotKham &Luot, TrangThai Loai);
    void DongBoBoDem();
    int ChonLuot(const bool *DaLay, int *Dem) const;
    KetQua KiemTraHoSo(const HoSo &DuLieu) const;

  public:
    int LaySoHoSo() const {
        return SoHoSo;
    }
    int LaySoLuot() const {
        return SoLuot;
    }
    long long LayBayGio() const {
        return BayGio;
    }
    Date LayNgay() const;
    const HoSo &LayHoSo(int ChiSo) const;
    const LuotKham &LayLuot(int ChiSo) const;
    int LayTuoi(const HoSo &DuLieu) const;
    int TimLuotDangMo(int HoSoSo) const;
    bool CanDanhGia(int ChiSo) const;
    HangCho LayHangCho() const;
    ThongKe LayThongKe() const;
    KetQua TiepNhan(const HoSo &DuLieu, const std::string &TrieuChung, int DaCo = -1);
    KetQua CapNhatHoSo(int ChiSo, const HoSo &DuLieu);
    KetQua XacNhan(int ChiSo, const SinhHieu &SoDo, int Muc);
    KetQua GoiTiep();
    KetQua ChuyenCanLamSang(int ChiSo, const std::string &ChiDinh);
    KetQua QuayLai(int ChiSo, const std::string &KetQuaXetNghiem);
    KetQua DongLuot(int ChiSo, bool Huy = false);
    void TienThoiGian() {
        BayGio += 10;
    }
    void NapMau(bool KichBanBaMot = false);
    void XoaDuLieu();
    static KetQua KiemTraSinhHieu(const SinhHieu &SoDo);
    static DeXuat DeXuatMuc(const SinhHieu &SoDo, int Tuoi);
    static bool DangCho(const LuotKham &Luot);
    static bool DaDong(const LuotKham &Luot);
    static const char *TenTrangThai(TrangThai Loai);
};
} // namespace Demo
