#include "repository/ScheduleFileManager.h"
#include "repository/AdminFileManager.h"
#include "service/SchedulerService.h"
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "model/Visit.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <sstream>

template<class F> void PhaiLoiTai(int Dong, F Ham) {
    bool Loi=false; try { Ham(); } catch (const My_stdexcept&) { Loi=true; } if (!Loi) std::fprintf(stderr,"Thieu loi tai dong %d\n",Dong); assert(Loi);
}
#define PhaiLoi(...) PhaiLoiTai(__LINE__, __VA_ARGS__)
int main() {
    // 00:00 thứ Hai 02/11/2026, UTC+7.
    constexpr std::time_t Tuan=1793552400;
    const auto C=Shift::TuThoiDiem("S",Tuan+7*3600,Tuan+15*3600,1,1,true);
    assert(C.HopLe() && C.TinhSoGio()==8 && !C.LaCaDem() && !C.LaCaCuoiTuan());
    assert(C.LayPhutBatDau()==420 && C.LayPhutKetThuc()==900);
    auto Ke=Shift::TuThoiDiem("K",C.LayKetThuc(),C.LayKetThuc()+3600,1,0,false);
    assert(!C.CoChongGio(Ke));
    auto Trung=Shift::TuThoiDiem("K",C.LayKetThuc()-60,C.LayKetThuc()+3600,1,0,false);
    assert(C.CoChongGio(Trung));
    auto Dem=Shift::TuThoiDiem("D",Tuan+6*86400+22*3600,Tuan+7*86400+6*3600,1,0,true);
    assert(Dem.LaCaDem() && Dem.LaCaCuoiTuan() && Dem.TinhSoGio()==8);
    PhaiLoi([&] { Shift::TuThoiDiem("X",Tuan,Tuan,1,0,false); });
    PhaiLoi([&] { Shift::TuThoiDiem("X",Tuan,Tuan+86460,1,0,false); });
    PhaiLoi([&] { Shift::TuThoiDiem("X",Tuan+1,Tuan+3600,1,0,false); });
    ShiftAssignment P("P","B1","S",AssignmentRole::TruongCa);
    PhaiLoi([&] { P.GhiNhanBatDau(C.LayBatDau()); });
    P.XacNhan(); P.XacNhan(); P.GhiNhanBatDau(C.LayBatDau());
    PhaiLoi([&] { P.Huy("nghi"); });
    PhaiLoi([&] { P.GhiNhanBatDau(C.LayBatDau()); });
    ShiftAssignment H("H","B2","S",AssignmentRole::BacSi);
    PhaiLoi([&] { H.Huy("  "); }); H.Huy("Nghi om");
    PhaiLoi([&] { H.XacNhan(); });
    StaffRegistry N;
    N.ThemNhanVien(BacSi("B1","An",CapBac::BacSiChinh,5,"Cap cuu"));
    N.ThemNhanVien(BacSi("B2","Binh",CapBac::BacSiChinh,5,"Cap cuu"));
    N.ThemNhanVien(BacSi("B3","Chi",CapBac::BacSiChinh,5,"Noi"));
    N.ThemNhanVien(DieuDuong("D1","Lan",CapBac::DieuDuongChinh,5));
    assert(N.TimNhanVien("B1"));
    ScheduleRegistry R; R.ThemCaTruc(C);
    PhaiLoi([&] { R.ThemCaTruc(C); });
    PhaiLoi([&] { R.ThemCaTruc(Shift::TuThoiDiem("TuanSau",Tuan+604800,Tuan+604800+3600)); });
    PhaiLoi([&] { R.ThemPhanCong(ShiftAssignment("X","KhongCo","S",AssignmentRole::BacSi),N); });
    auto DeXuat=SchedulerService(N,R).LapLichDuKien();
    assert(DeXuat.KichThuoc()==2 && R.SoPhanCong()==0); // Đề xuất không tự sửa registry.
    for (int I=0; I<DeXuat.KichThuoc(); ++I) R.ThemPhanCong(DeXuat[I],N);
    const auto Ma=R.LayPhanCong(0).LayMaPhanCong();
    const auto DiaChi=R.TimPhanCong(Ma);
    assert(SchedulerService(N,R).LapLichDuKien().Rong());
    auto Thay=SchedulerService(N,R).TimNguoiThayCa(Ma);
    assert(Thay.KichThuoc()==1 && Thay[0]->LayMaDinhDanh()=="B2");
    StandbyPool Rong; assert(SchedulerService(N,R).TimNguoiThayCa(Ma,&Rong).Rong());
    R.XacNhanPhanCong(Ma,N); assert(R.TimPhanCong(Ma)==DiaChi);
    PhaiLoi([&] { R.GhiNhanBatDau(Ma,C.LayBatDau()-1); });
    auto Tang=SchedulerService(N,R).DeXuatTangCuong("S",1,0);
    assert(Tang.KichThuoc()==1 && Tang[0].MaNhanVien!="B1" && R.SoPhanCong()==2);
    PhaiLoi([&] { SchedulerService(N,R).DeXuatTangCuong("S",0,1); });
    PhaiLoi([&] { SchedulerService(N,R).DeXuatTangCuong("S",2,0,1); });
    R.HuyPhanCong(Ma,"Nghi dot xuat");
    assert(R.LayLichHieuLuc().PhanCong.KichThuoc()==1);
    auto Bu=SchedulerService(N,R).LapLichDuKien();
    assert(Bu.KichThuoc()==1 && Bu[0].LayMaPhanCong()!=Ma);
    R.ThemPhanCong(Bu[0],N); R.XacNhanPhanCong(Bu[0].LayMaPhanCong(),N);
    R.GhiNhanBatDau(Bu[0].LayMaPhanCong(),C.LayBatDau());
    PhaiLoi([&] { SchedulerService(N,R).TimNguoiThayCa(Bu[0].LayMaPhanCong()); });
    const auto ThuMuc=std::filesystem::temp_directory_path()/("pbl2-uml-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(ThuMuc); const auto Tep=(ThuMuc/"lich.txt").string();
    ScheduleFileManager::LuuLichTruc(Tep,R); ScheduleFileManager::LuuLichTruc(Tep,R);
    auto Doc=ScheduleFileManager::DocLichTruc(Tep,N);
    assert(Doc->SoPhanCong()==3 && Doc->TimCaTruc("S")->LayBatDau()==C.LayBatDau());
    assert(Doc->TimPhanCong(Ma)->LayLyDoHuy()=="Nghi dot xuat");
    assert(Doc->TimPhanCong(Bu[0].LayMaPhanCong())->LayThoiDiemBatDau()==C.LayBatDau());
    StaffRegistry Thieu; PhaiLoi([&] { ScheduleFileManager::DocLichTruc(Tep,Thieu); });
    { std::ofstream F(Tep,std::ios::app); F<<"PC hong\n"; }
    PhaiLoi([&] { ScheduleFileManager::DocLichTruc(Tep,N); });
    assert(Doc->SoPhanCong()==3); // Registry đang dùng không bị lỗi file phá hủy.
    { std::ofstream F(Tep); F<<"PBL2_SCHEDULE 1\nCA hong\nEND\n"; }
    PhaiLoi([&] { ScheduleFileManager::DocLichTruc(Tep,N); });
    { std::ofstream F(Tep); F<<"PBL2_SCHEDULE 1\n"; }
    PhaiLoi([&] { ScheduleFileManager::DocLichTruc(Tep,N); });
    std::istringstream Sai("\"P\" \"B1\" \"S\" 0 0 10 \"\"");
    PhaiLoi([&] { ScheduleFileManager::DocPhanCong(Sai); });
    // Snapshot v4 giữ trạng thái; không mất lý do hủy hoặc thời điểm bắt đầu.
    AdminData A;
    for (int I=0; I<N.KichThuoc(); ++I) A.NhanSu->Them(N.Lay(I));
    A.Lich.CacCa.ThemCuoi(C);
    for (int I=0; I<R.SoPhanCong(); ++I) A.Lich.PhanCong.ThemCuoi(R.LayPhanCong(I));
    AdminFileManager::Luu(Tep,A); auto AD=AdminFileManager::Doc(Tep);
    assert(AD.Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DaHuy);
    assert(AD.Lich.PhanCong[2].LayTrangThai()==AssignmentStatus::DangTruc);
    // Visit lưu vai trò/thời điểm, phân công trước không được tính là đã khám.
    Patient BenhNhan("BN","Nguoi benh"); BenhNhan.DatTuoiUocLuong(30);
    Visit V("V","BN",Tuan,"Dau dau");
    V.PhanCongBacSi("B1"); assert(V.SoLanBacSiThamGiaKham()==0);
    V.DanhGia(BenhNhan,VitalSigns(80,120,18,98,15),Tuan,"D1");
    V.BatDauKham(DoctorParticipation("B1",Tuan+60));
    V.GhiNhanBacSiThamGiaKham(DoctorParticipation("B2",Tuan+120,DoctorRole::ThayThe,"B1"));
    assert(V.LayThamGiaTai(1).LayVaiTro()==DoctorRole::ThayThe && V.LayThamGiaTai(1).LayMaBacSiDuocThay()=="B1");
    PhaiLoi([&] { V.GhiNhanBacSiThamGiaKham(DoctorParticipation("B3",Tuan+100)); });
    PhaiLoi([&] { V.GhiNhanBacSiThamGiaKham(DoctorParticipation("B3",Tuan+180,DoctorRole::ThayThe,"KhongCo")); });
    PhaiLoi([&] { DoctorParticipation("B1",Tuan,DoctorRole::ThayThe,"B1"); });
    std::filesystem::remove_all(ThuMuc);
}
