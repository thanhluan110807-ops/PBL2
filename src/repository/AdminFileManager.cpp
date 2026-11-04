// Phần B — Chức năng chính: Đọc/ghi snapshot chung nhân sự, lịch và dự phòng cho GUI.
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "repository/AdminFileManager.h"
#include "repository/StaffFileManager.h"
#include "repository/ScheduleFileManager.h"
#include "service/SchedulerService.h"
#include "model/Date.h"
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
void HetDong(std::istringstream& Dong) {
    Dong >> std::ws;
    if (!Dong.eof()) Loi("Du lieu thua cuoi dong");
}
bool TuanHopLe(const std::string& Tuan) {
    if (Tuan.size() != 10 || Tuan[4] != '-' || Tuan[7] != '-') return false;
    for (int I = 0; I < 10; ++I) if (I != 4 && I != 7 && (Tuan[I] < '0' || Tuan[I] > '9')) return false;
    const int Nam = std::stoi(Tuan.substr(0, 4)), Thang = std::stoi(Tuan.substr(5, 2)), Ngay = std::stoi(Tuan.substr(8, 2));
    if (!Date{Ngay, Thang, Nam}.HopLe()) return false;
    // Công thức thứ trong tuần Gregorian (0 = Chủ nhật), không phụ thuộc timezone.
    const int Bu[] = {0,3,2,5,0,3,5,1,4,6,2,4};
    const int Y = Nam - (Thang < 3 ? 1 : 0);
    return (Y + Y/4 - Y/100 + Y/400 + Bu[Thang-1] + Ngay) % 7 == 1;
}
void KiemTraChuoi(const std::string& S) {
    if (S.size() > 1000 || S.find_first_of("\r\n") != std::string::npos || S.find('\0') != std::string::npos)
        Loi("Chuoi qua dai hoac chua ky tu khong ho tro");
}
void KiemTra(const AdminData& D) {
    if (!D.NhanSu || D.NhanSu->KichThuoc() > 100 || !TuanHopLe(D.TuanBatDau)) Loi("Nhan su hoac ngay thu Hai khong hop le");
    ScheduleRegistry R;
    for (int I=0; I<D.Lich.CacCa.KichThuoc(); ++I) R.ThemCaTruc(D.Lich.CacCa[I]);
    for (int I=0; I<D.Lich.PhanCong.KichThuoc(); ++I) R.ThemPhanCong(D.Lich.PhanCong[I],*D.NhanSu);
    for (int I = 0; I < D.NhanSu->KichThuoc(); ++I) {
        const auto& NV = D.NhanSu->Lay(I);
        KiemTraChuoi(NV.LayMaDinhDanh()); KiemTraChuoi(NV.LayHoTen());
        if (NV.LayLoaiNhanVien() == StaffType::BacSi) KiemTraChuoi(static_cast<const BacSi&>(NV).LayChuyenKhoa());
    }
    for (int I = 0; I < D.Lich.CacCa.KichThuoc(); ++I) KiemTraChuoi(D.Lich.CacCa[I].LayMaCaTruc());
    D.DuPhong.Duyet([&](const std::string& Ma) { if (!D.NhanSu->Tim(Ma)) Loi("Ma du phong khong ton tai"); });
}
}
void AdminFileManager::Luu(const std::string& DuongDan, const AdminData& D) {
    KiemTra(D);
    const std::filesystem::path Dich(DuongDan);
    // Ghi xong file tạm cùng thư mục mới đổi tên: lỗi ghi không phá file gốc.
    const auto Tam = Dich.string() + ".tmp-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    try {
        std::ofstream Tep(Tam, std::ios::binary | std::ios::trunc);
        if (!Tep) Loi("Khong tao duoc file tam");
        Tep << "PBL2_ADMIN 4 " << D.TuanBatDau << '\n' << std::setprecision(17);
        for (int I = 0; I < D.NhanSu->KichThuoc(); ++I) StaffFileManager::GhiDong(Tep,D.NhanSu->Lay(I));
        for (int I=0; I<D.Lich.CacCa.KichThuoc(); ++I) ScheduleFileManager::GhiCa(Tep,D.Lich.CacCa[I]);
        for (int I=0; I<D.Lich.PhanCong.KichThuoc(); ++I) ScheduleFileManager::GhiPhanCong(Tep,D.Lich.PhanCong[I]);
        D.DuPhong.Duyet([&](const std::string& Ma) { Tep << "DP " << std::quoted(Ma) << '\n'; });
        Tep << "END\n";
        Tep.flush();
        if (!Tep) Loi("Khong ghi duoc day du du lieu");
        Tep.close();
        if (Tep.fail()) Loi("Khong dong duoc file");
#ifdef _WIN32
        // Windows rename không thay file hiện có; dùng API thay thế cùng ổ đĩa.
        if (!MoveFileExW(std::filesystem::path(Tam).c_str(), Dich.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            Loi("Khong thay the duoc file dich; file goc duoc giu nguyen");
#else
        std::error_code EC;
        std::filesystem::rename(Tam, Dich, EC);
        if (EC) Loi("Khong thay the duoc file dich; file goc duoc giu nguyen");
#endif
    } catch (...) { std::error_code EC; std::filesystem::remove(Tam, EC); throw; }
}
AdminData AdminFileManager::Doc(const std::string& DuongDan) {
    int DongHienTai=1;
    try {
    std::ifstream Tep(DuongDan, std::ios::binary);
    if (!Tep) Loi("Khong mo duoc file");
    AdminData D;
    std::string Dong, Loai;
    if (!std::getline(Tep, Dong) || Dong.size()>8192) Loi("File rong");
    std::istringstream Dau(Dong);
    int PhienBan = 0;
    if (!(Dau >> Loai >> PhienBan >> D.TuanBatDau) || Loai != "PBL2_ADMIN" || (PhienBan != 1 && PhienBan != 2 && PhienBan != 3 && PhienBan != 4)) Loi("Sai dinh dang hoac phien ban");
    HetDong(Dau);
    bool DaKetThuc = false;
    int SoDong = 0;
    while (std::getline(Tep, Dong)) {
        ++DongHienTai;
        if (++SoDong > 800 || Dong.size() > 8192) Loi("File vuot gioi han");
        if (!Dong.empty() && Dong.back() == '\r') Dong.pop_back();
        if (Dong.empty()) continue;
        std::istringstream S(Dong); if (!(S >> Loai)) continue;
        if (DaKetThuc) Loi("Du lieu sau END");
        if (Loai == "NV") {
            StaffFileManager::DocDong(S,*D.NhanSu,PhienBan==4?3:PhienBan);
        } else if (Loai == "CA" && PhienBan==4) {
            if (D.Lich.CacCa.KichThuoc()>=32) Loi("Qua 32 ca");
            D.Lich.CacCa.ThemCuoi(ScheduleFileManager::DocCa(S));
            continue;
        } else if (Loai == "PC" && PhienBan==4) {
            if (D.Lich.PhanCong.KichThuoc()>=512) Loi("Qua 512 phan cong");
            D.Lich.PhanCong.ThemCuoi(ScheduleFileManager::DocPhanCong(S));
            continue;
        } else if (Loai == "CA") {
            Shift C; int Truong, Dau, Cuoi, BS, DD; std::string Ma;
            if (!(S >> std::quoted(Ma) >> Dau >> Cuoi >> BS >> DD >> Truong) || Truong < 0 || Truong > 1)
                Loi("Dong ca khong hop le");
            C = Shift(Ma,Dau,Cuoi,BS,DD,Truong == 1);
            if (D.Lich.CacCa.KichThuoc() >= 32) Loi("Toi da 32 ca");
            D.Lich.CacCa.ThemCuoi(C);
        } else if (Loai == "PC") {
            ShiftAssignment P; int VaiTro;
            if (!(S >> std::quoted(P.MaNhanVien) >> std::quoted(P.MaCa) >> VaiTro) || VaiTro < 0 || VaiTro > 2) Loi("Dong phan cong khong hop le");
            P = ShiftAssignment(P.MaNhanVien,P.MaCa,static_cast<AssignmentRole>(VaiTro));
            if (D.Lich.PhanCong.KichThuoc() >= 128) Loi("Toi da 128 phan cong");
            D.Lich.PhanCong.ThemCuoi(P);
        } else if (Loai == "DP") {
            std::string Ma;
            if (!(S >> std::quoted(Ma)) || !D.DuPhong.Them(Ma)) Loi("Dong du phong khong hop le hoac trung ma");
        } else if (Loai == "END") DaKetThuc = true;
        else Loi("Loai dong khong duoc ho tro");
        HetDong(S);
    }
    if (Tep.bad() || !DaKetThuc) Loi("File doc loi hoac bi cat ngang");
    if (PhienBan<4 && !SchedulerService::LichHopLe(*D.NhanSu,D.Lich)) Loi("Lich cu khong day du hoac sai rang buoc");
    if (PhienBan<4) {
        // Tính số ngày từ 01/01/1970; chỉ chạy một lần khi nâng định dạng cũ.
        if (!TuanHopLe(D.TuanBatDau)) Loi("Ngay tuan cu khong hop le");
        const int Nam=std::stoi(D.TuanBatDau.substr(0,4));
        const int Thang=std::stoi(D.TuanBatDau.substr(5,2)), Ngay=std::stoi(D.TuanBatDau.substr(8,2));
        if (!TuanHopLe(D.TuanBatDau) || Nam<1970) Loi("Ngay tuan cu khong hop le");
        long long SoNgay=0;
        for (int Y=1970; Y<Nam; ++Y) SoNgay+=(Y%4==0 && (Y%100!=0 || Y%400==0))?366:365;
        const int NgayThang[]={31,28,31,30,31,30,31,31,30,31,30,31};
        for (int M=1; M<Thang; ++M) SoNgay+=NgayThang[M-1]+(M==2 && Nam%4==0 && (Nam%100!=0 || Nam%400==0)?1:0);
        SoNgay+=Ngay-1;
        const std::time_t Moc=SoNgay*86400-7*3600;
        for (int I=0; I<D.Lich.CacCa.KichThuoc(); ++I) {
            const auto C=D.Lich.CacCa[I];
            D.Lich.CacCa[I]=Shift::TuThoiDiem(C.LayMaCaTruc(),Moc+C.LayPhutBatDau()*60,Moc+C.LayPhutKetThuc()*60,C.SoBacSi,C.SoDieuDuong,C.CanTruongCa);
        }
    }
    KiemTra(D);
    return D; // Chỉ giao dữ liệu mới sau khi toàn bộ file đã hợp lệ.
    } catch (const My_stdexcept& E) {
        const std::string ThongBao="Dong " + std::to_string(DongHienTai) + ": " + E.LayThongBao() + " | File: " + DuongDan;
        throw My_stdexcept(E.LayLoaiLoi(),ThongBao.c_str());
    }
}
