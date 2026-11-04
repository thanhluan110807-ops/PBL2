// Phần B — Chức năng chính: Quản lý ca và phân công trong RAM, kiểm tra liên kết và trạng thái.
#pragma once
#include "model/Shift.h"
#include "dsa/HashTable.h"
#include "repository/StaffRegistry.h"

// Registry sở hữu các đối tượng; con trỏ trả ra chỉ được mượn để đọc.
// Không copy: tránh hai registry giải phóng cùng một địa chỉ.
class ScheduleRegistry {
    HashTable<std::string, Shift*> CacCaTruc;
    HashTable<std::string, ShiftAssignment*> CacPhanCong;
    Vector<Shift*> DanhSachCa;
    Vector<ShiftAssignment*> DanhSachPhanCong;
public:
    ScheduleRegistry() = default;
    ~ScheduleRegistry();
    ScheduleRegistry(const ScheduleRegistry&) = delete;
    ScheduleRegistry& operator=(const ScheduleRegistry&) = delete;
    void ThemCaTruc(const Shift& Ca);
    void ThemPhanCong(const ShiftAssignment& PhanCong, const StaffRegistry& NhanSu);
    const Shift* TimCaTruc(const std::string& Ma) const;
    const ShiftAssignment* TimPhanCong(const std::string& Ma) const;
    int SoCa() const { return DanhSachCa.KichThuoc(); }
    int SoPhanCong() const { return DanhSachPhanCong.KichThuoc(); }
    const Shift& LayCa(int I) const { return *DanhSachCa[I]; }
    const ShiftAssignment& LayPhanCong(int I) const { return *DanhSachPhanCong[I]; }
    Schedule LayLichHieuLuc() const;
    void XacNhanPhanCong(const std::string& Ma, const StaffRegistry& NhanSu);
    void HuyPhanCong(const std::string& Ma, const std::string& LyDo);
    void GhiNhanBatDau(const std::string& Ma, std::time_t ThoiDiem);
};
