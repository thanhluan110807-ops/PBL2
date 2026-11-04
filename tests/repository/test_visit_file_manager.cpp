#include <cassert>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <string>
#include <iterator>
#include "repository/PatientFileManager.h"
#include "repository/VisitFileManager.h"
#include "repository/PatientRegistry.h"
#include "model/Patient.h"
#include "model/Gender.h"
#include "model/Visit.h"
#include "service/TriageService.h"
#include "util/My_stdexcept.h"

namespace {
const char* Tep = "test_visit_roundtrip.csv";
void Them(PatientRegistry& R,const char* Ma) {
    Patient P(Ma,"Tên, \"thử\"\nCSV",Gender::Nu); P.DatTuoiUocLuong(30); R.ThemBenhNhan(P);
}
void VaoHang(TriageService& T,const char* Ma,const char* P) {
    T.TiepNhan(Ma,P,100,"Triệu chứng, \"đau\"\nDòng 2");
    T.DanhGia(Ma,VitalSigns(80,120,18,98.25,15),101,"DD1");
}
void KiemTraKhoiPhuc() {
    PatientRegistry R; for(const char* Ma:{"P1","P2","P3","P4","P5","P6"}) Them(R,Ma);
    TriageService T(R);
    VaoHang(T,"V1","P1"); T.GoiTiep("BS1");
    T.ChuyenCanLamSang("V1","Xét nghiệm, \"thử\"\nABC","BS1"); T.QuayLai("V1","KQ\nBình thường",102);
    VaoHang(T,"V2","P2");
    assert(T.GoiTiep("BS2").LayMaLanKham()=="V1"); // Lượt hòa sau phải lần đầu.
    T.ChuyenCanLamSang("V1","Chụp","BS2"); T.QuayLai("V1","KQ2",103);
    T.DanhGia("V1",VitalSigns(80,120,18,98.25,15),104,"DD2");
    VaoHang(T,"V3","P3"); T.DanhGia("V3",VitalSigns(80,120,18,89,15),105,"DD1");
    assert(T.GoiTiep("BS3").LayMaLanKham()=="V3"); T.KetThuc("V3");
    T.TiepNhan("V4","P4",100,"Chờ đánh giá");
    T.TiepNhan("V5","P5",100,"Hủy"); T.Huy("V5");
    VaoHang(T,"V6","P6"); T.DanhGia("V6",VitalSigns(80,120,18,89,15),105,"DD1");
    T.GoiTiep("BS4"); T.ChuyenCanLamSang("V6","CLS chưa có kết quả","BS4");
    VisitFileManager F; F.LuuLanKham(R,T,Tep);
    PatientRegistry Moi; for(const char* Ma:{"P1","P2","P3","P4","P5","P6"}) Them(Moi,Ma);
    const auto Hang=F.DocLanKham(Moi,Tep); TriageService KhoiPhuc(Moi); KhoiPhuc.KhoiPhucHangCho(Hang);
    assert(Moi.SoLanKham()==6 && KhoiPhuc.SoDangCho()==2);
    assert(Moi.TimLanKham("V1")->SoDanhGia()==2);
    assert(Moi.TimLanKham("V1")->SoLanCanLamSang()==2);
    assert(Moi.TimLanKham("V1")->LayCanLamSangTai(0).LayKetQua()=="KQ\nBình thường");
    assert(Moi.TimLanKham("V1")->LayBacSiThamGiaKhamTai(1)=="BS2");
    assert(Moi.TimLanKham("V1")->LayDanhGiaGanNhat().LaySinhHieu().LaySpo2()==98.25);
    assert(Moi.TimLanKham("V3")->LayTrangThai()==VisitStatus::DaKetThuc);
    assert(Moi.TimLanKham("V4")->SoDanhGia()==0);
    assert(Moi.TimLanKham("V5")->LayTrangThai()==VisitStatus::DaHuy);
    assert(Moi.TimLanKham("V6")->LayTrangThai()==VisitStatus::ChoCanLamSang);
    assert(KhoiPhuc.GoiTiep("BS5").LayMaLanKham()=="V2");
    assert(KhoiPhuc.GoiTiep("BS6").LayMaLanKham()=="V1");
    F.LuuLanKham(Moi,KhoiPhuc,Tep); // Đang khám cũng phải round-trip.
    PatientRegistry Lan3; for(const char* Ma:{"P1","P2","P3","P4","P5","P6"}) Them(Lan3,Ma);
    F.DocLanKham(Lan3,Tep); assert(Lan3.TimLanKham("V2")->LayTrangThai()==VisitStatus::DangKham);
    F.LuuLanKham(R,T,Tep); // Giữ bản có hai mục chờ cho các kiểm tra file lỗi bên dưới.
}
void KiemTraFifo() {
    PatientRegistry R; Them(R,"P1"); Them(R,"P2"); TriageService T(R);
    VaoHang(T,"Z","P1"); VaoHang(T,"A","P2");
    // Cùng thời điểm: Z vào trước; không được sắp theo mã sau khi đọc lại.
    VisitFileManager F; F.LuuLanKham(R,T,Tep);
    PatientRegistry Moi; Them(Moi,"P1"); Them(Moi,"P2");
    const auto Hang=F.DocLanKham(Moi,Tep); TriageService Tai(Moi); Tai.KhoiPhucHangCho(Hang);
    assert(Tai.GoiTiep("BS").LayMaLanKham()=="Z");
    assert(Tai.GoiTiep("BS").LayMaLanKham()=="A");
    const auto Cu=T.LayTrangThaiHangCho(); auto Sai=Cu;
    Sai.CacMuc[1].ThuTu=Sai.CacMuc[0].ThuTu;
    bool Loi=false; try { T.KhoiPhucHangCho(Sai); } catch(const My_stdexcept&) { Loi=true; }
    assert(Loi && T.LayDanhSachCho()[0]->LayMaLanKham()=="Z");
}
void KiemTraLoi() {
    std::ifstream Vao(Tep,std::ios::binary); const std::string DayDu((std::istreambuf_iterator<char>(Vao)),{});
    assert(!DayDu.empty());
    const auto ThuLoi=[&](const std::string& Chuoi) {
        { std::ofstream Ra(Tep,std::ios::binary|std::ios::trunc); Ra<<Chuoi; }
        PatientRegistry R; for(const char* Ma:{"P1","P2","P3","P4","P5","P6"}) Them(R,Ma); bool Loi=false;
        try { VisitFileManager().DocLanKham(R,Tep); } catch(const My_stdexcept&) { Loi=true; }
        assert(Loi && R.SoLanKham()==0 && R.SoBenhNhan()==6);
    };
    ThuLoi(DayDu.substr(0,DayDu.rfind("KETTHUC,")));
    ThuLoi(DayDu + "LA,DU,LIEU\n");
    std::string Sai=DayDu; Sai.replace(Sai.find("PBL2_VISIT,1"),12,"PBL2_VISIT,9"); ThuLoi(Sai);
    Sai=DayDu; auto ViTri=Sai.find("HANGCHO,"); assert(ViTri!=std::string::npos);
    Sai.replace(ViTri,7,"KHONGBIET"); ThuLoi(Sai);
    Sai=DayDu; ViTri=Sai.find("META,"); auto Cuoi=Sai.find('\n',ViTri);
    Sai.replace(ViTri,Cuoi-ViTri,"META,0,1"); ThuLoi(Sai); // Mục chờ có thứ tự vượt metadata.
    { std::ofstream Ra(Tep,std::ios::binary|std::ios::trunc); Ra<<DayDu; }
    PatientRegistry Thieu; Them(Thieu,"P1"); bool DaLoi=false;
    try { VisitFileManager().DocLanKham(Thieu,Tep); } catch(const My_stdexcept&) { DaLoi=true; }
    assert(DaLoi && Thieu.SoLanKham()==0);
}
}
int main() {
    KiemTraFifo();
    KiemTraKhoiPhuc();
    KiemTraLoi();
    std::remove(Tep);
}
