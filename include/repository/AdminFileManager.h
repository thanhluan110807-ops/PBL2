
#pragma once
#include "repository/StaffRegistry.h"
#include "model/QuanLy.h"
#include "model/Shift.h"
#include "repository/StandbyPool.h"
#include "dsa/Vector.h"
#include <memory>
#include <string>



struct AdminData {
    std::unique_ptr<StaffRegistry> NhanSu = std::make_unique<StaffRegistry>();
    Vector<QuanLy> DanhSachQuanLy;
    Schedule Lich;
    StandbyPool DuPhong;
    std::string TuanBatDau = "2026-11-02";
    AdminData();
    const QuanLy* TimQuanLy(const std::string& Ma) const;
};
class AdminFileManager {
public:

    static void Luu(const std::string& DuongDan, const AdminData& DuLieu);
    static AdminData Doc(const std::string& DuongDan);
};
