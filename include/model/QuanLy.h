#pragma once
#include <string>

// Người quản lý có mã riêng để ghi nhận ai xác nhận phân công trực.
class QuanLy {
    std::string MaQuanLy;
    std::string HoTen;

public:
    QuanLy() = default; // Ô trống của Vector, không đưa vào danh sách quản lý.
    QuanLy(const std::string& MaQuanLy, const std::string& HoTen);
    static bool MaHopLe(const std::string& MaQuanLy);
    const std::string& LayMaQuanLy() const { return MaQuanLy; }
    const std::string& LayHoTen() const { return HoTen; }
};
