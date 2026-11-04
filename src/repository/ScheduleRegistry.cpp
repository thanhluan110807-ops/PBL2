// Phần B — Chức năng chính: Quản lý ca và phân công trong RAM, kiểm tra liên kết và trạng thái.
#include "repository/ScheduleRegistry.h"
#include "service/SchedulerService.h"
#include "util/My_stdexcept.h"
#include <memory>
namespace {
void Loi(const char* S) { throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,S); }
}
ScheduleRegistry::~ScheduleRegistry() {
    for (int I=0; I<DanhSachPhanCong.KichThuoc(); ++I) delete DanhSachPhanCong[I];
    for (int I=0; I<DanhSachCa.KichThuoc(); ++I) delete DanhSachCa[I];
}
const Shift* ScheduleRegistry::TimCaTruc(const std::string& M) const {
    const auto P=CacCaTruc.Timkiem(M); return P ? *P : nullptr;
}
const ShiftAssignment* ScheduleRegistry::TimPhanCong(const std::string& M) const {
    const auto P=CacPhanCong.Timkiem(M); return P ? *P : nullptr;
}
void ScheduleRegistry::ThemCaTruc(const Shift& C) {
    if (!C.HopLe() || TimCaTruc(C.LayMaCaTruc()) || SoCa()>=32) Loi("Ca khong hop le, trung ma hoac qua 32 ca");
    if (SoCa()>0 && (C.LayBatDau()-320400)/604800 != (LayCa(0).LayBatDau()-320400)/604800)
        Loi("Moi registry lap lich cho mot tuan; ca cuoi tuan duoc ket thuc sang tuan sau");
    // unique_ptr thu hồi bản sao nếu thêm Vector/HashTable thất bại.
    auto P=std::make_unique<Shift>(C);
    DanhSachCa.ThemCuoi(P.get());
    try { CacCaTruc.Them(C.LayMaCaTruc(), P.get()); }
    catch (...) { DanhSachCa.XoaCuoi(); throw; }
    P.release();
}
Schedule ScheduleRegistry::LayLichHieuLuc() const {
    Schedule L;
    for (int I=0; I<SoCa(); ++I) L.CacCa.ThemCuoi(LayCa(I));
    for (int I=0; I<SoPhanCong(); ++I)
        if (LayPhanCong(I).DangHieuLuc()) L.PhanCong.ThemCuoi(LayPhanCong(I));
    return L;
}
void ScheduleRegistry::ThemPhanCong(const ShiftAssignment& P, const StaffRegistry& N) {
    const auto C=TimCaTruc(P.MaCa); const auto NV=N.Tim(P.MaNhanVien);
    if (P.LayMaPhanCong().empty() || TimPhanCong(P.LayMaPhanCong()) || !C || !NV || SoPhanCong()>=512)
        Loi("Ma phan cong trung/rong, thieu ca/nhan vien hoac qua 512 ban ghi");
    // Kiểm tra cả bản hủy để dữ liệu lịch sử không có mã/vai trò rác.
    ShiftAssignment(P.LayMaPhanCong(),P.MaNhanVien,P.MaCa,P.VaiTro);
    if (P.DangHieuLuc() && P.VaiTro==AssignmentRole::TruongCa) {
        for (int I=0; I<SoPhanCong(); ++I)
            if (LayPhanCong(I).DangHieuLuc() && LayPhanCong(I).MaCa==P.MaCa && LayPhanCong(I).VaiTro==AssignmentRole::TruongCa)
                Loi("Mot ca chi co mot truong ca");
    }
    // Cả dự kiến cũng giữ chỗ; bản đã hủy chỉ giữ lịch sử, không giữ chỗ.
    if (P.DangHieuLuc() && !SchedulerService::CoThePhanCong(*NV,*C,P.VaiTro,LayLichHieuLuc(),{}))
        Loi("Phan cong vi pham vai tro, gio nghi, so gio hoac met moi");
    if (P.LayTrangThai()==AssignmentStatus::DangTruc &&
        (P.LayThoiDiemBatDau()<C->LayBatDau() || P.LayThoiDiemBatDau()>=C->LayKetThuc())) Loi("Bat dau ngoai ca");
    auto Moi=std::make_unique<ShiftAssignment>(P);
    DanhSachPhanCong.ThemCuoi(Moi.get());
    try { CacPhanCong.Them(P.LayMaPhanCong(),Moi.get()); }
    catch (...) { DanhSachPhanCong.XoaCuoi(); throw; }
    Moi.release();
}
void ScheduleRegistry::XacNhanPhanCong(const std::string& M, const StaffRegistry& N) {
    auto P=CacPhanCong.Timkiem(M); if (!P) Loi("Khong tim thay phan cong");
    Schedule L=LayLichHieuLuc(); L.PhanCong.XoaHet();
    for (int I=0; I<SoPhanCong(); ++I)
        if (LayPhanCong(I).LayMaPhanCong()!=M && LayPhanCong(I).DangHieuLuc()) L.PhanCong.ThemCuoi(LayPhanCong(I));
    const auto NV=N.Tim((*P)->MaNhanVien); const auto C=TimCaTruc((*P)->MaCa);
    if (!NV || !C || !SchedulerService::CoThePhanCong(*NV,*C,(*P)->VaiTro,L,{})) Loi("Nhan su khong con du dieu kien");
    (*P)->XacNhan();
}
void ScheduleRegistry::HuyPhanCong(const std::string& M, const std::string& LyDo) {
    auto P=CacPhanCong.Timkiem(M); if (!P) Loi("Khong tim thay phan cong");
    (*P)->Huy(LyDo);
}
void ScheduleRegistry::GhiNhanBatDau(const std::string& M, std::time_t T) {
    auto P=CacPhanCong.Timkiem(M); if (!P) Loi("Khong tim thay phan cong");
    const auto C=TimCaTruc((*P)->MaCa);
    if (T<C->LayBatDau() || T>=C->LayKetThuc()) Loi("Thoi diem bat dau phai nam trong ca");
    (*P)->GhiNhanBatDau(T);
}
