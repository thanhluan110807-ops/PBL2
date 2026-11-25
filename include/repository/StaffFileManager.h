
#pragma once
#include "repository/StaffRegistry.h"
#include <iosfwd>
#include <memory>


class StaffFileManager {
public:
    static void GhiDong(std::ostream& Tep, const NhanVienYTe& NV);

    static void DocDong(std::istream& Dong, StaffRegistry& NhanSu, int PhienBan);
    static void LuuNhanVien(const std::string& TenTep, const StaffRegistry& NhanSu);
    static std::unique_ptr<StaffRegistry> DocNhanVien(const std::string& TenTep);
};
