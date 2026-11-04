// Phần B — Chức năng chính: Đọc/ghi file nhân viên và phục hồi đúng lớp BacSi/DieuDuong.
#pragma once
#include "repository/StaffRegistry.h"
#include <iosfwd>
#include <memory>

// Model không biết định dạng file. Các hàm dòng được AdminFileManager dùng lại.
class StaffFileManager {
public:
    static void GhiDong(std::ostream& Tep, const NhanVienYTe& NV);
    // Đọc phần sau nhãn NV; tạo đúng lớp con và chỉ thêm khi bản ghi hợp lệ.
    static void DocDong(std::istream& Dong, StaffRegistry& NhanSu, int PhienBan);
    static void LuuNhanVien(const std::string& TenTep, const StaffRegistry& NhanSu);
    static std::unique_ptr<StaffRegistry> DocNhanVien(const std::string& TenTep);
};
