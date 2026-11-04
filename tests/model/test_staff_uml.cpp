#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "repository/StaffFileManager.h"
#include "repository/AdminFileManager.h"
#include "service/SchedulerService.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <sstream>

template<class F> void PhaiLoi(F Ham) {
    bool Loi=false; try { Ham(); } catch(const My_stdexcept&) { Loi=true; } assert(Loi);
}
int main() {
    StaffRegistry R;
    BacSi A("A","Nguyễn An",CapBac::BacSiChinh,5,"Noi");
    BacSi B("B","Nguyễn An",CapBac::BacSiChinh,6,"Noi");
    R.Them(A); R.Them(B); R.Them(DieuDuong("D","Nguyễn An",CapBac::DieuDuongTre,1));
    auto DS=R.TimBacSiTheoTen("Nguyễn An");
    assert(DS.KichThuoc()==2 && R.TimBacSiTheoTen("An").KichThuoc()==2);
    assert(R.TimBacSiTheoTen("").Rong() && R.TimBacSiTheoTen("Khong co").Rong());
    const auto* DiaChiCu=R.Tim("A");
    A.DatHoTen("Tên mới"); A.DatChuyenKhoa("Ngoai"); A.KhoiPhucTrangThai({12,8,1,2,true});
    A.CapNhatThongTinCaNhan("Tên mới",Gender::Nam,Date{1,1,1990});
    R.CapNhat(A);
    assert(R.Tim("A")==DiaChiCu && DS[0]->LayHoTen()=="Tên mới");
    assert(DS[0]->LayChuyenKhoa()=="Ngoai" && DS[0]->LayGioLamTuan()==8);
    assert(DS[0]->LayNgaySinh().Nam==1990);
    PhaiLoi([&] { R.CapNhat(DieuDuong("A","Sai loai",CapBac::DieuDuongChinh,5)); });
    assert(R.Tim("A")==DiaChiCu && R.Tim("A")->LayHoTen()=="Tên mới");
    R.CapNhat(*R.Tim("A")); assert(R.Tim("A")==DiaChiCu); // Tự cập nhật.
    assert(R.TimBacSiTheoTen("Nguyễn An").KichThuoc()==1);

    DieuDuong DD("D0","Lan",CapBac::DieuDuongChinh,4);
    const std::time_t Dau=100000;
    DD.BatDauTrucSangLoc(Dau);
    assert(!DD.CanXoayTuaSangLoc(Dau+4*3600-1));
    assert(DD.CanXoayTuaSangLoc(Dau+4*3600));
    DD.BatDauTrucSangLoc(Dau+3600);
    assert(DD.LayBatDauSangLoc()==Dau && DD.LayGioSangLocLienTuc()==1);
    DD.CapNhatThoiGianSangLoc(Dau+7200);
    assert(DD.LayGioSangLocLienTuc()==2);
    PhaiLoi([&] { DD.CapNhatThoiGianSangLoc(Dau+7199); });
    PhaiLoi([&] { DD.CongDonGioSangLoc(1); });
    assert(DD.LayGioSangLocLienTuc()==2);
    PhaiLoi([&] { DD.KhoiPhucSangLoc(true,99,true,Dau,Dau+7200); });
    assert(DD.LayGioSangLocLienTuc()==2);
    R.Them(DD);
    const auto* DDCu=R.Tim("D0");
    DD.CapNhatThoiGianSangLoc(Dau+10800); R.CapNhat(DD);
    assert(R.Tim("D0")==DDCu);
    assert(static_cast<const DieuDuong*>(DDCu)->LayMocDongHo()==Dau+10800);

    const auto ThuMuc=std::filesystem::temp_directory_path()/("staff-uml-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(ThuMuc);
    const auto Tep=(ThuMuc/"staff.txt").string();
    StaffFileManager::LuuNhanVien(Tep,R);
    auto Doc=StaffFileManager::DocNhanVien(Tep);
    const auto* DocDD=static_cast<const DieuDuong*>(Doc->Tim("D0"));
    assert(DocDD->LayBatDauSangLoc()==Dau && DocDD->CanXoayTuaSangLoc(Dau+14400));
    assert(DocDD->TinhThoiGianSangLocLienTuc(Dau+14400)==4);
    AdminData Data; Data.NhanSu=std::move(Doc);
    const auto Tong=(ThuMuc/"admin.txt").string();
    AdminFileManager::Luu(Tong,Data); auto Nap=AdminFileManager::Doc(Tong);
    assert(static_cast<const DieuDuong*>(Nap.NhanSu->Tim("D0"))->LayBatDauSangLoc()==Dau);
    DD.KetThucTrucSangLoc(Dau+14400);
    assert(!DD.DangSangLoc() && DD.TinhThoiGianSangLocLienTuc(Dau+15000)==0);
    DD.BatDauTrucSangLoc(); DD.CongDonGioSangLoc(4); assert(DD.CanXoayTuaSangLoc());
    PhaiLoi([&] { DD.BatDauTrucSangLoc(Dau); }); // Không trộn hai chế độ.
    // V2 không có mốc đồng hồ: vẫn khôi phục giờ cộng tay.
    std::istringstream V2("1 \"V2\" \"Lan\" 1 4 0 \"\" 7 15 0 0 8 0 0 0 0 1 4 3 0 0 0 0");
    StaffRegistry Cu; StaffFileManager::DocDong(V2,Cu,2);
    assert(static_cast<const DieuDuong*>(Cu.Tim("V2"))->CanXoayTuaSangLoc());

    Shift Ca{"S",420,900,1,0,true}, Ke{"K",900,1000,1,0,true};
    assert(!Ca.CoChongGio(Ke)); Ke=Shift("Ke",899,Ke.LayPhutKetThuc(),Ke.SoBacSi,Ke.SoDieuDuong,Ke.CanTruongCa); assert(Ca.CoChongGio(Ke));
    Schedule Trong;
    A.KhoiPhucTrangThai({98,40,0,0,false});
    assert(!SchedulerService::CoThePhanCong(A,Ca,AssignmentRole::TruongCa,Trong,{})); // Điểm hiện tại.
    A.KhoiPhucTrangThai({0,41,0,0,false});
    assert(!SchedulerService::CoThePhanCong(A,Ca,AssignmentRole::TruongCa,Trong,{})); // Giờ đã làm.
    A.KhoiPhucTrangThai({0,40,0,0,false});
    assert(SchedulerService::CoThePhanCong(A,Ca,AssignmentRole::TruongCa,Trong,{}));
    std::filesystem::remove_all(ThuMuc);
}
