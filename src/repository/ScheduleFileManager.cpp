#include "repository/ScheduleFileManager.h"
#include "util/My_stdexcept.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace {
void Loi(const char* S) { throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,S); }
void Chuoi(const std::string& S) {
    if (S.size()>1000 || S.find_first_of("\r\n")!=std::string::npos || S.find('\0')!=std::string::npos)
        Loi("Chuoi lich qua dai hoac chua ky tu dieu khien");
}
void HetDong(std::istream& S) { S>>std::ws; if (!S.eof()) Loi("Du du lieu cuoi dong lich"); }
}
void ScheduleFileManager::GhiCa(std::ostream& S, const Shift& C) {
    Chuoi(C.LayMaCaTruc());
    S<<"CA "<<std::quoted(C.LayMaCaTruc())<<' '<<C.LayBatDau()<<' '<<C.LayKetThuc()<<' '
     <<C.SoBacSi<<' '<<C.SoDieuDuong<<' '<<C.CanTruongCa<<'\n';
}
Shift ScheduleFileManager::DocCa(std::istream& S) {
    std::string M; std::time_t Dau,Cuoi; int BS,DD,Truong;
    if (!(S>>std::quoted(M)>>Dau>>Cuoi>>BS>>DD>>Truong) || Truong<0 || Truong>1) Loi("Dong ca sai");
    Chuoi(M); HetDong(S);
    return Shift::TuThoiDiem(M,Dau,Cuoi,BS,DD,Truong==1);
}
void ScheduleFileManager::GhiPhanCong(std::ostream& S, const ShiftAssignment& P) {
    Chuoi(P.LayMaPhanCong()); Chuoi(P.MaNhanVien); Chuoi(P.MaCa); Chuoi(P.LayLyDoHuy());
    S<<"PC "<<std::quoted(P.LayMaPhanCong())<<' '<<std::quoted(P.MaNhanVien)<<' '<<std::quoted(P.MaCa)<<' '
     <<static_cast<int>(P.VaiTro)<<' '<<static_cast<int>(P.LayTrangThai())<<' '<<P.LayThoiDiemBatDau()<<' '
     <<std::quoted(P.LayLyDoHuy())<<'\n';
}
ShiftAssignment ScheduleFileManager::DocPhanCong(std::istream& S) {
    std::string M,N,C,LyDo; int Vai,TrangThai; std::time_t Dau;
    if (!(S>>std::quoted(M)>>std::quoted(N)>>std::quoted(C)>>Vai>>TrangThai>>Dau>>std::quoted(LyDo)) ||
        Vai<0 || Vai>2 || TrangThai<0 || TrangThai>3 || Dau<0) Loi("Dong phan cong sai");
    Chuoi(M); Chuoi(N); Chuoi(C); Chuoi(LyDo); HetDong(S);
    ShiftAssignment P(M,N,C,static_cast<AssignmentRole>(Vai));
    if (TrangThai==3) {
        if (Dau!=0) Loi("Ban huy khong duoc co gio bat dau");
        P.Huy(LyDo);
    } else {
        if (!LyDo.empty() || (TrangThai!=2 && Dau!=0)) Loi("Trang thai va du lieu khong khop");
        if (TrangThai>=1) P.XacNhan();
        if (TrangThai==2) P.GhiNhanBatDau(Dau);
    }
    return P;
}
void ScheduleFileManager::LuuLichTruc(const std::string& Ten, const ScheduleRegistry& R) {
    const auto Tam=Ten+".tmp-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    try {
        std::ofstream S(Tam,std::ios::binary|std::ios::trunc);
        if (!S) Loi("Khong tao duoc file tam lich");
        S<<"PBL2_SCHEDULE 1\n";
        for (int I=0; I<R.SoCa(); ++I) GhiCa(S,R.LayCa(I));
        for (int I=0; I<R.SoPhanCong(); ++I) GhiPhanCong(S,R.LayPhanCong(I));
        S<<"END\n"; S.flush(); if (!S) Loi("Ghi lich khong day du");
        S.close(); if (S.fail()) Loi("Khong dong duoc file lich");
#ifdef _WIN32
        if (!MoveFileExW(std::filesystem::path(Tam).c_str(),std::filesystem::path(Ten).c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) Loi("Khong thay duoc file lich");
#else
        std::error_code E; std::filesystem::rename(Tam,Ten,E); if (E) Loi("Khong thay duoc file lich");
#endif
    } catch (...) { std::error_code E; std::filesystem::remove(Tam,E); throw; }
}
std::unique_ptr<ScheduleRegistry> ScheduleFileManager::DocLichTruc(const std::string& Ten, const StaffRegistry& N) {
    std::ifstream Tep(Ten,std::ios::binary); if (!Tep) Loi("Khong mo duoc file lich");
    auto R=std::make_unique<ScheduleRegistry>();
    std::string Dong,Loai; int PhienBan;
    if (!std::getline(Tep,Dong) || Dong.size()>8192) Loi("Thieu header lich");
    std::istringstream Dau(Dong);
    if (!(Dau>>Loai>>PhienBan) || Loai!="PBL2_SCHEDULE" || PhienBan!=1) Loi("Sai phien ban file lich");
    HetDong(Dau);
    Vector<ShiftAssignment> CacP; bool Xong=false; int SoDong=0;
    while (std::getline(Tep,Dong)) {
        if (++SoDong>550 || Dong.size()>8192) Loi("File lich vuot gioi han");
        std::istringstream S(Dong); if (!(S>>Loai)) continue;
        if (Xong) Loi("Du lieu sau END");
        if (Loai=="CA") R->ThemCaTruc(DocCa(S));
        else if (Loai=="PC") { if (CacP.KichThuoc()>=512) Loi("Qua 512 phan cong"); CacP.ThemCuoi(DocPhanCong(S)); }
        else if (Loai=="END") { HetDong(S); Xong=true; }
        else Loi("Loai dong lich khong ho tro");
    }
    if (Tep.bad() || !Xong) Loi("File lich bi cat ngang");
    // Đọc hết ca trước rồi liên kết mã; PC có thể xuất hiện trước CA trong file.
    for (int I=0; I<CacP.KichThuoc(); ++I) R->ThemPhanCong(CacP[I],N);
    return R;
}
