#include "DocFile.h"
#include <fstream>
#include <sstream>

bool DocNhanVien(const std::string& TenFile, DanhSachMau& KetQua) {
    std::ifstream File(TenFile);
    if (!File.is_open()) return false; // Không tìm thấy hoặc không có quyền đọc.

    DanhSachMau Tam;
    std::string Dong;
    while (std::getline(File, Dong)) { // Mỗi lần lặp lấy đúng một dòng.
        if (!Dong.empty() && Dong.back() == '\r') Dong.pop_back();
        if (Dong.empty()) continue;
        if (Tam.SoLuong == 10) return false; // Không ghi vượt mảng.

        std::istringstream TachDong(Dong);
        NhanVienMau NV;
        std::string ChuoiKinhNghiem;
        if (!std::getline(TachDong, NV.Ma, ';')) return false;
        if (!std::getline(TachDong, NV.HoTen, ';')) return false;
        if (!std::getline(TachDong, ChuoiKinhNghiem)) return false;
        if (NV.Ma.empty() || NV.HoTen.empty()) return false;

        // Chuyển chuỗi sang số; không chấp nhận "5abc" hoặc "5;du-lieu-thua".
        std::istringstream TachSo(ChuoiKinhNghiem);
        if (!(TachSo >> NV.KinhNghiem)) return false;
        TachSo >> std::ws;
        if (!TachSo.eof() || NV.KinhNghiem < 0 || NV.KinhNghiem > 80) return false;

        for (int I = 0; I < Tam.SoLuong; ++I)
            if (Tam.NhanVien[I].Ma == NV.Ma) return false;
        Tam.NhanVien[Tam.SoLuong] = NV;
        ++Tam.SoLuong;
    }
    if (File.bad()) return false; // Lỗi đọc khác với hết file bình thường.

    // Chỉ thay danh sách thật khi tất cả các dòng đều đọc thành công.
    KetQua = Tam;
    return true;
} // ifstream tự đóng file khi ra khỏi hàm.
