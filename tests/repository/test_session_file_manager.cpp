#include "repository/SessionFileManager.h"
#include "repository/PatientFileManager.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "model/Patient.h"
#include "model/Gender.h"
#include "model/Visit.h"
#include "util/My_stdexcept.h"
#include <filesystem>
#include <cassert>
#include <string>

int main() {
    namespace Fs=std::filesystem;
    const Fs::path Goc="test_session_owned";
    assert(Fs::create_directory(Goc));
    PatientRegistry R; Patient P("P","Nguyễn, \"An\"\nX",Gender::Nam); P.DatTuoiUocLuong(0); R.ThemBenhNhan(P);
    TriageService T(R); T.TiepNhan("V","P",10,"Thử"); T.DanhGia("V",VitalSigns(140,80,40,98.125,15),11,"DD");
    SessionFileManager F;
    const std::string Lan1=F.LuuPhien(R,T,Goc.u8string());
    T.GoiTiep("BS");
    const std::string Lan2=F.LuuPhien(R,T,Goc.u8string()); assert(Lan1!=Lan2);
    TrangThaiHangCho Hang;
    PatientRegistry* Moi=F.DocPhien((Fs::u8path(Lan1)/"patient.csv").u8string(),(Fs::u8path(Lan1)/"visit.csv").u8string(),Hang);
    { TriageService Tai(*Moi); Tai.KhoiPhucHangCho(Hang); assert(Tai.SoDangCho()==1);
      assert(Moi->TimBenhNhan("P")->LayTuoiUocLuong()==0); assert(Tai.GoiTiep("BS").LayMaLanKham()=="V"); }
    delete Moi;
    Moi=F.DocPhien((Fs::u8path(Lan2)/"patient.csv").u8string(),(Fs::u8path(Lan2)/"visit.csv").u8string(),Hang);
    assert(Moi->TimLanKham("V")->LayTrangThai()==VisitStatus::DangKham); delete Moi;
    bool Loi=false;
    try { delete F.DocPhien((Fs::u8path(Lan1)/"patient.csv").u8string(),"khong_co.csv",Hang); }
    catch(const My_stdexcept&) { Loi=true; } assert(Loi);
    // Lưu thất bại không làm mất hai bản đã lưu.
    Loi=false; try { F.LuuPhien(R,T,(Goc/"khong-co").u8string()); } catch(const My_stdexcept&) { Loi=true; }
    assert(Loi && Fs::exists(Fs::u8path(Lan1)/"visit.csv") && Fs::exists(Fs::u8path(Lan2)/"patient.csv"));
    Fs::remove_all(Goc); // Chỉ thư mục riêng do test tự tạo.
}
