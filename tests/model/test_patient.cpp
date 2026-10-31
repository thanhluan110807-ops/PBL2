#include "model/Patient.h"
#include "util/My_stdexcept.h"
#include <cassert>

int main() {
    Patient VoDanh("BN001");
    assert(VoDanh.LaBenhNhanVoDanh());
    assert(VoDanh.LayHoTen().empty());
    assert(!VoDanh.CoNgaySinh());
    assert(VoDanh.LayNhomMau() == BloodGroup::ChuaXacDinh);
    assert(!VoDanh.CoTuoiUocLuong());
    VoDanh.DatTuoiUocLuong(25);
    assert(VoDanh.LayTuoiUocLuong() == 25);
    VoDanh.CapNhatNhomMau(BloodGroup::O);
    VoDanh.CapNhatThongTinDiUng("Penicillin");
    VoDanh.CapNhatTienSuBenh("Hen");
    VoDanh.BoSungDanhTinh("123456789012", "Nguyen Van A", Gender::Nam, Date{1, 1, 2000});
    assert(!VoDanh.LaBenhNhanVoDanh());
    assert(VoDanh.LaySoCanCuoc() == "123456789012");
    assert(VoDanh.LayNhomMau() == BloodGroup::O);
    assert(VoDanh.LayThongTinDiUng() == "Penicillin");
    assert(VoDanh.LayTienSuBenh() == "Hen");
    assert(VoDanh.LayMaDinhDanh() == "BN001");
    VoDanh.CapNhatThongTinCaNhan("Nguyen Van A", Gender::Nam, Date{1, 1, 2000});
    assert(VoDanh.LayMaDinhDanh() == "BN001");
    assert(VoDanh.LayHoTen() == "Nguyen Van A");
    assert(VoDanh.TinhTuoi(Date{1, 1, 2026}) == 26);

    Patient Nguoi("BN002", "Tran Thi B", Gender::Nu, Date{2, 3, 1990});
    assert(Nguoi.LayMaDinhDanh() == "BN002");
    assert(Nguoi.TinhTuoi(Date{1, 3, 2026}) == 35);
    bool DaBaoLoi = false;
    try { Patient KhongHopLe(""); }
    catch (const My_stdexcept& Loi) {
        DaBaoLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::DoiSoKhongHopLe;
    }
    assert(DaBaoLoi);
    DaBaoLoi = false;
    try { VoDanh.DatTuoiUocLuong(151); }
    catch (const My_stdexcept& Loi) { DaBaoLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::DoiSoKhongHopLe; }
    assert(DaBaoLoi && VoDanh.LayTuoiUocLuong() == 25);
    DaBaoLoi = false;
    try { VoDanh.CapNhatNhomMau(static_cast<BloodGroup>(99)); }
    catch (const My_stdexcept& Loi) { DaBaoLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::DoiSoKhongHopLe; }
    assert(DaBaoLoi && VoDanh.LayNhomMau() == BloodGroup::O);
    DaBaoLoi = false;
    try { VoDanh.BoSungDanhTinh("moi", "Ten Moi", Gender::Nu, Date{31, 2, 2000}); }
    catch (const My_stdexcept& Loi) { DaBaoLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::DoiSoKhongHopLe; }
    assert(DaBaoLoi && VoDanh.LaySoCanCuoc() == "123456789012" && VoDanh.LayHoTen() == "Nguyen Van A");
    VoDanh.XoaTuoiUocLuong();
    assert(!VoDanh.CoTuoiUocLuong());
}
