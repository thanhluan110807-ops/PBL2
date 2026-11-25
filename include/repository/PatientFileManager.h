#pragma once

#include <string>

#include "repository/PatientRegistry.h"


class PatientFileManager {
public:

    PatientRegistry* DocBenhNhan(const std::string& DuongDan) const;

    void LuuBenhNhan(const PatientRegistry& SoBo, const std::string& DuongDan) const;
};
