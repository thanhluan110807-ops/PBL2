#pragma once
#include "service/FatigueService.h"
#include "repository/StaffRegistry.h"
#include "model/Shift.h"
#include "repository/StandbyPool.h"

struct SchedulerRules {
    double GioToiDaTuan = 48;
    double DiemMetMoiToiDa = 100;
    int NghiGiuaCaPhut = 12 * 60;
    int NghiSauCaDemPhut = 24 * 60;
};
enum class ScheduleStatus { ThanhCong, KhongCoLich, HetLuotTim };
struct ScheduleResult {
    ScheduleStatus TrangThai = ScheduleStatus::KhongCoLich;
    Schedule Lich;
    int SoLuotThu = 0;
};

class ScheduleRegistry;
class SchedulerService {
    const StaffRegistry* NhanSu = nullptr;
    const ScheduleRegistry* Lich = nullptr;
public:
    SchedulerService() = default;
    SchedulerService(const StaffRegistry& N, const ScheduleRegistry& L) : NhanSu(&N), Lich(&L) {}
    // Chỉ đề xuất; người dùng thêm/xác nhận qua Registry sau khi xem kết quả.
    Vector<ShiftAssignment> LapLichDuKien(int GioiHanLuot = 200000) const;
    Vector<ShiftAssignment> DeXuatTangCuong(const std::string& MaCa, int ThemBacSi, int ThemDieuDuong,
                                          int GioiHanLuot = 200000) const;
    Vector<const NhanVienYTe*> TimNguoiThayCa(const std::string& MaPhanCong,
                                           const StandbyPool* DuPhong = nullptr) const;
    static bool CoThePhanCong(const NhanVienYTe& NV, const Shift& Ca,
                             AssignmentRole VaiTro, const Schedule& Lich, const SchedulerRules& Luat);
    static ScheduleResult LapLich(const StaffRegistry& NhanSu, const Vector<Shift>& CacCa,
                                  const SchedulerRules& Luat = {}, int GioiHanLuot = 200000);
    static bool LichHopLe(const StaffRegistry& NhanSu, const Schedule& Lich, const SchedulerRules& Luat = {});
    // Chỉ trả ứng viên đủ điều kiện; không tự đổi lịch đã xác nhận.
    static Vector<std::string> TimNguoiThay(const StaffRegistry& NhanSu, const Schedule& Lich,
                                          int ViTriPhanCong, const SchedulerRules& Luat = {},
                                          const StandbyPool* DuPhong = nullptr);
};
