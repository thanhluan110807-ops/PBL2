// Phần B — Chức năng chính: Định nghĩa cấp bậc và loại nhân viên.
#pragma once
#include <string>

// Enum là tập giá trị, không phải một lớp nhân viên mới.
enum class CapBac { DieuDuongTre, DieuDuongChinh, BacSiNoiTru, BacSiChinh, TruongKhoa };
enum class StaffType { BacSi, DieuDuong };
std::string CapBacThanhChuoi(CapBac Bac);
CapBac ChuoiThanhCapBac(const std::string& Chuoi);
