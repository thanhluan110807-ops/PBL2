#pragma once
#include <string>

struct NhanVienMau {
    std::string Ma;
    std::string HoTen;
    int KinhNghiem = 0;
};

// Mảng cố định giúp tập trung học đọc file trước khi dùng Vector/HashTable.
struct DanhSachMau {
    NhanVienMau NhanVien[10];
    int SoLuong = 0;
};

bool DocNhanVien(const std::string& TenFile, DanhSachMau& KetQua);
