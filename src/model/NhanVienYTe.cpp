#include "model/NhanVienYTe.h"
#include "util/My_stdexcept.h"
#include <cmath>
#include <climits>
#include <memory>
#include <utility>
namespace {
void YeuCau(bool Dung, const char* Loi) {
    if (!Dung) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, Loi);
}
}
NhanVienYTe::NhanVienYTe(const std::string& Ma, const std::string& Ten, CapBac Bac,
                       int KinhNghiem, double DiemNen, const CaTrucMongMuon& MongMuon)
    : Person(Ma, Ten), Bac(Bac), KinhNghiem(KinhNghiem), DiemNen(DiemNen), DiemMetMoi(DiemNen), MongMuon(MongMuon) {
    DatHoTen(Ten);
    DatSoNamKinhNghiem(KinhNghiem);
    YeuCau(std::isfinite(DiemNen) && DiemNen >= 0, "Diem met moi phai huu han va khong am");
    DatCaMongMuon(MongMuon);
}
void NhanVienYTe::DatHoTen(const std::string& Ten) {
    YeuCau(Ten.find_first_not_of(" \t\r\n") != std::string::npos, "Ho ten nhan vien khong duoc rong");
    CapNhatThongTinCaNhan(Ten, LayGioiTinh());
}
void NhanVienYTe::DatCapBac(CapBac BacMoi) {
    const bool BacSiHopLe = BacMoi == CapBac::BacSiNoiTru || BacMoi == CapBac::BacSiChinh || BacMoi == CapBac::TruongKhoa;
    const bool DieuDuongHopLe = BacMoi == CapBac::DieuDuongTre || BacMoi == CapBac::DieuDuongChinh;
    YeuCau(LayLoaiNhanVien() == StaffType::BacSi ? BacSiHopLe : DieuDuongHopLe, "Cap bac khong dung nghe nghiep");
    Bac = BacMoi;
}
void NhanVienYTe::DatSoNamKinhNghiem(int SoNam) {
    YeuCau(SoNam >= 0 && SoNam <= 80, "Kinh nghiem phai tu 0 den 80 nam");
    KinhNghiem = SoNam;
}
void NhanVienYTe::DatCaMongMuon(const CaTrucMongMuon& Ca) {
    YeuCau(Ca.HopLe(), "Gio ca mong muon khong hop le"); MongMuon = Ca;
}
void NhanVienYTe::CongDonGioLam(double SoGio) {
    YeuCau(std::isfinite(SoGio) && SoGio >= 0 && std::isfinite(GioLamTuan + SoGio), "So gio lam khong hop le");
    GioLamTuan += SoGio;
}
void NhanVienYTe::GhiNhanCaTruc(bool LaCaDem, bool LaCuoiTuan) {
    YeuCau((!LaCaDem || SoCaDemThang < INT_MAX) && (!LaCuoiTuan || SoCaCuoiTuanThang < INT_MAX), "Bo dem ca vuot gioi han");
    if (LaCaDem) ++SoCaDemThang;
    if (LaCuoiTuan) ++SoCaCuoiTuanThang;
    VuaTrucDem = LaCaDem;
}
void NhanVienYTe::DatLaiTuanMoi() { GioLamTuan = 0; }
bool NhanVienYTe::CanNghiNgoiBatBuoc() const {
    // Giữ cờ của bản gốc; muốn kiểm tra đủ 24h phải dùng thời điểm ca ở SchedulerService.
    return VuaTrucDem;
}
bool NhanVienYTe::VuotNguongMetMoi(double Nguong) const {
    YeuCau(std::isfinite(Nguong) && Nguong >= 0, "Nguong met moi khong hop le");
    return DiemMetMoi > Nguong;
}
void NhanVienYTe::GhiNhanDiemMetMoi(double Diem) {
    YeuCau(std::isfinite(Diem) && Diem >= 0, "Diem met moi khong hop le"); DiemMetMoi = Diem;
}

void NhanVienYTe::KhoiPhucTrangThai(const TrangThaiNhanVien& T) {
    YeuCau(std::isfinite(T.DiemMetMoi) && T.DiemMetMoi >= 0 &&
           std::isfinite(T.GioLamTuan) && T.GioLamTuan >= 0 &&
           T.SoCaDemThang >= 0 && T.SoCaCuoiTuanThang >= 0, "Trang thai nhan vien khong hop le");
    DiemMetMoi = T.DiemMetMoi; GioLamTuan = T.GioLamTuan;
    SoCaDemThang = T.SoCaDemThang; SoCaCuoiTuanThang = T.SoCaCuoiTuanThang;
    VuaTrucDem = T.VuaTrucDem;
}

void NhanVienYTe::HoanDoiThongTinChung(NhanVienYTe& Khac) noexcept {
    HoanDoiThongTin(Khac);
    std::swap(Bac,Khac.Bac); std::swap(KinhNghiem,Khac.KinhNghiem);
    std::swap(DiemNen,Khac.DiemNen); std::swap(DiemMetMoi,Khac.DiemMetMoi);
    std::swap(GioLamTuan,Khac.GioLamTuan); std::swap(SoCaDemThang,Khac.SoCaDemThang);
    std::swap(SoCaCuoiTuanThang,Khac.SoCaCuoiTuanThang); std::swap(VuaTrucDem,Khac.VuaTrucDem);
    std::swap(MongMuon,Khac.MongMuon);
}
void NhanVienYTe::CapNhatTu(const NhanVienYTe& Nguon) {
    if (LayMaDinhDanh()!=Nguon.LayMaDinhDanh())
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Khong duoc doi ma nhan vien");
    // Chép trước: nếu cấp phát lỗi thì đối tượng đang dùng còn nguyên.
    std::unique_ptr<NhanVienYTe> Tam(Nguon.SaoChep());
    HoanDoiDuLieu(*Tam); // Lớp con kiểm tra đúng kiểu trước khi đổi các trường.
}
