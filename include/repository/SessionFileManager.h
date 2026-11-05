#pragma once
#include <string>
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"

// Điều phối cặp file Patient/Visit. Registry trả về thuộc bên gọi.
class SessionFileManager {
public:
    PatientRegistry* DocPhien(const std::string& TepPatient, const std::string& TepVisit,
                              TrangThaiHangCho& HangCho) const;
    // Mỗi lần lưu tạo thư mục mới, không trộn hai phiên hoặc ghi đè file mẫu.
    std::string LuuPhien(const PatientRegistry& Registry, const TriageService& Triage,
                         const std::string& ThuMucGoc) const;
    // Sao lưu raw và ghi nhật ký bền trước khi thay hai file do người dùng chọn.
    std::string LuuVaoBoFile(const PatientRegistry& Registry, const TriageService& Triage,
                             const std::string& TepPatient, const std::string& TepVisit) const;
    // Có nhật ký thì hoàn tất cả cặp trước khi đọc; bình thường đọc file chính.
    PatientRegistry* DocBoFile(const std::string& TepPatient, const std::string& TepVisit,
                               TrangThaiHangCho& HangCho) const;
};
