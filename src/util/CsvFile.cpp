#include "util/CsvFile.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <system_error>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "dsa/Vector.h"
#include "util/My_stdexcept.h"

namespace {
enum class TrangThaiCsv { DauTruong, TruongThuong, TrongNgoac, SauNgoac };

void BaoLoiDong(int Dong, const char* NoiDung) {
    const std::string ThongBao = "Dong " + std::to_string(Dong) + ": " + NoiDung;
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, ThongBao.c_str());
}

void BaoLoiGhi(const char* NoiDung) {
    throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, NoiDung);
}



std::filesystem::path TaoThuMucTam(const std::filesystem::path& DuongDan) {
    const auto Moc = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    for (int LanThu = 0; LanThu < 100; ++LanThu) {
        std::filesystem::path ThuMuc = DuongDan;
        ThuMuc += ".tmp-" + std::to_string(Moc) + "-" + std::to_string(LanThu);
        std::error_code Loi;
        if (std::filesystem::create_directory(ThuMuc, Loi)) return ThuMuc;
        if (Loi) BaoLoiGhi("Khong tao duoc thu muc tam de luu CSV");
    }
    BaoLoiGhi("Khong tim duoc ten thu muc tam de luu CSV");
    return {};
}


void XoaTepTam(const std::filesystem::path& TepTam, const std::filesystem::path& ThuMucTam) {
    std::error_code Loi;
    std::filesystem::remove(TepTam, Loi);
    std::filesystem::remove(ThuMucTam, Loi);
}



void DongBoTep(const std::filesystem::path& Tep) {
#ifdef _WIN32
    HANDLE Mo = CreateFileW(Tep.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
                            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (Mo == INVALID_HANDLE_VALUE) BaoLoiGhi("Khong mo duoc file CSV de dong bo");
    const bool DaDongBo = FlushFileBuffers(Mo) != 0;
    const bool DaDong = CloseHandle(Mo) != 0;
#else
    const int Mo = open(Tep.c_str(), O_RDWR);
    if (Mo == -1) BaoLoiGhi("Khong mo duoc file CSV de dong bo");
    int KetQua;
    do { KetQua = fsync(Mo); } while (KetQua == -1 && errno == EINTR);
    const bool DaDongBo = KetQua == 0;
    const bool DaDong = close(Mo) == 0;
#endif
    if (!DaDongBo || !DaDong) BaoLoiGhi("Khong dong bo duoc file CSV xuong o dia");
}

void CongBoTep(const std::filesystem::path& TepTam, const std::filesystem::path& Dich) {
#ifdef _WIN32
    if (!MoveFileExW(TepTam.c_str(), Dich.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        BaoLoiGhi("Khong thay duoc file dich khi luu CSV");
#else
    std::error_code Loi;
    std::filesystem::rename(TepTam, Dich, Loi);
    if (Loi) BaoLoiGhi("Khong thay duoc file dich khi luu CSV");
#endif
}
}

namespace CsvFile {

bool DocBanGhi(std::istream& Tep, Vector<std::string>& CacTruong,
               int& DongHienTai, int& DongBatDau) {
    CacTruong = Vector<std::string>();
    DongBatDau = DongHienTai;
    std::string Truong;
    TrangThaiCsv TrangThai = TrangThaiCsv::DauTruong;
    bool DaDocKyTu = false;
    while (true) {
        const int MaKyTu = Tep.get();
        if (MaKyTu == std::char_traits<char>::eof()) {
            if (Tep.bad() || (Tep.fail() && !Tep.eof()))
                throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi khi doc file CSV");
            if (TrangThai == TrangThaiCsv::TrongNgoac) BaoLoiDong(DongBatDau, "Thieu dau ngoac kep dong");
            if (!DaDocKyTu) return false;
            CacTruong.ThemCuoi(Truong);
            return true;
        }
        DaDocKyTu = true;
        const char KyTu = static_cast<char>(MaKyTu);
        if (KyTu == '\r' || KyTu == '\n') {
            std::string XuongDong(1, KyTu);
            if (KyTu == '\r' && Tep.peek() == '\n') {
                Tep.get();
                XuongDong += '\n';
            }
            ++DongHienTai;
            if (TrangThai == TrangThaiCsv::TrongNgoac) {
                Truong += XuongDong;
                continue;
            }
            CacTruong.ThemCuoi(Truong);
            return true;
        }
        if (TrangThai == TrangThaiCsv::DauTruong) {
            if (KyTu == ',') CacTruong.ThemCuoi(Truong);
            else if (KyTu == '"') TrangThai = TrangThaiCsv::TrongNgoac;
            else { Truong += KyTu; TrangThai = TrangThaiCsv::TruongThuong; }
        } else if (TrangThai == TrangThaiCsv::TruongThuong) {
            if (KyTu == ',') { CacTruong.ThemCuoi(Truong); Truong.clear(); TrangThai = TrangThaiCsv::DauTruong; }
            else if (KyTu == '"') BaoLoiDong(DongBatDau, "Dau ngoac kep sai vi tri");
            else Truong += KyTu;
        } else if (TrangThai == TrangThaiCsv::TrongNgoac) {
            if (KyTu == '"') TrangThai = TrangThaiCsv::SauNgoac;
            else Truong += KyTu;
        } else {
            if (KyTu == '"') { Truong += '"'; TrangThai = TrangThaiCsv::TrongNgoac; }
            else if (KyTu == ',') { CacTruong.ThemCuoi(Truong); Truong.clear(); TrangThai = TrangThaiCsv::DauTruong; }
            else BaoLoiDong(DongBatDau, "Ky tu sau dau ngoac kep dong khong hop le");
        }
    }
}

void BoQuaBom(std::istream& Tep) {
    if (Tep.bad() || (Tep.fail() && !Tep.eof()))
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi khi doc file CSV");
    char DauTep[3];
    Tep.read(DauTep, 3);
    if (Tep.bad())
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi khi doc file CSV");
    const bool CoBom = Tep.gcount() == 3 &&
        DauTep[0] == static_cast<char>(0xEF) &&
        DauTep[1] == static_cast<char>(0xBB) &&
        DauTep[2] == static_cast<char>(0xBF);
    if (!CoBom) {

        Tep.clear();
        Tep.seekg(0);
        if (!Tep)
            throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Khong doc lai duoc dau file CSV");
    }
}

void GhiBanGhi(std::ostream& Tep, const Vector<std::string>& CacTruong) {
    for (int I = 0; I < CacTruong.KichThuoc(); ++I) {
        if (I != 0) Tep.put(',');
        const std::string& Truong = CacTruong[I];
        const bool CanNgoac = Truong.find_first_of(",\"\r\n") != std::string::npos;
        if (CanNgoac) Tep.put('"');
        for (char KyTu : Truong) {
            if (KyTu == '"') Tep.put('"');
            Tep.put(KyTu);
        }
        if (CanNgoac) Tep.put('"');
    }
    Tep.put('\n');
    if (!Tep) BaoLoiGhi("Loi khi ghi ban ghi CSV");
}

void DongBoThuMuc(const std::string& DuongDan) {
#ifndef _WIN32
    const std::filesystem::path ThuMuc = std::filesystem::u8path(DuongDan);
    const int Mo = open(ThuMuc.c_str(), O_RDONLY | O_DIRECTORY);
    if (Mo == -1) BaoLoiGhi("Khong mo duoc thu muc luu de dong bo");
    int KetQua;
    do { KetQua = fsync(Mo); } while (KetQua == -1 && errno == EINTR);
    const bool DaDong = close(Mo) == 0;
    if (KetQua != 0 || !DaDong) BaoLoiGhi("Khong dong bo duoc thu muc luu xuong o dia");
#else
    (void)DuongDan;
#endif
}

void GhiFileAnToan(const std::string& DuongDan, const std::string& NoiDung) {
    if (DuongDan.empty()) BaoLoiGhi("Duong dan luu CSV rong");
    const std::filesystem::path Dich = std::filesystem::u8path(DuongDan);
    const std::filesystem::path ThuMucTam = TaoThuMucTam(Dich);
    const std::filesystem::path TepTam = ThuMucTam / "noi-dung.csv";
    try {
        std::ofstream Tep(TepTam, std::ios::binary | std::ios::trunc);
        if (!Tep.is_open()) BaoLoiGhi("Khong mo duoc file tam de luu CSV");
        Tep.write(NoiDung.data(), static_cast<std::streamsize>(NoiDung.size()));
        Tep.flush();
        if (!Tep) BaoLoiGhi("Loi khi ghi file CSV");
        Tep.close();
        if (!Tep) BaoLoiGhi("Loi khi dong file CSV");
        DongBoTep(TepTam);

        CongBoTep(TepTam, Dich);
        XoaTepTam(TepTam, ThuMucTam);
        const std::filesystem::path Cha = Dich.has_parent_path() ? Dich.parent_path() : ".";

        DongBoThuMuc(Cha.u8string());
    } catch (...) {
        XoaTepTam(TepTam, ThuMucTam);
        throw;
    }
    XoaTepTam(TepTam, ThuMucTam);
}

void XoaFileDaDongBo(const std::string& DuongDan) {
    const std::filesystem::path Tep = std::filesystem::u8path(DuongDan);
#ifdef _WIN32


    std::error_code Loi;
    if (!std::filesystem::exists(Tep, Loi) && !Loi) return;
    if (Loi) BaoLoiGhi("Khong kiem tra duoc nhat ky da ghi xong");
    const std::filesystem::path ThuMucTam = TaoThuMucTam(Tep);
    const std::filesystem::path DauCu = ThuMucTam / "da-hoan-tat.csv";
    if (!MoveFileExW(Tep.c_str(), DauCu.c_str(), MOVEFILE_WRITE_THROUGH)) {
        XoaTepTam(DauCu, ThuMucTam);
        BaoLoiGhi("Khong cong bo duoc viec xoa nhat ky da ghi xong");
    }
    XoaTepTam(DauCu, ThuMucTam);
#else
    std::error_code Loi;
    std::filesystem::remove(Tep, Loi);
    if (Loi) BaoLoiGhi("Khong xoa duoc nhat ky da ghi xong");
    const std::filesystem::path Cha = Tep.has_parent_path() ? Tep.parent_path() : ".";
    DongBoThuMuc(Cha.u8string());
#endif
}
}
