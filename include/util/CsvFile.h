#pragma once

#include <istream>
#include <ostream>
#include <string>

#include "dsa/Vector.h"


namespace CsvFile {
bool DocBanGhi(std::istream& Tep, Vector<std::string>& CacTruong,
               int& DongHienTai, int& DongBatDau);
void BoQuaBom(std::istream& Tep);
void GhiBanGhi(std::ostream& Tep, const Vector<std::string>& CacTruong);


void DongBoThuMuc(const std::string& DuongDan);

void XoaFileDaDongBo(const std::string& DuongDan);


void GhiFileAnToan(const std::string& DuongDan, const std::string& NoiDung);
}
