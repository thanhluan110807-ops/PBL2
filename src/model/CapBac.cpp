// Phần B — Chức năng chính: Định nghĩa cấp bậc và loại nhân viên.
#include "model/CapBac.h"
#include "util/My_stdexcept.h"
std::string CapBacThanhChuoi(CapBac Bac) {
    switch (Bac) {
        case CapBac::DieuDuongTre: return "Dieu_duong_tre";
        case CapBac::DieuDuongChinh: return "Dieu_duong_chinh";
        case CapBac::BacSiNoiTru: return "Bac_si_noi_tru";
        case CapBac::BacSiChinh: return "Bac_si_chinh";
        case CapBac::TruongKhoa: return "Truong_khoa";
    }
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Cap bac khong hop le");
}
CapBac ChuoiThanhCapBac(const std::string& Chuoi) {
    if (Chuoi == "Dieu_duong_tre") return CapBac::DieuDuongTre;
    if (Chuoi == "Dieu_duong_chinh") return CapBac::DieuDuongChinh;
    if (Chuoi == "Bac_si_noi_tru") return CapBac::BacSiNoiTru;
    if (Chuoi == "Bac_si_chinh") return CapBac::BacSiChinh;
    if (Chuoi == "Truong_khoa") return CapBac::TruongKhoa;
    // Không âm thầm biến dữ liệu sai thành một điều dưỡng trẻ.
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Chuoi cap bac khong hop le");
}
