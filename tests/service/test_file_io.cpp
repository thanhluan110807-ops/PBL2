// Phần B — Chức năng chính: kiểm thử file nhân viên/lịch và dữ liệu lỗi.
#include "repository/StaffFileManager.h"
#include "repository/ScheduleFileManager.h"
#include "repository/AdminFileManager.h"
#include "model/BacSi.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <string>
namespace {
void Ghi(const std::string& Ten, const std::string& NoiDung) {
    std::ofstream Tep(Ten,std::ios::binary); Tep<<NoiDung; assert(Tep.good());
}
template<class F> void PhaiLoiTai(F Ham, int Dong) {
    bool Loi=false;
    try { Ham(); } catch (const My_stdexcept& E) {
        Loi=true;
        assert(std::string(E.LayThongBao()).find("Dong "+std::to_string(Dong)+":")!=std::string::npos);
        assert(std::string(E.LayThongBao()).find("File:")!=std::string::npos);
    }
    assert(Loi);
}
}
int main() {
    const auto ThuMuc=std::filesystem::temp_directory_path()/("pbl2-file-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(ThuMuc); const auto Tep=(ThuMuc/"du-lieu.pbl2").string();
    StaffRegistry N;
    N.Them(BacSi("B1","Nguyễn An \"A\"",CapBac::BacSiChinh,5,"Cấp cứu"));
    StaffFileManager::LuuNhanVien(Tep,N);
    auto Doc=StaffFileManager::DocNhanVien(Tep);
    assert(Doc->Tim("B1")->LayHoTen()==N.Tim("B1")->LayHoTen());
    StaffRegistry Hong;
    Hong.Them(BacSi("B2","Ten\nkhong hop le",CapBac::BacSiChinh,5,"Noi"));
    bool Loi=false;
    try { StaffFileManager::LuuNhanVien(Tep,Hong); } catch (const My_stdexcept&) { Loi=true; }
    assert(Loi && StaffFileManager::DocNhanVien(Tep)->Tim("B1")); // Lưu lỗi giữ file cũ.
    Ghi(Tep,"PBL2_STAFF 3\r\n  \t\r\nNV hong\r\nEND\r\n");
    PhaiLoiTai([&] { StaffFileManager::DocNhanVien(Tep); },3);
    Ghi(Tep,"PBL2_STAFF 3\nEND\nNV hong\n");
    PhaiLoiTai([&] { StaffFileManager::DocNhanVien(Tep); },3);
    Ghi(Tep,std::string(8193,'X')+"\n");
    PhaiLoiTai([&] { StaffFileManager::DocNhanVien(Tep); },1);
    // Bản 1 vẫn khôi phục đúng lớp con, dùng mặc định cho trạng thái mới.
    Ghi(Tep,"PBL2_STAFF 1\nNV 0 \"B1\" \"An\" 3 5 0 \"Noi\" 7 15 0\nEND\n \t\n");
    assert(StaffFileManager::DocNhanVien(Tep)->Tim("B1"));
    // PC trước CA được phép; mã liên kết sai phải chỉ đúng dòng PC khi kiểm tra cuối.
    Ghi(Tep,"PBL2_SCHEDULE 1\nPC \"P\" \"KhongCo\" \"C\" 1 0 0 \"\"\nCA \"C\" 1793577600 1793606400 1 0 1\nEND\n");
    PhaiLoiTai([&] { ScheduleFileManager::DocLichTruc(Tep,N); },2);
    Ghi(Tep,"PBL2_SCHEDULE 1\nPC \"P\" \"B1\" \"C\" 1 0 0 \"\"\nCA \"C\" 1793577600 1793606400 1 0 1\nEND\n");
    auto L=ScheduleFileManager::DocLichTruc(Tep,N); assert(L->SoPhanCong()==1);
    Ghi(Tep,"PBL2_ADMIN 4 2026-11-02\n \t\r\nEND\n  \t\r\n");
    assert(AdminFileManager::Doc(Tep).NhanSu->KichThuoc()==0);
    Ghi(Tep,"PBL2_ADMIN 4 2026-11-02\nNV hong\nEND\n");
    PhaiLoiTai([&] { AdminFileManager::Doc(Tep); },2);
    Ghi(Tep,"PBL2_ADMIN 4 2026-11-02\nEND\nCA hong\n");
    PhaiLoiTai([&] { AdminFileManager::Doc(Tep); },3);
    // Không còn file tạm khi ghi bị lỗi.
    for (const auto& Muc:std::filesystem::directory_iterator(ThuMuc))
        assert(Muc.path().filename().string().find(".tmp-")==std::string::npos);
    std::filesystem::remove_all(ThuMuc);
}
