
#pragma once
#include "repository/ScheduleRegistry.h"
#include <memory>
#include <iosfwd>
#include <optional>
class ScheduleFileManager {
public:
    static void LuuLichTruc(const std::string& DuongDan, const ScheduleRegistry& Lich);

    static std::unique_ptr<ScheduleRegistry> DocLichTruc(const std::string& DuongDan, const StaffRegistry& NhanSu);
    static void GhiCa(std::ostream& Tep, const Shift& Ca);
    static Shift DocCa(std::istream& Tep);
    static void GhiPhanCong(std::ostream& Tep, const ShiftAssignment& P);

    static ShiftAssignment DocPhanCong(std::istream& Tep, std::optional<bool> CoCotMaQuanLy = {});
};
