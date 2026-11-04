// Phần B — Chức năng chính: Quản lý chuyên khoa, nội trú và khả năng làm trưởng ca.
#include "model/BacSi.h"
#include "util/My_stdexcept.h"
#include <utility>
BacSi::BacSi(const std::string& Ma, const std::string& Ten, CapBac Bac,
             int KinhNghiem, const std::string& ChuyenKhoa, double DiemNen, const CaTrucMongMuon& MongMuon)
    : BacSi(Ma, Ten, Bac, KinhNghiem, ChuyenKhoa, false, DiemNen, MongMuon) {}
BacSi::BacSi(const std::string& Ma, const std::string& Ten, CapBac Bac,
             int KinhNghiem, const std::string& ChuyenKhoa, bool NoiTru, double DiemNen, const CaTrucMongMuon& MongMuon)
    : NhanVienYTe(Ma, Ten, Bac, KinhNghiem, DiemNen, MongMuon), ChuyenKhoa(ChuyenKhoa), LaNoiTru(NoiTru) {
    DatCapBac(Bac); DatChuyenKhoa(ChuyenKhoa);
}
void BacSi::DatChuyenKhoa(const std::string& Ten) {
    if (Ten.find_first_not_of(" \t\r\n") == std::string::npos)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Chuyen khoa khong duoc rong");
    ChuyenKhoa = Ten;
}
bool BacSi::LaBacSiNoiTru() const { return LaNoiTru || LayCapBac() == CapBac::BacSiNoiTru; }
bool BacSi::CoTheLamTruongCa() const {
    return !LaBacSiNoiTru() && (LayCapBac() == CapBac::BacSiChinh || LayCapBac() == CapBac::TruongKhoa);
}

void BacSi::HoanDoiDuLieu(NhanVienYTe& Khac) {
    auto* BS=dynamic_cast<BacSi*>(&Khac);
    if (!BS) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Khong the doi bac si thanh dieu duong");
    HoanDoiThongTinChung(*BS);
    ChuyenKhoa.swap(BS->ChuyenKhoa); std::swap(LaNoiTru,BS->LaNoiTru);
}
