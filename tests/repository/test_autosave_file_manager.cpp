#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#ifndef _WIN32
#include <csignal>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include "dsa/Vector.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "repository/PatientFileManager.h"
#include "repository/PatientRegistry.h"
#include "repository/SessionFileManager.h"
#include "service/TriageService.h"
#include "util/CsvFile.h"
#include "util/My_stdexcept.h"

namespace {
namespace Fs = std::filesystem;
void GhiThu(const Fs::path& Tep, const std::string& NoiDung) {
    std::ofstream Ghi(Tep, std::ios::binary | std::ios::trunc);
    Ghi << NoiDung; Ghi.close(); assert(Ghi.good());
}
std::string DocThu(const Fs::path& Tep) {
    std::ifstream Doc(Tep, std::ios::binary); assert(Doc.is_open());
    return std::string(std::istreambuf_iterator<char>(Doc), {});
}
Fs::path LayKho(const Fs::path& Patient) { return Fs::u8path(Patient.u8string() + ".pbl2"); }
std::string TaoNhatKy(const std::string& Ten, const Fs::path& Patient, const Fs::path& Visit) {
    Vector<std::string> Cot;
    Cot.ThemCuoi("PBL2_PENDING"); Cot.ThemCuoi("1"); Cot.ThemCuoi(Ten);
    Cot.ThemCuoi(Patient.u8string()); Cot.ThemCuoi(Visit.u8string());
    std::ostringstream Tep; CsvFile::GhiBanGhi(Tep, Cot); return Tep.str();
}
void KiemTraLuuTrucTiep(const Fs::path& Patient, const Fs::path& Visit) {
    const std::string Goc = "\xEF\xBB\xBFMaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong\r\nP,An,Nam,,001,ChuaXacDinh,,,0\r\n";
    GhiThu(Patient, Goc);
    SessionFileManager F; TrangThaiHangCho Hang;
    PatientRegistry* R = F.DocBoFile(Patient.u8string(), Visit.u8string(), Hang);
    assert(R->SoBenhNhan() == 1 && R->SoLanKham() == 0);
    std::string BanLuu;
    { TriageService T(*R); T.TiepNhan("V", "P", 10, "Tho met");
      T.DanhGia("V", VitalSigns(140,80,40,98.125,15), 11, "DD");
      BanLuu = F.LuuVaoBoFile(*R, T, Patient.u8string(), Visit.u8string()); }
    const Fs::path Kho = LayKho(Patient);
    assert(DocThu(Kho / "backup-original" / "patient.csv") == Goc);
    assert(!Fs::exists(Kho / "backup-original" / "visit.csv"));
    assert(Fs::exists(Kho / "backup-original" / "manifest.csv"));
    assert(!Fs::exists(Kho / "pending.csv"));
    assert(DocThu(Patient) == DocThu(Fs::u8path(BanLuu) / "patient.csv"));
    assert(DocThu(Visit) == DocThu(Fs::u8path(BanLuu) / "visit.csv"));
    delete R;
    R = F.DocBoFile(Patient.u8string(), Visit.u8string(), Hang);
    { TriageService Tai(*R); Tai.KhoiPhucHangCho(Hang); assert(Tai.GoiTiep("BS").LayMaLanKham() == "V"); }
    // Sửa file chính từ bên ngoài phải được tôn trọng khi không có journal.
    R->ThemBenhNhan("EXTERNAL"); PatientFileManager().LuuBenhNhan(*R, Patient.u8string());
    delete R;
    R = F.DocBoFile(Patient.u8string(), Visit.u8string(), Hang);
    assert(R->TimBenhNhan("EXTERNAL") != nullptr);
    { TriageService Tai(*R); Tai.KhoiPhucHangCho(Hang);
      F.LuuVaoBoFile(*R, Tai, Patient.u8string(), Visit.u8string()); }
    delete R;
    assert(DocThu(Kho / "backup-original" / "patient.csv") == Goc);
    const std::string Cu = DocThu(Visit); Fs::remove(Visit);
    bool Loi = false;
    try { delete F.DocBoFile(Patient.u8string(), Visit.u8string(), Hang); }
    catch (const My_stdexcept&) { Loi = true; } assert(Loi);
    GhiThu(Visit, Cu);
    Loi = false;
    PatientRegistry Rong; TriageService TRong(Rong);
    try { F.LuuVaoBoFile(Rong, TRong, Patient.u8string(), Patient.u8string()); }
    catch (const My_stdexcept&) { Loi = true; } assert(Loi);
}
void KiemTraNhatKyLoi(const Fs::path& Patient, const Fs::path& Visit) {
    const Fs::path Kho = LayKho(Patient);
    const std::string PCu = DocThu(Patient), VCu = DocThu(Visit);
    const std::string CacLoi[] = { "", "PBL2_PENDING,2,phien-x,a,b\n",
        TaoNhatKy("../phien-x", Patient, Visit), TaoNhatKy("phien-a\\x", Patient, Visit),
        TaoNhatKy("phien-khong-ton-tai", Patient, Visit),
        TaoNhatKy("phien-x", Patient, Fs::u8path(Visit.u8string() + "khac")),
        TaoNhatKy("phien-x", Patient, Visit) + "THUA\n" };
    for (const std::string& NoiDung : CacLoi) {
        GhiThu(Kho / "pending.csv", NoiDung);
        TrangThaiHangCho Hang; Hang.ThuTu = 888; bool Loi = false;
        try { delete SessionFileManager().DocBoFile(Patient.u8string(), Visit.u8string(), Hang); }
        catch (const My_stdexcept&) { Loi = true; }
        assert(Loi && Hang.ThuTu == 888 && DocThu(Patient) == PCu && DocThu(Visit) == VCu);
    }
    Fs::remove(Kho / "pending.csv");
}
void KiemTraBoMoi(const Fs::path& Patient, const Fs::path& Visit) {
    PatientRegistry R; TriageService T(R); SessionFileManager F;
    F.LuuVaoBoFile(R, T, Patient.u8string(), Visit.u8string());
    const Fs::path Backup = LayKho(Patient) / "backup-original";
    assert(!Fs::exists(Backup / "patient.csv") && !Fs::exists(Backup / "visit.csv"));
    assert(Fs::exists(Backup / "manifest.csv"));
    TrangThaiHangCho Hang; PatientRegistry* Moi = F.DocBoFile(Patient.u8string(), Visit.u8string(), Hang);
    assert(Moi->SoBenhNhan() == 0 && Moi->SoLanKham() == 0); delete Moi;
}
#ifndef _WIN32
void KiemTraLanDauLoi(const Fs::path& Patient,const Fs::path& Visit) {
    const std::string Goc="MaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong\nP,An,Nam,,,O,,,30\n";
    GhiThu(Patient,Goc);
    PatientRegistry R; R.ThemBenhNhan("P","An"); R.ThemBenhNhan("MOI",std::string(8192,'x')); TriageService T(R);
    struct rlimit Cu; assert(getrlimit(RLIMIT_FSIZE,&Cu)==0);
    const struct rlimit GioiHan{1024,Cu.rlim_max};
    const auto XuLyCu=std::signal(SIGXFSZ,SIG_IGN); assert(setrlimit(RLIMIT_FSIZE,&GioiHan)==0);
    bool GhiLoi=false;
    try { SessionFileManager().LuuVaoBoFile(R,T,Patient.u8string(),Visit.u8string()); }
    catch(const My_stdexcept&) { GhiLoi=true; }
    assert(setrlimit(RLIMIT_FSIZE,&Cu)==0); std::signal(SIGXFSZ,XuLyCu);
    assert(GhiLoi && !Fs::exists(Visit) && !Fs::exists(LayKho(Patient)/"pending.csv"));
    assert(Fs::exists(LayKho(Patient)/"backup-original"));
    TrangThaiHangCho Hang; PatientRegistry* Tai=nullptr;
    try { Tai=SessionFileManager().DocBoFile(Patient.u8string(),Visit.u8string(),Hang); }
    catch(const My_stdexcept&) {}
    assert(Tai && Tai->SoBenhNhan()==1 && Tai->SoLanKham()==0);
    delete Tai; assert(DocThu(Patient)==Goc);
}
void KiemTraLoiGhiGiuFile(const Fs::path& Patient, const Fs::path& Visit) {
    const std::string PCu = DocThu(Patient), VCu = DocThu(Visit);
    struct rlimit Cu; assert(getrlimit(RLIMIT_FSIZE, &Cu) == 0);
    const struct rlimit Moi{128, Cu.rlim_max};
    const auto XuLyCu = std::signal(SIGXFSZ, SIG_IGN);
    assert(setrlimit(RLIMIT_FSIZE, &Moi) == 0);
    PatientRegistry R; R.ThemBenhNhan("LOI", std::string(8192, 'x')); TriageService T(R);
    bool Loi = false;
    try { SessionFileManager().LuuVaoBoFile(R, T, Patient.u8string(), Visit.u8string()); }
    catch (const My_stdexcept&) { Loi = true; }
    assert(setrlimit(RLIMIT_FSIZE, &Cu) == 0); std::signal(SIGXFSZ, XuLyCu);
    assert(Loi && DocThu(Patient) == PCu && DocThu(Visit) == VCu);
}
#ifdef __linux__
void KiemTraLoiDongBoThuMuc() {
    // procfs từ chối fsync: lỗi đồng bộ thật phải được báo cho bên gọi.
    if (!Fs::is_directory("/proc")) return;
    bool Loi = false;
    try { CsvFile::DongBoThuMuc("/proc"); }
    catch (const My_stdexcept&) { Loi = true; }
    assert(Loi);
}
#endif
void KiemTraDungDotNgot(const Fs::path& Patient, const Fs::path& Visit) {
    SessionFileManager F;
    PatientRegistry Cu; Cu.ThemBenhNhan("P"); TriageService TCu(Cu); TCu.TiepNhan("V", "P", 10, "cu");
    F.LuuVaoBoFile(Cu, TCu, Patient.u8string(), Visit.u8string());
    for (int GiaiDoan = 0; GiaiDoan < 3; ++GiaiDoan) {
        const pid_t Con = fork(); assert(Con >= 0);
        if (Con == 0) {
            PatientRegistry Moi; Moi.ThemBenhNhan("P"); Moi.ThemBenhNhan("P2");
            TriageService TMoi(Moi); TMoi.TiepNhan("V2", "P2", 20, "moi");
            const Fs::path BanLuu = Fs::u8path(F.LuuPhien(Moi, TMoi, LayKho(Patient).u8string()));
            if (GiaiDoan >= 1) CsvFile::GhiFileAnToan((LayKho(Patient) / "pending.csv").u8string(),
                TaoNhatKy(BanLuu.filename().u8string(), Patient, Visit));
            if (GiaiDoan == 2) CsvFile::GhiFileAnToan(Patient.u8string(), DocThu(BanLuu / "patient.csv"));
            kill(getpid(), SIGKILL); _exit(99);
        }
        int TrangThai = 0; assert(waitpid(Con, &TrangThai, 0) == Con);
        assert(WIFSIGNALED(TrangThai) && WTERMSIG(TrangThai) == SIGKILL);
        TrangThaiHangCho Hang; PatientRegistry* R = F.DocBoFile(Patient.u8string(), Visit.u8string(), Hang);
        assert(R->SoBenhNhan() == (GiaiDoan ? 2 : 1));
        assert(R->TimLanKham(GiaiDoan ? "V2" : "V") != nullptr); delete R;
        assert(!Fs::exists(LayKho(Patient) / "pending.csv"));
        // Trả baseline cho điểm dừng kế tiếp.
        F.LuuVaoBoFile(Cu, TCu, Patient.u8string(), Visit.u8string());
    }
}
#endif
} // namespace
int main() {
    const Fs::path Goc = Fs::absolute(Fs::u8path(u8"test_autosave_owned-ử"));
    assert(Fs::create_directory(Goc));
    const Fs::path Patient = Goc / "HoSo.csv", Visit = Goc / "LanKham.csv";
    KiemTraLuuTrucTiep(Patient, Visit); KiemTraNhatKyLoi(Patient, Visit);
    KiemTraBoMoi(Goc / "Moi.csv", Goc / "MoiVisit.csv");
#ifndef _WIN32
    KiemTraLanDauLoi(Goc/"LanDau.csv",Goc/"LanDauVisit.csv");
    KiemTraLoiGhiGiuFile(Patient, Visit);
    KiemTraDungDotNgot(Goc / "Crash.csv", Goc / "CrashVisit.csv");
#ifdef __linux__
    KiemTraLoiDongBoThuMuc();
#endif
#endif
    Fs::remove_all(Goc);
}
