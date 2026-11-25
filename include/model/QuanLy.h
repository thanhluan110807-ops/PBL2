#pragma once
#include <string>


class QuanLy {
    std::string MaQuanLy;
    std::string HoTen;

public:
    QuanLy() = default;
    QuanLy(const std::string& MaQuanLy, const std::string& HoTen);
    static bool MaHopLe(const std::string& MaQuanLy);
    const std::string& LayMaQuanLy() const { return MaQuanLy; }
    const std::string& LayHoTen() const { return HoTen; }
};
