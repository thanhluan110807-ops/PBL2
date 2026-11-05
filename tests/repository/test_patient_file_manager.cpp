#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "repository/PatientFileManager.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"
#include "util/CsvFile.h"

namespace {
const std::string DuongDan = "test_patient_file_manager.csv";
const std::string TieuDe =
    "MaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong\n";

void GhiDuLieuThu(const std::string& NoiDung) {
    std::ofstream Tep(DuongDan, std::ios::binary | std::ios::trunc);
    assert(Tep.is_open());
    Tep.write(NoiDung.data(), static_cast<std::streamsize>(NoiDung.size()));
    assert(Tep.good());
}

void KiemTraDuLieuHopLe() {
    // Một dòng Patient có dấu phẩy, nháy kép và xuống dòng trong trường văn bản.
    GhiDuLieuThu("\xEF\xBB\xBF" + TieuDe +
        u8"BN001,\"Nguyễn, \"\"An\"\"\",Nam,2000-01-31,001234567890,AB,\"Penicillin,\r\n aspirin\",Hen,\r\n"
        "BN002,,ChuaXacDinh,,,ChuaXacDinh,,,0\n");
    const PatientFileManager QuanLy;
    PatientRegistry* SoBo = QuanLy.DocBenhNhan(DuongDan);
    assert(SoBo != nullptr && SoBo->SoBenhNhan() == 2 && SoBo->SoLanKham() == 0);

    const Patient* An = SoBo->TimBenhNhan("BN001");
    assert(An != nullptr);
    assert(An->LayHoTen() == u8"Nguyễn, \"An\"");
    assert(An->LayGioiTinh() == Gender::Nam);
    assert(An->CoNgaySinh());
    const Date NgaySinh = An->LayNgaySinh();
    assert(NgaySinh.Ngay == 31 && NgaySinh.Thang == 1 && NgaySinh.Nam == 2000);
    assert(An->LaySoCanCuoc() == "001234567890");
    assert(An->LayNhomMau() == BloodGroup::AB);
    assert(An->LayThongTinDiUng() == "Penicillin,\r\n aspirin");
    assert(An->LayTienSuBenh() == "Hen");
    assert(!An->CoTuoiUocLuong());
    assert(An->LayTrangThaiDanhTinh() == IdentityStatus::DaXacDinh);

    const Patient* VoDanh = SoBo->TimBenhNhan("BN002");
    assert(VoDanh != nullptr && VoDanh->LaBenhNhanVoDanh());
    assert(!VoDanh->CoNgaySinh());
    assert(VoDanh->LaySoCanCuoc().empty());
    assert(VoDanh->CoTuoiUocLuong() && VoDanh->LayTuoiUocLuong() == 0);
    delete SoBo; // Bên gọi nhận quyền sở hữu Registry trả về.

    // BOM đứng trước dấu ngoặc kép vẫn phải đọc được ô tiêu đề CSV hợp lệ.
    GhiDuLieuThu("\xEF\xBB\xBF\"MaBenhNhan\",HoTen,GioiTinh,NgaySinh,SoCanCuoc,"
                "NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong\n"
                "BN003,Binh,Nam,,,O,,,\n");
    PatientRegistry* CoBom = QuanLy.DocBenhNhan(DuongDan);
    assert(CoBom->TimBenhNhan("BN003") != nullptr);
    delete CoBom;

    GhiDuLieuThu(TieuDe);
    PatientRegistry* Rong = QuanLy.DocBenhNhan(DuongDan);
    assert(Rong->SoBenhNhan() == 0);
    delete Rong;
}

void KiemTraFileLoi() {
    const PatientFileManager BoDoc;
    std::remove(DuongDan.c_str());
    bool DaBaoLoi = false;
    try {
        delete BoDoc.DocBenhNhan(DuongDan);
    } catch (const My_stdexcept& Loi) {
        DaBaoLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi);

    const auto PhaiLoiTai = [&](const std::string& NoiDung, const std::string& Dong) {
        GhiDuLieuThu(NoiDung);
        bool DaLoi = false;
        try {
            delete BoDoc.DocBenhNhan(DuongDan);
        } catch (const My_stdexcept& Loi) {
            DaLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::DoiSoKhongHopLe &&
                    std::string(Loi.LayThongBao()).find(Dong) != std::string::npos;
        }
        assert(DaLoi);
    };
    PhaiLoiTai("MaBenhNhan,HoTen\n", "Dong 1");
    PhaiLoiTai(TieuDe + "BN1,An\n", "Dong 2");
    PhaiLoiTai(TieuDe + "BN1,\"An\n", "Dong 2");
    PhaiLoiTai(TieuDe + "BN1,An,Sai,,,ChuaXacDinh,,,\n", "Dong 2");
    PhaiLoiTai(TieuDe + "BN1,An,Nam,2026-02-29,,A,,,\n", "Dong 2");
    PhaiLoiTai(TieuDe + "BN1,An,Nam,,,A,,,-1\n", "Dong 2");
    PhaiLoiTai(TieuDe + "BN1,An,Nam,,,A,,,\nBN1,Binh,Nam,,,A,,,\n", "Dong 3");
    PhaiLoiTai(TieuDe + "BN1,An,Nam,,001,A,,,\nBN2,Binh,Nam,,001,A,,,\n", "Dong 3");
}

void KiemTraGhiVaDocLai() {
    // Bắt lỗi mất dấu phân cách, tuổi 0, số 0 đầu CCCD hoặc ngày chưa đủ chữ số.
    PatientRegistry SoBo;
    Patient An("BN001", u8"Nguyễn, \"An\"\r\nHai", Gender::Khac,
               Date{2, 3, 2009}, "001234567890");
    An.CapNhatNhomMau(BloodGroup::AB);
    An.CapNhatThongTinDiUng("Penicillin,\r\n\"aspirin\"");
    An.CapNhatTienSuBenh("Hen\nTu nho\rCon sot");
    SoBo.ThemBenhNhan(An);
    Patient SoSinh("BN002");
    SoSinh.DatTuoiUocLuong(0);
    SoBo.ThemBenhNhan(SoSinh);
    SoBo.ThemBenhNhan("BN003", "Binh", Gender::Nu);

    const PatientFileManager QuanLy;
    QuanLy.LuuBenhNhan(SoBo, DuongDan);
    PatientRegistry* DocLai = QuanLy.DocBenhNhan(DuongDan);
    assert(DocLai->SoBenhNhan() == 3 && DocLai->SoLanKham() == 0);
    const Patient* AnDocLai = DocLai->TimBenhNhan("BN001");
    assert(AnDocLai->LayHoTen() == u8"Nguyễn, \"An\"\r\nHai");
    assert(AnDocLai->LayGioiTinh() == Gender::Khac);
    assert(AnDocLai->LaySoCanCuoc() == "001234567890");
    assert(AnDocLai->LayNhomMau() == BloodGroup::AB);
    assert(AnDocLai->LayThongTinDiUng() == "Penicillin,\r\n\"aspirin\"");
    assert(AnDocLai->LayTienSuBenh() == "Hen\nTu nho\rCon sot");
    assert(AnDocLai->CoNgaySinh() && AnDocLai->LayNgaySinh().Nam == 2009);
    assert(AnDocLai->LayNgaySinh().Thang == 3 && AnDocLai->LayNgaySinh().Ngay == 2);
    assert(!AnDocLai->CoTuoiUocLuong());
    const Patient* SoSinhDocLai = DocLai->TimBenhNhan("BN002");
    assert(SoSinhDocLai->LaBenhNhanVoDanh() && !SoSinhDocLai->CoNgaySinh());
    assert(SoSinhDocLai->CoTuoiUocLuong() && SoSinhDocLai->LayTuoiUocLuong() == 0);
    assert(!DocLai->TimBenhNhan("BN003")->CoTuoiUocLuong());
    delete DocLai;

    // Registry rỗng vẫn phải có tiêu đề để nạp lại được.
    const PatientRegistry Rong;
    QuanLy.LuuBenhNhan(Rong, DuongDan);
    DocLai = QuanLy.DocBenhNhan(DuongDan);
    assert(DocLai->SoBenhNhan() == 0);
    delete DocLai;
}

void KiemTraCsvChung() {
    // Parser chung giữ nguyên bản ghi nhiều dòng và dòng bắt đầu khi báo lỗi.
    std::istringstream Tep("\xEF\xBB\xBF\"A\",\"B\r\nC\",\"D\"\"E\",\r\nX,Y\n");
    CsvFile::BoQuaBom(Tep);
    int Dong = 1, Dau = 1;
    Vector<std::string> Cot;
    assert(CsvFile::DocBanGhi(Tep, Cot, Dong, Dau));
    assert(Cot.KichThuoc() == 4 && Cot[0] == "A" && Cot[1] == "B\r\nC");
    assert(Cot[2] == "D\"E" && Cot[3].empty() && Dau == 1 && Dong == 3);
    assert(CsvFile::DocBanGhi(Tep, Cot, Dong, Dau));
    assert(Cot[0] == "X" && Dau == 3 && Dong == 4);
    assert(!CsvFile::DocBanGhi(Tep, Cot, Dong, Dau));

    std::istringstream Loi("A,\"B\nC\"X\n");
    Dong = 7;
    bool DaBaoLoi = false;
    try { CsvFile::DocBanGhi(Loi, Cot, Dong, Dau); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = std::string(NgoaiLe.LayThongBao()).find("Dong 7") != std::string::npos;
    }
    assert(DaBaoLoi);

    std::istringstream LoiDoc;
    LoiDoc.setstate(std::ios::badbit);
    DaBaoLoi = false;
    try { CsvFile::DocBanGhi(LoiDoc, Cot, Dong, Dau); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = NgoaiLe.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi);
    // failbit không kèm eofbit là lỗi luồng, không được xem như hết file sạch.
    std::istringstream LoiTrangThai("A,B\n");
    LoiTrangThai.setstate(std::ios::failbit);
    DaBaoLoi = false;
    try { CsvFile::DocBanGhi(LoiTrangThai, Cot, Dong, Dau); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = NgoaiLe.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi);
    std::istringstream LoiBom("A,B\n");
    LoiBom.setstate(std::ios::failbit);
    DaBaoLoi = false;
    try { CsvFile::BoQuaBom(LoiBom); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = NgoaiLe.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi);
    std::ostringstream LoiGhi;
    LoiGhi.setstate(std::ios::badbit);
    DaBaoLoi = false;
    try { CsvFile::GhiBanGhi(LoiGhi, Cot); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = NgoaiLe.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi);
}

void KiemTraGhiAnToan() {
    // Thay file thành công, giữ file .tmp có sẵn và báo lỗi khi không mở được nơi lưu.
    GhiDuLieuThu("du lieu cu");
    { std::ofstream Tep(DuongDan + ".tmp"); Tep << "file cua nguoi khac"; }
    CsvFile::GhiFileAnToan(DuongDan, "du lieu moi");
    std::ifstream TepMoi(DuongDan);
    assert(std::string(std::istreambuf_iterator<char>(TepMoi), {}) == "du lieu moi");
    std::ifstream TepKhac(DuongDan + ".tmp");
    assert(std::string(std::istreambuf_iterator<char>(TepKhac), {}) == "file cua nguoi khac");
    std::remove((DuongDan + ".tmp").c_str());

    const std::string ThuMuc = DuongDan + ".directory";
    assert(std::filesystem::create_directory(ThuMuc));
    { std::ofstream Dau(ThuMuc + "/giu-lai"); Dau << "van con"; }
    bool DaBaoLoi = false;
    try { CsvFile::GhiFileAnToan(ThuMuc, "khong duoc thay thu muc"); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = NgoaiLe.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi && std::filesystem::exists(ThuMuc + "/giu-lai"));
    DaBaoLoi = false;
    try { PatientFileManager().LuuBenhNhan(PatientRegistry(), ThuMuc + "/thieu/a.csv"); }
    catch (const My_stdexcept& NgoaiLe) {
        DaBaoLoi = NgoaiLe.LayLoaiLoi() == My_stdexcept::LoaiLoi::LoiThucThi;
    }
    assert(DaBaoLoi && std::filesystem::exists(ThuMuc + "/giu-lai"));
    std::remove((ThuMuc + "/giu-lai").c_str());
    std::filesystem::remove(ThuMuc);
}
} // namespace

int main() {
    KiemTraDuLieuHopLe();
    KiemTraFileLoi();
    KiemTraGhiVaDocLai();
    KiemTraCsvChung();
    KiemTraGhiAnToan();
    std::remove(DuongDan.c_str());
}
