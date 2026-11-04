#include "repository/VisitFileManager.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "dsa/Vector.h"
#include "util/CsvFile.h"
#include "util/My_stdexcept.h"
#include <ctime>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <locale>
#include <limits>
#include <initializer_list>
#include <string>

namespace {
void Loi(const char* Chuoi) {
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, Chuoi);
}
void SoCot(const Vector<std::string>& Dong, int So) {
    if (Dong.KichThuoc() != So) Loi("So cot Visit CSV khong hop le");
}
long long DocSo(const std::string& Chuoi) {
    std::istringstream Vao(Chuoi); Vao.imbue(std::locale::classic());
    long long So = 0; Vao >> std::noskipws >> So;
    if (Vao.fail() || !Vao.eof()) Loi("So nguyen Visit khong hop le");
    return So;
}
unsigned long long DocThuTu(const std::string& Chuoi) {
    if (Chuoi.empty()) Loi("Thieu thu tu");
    unsigned long long So = 0;
    for (char C : Chuoi) {
        if (C < '0' || C > '9' || So > (std::numeric_limits<unsigned long long>::max() - (C-'0'))/10)
            Loi("Thu tu hang cho khong hop le");
        So = So*10 + (C-'0');
    }
    return So;
}
int DocInt(const std::string& Chuoi) {
    const long long So = DocSo(Chuoi);
    if (So < std::numeric_limits<int>::min() || So > std::numeric_limits<int>::max()) Loi("So nguyen vuot gioi han");
    return static_cast<int>(So);
}
bool DocBool(const std::string& Chuoi) {
    if (Chuoi == "0") return false;
    if (Chuoi == "1") return true;
    Loi("Gia tri bool phai la 0 hoac 1"); return false;
}
std::time_t DocLuc(const std::string& Chuoi) {
    const long long So = DocSo(Chuoi);
    const std::time_t Luc = static_cast<std::time_t>(So);
    if ((!std::numeric_limits<std::time_t>::is_signed && So < 0) || static_cast<long long>(Luc) != So)
        Loi("Thoi diem vuot gioi han time_t");
    return Luc;
}
double DocThuc(const std::string& Chuoi) {
    std::istringstream Vao(Chuoi); Vao.imbue(std::locale::classic());
    double So = 0; Vao >> std::noskipws >> So;
    if (Vao.fail() || !Vao.eof()) Loi("So thuc Visit khong hop le");
    return So; // VitalSigns kiểm tra NaN, vô cực và phạm vi số đo.
}
std::string SoThuc(double So) {
    std::ostringstream Ra; Ra.imbue(std::locale::classic());
    Ra << std::setprecision(std::numeric_limits<double>::max_digits10) << So; return Ra.str();
}
const char* TenTrangThai(VisitStatus T) {
    switch (T) {
    case VisitStatus::ChoKhamLanDau: return "ChoKhamLanDau";
    case VisitStatus::DangKham: return "DangKham";
    case VisitStatus::ChoCanLamSang: return "ChoCanLamSang";
    case VisitStatus::ChoKhamQuayLai: return "ChoKhamQuayLai";
    case VisitStatus::DaKetThuc: return "DaKetThuc";
    case VisitStatus::DaHuy: return "DaHuy";
    }
    Loi("Trang thai Visit khong hop le"); return "";
}
VisitStatus DocTrangThai(const std::string& Chuoi) {
    const VisitStatus CacLoai[] = {VisitStatus::ChoKhamLanDau,VisitStatus::DangKham,VisitStatus::ChoCanLamSang,
        VisitStatus::ChoKhamQuayLai,VisitStatus::DaKetThuc,VisitStatus::DaHuy};
    for (VisitStatus T : CacLoai) if (Chuoi == TenTrangThai(T)) return T;
    Loi("Trang thai Visit CSV khong hop le"); return VisitStatus::DaHuy;
}
void Ghi(std::ostream& Ra, std::initializer_list<std::string> CacO) {
    Vector<std::string> Dong; for (const auto& O : CacO) Dong.ThemCuoi(O);
    CsvFile::GhiBanGhi(Ra,Dong);
}
// Dựng riêng để kiểm tra cả liên kết và heap; chưa động vào dữ liệu bên gọi.
void KiemTra(const PatientRegistry& R, const Vector<BanChupLanKham>& CacLan, const TrangThaiHangCho& Hang) {
    PatientRegistry Tam;
    const auto CacHoSo = R.LayDanhSachBenhNhan();
    for (int I=0;I<CacHoSo.KichThuoc();++I) Tam.ThemBenhNhan(*CacHoSo[I]);
    Tam.KhoiPhucLanKham(CacLan);
    TriageService T(Tam); T.KhoiPhucHangCho(Hang);
}
void DocLichSu(const Vector<std::string>& D, BanChupLanKham& V) {
    if (D[0] == "DANHGIA") {
        SoCot(D,12);
        TriageAssessment A(VitalSigns(DocInt(D[6]),DocInt(D[7]),DocInt(D[8]),DocThuc(D[9]),DocInt(D[10])),
                           DocInt(D[3]),DocBool(D[4]),DocLuc(D[2]),D[5]);
        if (A.LayMuc() != DocInt(D[11])) Loi("Muc luu khong khop sinh hieu/tuoi cua luoc do v1");
        V.DanhGia.ThemCuoi(A);
    } else if (D[0] == "PHANCONG") { SoCot(D,3); V.BacSiPhanCong.ThemCuoi(D[2]); }
    else if (D[0] == "THAMGIA") { SoCot(D,3); V.BacSiThamGia.ThemCuoi(D[2]); }
    else if (D[0] == "CLS") {
        SoCot(D,6); V.CanLamSang.ThemCuoi(BanGhiCanLamSang(D[2],D[3],DocBool(D[4]),D[5]));
    } else Loi("Loai ban ghi Visit khong ho tro");
}
}

void VisitFileManager::LuuLanKham(const PatientRegistry& R,const TriageService& T,const std::string& Tep) const {
    Vector<BanChupLanKham> CacLan;
    const auto CacDong = R.TraCuuLanKham();
    for (int I=0;I<CacDong.KichThuoc();++I) CacLan.ThemCuoi(CacDong[I].LanKham->LayBanChup());
    const auto Hang = T.LayTrangThaiHangCho(); KiemTra(R,CacLan,Hang);
    std::ostringstream Ra; Ra.imbue(std::locale::classic());
    Ghi(Ra,{"PBL2_VISIT","1"});
    Ghi(Ra,{"META",std::to_string(Hang.ThuTu),Hang.UuTienQuayLai ? "1":"0"});
    int SoBanGhi=1;
    for (int I=0;I<CacLan.KichThuoc();++I) {
        const auto& V=CacLan[I];
        Ghi(Ra,{"VISIT",V.MaLanKham,V.MaBenhNhan,std::to_string(V.ThoiDiemTiepNhan),TenTrangThai(V.TrangThai),V.TrieuChungChinh}); ++SoBanGhi;
        for (int J=0;J<V.DanhGia.KichThuoc();++J) {
            const auto& A=V.DanhGia[J]; const auto& S=A.LaySinhHieu();
            Ghi(Ra,{"DANHGIA",V.MaLanKham,std::to_string(A.LayThoiDiemDanhGia()),std::to_string(A.LayTuoi()),
                A.LaTuoiUocLuong()?"1":"0",A.LayMaDieuDuong(),std::to_string(S.LayNhipTim()),std::to_string(S.LayHuyetApTamThu()),
                std::to_string(S.LayNhipTho()),SoThuc(S.LaySpo2()),std::to_string(S.LayDiemGcs()),std::to_string(A.LayMuc())}); ++SoBanGhi;
        }
        for (int J=0;J<V.BacSiPhanCong.KichThuoc();++J) { Ghi(Ra,{"PHANCONG",V.MaLanKham,V.BacSiPhanCong[J]}); ++SoBanGhi; }
        for (int J=0;J<V.BacSiThamGia.KichThuoc();++J) { Ghi(Ra,{"THAMGIA",V.MaLanKham,V.BacSiThamGia[J]}); ++SoBanGhi; }
        for (int J=0;J<V.CanLamSang.KichThuoc();++J) {
            const auto& C=V.CanLamSang[J];
            Ghi(Ra,{"CLS",V.MaLanKham,C.LayChiDinh(),C.LayMaBacSiChiDinh(),C.DaCoKetQua()?"1":"0",C.LayKetQua()}); ++SoBanGhi;
        }
    }
    for (int I=0;I<Hang.CacMuc.KichThuoc();++I) {
        const auto& M=Hang.CacMuc[I];
        Ghi(Ra,{"HANGCHO",M.MaLanKham,std::to_string(M.ThoiDiemVaoHang),std::to_string(M.ThuTu)}); ++SoBanGhi;
    }
    Ghi(Ra,{"KETTHUC",std::to_string(SoBanGhi)});
    CsvFile::GhiFileAnToan(Tep,Ra.str());
}

TrangThaiHangCho VisitFileManager::DocLanKham(PatientRegistry& R,const std::string& Tep) const {
    if (R.SoLanKham()!=0) Loi("Chi nap Visit luc khoi dong vao Registry chua co lan kham");
    std::ifstream Vao(std::filesystem::u8path(Tep),std::ios::binary);
    if (!Vao) throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Khong mo duoc file Visit");
    CsvFile::BoQuaBom(Vao);
    int Dong=1,Dau=1; Vector<std::string> D;
    if (!CsvFile::DocBanGhi(Vao,D,Dong,Dau)) Loi("Thieu tieu de Visit");
    SoCot(D,2); if (D[0]!="PBL2_VISIT" || D[1]!="1") Loi("Khong ho tro luoc do Visit nay");
    Vector<BanChupLanKham> CacLan; TrangThaiHangCho Hang;
    bool CoMeta=false,DaHet=false; int SoBanGhi=0,ChiSo=-1;
    while (CsvFile::DocBanGhi(Vao,D,Dong,Dau)) {
        try {
            if (DaHet) Loi("Con du lieu sau KETTHUC");
            if (D[0]=="KETTHUC") {
                SoCot(D,2); if (DocInt(D[1])!=SoBanGhi) Loi("So ban ghi Visit khong khop");
                DaHet=true; continue;
            }
            if (D[0]=="META") {
                SoCot(D,3); if (CoMeta || SoBanGhi) Loi("META phai dung mot lan o dau");
                Hang.ThuTu=DocThuTu(D[1]); Hang.UuTienQuayLai=DocBool(D[2]); CoMeta=true;
            } else {
                if (!CoMeta) Loi("Thieu META");
                if (D[0]=="VISIT") {
                    SoCot(D,6); BanChupLanKham V;
                    V.MaLanKham=D[1]; V.MaBenhNhan=D[2]; V.ThoiDiemTiepNhan=DocLuc(D[3]);
                    V.TrangThai=DocTrangThai(D[4]); V.TrieuChungChinh=D[5];
                    CacLan.ThemCuoi(V); ChiSo=CacLan.KichThuoc()-1;
                } else if (D[0]=="HANGCHO") {
                    SoCot(D,4); Hang.CacMuc.ThemCuoi({D[1],DocLuc(D[2]),DocThuTu(D[3])}); ChiSo=-1;
                } else {
                    if (D.KichThuoc()<2 || ChiSo<0 || D[1]!=CacLan[ChiSo].MaLanKham) Loi("Lich su khong thuoc Visit hien tai");
                    DocLichSu(D,CacLan[ChiSo]);
                }
            }
            ++SoBanGhi;
        } catch (const My_stdexcept& E) {
            const std::string ThongBao="Dong "+std::to_string(Dau)+": "+E.LayThongBao();
            throw My_stdexcept(E.LayLoaiLoi(),ThongBao.c_str());
        }
    }
    if (!DaHet || !CoMeta) Loi("File Visit chua du: thieu META/KETTHUC");
    KiemTra(R,CacLan,Hang); // Bao gồm kiểm tra heap; lỗi không làm Registry bên gọi thay đổi.
    R.KhoiPhucLanKham(CacLan);
    return Hang;
}
