#include "model/Patient.h"

#include <cassert>
#include <string>

#include "model/Date.h"
#include "model/Gender.h"
#include "util/My_stdexcept.h"

int main() {
    // Có CCCD không có nghĩa đã biết ngày sinh; giữ cả các số 0 đầu.
    Patient CoCanCuoc("BN_CCCD", "Le An", Gender::Nu, "001234567890");
    assert(CoCanCuoc.LaySoCanCuoc() == "001234567890");
    assert(!CoCanCuoc.CoNgaySinh());
    Patient ChiBietCanCuoc("BN_CCCD2", "", Gender::ChuaXacDinh, "009876543210");
    assert(ChiBietCanCuoc.LaySoCanCuoc() == "009876543210");
    assert(ChiBietCanCuoc.LaBenhNhanVoDanh());
    Patient DuThongTin("BN_CCCD3", "Le B", Gender::Nam, Date{1, 1, 2000}, "001111111111");
    assert(DuThongTin.LaySoCanCuoc() == "001111111111" && DuThongTin.CoNgaySinh());
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
