#include "model/CapBac.h"
#include "model/CaTrucMongMuon.h"
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "service/FatigueService.h"
#include "repository/StaffFileManager.h"
#include "repository/AdminFileManager.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <limits>

template<class F> void PhaiLoi(F Ham) {
    bool Loi=false; try { Ham(); } catch(const My_stdexcept&) { Loi=true; } assert(Loi);
}
bool GanBang(double A,double B) { return std::fabs(A-B)<0.000001; }
int main() {
    for (int I=0;I<5;++I) {
        auto Bac=static_cast<CapBac>(I);
        assert(ChuoiThanhCapBac(CapBacThanhChuoi(Bac))==Bac);
    }
    PhaiLoi([] { ChuoiThanhCapBac("sai"); });
    PhaiLoi([] { CapBacThanhChuoi(static_cast<CapBac>(99)); });
    CaTrucMongMuon Dem{22,6,true}; assert(Dem.HopLe());
    PhaiLoi([] { BacSi B("B","An",CapBac::BacSiChinh,5,"Noi",0,{7,7,false}); });
    BacSi BS("B","An",CapBac::BacSiChinh,5,"Noi");
    BS.DatHoTen("Nguyễn An"); BS.DatSoNamKinhNghiem(8); BS.DatChuyenKhoa("Cấp cứu"); BS.DatCaMongMuon(Dem);
    assert(BS.LayHoTen()=="Nguyễn An" && BS.CoTheLamTruongCa());
    PhaiLoi([&] { BS.DatCapBac(CapBac::DieuDuongTre); });
    PhaiLoi([&] { BS.DatSoNamKinhNghiem(-1); }); assert(BS.LaySoNamKinhNghiem()==8);
    BS.CongDonGioLam(8); BS.GhiNhanCaTruc(true,true);
    assert(BS.LayGioLamTuan()==8 && BS.LaySoCaDemThang()==1 && BS.LaySoCaCuoiTuanThang()==1);
    assert(BS.CanNghiNgoiBatBuoc());
    FatigueService::CapNhatTheoCongThucGoc(BS,1); assert(GanBang(BS.LayDiemMetMoi(),2.46));
    assert(BS.VuotNguongMetMoi(2) && !BS.VuotNguongMetMoi(BS.LayDiemMetMoi()));
    PhaiLoi([&] { BS.CongDonGioLam(std::numeric_limits<double>::infinity()); });
    BacSi BanSao(BS); BanSao.DatHoTen("Ban sao"); assert(BS.LayHoTen()=="Nguyễn An");
    assert(BanSao.LayGioLamTuan()==8 && BanSao.LaySoCaDemThang()==1);
    BS.DatLaiTuanMoi(); assert(BS.LayGioLamTuan()==0 && BS.LaySoCaDemThang()==1);
    BacSi NT("NT","Binh",CapBac::BacSiChinh,1,"Noi",true,0);
    assert(NT.LaBacSiNoiTru() && !NT.CoTheLamTruongCa());
    NT.CongDonGioLam(8); NT.GhiNhanCaTruc(true,false);
    FatigueService::CapNhatTheoCongThucGoc(NT,1); assert(GanBang(NT.LayDiemMetMoi(),2.952));
    DieuDuong DD("D","Lan",CapBac::DieuDuongChinh,4);
    DD.CongDonGioSangLoc(1); assert(DD.LayGioSangLocLienTuc()==0);
    DD.BatDauTrucSangLoc(); DD.CongDonGioSangLoc(3.99); assert(!DD.CanXoayTuaSangLoc());
    DD.BatDauTrucSangLoc(); assert(DD.LayGioSangLocLienTuc()==3.99);
    DD.CongDonGioSangLoc(0.01); assert(DD.CanXoayTuaSangLoc());
    DD.CongDonGioLam(8); FatigueService::CapNhatTheoCongThucGoc(DD,1);
    assert(GanBang(DD.LayDiemMetMoi(),4.4));
    PhaiLoi([&] { FatigueService::CapNhatTheoCongThucGoc(DD,-1); });
    DD.CapNhatThongTinCaNhan("Lê Lan",Gender::Nu,Date{1,1,2000});
    AdminData D; D.NhanSu->Them(BS); D.NhanSu->Them(NT); D.NhanSu->Them(DD);
    DD.KetThucTrucSangLoc(); assert(!DD.DangSangLoc() && DD.LayGioSangLocLienTuc()==0);
    const auto ThuMuc=std::filesystem::temp_directory_path()/("staff-split-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(ThuMuc);
    const auto Tep=(ThuMuc/"staff.txt").string();
    StaffFileManager::LuuNhanVien(Tep,*D.NhanSu);
    StaffFileManager::LuuNhanVien(Tep,*D.NhanSu);
    auto Doc=StaffFileManager::DocNhanVien(Tep);
    assert(Doc->KichThuoc()==3);
    const auto* DocDD=static_cast<const DieuDuong*>(Doc->Tim("D"));
    assert(DocDD && DocDD->DangSangLoc() && DocDD->LayGioSangLocLienTuc()==4);
    assert(DocDD->LayGioiTinh()==Gender::Nu && DocDD->LayNgaySinh().Nam==2000);
    assert(GanBang(DocDD->LayDiemMetMoi(),4.4) && DocDD->LayGioLamTuan()==8);
    assert(static_cast<const BacSi*>(Doc->Tim("NT"))->LaBacSiNoiTru());
    const auto Tong=(ThuMuc/"admin.txt").string();
    AdminFileManager::Luu(Tong,D); auto Lai=AdminFileManager::Doc(Tong);
    assert(Lai.NhanSu->Tim("D")->LayGioLamTuan()==8);
    assert(static_cast<const DieuDuong*>(Lai.NhanSu->Tim("D"))->DangSangLoc());
    std::istringstream Cu("0 \"B0\" \"Cu\" 3 1 0 \"Noi\" 7 15 0");
    StaffRegistry TuBanCu; StaffFileManager::DocDong(Cu,TuBanCu,1);
    assert(TuBanCu.Tim("B0")->LayGioLamTuan()==0);
    { std::ofstream F(Tep); F<<"PBL2_STAFF 2\nNV hong\nEND\n"; }
    PhaiLoi([&] { StaffFileManager::DocNhanVien(Tep); });
    assert(Doc->KichThuoc()==3); // File lỗi không thay registry đã đọc tốt.
    std::filesystem::remove_all(ThuMuc);
}
