#include "model/BacSi.h"
#include "repository/ScheduleRegistry.h"
#include "model/DieuDuong.h"
#include "service/SchedulerService.h"
#include "util/My_stdexcept.h"
#include <cmath>
namespace {
const Shift* TimCa(const Schedule& Lich, const std::string& Ma) {
    for (int I = 0; I < Lich.CacCa.KichThuoc(); ++I)
        if (Lich.CacCa[I].LayMaCaTruc() == Ma) return &Lich.CacCa[I];
    return nullptr;
}
void KiemTraLuat(const SchedulerRules& L) {
    if (!std::isfinite(L.GioToiDaTuan) || L.GioToiDaTuan <= 0 ||
        !std::isfinite(L.DiemMetMoiToiDa) || L.DiemMetMoiToiDa < 0 ||
        L.NghiGiuaCaPhut < 0 || L.NghiSauCaDemPhut < 0)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Rang buoc lich khong hop le");
}
bool DungVaiTro(const NhanVienYTe& NV, AssignmentRole VaiTro) {
    if (VaiTro == AssignmentRole::DieuDuong) return NV.LayLoaiNhanVien() == StaffType::DieuDuong;
    if (VaiTro == AssignmentRole::TruongCa) return NV.CoTheLamTruongCa();
    return VaiTro == AssignmentRole::BacSi && NV.LayLoaiNhanVien() == StaffType::BacSi;
}
}
bool SchedulerService::CoThePhanCong(const NhanVienYTe& NV, const Shift& Ca,
                                    AssignmentRole VaiTro, const Schedule& Lich, const SchedulerRules& Luat) {
    KiemTraLuat(Luat);
    if (!Ca.HopLe() || !DungVaiTro(NV, VaiTro)) return false;
    if (NV.LayGioLamTuan() + FatigueService::TinhGio(NV.LayMaDinhDanh(), Lich) + Ca.TinhSoGio() > Luat.GioToiDaTuan) return false;
    if (FatigueService::TinhDiem(NV, Lich) + FatigueService::DiemCa(Ca) > Luat.DiemMetMoiToiDa) return false;
    for (int I = 0; I < Lich.PhanCong.KichThuoc(); ++I) {
        const auto& P = Lich.PhanCong[I];
        if (!P.DangHieuLuc() || P.MaNhanVien != NV.LayMaDinhDanh()) continue;
        const Shift* Cu = TimCa(Lich, P.MaCa);
        if (!Cu || P.MaCa == Ca.LayMaCaTruc()) return false;
        if (Ca.CoChongGio(*Cu)) return false;
        // Kiểm tra cả hai chiều vì ca đầu vào có thể chưa được sắp xếp theo giờ.
        const Shift& Truoc = Cu->LayBatDau() < Ca.LayBatDau() ? *Cu : Ca;
        const Shift& Sau = Cu->LayBatDau() < Ca.LayBatDau() ? Ca : *Cu;
        const int Nghi = Truoc.LaCaDem() ? Luat.NghiSauCaDemPhut : Luat.NghiGiuaCaPhut;
        if (std::difftime(Sau.LayBatDau(), Truoc.LayKetThuc()) < Nghi * 60.0) return false;
    }
    return true;
}
namespace {
struct OTrong { int Ca = 0; AssignmentRole VaiTro = AssignmentRole::BacSi; };
// Trạng thái tìm kiếm nằm cục bộ: không tăng giờ/điểm trên nhân viên thật.
struct TimKiem {
    const StaffRegistry& NhanSu;
    const SchedulerRules& Luat;
    Vector<OTrong> CacO;
    ScheduleResult KQ;
    int GioiHan;
    bool HetLuot = false;
    TimKiem(const StaffRegistry& N, const SchedulerRules& L, int G) : NhanSu(N), Luat(L), GioiHan(G) {}
    double DiemUuTien(const NhanVienYTe& NV, const Shift& Ca) const {
        double Diem = FatigueService::TinhGio(NV.LayMaDinhDanh(), KQ.Lich);
        for (int I=0;I<KQ.Lich.PhanCong.KichThuoc();++I) {
            const auto& P=KQ.Lich.PhanCong[I];
            if (P.MaNhanVien!=NV.LayMaDinhDanh()) continue;
            const auto* C=TimCa(KQ.Lich,P.MaCa);
            if (C) Diem += (C->LaCaDem()?2:0) + (C->LaCuoiTuan()?1:0);
        }
        const auto& M=NV.LayCaMongMuon();
        if (Ca.LayPhutBatDau()%1440==M.GioBatDau*60 && Ca.LayPhutKetThuc()%1440==M.GioKetThuc*60) Diem-=0.25;
        if (Ca.LaCuoiTuan()==M.MongMuonCuoiTuan) Diem-=0.1;
        return Diem;
    }
    bool Thu(int O) {
        if (O == CacO.KichThuoc()) return true;
        const Shift& Ca = KQ.Lich.CacCa[CacO[O].Ca];
        Vector<int> UngVien;
        for (int I = 0; I < NhanSu.KichThuoc(); ++I) UngVien.ThemCuoi(I);
        // Ưu tiên người ít giờ hơn; đây là thứ tự thử, không phải chứng minh tối ưu.
        for (int I = 1; I < UngVien.KichThuoc(); ++I) {
            int J = I;
            while (J > 0 && DiemUuTien(NhanSu.Lay(UngVien[J]),Ca) <
                             DiemUuTien(NhanSu.Lay(UngVien[J-1]),Ca)) {
                int Tam = UngVien[J]; UngVien[J] = UngVien[J-1]; UngVien[J-1] = Tam; --J;
            }
        }
        for (int I = 0; I < UngVien.KichThuoc(); ++I) {
            if (KQ.SoLuotThu >= GioiHan) { HetLuot = true; return false; }
            ++KQ.SoLuotThu;
            const auto& NV = NhanSu.Lay(UngVien[I]);
            if (!SchedulerService::CoThePhanCong(NV, Ca, CacO[O].VaiTro, KQ.Lich, Luat)) continue;
            KQ.Lich.PhanCong.ThemCuoi({NV.LayMaDinhDanh(), Ca.LayMaCaTruc(), CacO[O].VaiTro});
            if (Thu(O + 1)) return true;
            KQ.Lich.PhanCong.XoaCuoi(); // Hoàn tác đúng một lựa chọn trước khi thử người khác.
            if (HetLuot) return false;
        }
        return false;
    }
};
}
ScheduleResult SchedulerService::LapLich(const StaffRegistry& NhanSu, const Vector<Shift>& CacCa,
                                        const SchedulerRules& Luat, int GioiHanLuot) {
    KiemTraLuat(Luat);
    if (CacCa.Rong() || CacCa.KichThuoc() > 32 || NhanSu.KichThuoc() > 100 || GioiHanLuot <= 0)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Can 1-32 ca, toi da 100 nhan vien va gioi han tim duong");
    TimKiem T(NhanSu, Luat, GioiHanLuot);
    T.KQ.Lich.CacCa = CacCa;
    int SoO = 0;
    for (int I = 0; I < CacCa.KichThuoc(); ++I) {
        const Shift& Ca = CacCa[I];
        if (!Ca.HopLe()) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ca truc khong hop le");
        for (int J = 0; J < I; ++J)
            if (CacCa[J].LayMaCaTruc() == Ca.LayMaCaTruc())
                throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Trung ma ca");
        SoO += Ca.SoBacSi + Ca.SoDieuDuong;
        if (SoO > 128) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Toi da 128 vi tri phan cong");
        for (int J = 0; J < Ca.SoBacSi; ++J)
            T.CacO.ThemCuoi({I, J == 0 && Ca.CanTruongCa ? AssignmentRole::TruongCa : AssignmentRole::BacSi});
        for (int J = 0; J < Ca.SoDieuDuong; ++J) T.CacO.ThemCuoi({I, AssignmentRole::DieuDuong});
    }
    if (T.Thu(0)) T.KQ.TrangThai = ScheduleStatus::ThanhCong;
    else T.KQ.TrangThai = T.HetLuot ? ScheduleStatus::HetLuotTim : ScheduleStatus::KhongCoLich;
    return T.KQ;
}
bool SchedulerService::LichHopLe(const StaffRegistry& NhanSu, const Schedule& Lich, const SchedulerRules& Luat) {
    KiemTraLuat(Luat);
    Schedule Tam;
    Tam.CacCa = Lich.CacCa;
    if (Lich.CacCa.KichThuoc() > 32 || Lich.PhanCong.KichThuoc() > 512) return false;
    for (int I = 0; I < Lich.CacCa.KichThuoc(); ++I) {
        if (!Lich.CacCa[I].HopLe()) return false;
        for (int J = 0; J < I; ++J) if (Lich.CacCa[I].LayMaCaTruc() == Lich.CacCa[J].LayMaCaTruc()) return false;
    }
    for (int I = 0; I < Lich.PhanCong.KichThuoc(); ++I) {
        const auto& P = Lich.PhanCong[I];
        if (P.LayMaPhanCong().empty()) return false;
        for (int J=0; J<I; ++J) if (Lich.PhanCong[J].LayMaPhanCong()==P.LayMaPhanCong()) return false;
        const auto* NV = NhanSu.Tim(P.MaNhanVien);
        const auto* Ca = TimCa(Lich, P.MaCa);
        if (!NV || !Ca) return false;
        if (!P.DangHieuLuc()) continue;
        if (!CoThePhanCong(*NV,*Ca,P.VaiTro,Tam,Luat)) return false;
        Tam.PhanCong.ThemCuoi(P);
    }
    for (int I = 0; I < Lich.CacCa.KichThuoc(); ++I) {
        const auto& Ca = Lich.CacCa[I];
        int BS = 0, DD = 0, Truong = 0;
        for (int J = 0; J < Lich.PhanCong.KichThuoc(); ++J) {
            const auto& P = Lich.PhanCong[J];
            if (!P.DangHieuLuc() || P.MaCa != Ca.LayMaCaTruc()) continue;
            if (P.VaiTro == AssignmentRole::DieuDuong) ++DD;
            else { ++BS; if (P.VaiTro == AssignmentRole::TruongCa) ++Truong; }
        }
        if (BS < Ca.SoBacSi || DD < Ca.SoDieuDuong || Truong > 1 || (Ca.CanTruongCa && Truong != 1)) return false;
    }
    return !Lich.CacCa.Rong() || Lich.PhanCong.Rong();
}
Vector<std::string> SchedulerService::TimNguoiThay(const StaffRegistry& NhanSu, const Schedule& Lich,
                                                 int ViTri, const SchedulerRules& Luat, const StandbyPool* DuPhong) {
    if (!LichHopLe(NhanSu, Lich, Luat))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Lich goc khong hop le");
    const auto P = Lich.PhanCong[ViTri];
    if (!P.DangHieuLuc() || P.LayTrangThai()==AssignmentStatus::DangTruc)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Chi thay phan cong hieu luc chua bat dau");
    Schedule Tam; Tam.CacCa = Lich.CacCa;
    for (int I = 0; I < Lich.PhanCong.KichThuoc(); ++I) if (I != ViTri) Tam.PhanCong.ThemCuoi(Lich.PhanCong[I]);
    const Shift& Ca = *TimCa(Lich, P.MaCa);
    const auto& Cu = *NhanSu.Tim(P.MaNhanVien);
    Vector<std::string> KQ;
    for (int I = 0; I < NhanSu.KichThuoc(); ++I) {
        const auto& NV = NhanSu.Lay(I);
        if (DuPhong && !DuPhong->Co(NV.LayMaDinhDanh())) continue;
        if (NV.LayMaDinhDanh() == P.MaNhanVien || NV.LayCapBac() != Cu.LayCapBac()) continue;
        if (NV.LayLoaiNhanVien() == StaffType::BacSi &&
            static_cast<const BacSi&>(NV).LayChuyenKhoa() != static_cast<const BacSi&>(Cu).LayChuyenKhoa()) continue;
        if (CoThePhanCong(NV, Ca, P.VaiTro, Tam, Luat)) KQ.ThemCuoi(NV.LayMaDinhDanh());
    }
    for (int I=1;I<KQ.KichThuoc();++I) {
        int J=I;
        while (J>0 && FatigueService::TinhGio(KQ[J],Lich)<FatigueService::TinhGio(KQ[J-1],Lich)) {
            std::string Tam=KQ[J]; KQ[J]=KQ[J-1]; KQ[J-1]=Tam; --J;
        }
    }
    return KQ;
}

namespace {
// Tận dụng cùng bộ quay lui; giữ các phân công cũ làm điều kiện ban đầu.
Vector<ShiftAssignment> BoSung(const StaffRegistry& N, const ScheduleRegistry& R,
                             const std::string& Ma, int BS, int DD, int GioiHan) {
    if (GioiHan<=0 || N.KichThuoc()>100 || BS<0 || DD<0 || BS>10 || DD>10)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Tham so de xuat khong hop le");
    SchedulerRules Luat;
    TimKiem T(N,Luat,GioiHan);
    T.KQ.Lich=R.LayLichHieuLuc();
    const int SoCu=T.KQ.Lich.PhanCong.KichThuoc();
    Schedule KiemTra; KiemTra.CacCa=T.KQ.Lich.CacCa;
    for (int I=0; I<SoCu; ++I) {
        const auto& P=T.KQ.Lich.PhanCong[I];
        const auto NV=N.Tim(P.MaNhanVien); const auto C=R.TimCaTruc(P.MaCa);
        if (!NV || !C || !SchedulerService::CoThePhanCong(*NV,*C,P.VaiTro,KiemTra,{}))
            throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Lich hien co khong con hop le");
        KiemTra.PhanCong.ThemCuoi(P);
    }
    for (int I=0; I<T.KQ.Lich.CacCa.KichThuoc(); ++I) {
        auto& C=T.KQ.Lich.CacCa[I];
        int SoBS=0, SoDD=0, Truong=0;
        for (int J=0; J<SoCu; ++J) {
            const auto& P=T.KQ.Lich.PhanCong[J]; if (P.MaCa!=C.LayMaCaTruc()) continue;
            if (P.VaiTro==AssignmentRole::DieuDuong) ++SoDD;
            else { ++SoBS; if (P.VaiTro==AssignmentRole::TruongCa) ++Truong; }
        }
        if (!Ma.empty() && Ma!=C.LayMaCaTruc()) continue;
        // Tăng cường là số chỗ BỔ SUNG so với người hiện có, không đổi phân công cũ.
        const int CanBS=Ma.empty()?C.SoBacSi:SoBS+BS;
        const int CanDD=Ma.empty()?C.SoDieuDuong:SoDD+DD;
        for (int J=SoBS; J<CanBS; ++J)
            T.CacO.ThemCuoi({I,C.CanTruongCa && !Truong && J==SoBS ? AssignmentRole::TruongCa : AssignmentRole::BacSi});
        for (int J=SoDD; J<CanDD; ++J) T.CacO.ThemCuoi({I,AssignmentRole::DieuDuong});
        if (Ma.empty() && C.CanTruongCa && !Truong && SoBS>=CanBS)
            throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Ca du bac si nhung thieu truong ca");
    }
    if (SoCu+T.CacO.KichThuoc()>128)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Qua 128 vi tri hieu luc");
    if (!T.Thu(0)) throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,
        T.HetLuot?"Het gioi han tim; chua ket luan vo nghiem":"Khong du nhan su thoa rang buoc");
    Vector<ShiftAssignment> KQ;
    for (int I=SoCu; I<T.KQ.Lich.PhanCong.KichThuoc(); ++I) {
        const auto& P=T.KQ.Lich.PhanCong[I];
        // Tránh dùng lại mã của bản đã hủy; mã còn phải kiểm tra khi chấp nhận đề xuất.
        std::string M=P.LayMaPhanCong(); int HauTo=1;
        while (R.TimPhanCong(M)) M=P.LayMaPhanCong()+"-"+std::to_string(HauTo++);
        KQ.ThemCuoi(ShiftAssignment(M,P.MaNhanVien,P.MaCa,P.VaiTro));
    }
    return KQ;
}
}
Vector<ShiftAssignment> SchedulerService::LapLichDuKien(int G) const {
    if (!NhanSu || !Lich) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Can gan NhanSu va Lich");
    return BoSung(*NhanSu,*Lich,"",0,0,G);
}
Vector<ShiftAssignment> SchedulerService::DeXuatTangCuong(const std::string& M, int BS, int DD, int G) const {
    if (!NhanSu || !Lich || !Lich->TimCaTruc(M) || BS+DD<=0)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Can ca ton tai va so nguoi tang cuong duong");
    return BoSung(*NhanSu,*Lich,M,BS,DD,G);
}
Vector<const NhanVienYTe*> SchedulerService::TimNguoiThayCa(const std::string& M, const StandbyPool* DuPhong) const {
    if (!NhanSu || !Lich || !Lich->TimPhanCong(M))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Khong tim thay phan cong");
    const auto& P=*Lich->TimPhanCong(M);
    if (P.LayTrangThai()==AssignmentStatus::DangTruc || !P.DangHieuLuc())
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Chi thay phan cong chua bat dau");
    const auto Cu=NhanSu->Tim(P.MaNhanVien);
    if (!Cu) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Nhan vien cu khong con ton tai");
    Schedule Tam=Lich->LayLichHieuLuc(); Tam.PhanCong.XoaHet();
    for (int I=0; I<Lich->SoPhanCong(); ++I) {
        const auto& Khac=Lich->LayPhanCong(I);
        if (Khac.LayMaPhanCong()!=M && Khac.DangHieuLuc()) Tam.PhanCong.ThemCuoi(Khac);
    }
    Vector<const NhanVienYTe*> KQ;
    for (int I=0; I<NhanSu->KichThuoc(); ++I) {
        const auto& N=NhanSu->Lay(I);
        if (N.LayMaDinhDanh()==P.MaNhanVien || N.LayLoaiNhanVien()!=Cu->LayLoaiNhanVien() || N.LayCapBac()!=Cu->LayCapBac()) continue;
        if (DuPhong && !DuPhong->Co(N.LayMaDinhDanh())) continue;
        if (N.LayLoaiNhanVien()==StaffType::BacSi && static_cast<const BacSi&>(N).LayChuyenKhoa()!=static_cast<const BacSi*>(Cu)->LayChuyenKhoa()) continue;
        if (CoThePhanCong(N,*Lich->TimCaTruc(P.MaCa),P.VaiTro,Tam,{})) KQ.ThemCuoi(&N);
    }
    return KQ;
}
