#pragma once
#include <string>
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"


class VisitFileManager {
public:
    void LuuLanKham(const PatientRegistry& Registry, const TriageService& Triage,
                    const std::string& DuongDan) const;

    TrangThaiHangCho DocLanKham(PatientRegistry& Registry, const std::string& DuongDan) const;
};
