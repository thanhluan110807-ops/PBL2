
#pragma once
#include "dsa/Vector.h"
#include <string>
#include <ctime>
#include "model/ShiftAssignment.h"



class Shift {
    std::string MaCa;
    std::time_t BatDau = 0, KetThuc = 0;
public:
    Shift() = default;

    Shift(const std::string& Ma, int Dau, int Cuoi, int BS = 1, int DD = 1, bool Truong = true);
    static Shift TuThoiDiem(const std::string& Ma, std::time_t Dau, std::time_t Cuoi,
                           int BS = 1, int DD = 1, bool Truong = true);
    const std::string& LayMaCaTruc() const { return MaCa; }
    std::time_t LayBatDau() const { return BatDau; }
    std::time_t LayKetThuc() const { return KetThuc; }
    int LayPhutBatDau() const;
    int LayPhutKetThuc() const;
    bool LaCaCuoiTuan() const { return LaCuoiTuan(); }
    int SoBacSi = 1;
    int SoDieuDuong = 1;
    bool CanTruongCa = true;
    bool HopLe() const;
    bool LaCaDem() const;
    bool CoChongGio(const Shift& CaKhac) const;
    bool LaCuoiTuan() const;
    double TinhSoGio() const { return std::difftime(KetThuc, BatDau) / 3600.0; }
};
struct Schedule {
    Vector<Shift> CacCa;
    Vector<ShiftAssignment> PhanCong;
};
