#pragma once

#include <istream>
#include <ostream>
#include <string>

#include "dsa/Vector.h"

// Dùng chung CSV cho Patient và Visit; giữ nguyên dấu xuống dòng trong ô văn bản.
namespace CsvFile {
bool DocBanGhi(std::istream& Tep, Vector<std::string>& CacTruong,
               int& DongHienTai, int& DongBatDau);
void BoQuaBom(std::istream& Tep);
void GhiBanGhi(std::ostream& Tep, const Vector<std::string>& CacTruong);
// Đồng bộ thư mục POSIX để mục mới tạo/đổi tên tồn tại sau khi mất điện.
// Windows công bố file bằng MoveFileExW với WRITE_THROUGH.
void DongBoThuMuc(const std::string& DuongDan);
// Xóa dấu hoàn tất giao dịch rồi đồng bộ thư mục chứa nó.
void XoaFileDaDongBo(const std::string& DuongDan);
// Đồng bộ file tạm, đổi tên nguyên tử rồi đồng bộ thư mục đích.
// Lỗi sau đổi tên có thể đã công bố dữ liệu mới; bên gọi cần giữ RAM và thử lại.
void GhiFileAnToan(const std::string& DuongDan, const std::string& NoiDung);
} // namespace CsvFile
