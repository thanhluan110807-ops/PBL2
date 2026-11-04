#include "repository/StaffRegistry.h"
#include "model/BacSi.h"
#include "util/My_stdexcept.h"
StaffRegistry::~StaffRegistry() {
    for (int I = 0; I < DanhSach.KichThuoc(); ++I) delete DanhSach[I];
}
void StaffRegistry::Them(const NhanVienYTe& NhanVien) {
    const std::string& Ma = NhanVien.LayMaDinhDanh();
    if (ChiMuc.Cokhoa(Ma))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Trung ma nhan vien");
    NhanVienYTe* Moi = NhanVien.SaoChep();
    try {
        DanhSach.ThemCuoi(Moi);
        try { ChiMuc.Them(Ma, Moi); }
        catch (...) { DanhSach.XoaCuoi(); throw; }
    } catch (...) { delete Moi; throw; }
}
const NhanVienYTe* StaffRegistry::Tim(const std::string& Ma) const {
    NhanVienYTe* const* O = ChiMuc.Timkiem(Ma);
    return O == nullptr ? nullptr : *O;
}

void StaffRegistry::CapNhat(const NhanVienYTe& NhanVien) {
    NhanVienYTe** O = ChiMuc.Timkiem(NhanVien.LayMaDinhDanh());
    if (!O) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Khong tim thay nhan vien");
    (*O)->CapNhatTu(NhanVien); // Giữ địa chỉ cho các kết quả tra cứu đang mượn.

}
bool StaffRegistry::Xoa(const std::string& Ma) {
    NhanVienYTe** O = ChiMuc.Timkiem(Ma);
    if (!O) return false;
    NhanVienYTe* Cu = *O;
    Vector<NhanVienYTe*> ConLai;
    for (int I = 0; I < DanhSach.KichThuoc(); ++I) if (DanhSach[I] != Cu) ConLai.ThemCuoi(DanhSach[I]);
    ChiMuc.Xoa(Ma);
    DanhSach.HoanDoi(ConLai);
    delete Cu;
    return true;
}

Vector<const BacSi*> StaffRegistry::TimBacSiTheoTen(const std::string& Ten) const {
    Vector<const BacSi*> KetQua;
    if (Ten.find_first_not_of(" \t\r\n")==std::string::npos) return KetQua;
    // Tìm chuỗi con UTF-8, phân biệt hoa/thường và dấu; không tự đoán chuẩn hóa tên.
    for (int I=0;I<DanhSach.KichThuoc();++I) {
        const auto* NV=DanhSach[I];
        if (NV->LayLoaiNhanVien()==StaffType::BacSi && NV->LayHoTen().find(Ten)!=std::string::npos)
            KetQua.ThemCuoi(static_cast<const BacSi*>(NV));
    }
    return KetQua;
}
