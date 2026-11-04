#include "repository/PatientFileManager.h"

#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"
#include "util/CsvFile.h"

namespace {
struct BanGhiBenhNhan {
    std::string Ma, HoTen, SoCanCuoc, DiUng, TienSu;
    Gender GioiTinh = Gender::ChuaXacDinh;
    BloodGroup NhomMau = BloodGroup::ChuaXacDinh;
    Date NgaySinh{0, 0, 0};
    bool CoNgaySinh = false;
    int TuoiUocLuong = 0;
    bool CoTuoiUocLuong = false;
};

void BaoLoiDong(int Dong, const char* NoiDung) {
    const std::string ThongBao = "Dong " + std::to_string(Dong) + ": " + NoiDung;
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, ThongBao.c_str());
}

void KiemTraTieuDe(const Vector<std::string>& CacTruong, int Dong) {
    const char* TenCot[] = {"MaBenhNhan", "HoTen", "GioiTinh", "NgaySinh", "SoCanCuoc",
                            "NhomMau", "ThongTinDiUng", "TienSuBenh", "TuoiUocLuong"};
    if (CacTruong.KichThuoc() != 9) BaoLoiDong(Dong, "Tieu de CSV khong hop le");
    for (int I = 0; I < 9; ++I)
        if (CacTruong[I] != TenCot[I]) BaoLoiDong(Dong, "Tieu de CSV khong hop le");
}

Gender DocGioiTinh(const std::string& VanBan) {
    if (VanBan == "Nam") return Gender::Nam;
    if (VanBan == "Nu") return Gender::Nu;
    if (VanBan == "Khac") return Gender::Khac;
    if (VanBan == "ChuaXacDinh") return Gender::ChuaXacDinh;
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Gioi tinh khong hop le");
}

BloodGroup DocNhomMau(const std::string& VanBan) {
    if (VanBan == "A") return BloodGroup::A;
    if (VanBan == "B") return BloodGroup::B;
    if (VanBan == "AB") return BloodGroup::AB;
    if (VanBan == "O") return BloodGroup::O;
    if (VanBan == "ChuaXacDinh") return BloodGroup::ChuaXacDinh;
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Nhom mau khong hop le");
}

int DocChuSo(char KyTu) {
    if (KyTu < '0' || KyTu > '9')
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Gia tri so khong hop le");
    return KyTu - '0';
}

Date DocNgay(const std::string& VanBan) {
    if (VanBan.size() != 10 || VanBan[4] != '-' || VanBan[7] != '-')
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay sinh phai theo YYYY-MM-DD");
    Date Ngay{DocChuSo(VanBan[8]) * 10 + DocChuSo(VanBan[9]),
              DocChuSo(VanBan[5]) * 10 + DocChuSo(VanBan[6]),
              DocChuSo(VanBan[0]) * 1000 + DocChuSo(VanBan[1]) * 100 +
                  DocChuSo(VanBan[2]) * 10 + DocChuSo(VanBan[3])};
    if (!Ngay.HopLe())
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay sinh khong hop le");
    return Ngay;
}

int DocTuoi(const std::string& VanBan) {
    int Tuoi = 0;
    for (char KyTu : VanBan) {
        Tuoi = Tuoi * 10 + DocChuSo(KyTu);
        if (Tuoi > 150)
            throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Tuoi uoc luong khong hop le");
    }
    return Tuoi;
}

BanGhiBenhNhan ChuyenBanGhi(const Vector<std::string>& CacTruong) {
    if (CacTruong.KichThuoc() != 9)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "So cot khong hop le");
    BanGhiBenhNhan BanGhi;
    BanGhi.Ma = CacTruong[0];
    BanGhi.HoTen = CacTruong[1];
    BanGhi.GioiTinh = DocGioiTinh(CacTruong[2]);
    BanGhi.CoNgaySinh = !CacTruong[3].empty();
    if (BanGhi.CoNgaySinh) BanGhi.NgaySinh = DocNgay(CacTruong[3]);
    BanGhi.SoCanCuoc = CacTruong[4];
    BanGhi.NhomMau = DocNhomMau(CacTruong[5]);
    BanGhi.DiUng = CacTruong[6];
    BanGhi.TienSu = CacTruong[7];
    BanGhi.CoTuoiUocLuong = !CacTruong[8].empty();
    if (BanGhi.CoTuoiUocLuong) BanGhi.TuoiUocLuong = DocTuoi(CacTruong[8]);
    return BanGhi;
}

void ThemBenhNhan(PatientRegistry& SoBo, const BanGhiBenhNhan& BanGhi) {
    Patient HoSo(BanGhi.Ma, BanGhi.HoTen, BanGhi.GioiTinh, BanGhi.SoCanCuoc);
    if (BanGhi.CoNgaySinh)
        HoSo.CapNhatThongTinCaNhan(BanGhi.HoTen, BanGhi.GioiTinh, BanGhi.NgaySinh);
    HoSo.CapNhatNhomMau(BanGhi.NhomMau);
    HoSo.CapNhatThongTinDiUng(BanGhi.DiUng);
    HoSo.CapNhatTienSuBenh(BanGhi.TienSu);
    if (BanGhi.CoTuoiUocLuong) HoSo.DatTuoiUocLuong(BanGhi.TuoiUocLuong);
    SoBo.ThemBenhNhan(HoSo);
}

std::string GhiGioiTinh(Gender GioiTinh) {
    switch (GioiTinh) {
        case Gender::Nam: return "Nam";
        case Gender::Nu: return "Nu";
        case Gender::Khac: return "Khac";
        case Gender::ChuaXacDinh: return "ChuaXacDinh";
    }
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Gioi tinh khong hop le");
}

std::string GhiNhomMau(BloodGroup NhomMau) {
    switch (NhomMau) {
        case BloodGroup::A: return "A";
        case BloodGroup::B: return "B";
        case BloodGroup::AB: return "AB";
        case BloodGroup::O: return "O";
        case BloodGroup::ChuaXacDinh: return "ChuaXacDinh";
    }
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Nhom mau khong hop le");
}

std::string GhiNgay(const Patient& BenhNhan) {
    if (!BenhNhan.CoNgaySinh()) return "";
    const Date Ngay = BenhNhan.LayNgaySinh();
    // Đệm đủ YYYY-MM-DD, kể cả năm nhỏ hơn 1000, để parser đọc lại đúng lược đồ v1.
    std::ostringstream Tep;
    Tep << std::setfill('0') << std::setw(4) << Ngay.Nam << '-'
        << std::setw(2) << Ngay.Thang << '-' << std::setw(2) << Ngay.Ngay;
    return Tep.str();
}

Vector<std::string> TaoTruongBenhNhan(const Patient& BenhNhan) {
    Vector<std::string> CacTruong;
    CacTruong.ThemCuoi(BenhNhan.LayMaDinhDanh());
    CacTruong.ThemCuoi(BenhNhan.LayHoTen());
    CacTruong.ThemCuoi(GhiGioiTinh(BenhNhan.LayGioiTinh()));
    CacTruong.ThemCuoi(GhiNgay(BenhNhan));
    CacTruong.ThemCuoi(BenhNhan.LaySoCanCuoc());
    CacTruong.ThemCuoi(GhiNhomMau(BenhNhan.LayNhomMau()));
    CacTruong.ThemCuoi(BenhNhan.LayThongTinDiUng());
    CacTruong.ThemCuoi(BenhNhan.LayTienSuBenh());
    CacTruong.ThemCuoi(BenhNhan.CoTuoiUocLuong() ?
                       std::to_string(BenhNhan.LayTuoiUocLuong()) : "");
    return CacTruong;
}

void SapXepTheoMa(Vector<const Patient*>& DanhSach) {
    // Sắp xếp chèn đơn giản giúp bản lưu ổn định dù bảng băm đổi thứ tự duyệt.
    for (int I = 1; I < DanhSach.KichThuoc(); ++I) {
        const Patient* BenhNhan = DanhSach[I];
        int J = I;
        while (J > 0 && DanhSach[J - 1]->LayMaDinhDanh() > BenhNhan->LayMaDinhDanh()) {
            DanhSach[J] = DanhSach[J - 1];
            --J;
        }
        DanhSach[J] = BenhNhan;
    }
}
} // namespace

PatientRegistry* PatientFileManager::DocBenhNhan(const std::string& DuongDan) const {
    std::ifstream Tep(std::filesystem::u8path(DuongDan), std::ios::binary);
    if (!Tep.is_open())
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Khong mo duoc file benh nhan");
    CsvFile::BoQuaBom(Tep); // BOM thuộc file, không thuộc ô đầu tiên của bản ghi CSV.
    int DongHienTai = 1, DongBatDau = 1;
    Vector<std::string> CacTruong;
    if (!CsvFile::DocBanGhi(Tep, CacTruong, DongHienTai, DongBatDau))
        BaoLoiDong(1, "Thieu tieu de CSV");
    KiemTraTieuDe(CacTruong, DongBatDau);

    PatientRegistry* SoBo = new PatientRegistry();
    try {
        while (CsvFile::DocBanGhi(Tep, CacTruong, DongHienTai, DongBatDau)) {
            try {
                const BanGhiBenhNhan BanGhi = ChuyenBanGhi(CacTruong);
                ThemBenhNhan(*SoBo, BanGhi);
            } catch (const My_stdexcept& Loi) {
                BaoLoiDong(DongBatDau, Loi.LayThongBao());
            }
        }
    } catch (...) {
        // Đọc lỗi giữa file: thu hồi cả Patient đã nạp, không trả dữ liệu dở dang.
        delete SoBo;
        throw;
    }
    return SoBo;
}

void PatientFileManager::LuuBenhNhan(const PatientRegistry& SoBo,
                                    const std::string& DuongDan) const {
    std::ostringstream Tep;
    Tep << "MaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,"
           "TienSuBenh,TuoiUocLuong\n";
    Vector<const Patient*> DanhSach = SoBo.LayDanhSachBenhNhan();
    SapXepTheoMa(DanhSach);
    for (int I = 0; I < DanhSach.KichThuoc(); ++I)
        CsvFile::GhiBanGhi(Tep, TaoTruongBenhNhan(*DanhSach[I]));
    CsvFile::GhiFileAnToan(DuongDan, Tep.str());
}
