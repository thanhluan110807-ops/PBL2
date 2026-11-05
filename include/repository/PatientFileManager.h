#pragma once

#include <string>

#include "repository/PatientRegistry.h"

// Đọc/ghi lược đồ patients.csv v1; không sửa Registry đang được chương trình dùng.
class PatientFileManager {
public:
    // Bên gọi sở hữu Registry mới và phải delete sau khi hủy các service mượn nó.
    PatientRegistry* DocBenhNhan(const std::string& DuongDan) const;
    // Ghi đủ hồ sơ; tuổi chưa biết để rỗng, tuổi sơ sinh ghi 0.
    void LuuBenhNhan(const PatientRegistry& SoBo, const std::string& DuongDan) const;
};
