// Phần B — Chức năng chính: Đọc/ghi snapshot chung nhân sự, lịch và dự phòng cho GUI.
#pragma once
#include "repository/StaffRegistry.h"
#include "model/Shift.h"
#include "repository/StandbyPool.h"
#include <memory>

// Một snapshot chứa cả nhân sự và lịch để tránh đọc hai file lệch phiên bản.
// unique_ptr sở hữu Registry; nó tự giải phóng khi quá trình đọc thất bại.
struct AdminData {
    std::unique_ptr<StaffRegistry> NhanSu = std::make_unique<StaffRegistry>();
    Schedule Lich;
    StandbyPool DuPhong;
    std::string TuanBatDau = "2026-11-02";
};
class AdminFileManager {
public:
    static void Luu(const std::string& DuongDan, const AdminData& DuLieu);
    static AdminData Doc(const std::string& DuongDan);
};
