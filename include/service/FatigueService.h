
#pragma once
#include "model/NhanVienYTe.h"
#include "model/Shift.h"

class FatigueService {
public:

    static double DiemCa(const Shift& Ca);
    static double TinhDiem(const NhanVienYTe& NV, const Schedule& Lich);
    static double TinhGio(const std::string& Ma, const Schedule& Lich);


    static void CapNhatTheoCongThucGoc(NhanVienYTe& NhanVien, double HeSoApLuc);
};
