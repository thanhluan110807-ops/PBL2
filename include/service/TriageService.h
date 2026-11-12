#pragma once

#include <ctime>
#include <string>
#include "dsa/PriorityQueue.h"
#include "dsa/Vector.h"
#include "repository/PatientRegistry.h"

// Một mục chỉ mượn Visit do Registry sở hữu; không sao chép hồ sơ.
struct MucChoKham {
    Visit* LanKham = nullptr;
    int Muc = 5;
    std::time_t ThoiDiemVaoHang = 0;
    unsigned long long ThuTu = 0;
};

struct SoSanhChoKham {
    bool operator()(const MucChoKham& Trai, const MucChoKham& Phai) const noexcept {
        if (Trai.Muc != Phai.Muc) return Trai.Muc > Phai.Muc;
        if (Trai.ThoiDiemVaoHang != Phai.ThoiDiemVaoHang)
            return Trai.ThoiDiemVaoHang > Phai.ThoiDiemVaoHang;
        return Trai.ThuTu > Phai.ThuTu;
    }
};

// Bản lưu chỉ giữ mã, không lưu địa chỉ RAM; dùng để dựng lại hai heap khi mở file.
struct MucChoLuu {
    std::string MaLanKham;
    std::time_t ThoiDiemVaoHang = 0;
    unsigned long long ThuTu = 0;
};
struct TrangThaiHangCho {
    Vector<MucChoLuu> CacMuc;
    unsigned long long ThuTu = 0;
    bool UuTienQuayLai = true;
};

// Registry phải sống lâu hơn service. Mọi chuyển trạng thái phải qua service.
class TriageService {
    using HangCho = PriorityQueue<MucChoKham, SoSanhChoKham>;
    PatientRegistry& Registry;
    HangCho HangLanDau, HangQuayLai;
    unsigned long long ThuTu = 0;
    bool UuTienQuayLai = true;
    Visit& TimLanKham(const std::string& Ma);
    int TimTrongHang(const HangCho& Hang, const Visit* LanKham) const;
    void KiemTraHang() const;
    static bool ChonHangQuayLai(const HangCho& Dau, const HangCho& Lai, bool UuTienLai);
    MucChoKham TaoMuc(Visit& LanKham, std::time_t ThoiDiem);
public:
    explicit TriageService(PatientRegistry& Registry) : Registry(Registry) {}
    TriageService(const TriageService&) = delete;
    TriageService& operator=(const TriageService&) = delete;
    const Visit& TiepNhan(const std::string& MaLanKham, const std::string& MaBenhNhan,
                          std::time_t ThoiDiem, const std::string& TrieuChung);
    // Chấp nhận mã điều dưỡng rỗng trong giai đoạn chưa có đăng nhập.
    void DanhGia(const std::string& MaLanKham, const VitalSigns& SinhHieu,
                 std::time_t ThoiDiem, const std::string& MaDieuDuong);
    const Visit& GoiTiep(const std::string& MaBacSi);
    Vector<const Visit*> LayDanhSachCho() const;
    void ChuyenCanLamSang(const std::string& MaLanKham, const std::string& ChiDinh,
                          const std::string& MaBacSi);
    void QuayLai(const std::string& MaLanKham, const std::string& KetQua, std::time_t ThoiDiem);
    void KetThuc(const std::string& MaLanKham);
    void Huy(const std::string& MaLanKham);
    TrangThaiHangCho LayTrangThaiHangCho() const;
    // Chỉ công bố heap mới sau khi kiểm tra đủ mọi Visit đang chờ.
    void KhoiPhucHangCho(const TrangThaiHangCho& BanLuu);
    int SoDangCho() const { return HangLanDau.KichThuoc() + HangQuayLai.KichThuoc(); }
};
