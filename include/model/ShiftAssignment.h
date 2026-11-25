
#pragma once
#include <string>
#include <ctime>

enum class AssignmentRole { BacSi, TruongCa, DieuDuong };
enum class AssignmentStatus { DuKien, DaXacNhan, DangTruc, DaHuy };

class ShiftAssignment {
    std::string MaPhanCong;
    AssignmentStatus TrangThai = AssignmentStatus::DuKien;
    std::string LyDoHuy;
    std::string MaQuanLyXacNhan;
    std::time_t ThoiDiemBatDau = 0;
public:

    std::string MaNhanVien, MaCa;
    AssignmentRole VaiTro = AssignmentRole::BacSi;
    ShiftAssignment() = default;
    ShiftAssignment(const std::string& NhanVien, const std::string& Ca, AssignmentRole VaiTro);
    ShiftAssignment(const std::string& Ma, const std::string& NhanVien,
                    const std::string& Ca, AssignmentRole VaiTro);
    const std::string& LayMaPhanCong() const { return MaPhanCong; }
    AssignmentStatus LayTrangThai() const { return TrangThai; }
    const std::string& LayLyDoHuy() const { return LyDoHuy; }
    const std::string& LayMaQuanLyXacNhan() const { return MaQuanLyXacNhan; }
    std::time_t LayThoiDiemBatDau() const { return ThoiDiemBatDau; }
    bool DangHieuLuc() const { return TrangThai != AssignmentStatus::DaHuy; }

    void XacNhan(const std::string& MaQuanLy = {});
    void Huy(const std::string& LyDo);
    void GhiNhanBatDau(std::time_t ThoiDiem);
};
