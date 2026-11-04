#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "repository/AdminFileManager.h"
#include "service/SchedulerService.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <chrono>

template<class F> void PhaiLoi(F Ham) {
    bool Loi = false;
    try { Ham(); } catch (const My_stdexcept&) { Loi = true; }
    assert(Loi);
}
int main() {
    PhaiLoi([] { BacSi N("B", "B", CapBac::DieuDuongTre, 1, "Noi"); });
    PhaiLoi([] { DieuDuong N("D", "D", CapBac::DieuDuongTre, -1); });
    PhaiLoi([] { DieuDuong N("D", "D", CapBac::DieuDuongTre, 0, std::numeric_limits<double>::quiet_NaN()); });
    AdminData D;
    BacSi B("B1", "Bác sĩ \"An\"; Nguyễn", CapBac::BacSiChinh, 8, "Cấp cứu");
    D.NhanSu->Them(B);
    D.NhanSu->Them(BacSi("B2", "Binh", CapBac::BacSiChinh, 7, "Cấp cứu"));
    D.NhanSu->Them(DieuDuong("D1", "Lan", CapBac::DieuDuongChinh, 4));
    D.NhanSu->Them(DieuDuong("D2", "Mai", CapBac::DieuDuongTre, 0));
    PhaiLoi([&] { D.NhanSu->Them(B); });
    assert(D.NhanSu->Tim("B1")->CoTheLamTruongCa());
    assert(!D.NhanSu->Tim("D1")->CoTheLamTruongCa());
    assert(D.NhanSu->Tim("khong-co") == nullptr);
    StaffRegistry Sua;
    Sua.Them(B);
    Sua.CapNhat(BacSi("B1","Ten moi",CapBac::TruongKhoa,10,"Noi"));
    assert(Sua.Tim("B1")->LayHoTen()=="Ten moi" && Sua.KichThuoc()==1);
    assert(Sua.Xoa("B1") && !Sua.Xoa("B1") && !Sua.Tim("B1"));
    Vector<Shift> Ca;
    Ca.ThemCuoi({"Sang", 7*60, 15*60, 1, 1, true});
    Ca.ThemCuoi({"Dem", 22*60, 30*60, 1, 1, true});
    auto KQ = SchedulerService::LapLich(*D.NhanSu, Ca);
    assert(KQ.TrangThai == ScheduleStatus::ThanhCong);
    assert(SchedulerService::LichHopLe(*D.NhanSu, KQ.Lich));
    D.Lich = KQ.Lich;
    assert(D.Lich.PhanCong.KichThuoc() == 4);
    const double Diem = FatigueService::TinhDiem(B, D.Lich);
    assert(Diem == FatigueService::TinhDiem(B, D.Lich)); // Đọc lại không cộng lặp.
    assert(B.LayDiemMetMoiNen() == 0);
    auto Het = SchedulerService::LapLich(*D.NhanSu, Ca, {}, 1);
    assert(Het.TrangThai == ScheduleStatus::HetLuotTim && Het.Lich.PhanCong.Rong());
    StaffRegistry Thieu;
    Thieu.Them(B);
    assert(SchedulerService::LapLich(Thieu, Ca).TrangThai == ScheduleStatus::KhongCoLich);
    Schedule Mot;
    Mot.CacCa.ThemCuoi({"Dem", 22*60, 30*60, 1, 0, true});
    Mot.PhanCong.ThemCuoi({"B1", "Dem", AssignmentRole::TruongCa});
    Shift SatMoc{"Sau", 54*60, 62*60, 1, 0, true};
    assert(SchedulerService::CoThePhanCong(B, SatMoc, AssignmentRole::TruongCa, Mot, {}));
    SatMoc=Shift("Sau",54*60-1,62*60,1,0,true);
    assert(!SchedulerService::CoThePhanCong(B, SatMoc, AssignmentRole::TruongCa, Mot, {}));
    Shift Trung{"Trung", 23*60, 25*60, 1, 0, true};
    assert(!SchedulerService::CoThePhanCong(B, Trung, AssignmentRole::TruongCa, Mot, {}));
    SchedulerRules L; L.GioToiDaTuan = 15;
    assert(!SchedulerService::CoThePhanCong(B, Shift{"Sau",54*60,62*60,1,0,true}, AssignmentRole::TruongCa, Mot, L));
    L = {}; L.DiemMetMoiToiDa = 19;
    assert(!SchedulerService::CoThePhanCong(B, Shift{"Sau",54*60,62*60,1,0,true}, AssignmentRole::TruongCa, Mot, L));
    D.DuPhong.Them("B2");
    assert(!D.DuPhong.Them("B2"));
    StandbyPool Rong;
    assert(SchedulerService::TimNguoiThay(*D.NhanSu,Mot,0,{},&Rong).Rong());
    assert(SchedulerService::TimNguoiThay(*D.NhanSu,Mot,0,{},&D.DuPhong).KichThuoc()==1);
    auto Thay = SchedulerService::TimNguoiThay(*D.NhanSu, Mot, 0);
    assert(Thay.KichThuoc() == 1 && Thay[0] == "B2");
    Schedule Sai = D.Lich; Sai.PhanCong.ThemCuoi(Sai.PhanCong[0]);
    assert(!SchedulerService::LichHopLe(*D.NhanSu, Sai));
    PhaiLoi([&] { Vector<Shift> C; C.ThemCuoi({"X",0,1,1,0,true}); C.ThemCuoi(C[0]); SchedulerService::LapLich(*D.NhanSu,C); });
    // Buộc quay lui: nội trú chỉ làm ca A; bác sĩ chính phải được giữ cho ca B trùng giờ.
    StaffRegistry N;
    N.Them(BacSi("CH", "Chinh", CapBac::BacSiChinh, 5, "Noi"));
    N.Them(BacSi("NT", "Noi tru", CapBac::BacSiNoiTru, 1, "Noi"));
    Vector<Shift> C;
    C.ThemCuoi({"A",420,900,1,0,false}); C.ThemCuoi({"B",420,900,1,0,true});
    auto Lui = SchedulerService::LapLich(N,C);
    assert(Lui.TrangThai == ScheduleStatus::ThanhCong && Lui.Lich.PhanCong[0].MaNhanVien == "NT");
    assert(Lui.SoLuotThu > 2);
    const auto ThuMuc = std::filesystem::temp_directory_path() / ("pbl2-admin-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(ThuMuc);
    const auto Tep = (ThuMuc / "du-lieu.txt").string();
    AdminFileManager::Luu(Tep,D);
    AdminFileManager::Luu(Tep,D); // Lưu lại trên file hiện có.
    auto Doc = AdminFileManager::Doc(Tep);
    assert(Doc.NhanSu->KichThuoc() == 4 && Doc.NhanSu->Tim("B1")->LayHoTen() == B.LayHoTen());
    assert(SchedulerService::LichHopLe(*Doc.NhanSu, Doc.Lich));
    assert(Doc.Lich.PhanCong.KichThuoc() == 4 && Doc.DuPhong.Co("B2"));
    D.TuanBatDau = "2026-11-03";
    PhaiLoi([&] { AdminFileManager::Luu(Tep,D); });
    assert(AdminFileManager::Doc(Tep).TuanBatDau == "2026-11-02");
    const auto Hong = (ThuMuc / "hong.txt").string();
    { std::ofstream F(Hong); F << "PBL2_ADMIN 1 2026-11-02\nCA \"X\" 420 900 1 0 1\nEND\n"; }
    PhaiLoi([&] { AdminFileManager::Doc(Hong); }); // Thiếu phân công.
    { std::ofstream F(Hong); F << "PBL2_ADMIN 1 2026-11-02\n"; }
    PhaiLoi([&] { AdminFileManager::Doc(Hong); }); // File bị cắt ngang.
    { std::ofstream F(Hong); F << "PBL2_ADMIN 99 2026-11-02\nEND\n"; }
    PhaiLoi([&] { AdminFileManager::Doc(Hong); });
    std::filesystem::remove_all(ThuMuc);
}
