
#pragma once
#include <string>
#include <ctime>
enum class DoctorRole { KhamChinh, HoTro, ThayThe };

class DoctorParticipation {
    std::string MaBacSi, MaBacSiDuocThay;
    std::time_t ThoiDiemThamGia = 0;
    DoctorRole VaiTro = DoctorRole::KhamChinh;
public:
    DoctorParticipation() = default;
    DoctorParticipation(const std::string& Ma, std::time_t ThoiDiem,
                        DoctorRole VaiTro = DoctorRole::KhamChinh, const std::string& MaDuocThay = "");
    const std::string& LayMaBacSi() const { return MaBacSi; }
    DoctorRole LayVaiTro() const { return VaiTro; }
    const std::string& LayMaBacSiDuocThay() const { return MaBacSiDuocThay; }
    std::time_t LayThoiDiemThamGia() const { return ThoiDiemThamGia; }
};
