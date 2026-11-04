#pragma once
#include "model/NhanVienYTe.h"
#include "model/Shift.h"

class FatigueService {
public:
    // Điểm mô phỏng: ca mới chỉ được tính một lần từ danh sách phân công.
    static double DiemCa(const Shift& Ca);
    static double TinhDiem(const NhanVienYTe& NV, const Schedule& Lich);
    static double TinhGio(const std::string& Ma, const Schedule& Lich);
    // Công thức cũ của Staff được chuyển nguyên ý nghĩa sang service riêng.
    // Mỗi lần gọi là một lần cập nhật; không gọi khi chỉ xem bảng hoặc thử lịch.
    static void CapNhatTheoCongThucGoc(NhanVienYTe& NhanVien, double HeSoApLuc);
};
