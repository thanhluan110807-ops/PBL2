#pragma once
#include <ctime>
#include <stdexcept>

// Một ngày lịch; không chứa giờ và không phụ thuộc Qt.
struct Date {
    int Ngay;
    int Thang;
    int Nam;

    bool HopLe() const {
        if (Nam < 1 || Nam > 9999 || Thang < 1 || Thang > 12) {
            return false;
        }
        const int SoNgayTrongThang[] = {
            31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        };
        int SoNgayToiDa = SoNgayTrongThang[Thang - 1];
        const bool LaNamNhuan =
            (Nam % 400 == 0) || (Nam % 4 == 0 && Nam % 100 != 0);
        if (Thang == 2 && LaNamNhuan) {
            SoNgayToiDa = 29;
        }
        return Ngay >= 1 && Ngay <= SoNgayToiDa;
    }

    bool Truoc(const Date& NgayKhac) const {
        if (Nam != NgayKhac.Nam) return Nam < NgayKhac.Nam;
        if (Thang != NgayKhac.Thang) return Thang < NgayKhac.Thang;
        return Ngay < NgayKhac.Ngay;
    }

    static Date HomNay() {
        const std::time_t ThoiGian = std::time(nullptr);
        const std::tm* Lich = std::localtime(&ThoiGian);
        if (Lich == nullptr) {
            throw std::runtime_error("Khong lay duoc ngay hien tai");
        }
        return {Lich->tm_mday, Lich->tm_mon + 1, Lich->tm_year + 1900};
    }
};
