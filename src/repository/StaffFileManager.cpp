// Phần B — Chức năng chính: Đọc/ghi file nhân viên và phục hồi đúng lớp BacSi/DieuDuong.
#include "repository/StaffFileManager.h"
#include "model/BacSi.h"
#include "model/DieuDuong.h"
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
void Loi(const char* NoiDung) { throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, NoiDung); }
void KiemTraChuoi(const std::string& S) {
    if (S.size()>1000 || S.find_first_of("\r\n")!=std::string::npos || S.find('\0')!=std::string::npos)
        Loi("Truong nhan vien qua dai hoac chua ky tu cam");
}
void HetDong(std::istream& S) { S >> std::ws; if (!S.eof()) Loi("Du lieu thua tren dong nhan vien"); }
}
void StaffFileManager::GhiDong(std::ostream& Tep, const NhanVienYTe& NV) {
    KiemTraChuoi(NV.LayMaDinhDanh()); KiemTraChuoi(NV.LayHoTen());
    const bool LaBS = NV.LayLoaiNhanVien() == StaffType::BacSi;
    const auto* BS = LaBS ? static_cast<const BacSi*>(&NV) : nullptr;
    const auto* DD = !LaBS ? static_cast<const DieuDuong*>(&NV) : nullptr;
    const std::string CK = BS ? BS->LayChuyenKhoa() : ""; KiemTraChuoi(CK);
    const auto& M = NV.LayCaMongMuon();
    const Date Ngay = NV.CoNgaySinh() ? NV.LayNgaySinh() : Date{0,0,0};
    Tep << std::setprecision(17) << "NV " << static_cast<int>(NV.LayLoaiNhanVien()) << ' '
        << std::quoted(NV.LayMaDinhDanh()) << ' ' << std::quoted(NV.LayHoTen()) << ' '
        << static_cast<int>(NV.LayCapBac()) << ' ' << NV.LaySoNamKinhNghiem() << ' '
        << NV.LayDiemMetMoiNen() << ' ' << std::quoted(CK) << ' ' << M.GioBatDau << ' '
        << M.GioKetThuc << ' ' << M.MongMuonCuoiTuan << ' ' << NV.LayDiemMetMoi() << ' '
        << NV.LayGioLamTuan() << ' ' << NV.LaySoCaDemThang() << ' ' << NV.LaySoCaCuoiTuanThang() << ' '
        << NV.DaVuaTrucDem() << ' ' << (BS && BS->LaBacSiNoiTru()) << ' '
        << (DD && DD->DangSangLoc()) << ' ' << (DD ? DD->LayGioSangLocLienTuc() : 0) << ' '
        << static_cast<int>(NV.LayGioiTinh()) << ' ' << NV.CoNgaySinh() << ' '
        << Ngay.Ngay << ' ' << Ngay.Thang << ' ' << Ngay.Nam << ' '
        << (DD && DD->DangTheoDongHo()) << ' ' << (DD ? DD->LayBatDauSangLoc() : 0) << ' '
        << (DD ? DD->LayMocDongHo() : 0) << '\n';
}
void StaffFileManager::DocDong(std::istream& S, StaffRegistry& NhanSu, int PhienBan) {
    if (PhienBan != 1 && PhienBan != 2 && PhienBan != 3) Loi("Phien ban nhan vien khong ho tro");
    int Loai, Bac, KN, CuoiTuan;
    double DiemNen;
    std::string Ma, Ten, CK;
    CaTrucMongMuon M;
    if (!(S >> Loai >> std::quoted(Ma) >> std::quoted(Ten) >> Bac >> KN >> DiemNen >> std::quoted(CK)
            >> M.GioBatDau >> M.GioKetThuc >> CuoiTuan) || Bac<0 || Bac>4 || CuoiTuan<0 || CuoiTuan>1)
        Loi("Ban ghi nhan vien khong hop le");
    KiemTraChuoi(Ma); KiemTraChuoi(Ten); KiemTraChuoi(CK);
    M.MongMuonCuoiTuan = CuoiTuan==1;
    TrangThaiNhanVien T; T.DiemMetMoi=DiemNen;
    int VuaDem=0, NoiTru=0, SangLoc=0, GioiTinh=3, CoNgaySinh=0;
    double GioSangLoc=0; Date Ngay{0,0,0};
    if (PhienBan>=2) {
        if (!(S >> T.DiemMetMoi >> T.GioLamTuan >> T.SoCaDemThang >> T.SoCaCuoiTuanThang
                >> VuaDem >> NoiTru >> SangLoc >> GioSangLoc >> GioiTinh >> CoNgaySinh
                >> Ngay.Ngay >> Ngay.Thang >> Ngay.Nam)) Loi("Thieu trang thai nhan vien");
        if (VuaDem<0 || VuaDem>1 || NoiTru<0 || NoiTru>1 || SangLoc<0 || SangLoc>1 ||
            GioiTinh<0 || GioiTinh>3 || CoNgaySinh<0 || CoNgaySinh>1) Loi("Co trang thai khong hop le");
        if (!CoNgaySinh && (Ngay.Ngay || Ngay.Thang || Ngay.Nam)) Loi("Ngay sinh khong nhat quan");
    }
    int TheoDongHo=0; std::time_t BatDau=0,MocCuoi=0;
    if (PhienBan>=3 && (!(S>>TheoDongHo>>BatDau>>MocCuoi) || TheoDongHo<0 || TheoDongHo>1))
        Loi("Moc dong ho sang loc khong hop le");
    HetDong(S); T.VuaTrucDem=VuaDem==1;
    std::unique_ptr<NhanVienYTe> NV;
    if (Loai==0) {
        if (SangLoc || GioSangLoc!=0 || TheoDongHo || BatDau!=0 || MocCuoi!=0) Loi("Bac si khong co trang thai sang loc cua dieu duong");
        NV=std::make_unique<BacSi>(Ma,Ten,static_cast<CapBac>(Bac),KN,CK,NoiTru==1,DiemNen,M);
    } else if (Loai==1 && CK.empty() && !NoiTru) {
        auto DD=std::make_unique<DieuDuong>(Ma,Ten,static_cast<CapBac>(Bac),KN,DiemNen,M);
        DD->KhoiPhucSangLoc(SangLoc==1,GioSangLoc,TheoDongHo==1,BatDau,MocCuoi); NV=std::move(DD);
    } else Loi("Loai nhan vien khong hop le");
    NV->KhoiPhucTrangThai(T);
    if (CoNgaySinh) NV->CapNhatThongTinCaNhan(Ten,static_cast<Gender>(GioiTinh),Ngay);
    else NV->CapNhatThongTinCaNhan(Ten,static_cast<Gender>(GioiTinh));
    if (NhanSu.KichThuoc()>=100) Loi("Toi da 100 nhan vien");
    NhanSu.Them(*NV);
}
void StaffFileManager::LuuNhanVien(const std::string& TenTep, const StaffRegistry& NhanSu) {
    if (NhanSu.KichThuoc()>100) Loi("Toi da 100 nhan vien");
    const auto Tam=TenTep+".tmp-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    try {
        std::ofstream Tep(Tam,std::ios::binary|std::ios::trunc);
        if (!Tep) Loi("Khong tao duoc file nhan vien");
        Tep << "PBL2_STAFF 3\n";
        for (int I=0;I<NhanSu.KichThuoc();++I) GhiDong(Tep,NhanSu.Lay(I));
        Tep << "END\n"; Tep.flush();
        if (!Tep) Loi("Ghi file nhan vien that bai");
        Tep.close(); if (Tep.fail()) Loi("Dong file nhan vien that bai");
#ifdef _WIN32
        if (!MoveFileExW(std::filesystem::path(Tam).c_str(), std::filesystem::path(TenTep).c_str(), MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) Loi("Khong thay duoc file nhan vien");
#else
        std::error_code EC; std::filesystem::rename(Tam,TenTep,EC);
        if (EC) Loi("Khong thay duoc file nhan vien");
#endif
    } catch (...) { std::error_code EC; std::filesystem::remove(Tam,EC); throw; }
}
std::unique_ptr<StaffRegistry> StaffFileManager::DocNhanVien(const std::string& TenTep) {
    int DongHienTai=1;
    try {
    std::ifstream Tep(TenTep,std::ios::binary);
    if (!Tep) Loi("Khong mo duoc file nhan vien");
    std::string Dong, Nhan; int PhienBan;
    if (!std::getline(Tep,Dong) || Dong.size()>8192) Loi("File nhan vien rong");
    std::istringstream Dau(Dong);
    if (!(Dau>>Nhan>>PhienBan) || Nhan!="PBL2_STAFF" || (PhienBan!=1 && PhienBan!=2 && PhienBan!=3)) Loi("Sai dinh dang file nhan vien");
    HetDong(Dau);
    auto Moi=std::make_unique<StaffRegistry>(); bool KetThuc=false; int SoDong=0;
    while (std::getline(Tep,Dong)) {
        ++DongHienTai;
        if (++SoDong>110 || Dong.size()>8192) Loi("File nhan vien vuot gioi han");
        std::istringstream S(Dong); if (!(S>>Nhan)) continue;
        if (KetThuc) Loi("Du lieu sau END");
        if (Nhan=="NV") DocDong(S,*Moi,PhienBan);
        else if (Nhan=="END") { HetDong(S); KetThuc=true; }
        else Loi("Loai dong nhan vien khong hop le");
    }
    if (Tep.bad() || !KetThuc) Loi("File nhan vien bi cat ngang hoac doc loi");
    return Moi;
    } catch (const My_stdexcept& E) {
        const std::string ThongBao="Dong " + std::to_string(DongHienTai) + ": " + E.LayThongBao() + " | File: " + TenTep;
        throw My_stdexcept(E.LayLoaiLoi(),ThongBao.c_str());
    }
}
