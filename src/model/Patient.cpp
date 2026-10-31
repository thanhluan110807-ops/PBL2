#include "model/Patient.h"
#include "util/My_stdexcept.h"

Patient::Patient(const std::string& MaDinhDanh, const std::string& HoTen, Gender GioiTinh)
    : Person(MaDinhDanh, HoTen, GioiTinh),
      TrangThaiDanhTinh(HoTen.empty() ? IdentityStatus::ChuaXacDinh : IdentityStatus::DaXacDinh) {}

Patient::Patient(const std::string& MaDinhDanh, const std::string& HoTen,
                 Gender GioiTinh, const Date& NgaySinh)
    : Person(MaDinhDanh, HoTen, GioiTinh, NgaySinh),
      TrangThaiDanhTinh(HoTen.empty() ? IdentityStatus::ChuaXacDinh : IdentityStatus::DaXacDinh) {}

bool Patient::LaBenhNhanVoDanh() const { return TrangThaiDanhTinh == IdentityStatus::ChuaXacDinh; }
IdentityStatus Patient::LayTrangThaiDanhTinh() const { return TrangThaiDanhTinh; }
const std::string& Patient::LaySoCanCuoc() const { return SoCanCuoc; }
BloodGroup Patient::LayNhomMau() const { return NhomMau; }
const std::string& Patient::LayThongTinDiUng() const { return ThongTinDiUng; }
const std::string& Patient::LayTienSuBenh() const { return TienSuBenh; }
bool Patient::CoTuoiUocLuong() const { return DaCoTuoiUocLuong; }
int Patient::LayTuoiUocLuong() const {
    if (!DaCoTuoiUocLuong) throw My_stdexcept(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, "Chua co tuoi uoc luong");
    return TuoiUocLuong;
}
void Patient::DatTuoiUocLuong(int Tuoi) {
    if (Tuoi < 0 || Tuoi > 150) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Tuoi uoc luong khong hop le");
    TuoiUocLuong = Tuoi;
    DaCoTuoiUocLuong = true;
}
void Patient::XoaTuoiUocLuong() { DaCoTuoiUocLuong = false; }
void Patient::CapNhatNhomMau(BloodGroup NhomMau) {
    switch (NhomMau) {
        case BloodGroup::A: case BloodGroup::B: case BloodGroup::AB:
        case BloodGroup::O: case BloodGroup::ChuaXacDinh: this->NhomMau = NhomMau; return;
    }
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Nhom mau khong hop le");
}
void Patient::CapNhatThongTinDiUng(const std::string& ThongTin) { ThongTinDiUng = ThongTin; }
void Patient::CapNhatTienSuBenh(const std::string& ThongTin) { TienSuBenh = ThongTin; }
void Patient::BoSungDanhTinh(const std::string& SoCanCuoc, const std::string& HoTen,
                             Gender GioiTinh, const Date& NgaySinh) {
    if (HoTen.empty()) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ho ten khong duoc rong");
    // Tạo bản sao trước khi thay đổi Person để tránh cập nhật dở dang khi cấp phát lỗi.
    std::string CanCuocMoi = SoCanCuoc;
    CapNhatThongTinCaNhan(HoTen, GioiTinh, NgaySinh);
    this->SoCanCuoc.swap(CanCuocMoi);
    TrangThaiDanhTinh = IdentityStatus::DaXacDinh;
}
