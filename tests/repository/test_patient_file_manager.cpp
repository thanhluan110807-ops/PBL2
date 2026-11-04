#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>

#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "repository/PatientFileManager.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"

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
} // namespace

int main() {
    KiemTraDuLieuHopLe();
    KiemTraFileLoi();
    std::remove(DuongDan.c_str());
}
