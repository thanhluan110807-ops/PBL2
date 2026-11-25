#pragma once
#include <string>
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"


class SessionFileManager {
public:
    PatientRegistry* DocPhien(const std::string& TepPatient, const std::string& TepVisit,
                              TrangThaiHangCho& HangCho) const;

    std::string LuuPhien(const PatientRegistry& Registry, const TriageService& Triage,
                         const std::string& ThuMucGoc) const;

    std::string LuuVaoBoFile(const PatientRegistry& Registry, const TriageService& Triage,
                             const std::string& TepPatient, const std::string& TepVisit) const;

    PatientRegistry* DocBoFile(const std::string& TepPatient, const std::string& TepVisit,
                               TrangThaiHangCho& HangCho) const;
};
