#pragma once

#include <string>

#include "repository/PatientRegistry.h"

// Đọc lược đồ patients.csv v1; không sửa Registry đang được chương trình dùng.
class PatientFileManager {
public:
    // Bên gọi sở hữu Registry mới và phải delete sau khi hủy các service mượn nó.
    PatientRegistry* DocBenhNhan(const std::string& DuongDan) const;
};
