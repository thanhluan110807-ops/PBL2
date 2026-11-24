// Phần B — Chức năng chính: Đọc/ghi file ca trực và phân công, giữ lịch sử trạng thái.
#pragma once
#include "repository/ScheduleRegistry.h"
#include <memory>
#include <iosfwd>
#include <optional>
class ScheduleFileManager {
public:
    static void LuuLichTruc(const std::string& DuongDan, const ScheduleRegistry& Lich);
    // Đọc vào registry mới; lỗi không làm thay đổi registry đang sử dụng.
    static std::unique_ptr<ScheduleRegistry> DocLichTruc(const std::string& DuongDan, const StaffRegistry& NhanSu);
    static void GhiCa(std::ostream& Tep, const Shift& Ca);
    static Shift DocCa(std::istream& Tep);
    static void GhiPhanCong(std::ostream& Tep, const ShiftAssignment& P);
    // Khi đọc một dòng riêng, chấp nhận cả dòng cũ và dòng có mã quản lý.
    static ShiftAssignment DocPhanCong(std::istream& Tep, std::optional<bool> CoCotMaQuanLy = {});
};
