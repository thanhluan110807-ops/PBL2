#include "repository/SessionFileManager.h"
#include "repository/PatientFileManager.h"
#include "repository/VisitFileManager.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "util/My_stdexcept.h"
#include "util/CsvFile.h"
#include "dsa/Vector.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>
#include <ctime>
#include <string>

namespace {
void BaoLoiTuDong(const char* NoiDung) {
    throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, NoiDung);
}

// Nhật ký chỉ nhận tên con do ứng dụng tạo, không nhận đường dẫn tùy ý.
// Giới hạn ký tự cũng chặn ổ đĩa Windows, byte NUL và ký hiệu đường dẫn đặc biệt.
bool LaTenPhien(const std::string& Ten) {
    if (Ten.size() <= 6 || Ten.compare(0, 6, "phien-") != 0) return false;
    for (std::string::size_type I = 6; I < Ten.size(); ++I) {
        const char KyTu = Ten[I];
        if (!((KyTu >= '0' && KyTu <= '9') || (KyTu >= 'a' && KyTu <= 'z') ||
              (KyTu >= 'A' && KyTu <= 'Z') || KyTu == '-' || KyTu == '_')) return false;
    }
    return true;
}

void TaoGocTuDong(const std::filesystem::path& Goc) {
    std::error_code Loi;
    const bool DaTao = std::filesystem::create_directories(Goc, Loi);
    if (Loi) BaoLoiTuDong("Khong tao/mo duoc thu muc luu tu dong");
    const bool LaThuMuc = std::filesystem::is_directory(Goc, Loi);
    if (Loi || !LaThuMuc)
        BaoLoiTuDong("Khong tao/mo duoc thu muc luu tu dong");
    if (!DaTao) return;
    // Tên root mới cũng cần bền. Đi từ thư mục con lên cha để đồng bộ các entry
    // vừa tạo, kể cả khi create_directories đã tạo nhiều cấp cùng lúc.
    std::filesystem::path HienTai = std::filesystem::absolute(Goc, Loi).lexically_normal();
    if (Loi) BaoLoiTuDong("Khong xac dinh duoc thu muc luu tu dong");
    while (true) {
        CsvFile::DongBoThuMuc(HienTai.u8string());
        const std::filesystem::path Cha = HienTai.parent_path();
        if (Cha.empty() || Cha == HienTai) break;
        HienTai = Cha;
    }
}
} // namespace

PatientRegistry* SessionFileManager::DocPhien(const std::string& TepPatient, const std::string& TepVisit,
                                             TrangThaiHangCho& HangCho) const {
    PatientRegistry* R = PatientFileManager().DocBenhNhan(TepPatient);
    try {
        TrangThaiHangCho Tam;
        if (!TepVisit.empty()) Tam = VisitFileManager().DocLanKham(*R,TepVisit);
        // Chép output có thể cấp phát; nếu lỗi thì thu hồi Registry vừa đọc.
        HangCho = Tam;
    } catch (...) { delete R; throw; }
    return R;
}

std::string SessionFileManager::LuuPhien(const PatientRegistry& R,const TriageService& T,
                                        const std::string& ThuMucGoc) const {
    namespace Fs = std::filesystem;
    const Fs::path Goc = Fs::u8path(ThuMucGoc);
    std::error_code Loi;
    if (!Fs::is_directory(Goc,Loi) || Loi)
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Thu muc luu khong ton tai/khong truy cap duoc");
    const std::string Ten = "phien-" + std::to_string(std::time(nullptr));
    Fs::path Dich;
    bool DaTao = false;
    for (int I=0;I<1000 && !DaTao;++I) {
        Dich = Goc / (Ten + "-" + std::to_string(I));
        Loi.clear(); DaTao = Fs::create_directory(Dich,Loi);
        if (Loi) throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Khong tao duoc thu muc phien moi");
    }
    if (!DaTao) throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Khong cap duoc thu muc phien moi");
    // Chỉ xóa các file do lần lưu này tạo nếu thất bại; bản lưu cũ giữ nguyên.
    try {
        PatientFileManager().LuuBenhNhan(R,(Dich/"patient.csv").u8string());
        VisitFileManager().LuuLanKham(R,T,(Dich/"visit.csv").u8string());
    } catch (...) {
        Fs::remove(Dich/"patient.csv",Loi); Fs::remove(Dich/"visit.csv",Loi); Fs::remove(Dich,Loi);
        throw;
    }
    return Dich.u8string();
}

namespace {
namespace Fs = std::filesystem;
struct BoFileDaChon {
    Fs::path Patient, Visit, Kho;
};

Fs::path ChuanHoaDuongDan(const std::string& DuongDan) {
    if (DuongDan.empty()) BaoLoiTuDong("Phai chon du hai duong dan Patient va Visit");
    std::error_code Loi;
    const Fs::path TuyetDoi = Fs::absolute(Fs::u8path(DuongDan), Loi);
    if (Loi) BaoLoiTuDong("Khong xac dinh duoc duong dan da chon");
    const Fs::path KetQua = Fs::weakly_canonical(TuyetDoi, Loi);
    if (Loi) BaoLoiTuDong("Khong chuan hoa duoc duong dan da chon");
    return KetQua;
}

// Chỉ coi là chưa có file khi hệ điều hành thực sự báo không tồn tại.
// File không đọc được hoặc bị thay bằng thư mục phải báo lỗi để giữ dữ liệu.
bool CoTep(const Fs::path& Tep) {
    std::error_code Loi;
    const Fs::file_status TrangThai = Fs::symlink_status(Tep, Loi);
    if (Loi == std::errc::no_such_file_or_directory ||
        (!Loi && TrangThai.type() == Fs::file_type::not_found)) return false;
    if (Loi || !Fs::is_regular_file(TrangThai)) BaoLoiTuDong("Duong dan file khong hop le/khong truy cap duoc");
    return true;
}

BoFileDaChon LayBoFile(const std::string& Patient, const std::string& Visit) {
    BoFileDaChon Bo;
    Bo.Patient = ChuanHoaDuongDan(Patient);
    Bo.Visit = ChuanHoaDuongDan(Visit);
    Bo.Kho = Fs::u8path(Bo.Patient.u8string() + ".pbl2");
    if (Bo.Patient == Bo.Visit) BaoLoiTuDong("Patient va Visit phai la hai file khac nhau");
    const Fs::path TrongKho = Bo.Visit.lexically_relative(Bo.Kho);
    if (!TrongKho.empty() && *TrongKho.begin() != "..")
        BaoLoiTuDong("File Visit khong duoc nam trong thu muc sao luu Patient");
    if (CoTep(Bo.Patient) && CoTep(Bo.Visit)) {
        std::error_code Loi;
        const bool CungFile = Fs::equivalent(Bo.Patient, Bo.Visit, Loi);
        if (Loi || CungFile) BaoLoiTuDong("Patient va Visit phai la hai file doc lap");
    }
    return Bo;
}

std::string DocNoiDung(const Fs::path& DuongDan) {
    std::ifstream Tep(DuongDan, std::ios::binary);
    if (!Tep.is_open()) BaoLoiTuDong("Khong mo duoc file de sao luu/khoi phuc");
    std::string NoiDung;
    char Dem[4096];
    do {
        Tep.read(Dem, sizeof(Dem));
        NoiDung.append(Dem, static_cast<std::string::size_type>(Tep.gcount()));
        if (Tep.bad() || (Tep.fail() && !Tep.eof())) BaoLoiTuDong("Loi doc file de sao luu/khoi phuc");
    } while (!Tep.eof());
    return NoiDung;
}

std::string TaoBanGhi(const char* Loai, const std::string& Ten, const BoFileDaChon& Bo) {
    Vector<std::string> Cot;
    Cot.ThemCuoi(Loai); Cot.ThemCuoi("1"); Cot.ThemCuoi(Ten);
    Cot.ThemCuoi(Bo.Patient.u8string()); Cot.ThemCuoi(Bo.Visit.u8string());
    std::ostringstream Tep; CsvFile::GhiBanGhi(Tep, Cot);
    return Tep.str();
}

Fs::path TaoThuMucRaw(const Fs::path& Kho) {
    const std::string Ten = "raw-" + std::to_string(std::time(nullptr));
    for (int Lan = 0; Lan < 1000; ++Lan) {
        const Fs::path Dich = Kho / (Ten + "-" + std::to_string(Lan));
        std::error_code Loi;
        if (Fs::create_directory(Dich, Loi)) return Dich;
        if (Loi) BaoLoiTuDong("Khong tao duoc thu muc sao luu raw");
    }
    BaoLoiTuDong("Khong cap duoc thu muc sao luu raw");
    return {};
}

// Sao lưu đúng byte file gốc, kể cả BOM, CRLF và việc chưa có file nào.
// Mỗi lần lưu có bản raw riêng; backup-original giữ nguyên lần đầu tiên.
void SaoLuuFileGoc(const BoFileDaChon& Bo) {
    const bool CoPatient = CoTep(Bo.Patient), CoVisit = CoTep(Bo.Visit);
    const Fs::path Raw = TaoThuMucRaw(Bo.Kho);
    if (CoPatient) CsvFile::GhiFileAnToan((Raw / "patient.csv").u8string(), DocNoiDung(Bo.Patient));
    if (CoVisit) CsvFile::GhiFileAnToan((Raw / "visit.csv").u8string(), DocNoiDung(Bo.Visit));
    Vector<std::string> Cot;
    Cot.ThemCuoi("PBL2_BACKUP"); Cot.ThemCuoi("1");
    Cot.ThemCuoi(Bo.Patient.u8string()); Cot.ThemCuoi(Bo.Visit.u8string());
    Cot.ThemCuoi(CoPatient ? "1" : "0"); Cot.ThemCuoi(CoVisit ? "1" : "0");
    std::ostringstream Tep; CsvFile::GhiBanGhi(Tep, Cot);
    CsvFile::GhiFileAnToan((Raw / "manifest.csv").u8string(), Tep.str());
    std::error_code Loi;
    const bool CoBanDau = Fs::exists(Bo.Kho / "backup-original", Loi);
    if (Loi) BaoLoiTuDong("Khong kiem tra duoc ban sao luu goc");
    if (!CoBanDau) {
        Fs::rename(Raw, Bo.Kho / "backup-original", Loi);
        if (Loi) BaoLoiTuDong("Khong cong bo duoc ban sao luu goc");
    }
    CsvFile::DongBoThuMuc(Bo.Kho.u8string());
}

Fs::path DocNhatKy(const BoFileDaChon& Bo) {
    std::ifstream Tep(Bo.Kho / "pending.csv", std::ios::binary);
    if (!Tep.is_open()) BaoLoiTuDong("Khong mo duoc nhat ky pending.csv");
    CsvFile::BoQuaBom(Tep);
    Vector<std::string> Cot;
    int Dong = 1, Dau = 1;
    if (!CsvFile::DocBanGhi(Tep, Cot, Dong, Dau) || Cot.KichThuoc() != 5 ||
        Cot[0] != "PBL2_PENDING" || Cot[1] != "1" || !LaTenPhien(Cot[2]) ||
        Cot[3] != Bo.Patient.u8string() || Cot[4] != Bo.Visit.u8string())
        BaoLoiTuDong("Nhat ky sai phien ban/noi dung/bo file da chon");
    const Fs::path BanLuu = Bo.Kho / Fs::u8path(Cot[2]);
    if (CsvFile::DocBanGhi(Tep, Cot, Dong, Dau)) BaoLoiTuDong("Nhat ky phai chi co mot ban ghi");
    std::error_code Loi;
    const Fs::file_status TrangThai = Fs::symlink_status(BanLuu, Loi);
    if (Loi || !Fs::is_directory(TrangThai)) BaoLoiTuDong("Ban luu cua nhat ky khong ton tai/khong hop le");
    return BanLuu;
}

void GhiCaBo(const BoFileDaChon& Bo, const Fs::path& BanLuu) {
    // Đọc đủ hai nội dung trước khi thay file chính. Journal còn nguyên khi lỗi,
    // nên lần mở/thử lại sẽ hoàn tất cả hai thay vì dùng cặp file lệch phiên.
    const std::string Patient = DocNoiDung(BanLuu / "patient.csv");
    const std::string Visit = DocNoiDung(BanLuu / "visit.csv");
    CsvFile::GhiFileAnToan(Bo.Patient.u8string(), Patient);
    CsvFile::GhiFileAnToan(Bo.Visit.u8string(), Visit);
    // Chỉ dấu này chứng minh cặp file đã từng được ghi đủ. Backup ban đầu chưa
    // phải commit: có thể bị dừng ngay sau backup khi Visit còn chưa tồn tại.
    CsvFile::GhiFileAnToan((Bo.Kho / "committed.csv").u8string(),
        TaoBanGhi("PBL2_COMMITTED",BanLuu.filename().u8string(),Bo));
    CsvFile::XoaFileDaDongBo((Bo.Kho / "pending.csv").u8string());
}

bool DaTungCoVisit(const BoFileDaChon& Bo) {
    const bool CoCommit=CoTep(Bo.Kho/"committed.csv");
    const Fs::path Tep=CoCommit ? Bo.Kho/"committed.csv" : Bo.Kho/"backup-original"/"manifest.csv";
    if (!CoTep(Tep)) return false;
    std::ifstream Vao(Tep,std::ios::binary); CsvFile::BoQuaBom(Vao);
    Vector<std::string> D,Du; int Dong=1,Dau=1;
    if (!CsvFile::DocBanGhi(Vao,D,Dong,Dau) || D.KichThuoc()!=(CoCommit?5:6) || D[1]!="1" ||
        CsvFile::DocBanGhi(Vao,Du,Dong,Dau)) BaoLoiTuDong("Metadata du phong/commit bi hong");
    if (CoCommit) {
        if (D[0]!="PBL2_COMMITTED" || !LaTenPhien(D[2]) || D[3]!=Bo.Patient.u8string() || D[4]!=Bo.Visit.u8string())
            BaoLoiTuDong("Metadata commit khong khop bo file");
        return true;
    }
    if (D[0]!="PBL2_BACKUP" || D[2]!=Bo.Patient.u8string() || D[3]!=Bo.Visit.u8string() ||
        (D[4]!="0" && D[4]!="1") || (D[5]!="0" && D[5]!="1"))
        BaoLoiTuDong("Metadata du phong khong khop bo file");
    return D[5]=="1"; // Trước lần lưu đầu, Visit có thể thật sự chưa từng tồn tại.
}

void HoanTatNhatKy(const SessionFileManager& QuanLy, const BoFileDaChon& Bo) {
    if (!CoTep(Bo.Kho / "pending.csv")) return;
    const Fs::path BanLuu = DocNhatKy(Bo);
    TrangThaiHangCho HangTam;
    // Kiểm tra toàn bộ quan hệ Patient/Visit/hàng chờ trước khi sửa file chính.
    delete QuanLy.DocPhien((BanLuu / "patient.csv").u8string(),
                           (BanLuu / "visit.csv").u8string(), HangTam);
    GhiCaBo(Bo, BanLuu);
}
} // namespace

std::string SessionFileManager::LuuVaoBoFile(const PatientRegistry& Registry,
                                             const TriageService& Triage,
                                             const std::string& TepPatient,
                                             const std::string& TepVisit) const {
    const BoFileDaChon Bo = LayBoFile(TepPatient, TepVisit);
    TaoGocTuDong(Bo.Kho);
    HoanTatNhatKy(*this, Bo);
    SaoLuuFileGoc(Bo);
    const std::string BanLuu = LuuPhien(Registry, Triage, Bo.Kho.u8string());
    CsvFile::GhiFileAnToan((Bo.Kho / "pending.csv").u8string(),
        TaoBanGhi("PBL2_PENDING", Fs::u8path(BanLuu).filename().u8string(), Bo));
    GhiCaBo(Bo, Fs::u8path(BanLuu));
    return BanLuu;
}

PatientRegistry* SessionFileManager::DocBoFile(const std::string& TepPatient,
                                               const std::string& TepVisit,
                                               TrangThaiHangCho& HangCho) const {
    const BoFileDaChon Bo = LayBoFile(TepPatient, TepVisit);
    HoanTatNhatKy(*this, Bo);
    const bool CoVisit = CoTep(Bo.Visit);
    if (!CoVisit && DaTungCoVisit(Bo)) BaoLoiTuDong("File Visit da luu bi thieu; khong duoc bo qua lich su");
    return DocPhien(Bo.Patient.u8string(), CoVisit ? Bo.Visit.u8string() : "", HangCho);
}
