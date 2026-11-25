
#pragma once
#include <string>


enum class CapBac { DieuDuongTre, DieuDuongChinh, BacSiNoiTru, BacSiChinh, TruongKhoa };
enum class StaffType { BacSi, DieuDuong };
std::string CapBacThanhChuoi(CapBac Bac);
CapBac ChuoiThanhCapBac(const std::string& Chuoi);
