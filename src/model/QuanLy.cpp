#include "model/QuanLy.h"
#include <string>
#include "util/My_stdexcept.h"

QuanLy::QuanLy(const std::string& MaQuanLy, const std::string& HoTen)
    : MaQuanLy(MaQuanLy), HoTen(HoTen) {
    if (!MaHopLe(MaQuanLy) || HoTen.find_first_not_of(" \t\r\n\f\v") == std::string::npos)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ma hoac ho ten quan ly khong hop le");
}

bool QuanLy::MaHopLe(const std::string& MaQuanLy) {
    return !MaQuanLy.empty() && MaQuanLy.find_first_of(" \t\r\n\f\v") == std::string::npos &&
           MaQuanLy.find('\0') == std::string::npos;
}
