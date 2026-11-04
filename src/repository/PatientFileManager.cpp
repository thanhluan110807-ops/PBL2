#include "repository/PatientFileManager.h"

#include <fstream>
#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"

namespace {
enum class TrangThaiCsv { DauTruong, TruongThuong, TrongNgoac, SauNgoac };

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

// Đọc một bản ghi, không phải một dòng vật lý: trường trong ngoặc có thể xuống dòng.
bool DocBanGhi(std::ifstream& Tep, Vector<std::string>& CacTruong,
               int& DongHienTai, int& DongBatDau) {
    CacTruong = Vector<std::string>();
    DongBatDau = DongHienTai;
    std::string Truong;
    TrangThaiCsv TrangThai = TrangThaiCsv::DauTruong;
    bool DaDocKyTu = false;
    while (true) {
        const int MaKyTu = Tep.get();
        if (MaKyTu == std::char_traits<char>::eof()) {
            if (Tep.bad())
                throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi khi doc file benh nhan");
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

void BoQuaBom(std::ifstream& Tep) {
    char DauTep[3];
    Tep.read(DauTep, 3);
    if (Tep.bad())
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi khi doc file benh nhan");
    const bool CoBom = Tep.gcount() == 3 &&
        DauTep[0] == static_cast<char>(0xEF) &&
        DauTep[1] == static_cast<char>(0xBB) &&
        DauTep[2] == static_cast<char>(0xBF);
    if (!CoBom) {
        // Không phải BOM thì đọc lại cả ba byte vì chúng thuộc tiêu đề CSV.
        Tep.clear();
        Tep.seekg(0);
    }
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
} // namespace

PatientRegistry* PatientFileManager::DocBenhNhan(const std::string& DuongDan) const {
    std::ifstream Tep(DuongDan, std::ios::binary);
    if (!Tep.is_open())
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Khong mo duoc file benh nhan");
    BoQuaBom(Tep); // BOM thuộc file, không thuộc ô đầu tiên của bản ghi CSV.
    int DongHienTai = 1, DongBatDau = 1;
    Vector<std::string> CacTruong;
    if (!DocBanGhi(Tep, CacTruong, DongHienTai, DongBatDau))
        BaoLoiDong(1, "Thieu tieu de CSV");
    KiemTraTieuDe(CacTruong, DongBatDau);

    PatientRegistry* SoBo = new PatientRegistry();
    try {
        while (DocBanGhi(Tep, CacTruong, DongHienTai, DongBatDau)) {
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
