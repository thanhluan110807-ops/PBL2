// Phần B — Chức năng chính: Tính điểm mệt mỏi mô phỏng và giờ đã xếp.
#include "service/FatigueService.h"
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "util/My_stdexcept.h"
#include <cmath>
namespace {
const Shift* TimCa(const Schedule& Lich, const std::string& Ma) {
    for (int I = 0; I < Lich.CacCa.KichThuoc(); ++I)
        if (Lich.CacCa[I].LayMaCaTruc() == Ma) return &Lich.CacCa[I];
    return nullptr;
}
}
double FatigueService::DiemCa(const Shift& Ca) {
    return Ca.TinhSoGio() * (Ca.LaCaDem() ? 1.5 : 1.0) + (Ca.LaCuoiTuan() ? 2 : 0);
}
double FatigueService::TinhDiem(const NhanVienYTe& NV, const Schedule& Lich) {
    double Diem = NV.LayDiemMetMoi();
    for (int I = 0; I < Lich.PhanCong.KichThuoc(); ++I) {
        const auto& P = Lich.PhanCong[I];
        if (!P.DangHieuLuc() || P.MaNhanVien != NV.LayMaDinhDanh()) continue;
        const Shift* Ca = TimCa(Lich, P.MaCa);
        if (Ca) Diem += DiemCa(*Ca);
    }
    return Diem;
}
double FatigueService::TinhGio(const std::string& Ma, const Schedule& Lich) {
    double Gio = 0;
    for (int I = 0; I < Lich.PhanCong.KichThuoc(); ++I) {
        if (!Lich.PhanCong[I].DangHieuLuc() || Lich.PhanCong[I].MaNhanVien != Ma) continue;
        const Shift* Ca = TimCa(Lich, Lich.PhanCong[I].MaCa);
        if (Ca) Gio += Ca->TinhSoGio();
    }
    return Gio;
}

void FatigueService::CapNhatTheoCongThucGoc(NhanVienYTe& NV, double HeSoApLuc) {
    if (!std::isfinite(HeSoApLuc) || HeSoApLuc < 0)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "He so ap luc khong hop le");
    double DiemTang = 0;
    if (NV.LayLoaiNhanVien() == StaffType::BacSi) {
        const auto& BS = static_cast<const BacSi&>(NV);
        DiemTang = HeSoApLuc * (NV.LaySoCaDemThang() * 1.5 + NV.LayGioLamTuan() * 0.12);
        if (BS.LaBacSiNoiTru()) DiemTang *= 1.2;
    } else {
        const auto& DD = static_cast<const DieuDuong&>(NV);
        DiemTang = HeSoApLuc * NV.LayGioLamTuan() * 0.15;
        if (DD.DangSangLoc()) DiemTang += HeSoApLuc * DD.LayGioSangLocLienTuc() * 0.8;
    }
    NV.GhiNhanDiemMetMoi(NV.LayDiemMetMoi() + DiemTang);
}
