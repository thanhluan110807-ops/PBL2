#pragma once
#include <string>
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"

// CSV Visit v2 (đọc được v1) lưu cả lịch sử và thông tin dựng lại hàng chờ; không sở hữu Registry.
class VisitFileManager {
public:
    void LuuLanKham(const PatientRegistry& Registry, const TriageService& Triage,
                    const std::string& DuongDan) const;
    // Registry đã có Patient, chưa có Visit. File lỗi không làm nạp dở Registry.
    TrangThaiHangCho DocLanKham(PatientRegistry& Registry, const std::string& DuongDan) const;
};
