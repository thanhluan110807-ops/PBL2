#include "model/DemoTriageService.h"
#include <cassert>
#include <iostream>
#include <limits>
using namespace Demo;
namespace {
HoSo HoSoThu() {
    HoSo H;
    H.HoTen = "Bệnh nhân thử";
    H.CoNgaySinh = true;
    H.NgaySinh = {10, 10, 1990};
    return H;
}
const SinhHieu SoDo{80, 120, 18, 15, 98};
int Tao(DemoTriageService &D, int Muc = 3) {
    const auto K = D.TiepNhan(HoSoThu(), "Đau đầu mẫu");
    assert(K.ThanhCong);
    assert(D.XacNhan(K.ChiSo, SoDo, Muc).ThanhCong);
    return K.ChiSo;
}
void LuongChinh(DemoTriageService &D) {
    D.XoaDuLieu();
    const auto K = D.TiepNhan(HoSoThu(), "Thử luồng");
    assert(K.ThanhCong);
    assert(D.LayHangCho().SoLuong == 0);
    assert(!D.TiepNhan(HoSoThu(), "Trùng", 0).ThanhCong);
    assert(!D.DongLuot(0).ThanhCong);
    assert(D.XacNhan(0, SoDo, 3).ThanhCong);
    assert(D.GoiTiep().ChiSo == 0);
    assert(D.DongLuot(0).ThanhCong);
    assert(!D.XacNhan(0, SoDo, 1).ThanhCong);
    assert(D.TiepNhan(HoSoThu(), "Lần đến mới", 0).ThanhCong);
    assert(D.LaySoHoSo() == 1);
}
void CanLamSangLap(DemoTriageService &D) {
    D.XoaDuLieu();
    const int So = Tao(D);
    const std::string Ma = D.LayLuot(So).Ma;
    for (int Lan = 0; Lan < 3; ++Lan) {
        assert(D.GoiTiep().ChiSo == So);
        assert(D.ChuyenCanLamSang(So, "Xét nghiệm").ThanhCong);
        D.TienThoiGian();
        assert(D.QuayLai(So, "Kết quả").ThanhCong);
        assert(D.LayLuot(So).Ma == Ma);
        assert(D.LayLuot(So).VaoHangLuc == D.LayBayGio());
        assert(D.LayHangCho().SoLuong == 1);
        assert(!D.QuayLai(So, "Trùng").ThanhCong);
    }
    assert(D.LaySoLuot() == 1);
}
void DanhTinh(DemoTriageService &D) {
    D.XoaDuLieu();
    HoSo H;
    H.ChuaRo = true;
    assert(D.TiepNhan(H, "Chưa rõ danh tính").ThanhCong);
    assert(D.LayTuoi(D.LayHoSo(0)) == -1);
    assert(D.XacNhan(0, SoDo, 3).ThanhCong);
    assert(D.LayLuot(0).LichSu[0].GoiY.Muc == 0);
    const std::string Ma = D.LayHoSo(0).Ma;
    assert(D.CapNhatHoSo(0, HoSoThu()).ThanhCong);
    assert(D.LayHoSo(0).Ma == Ma);
    assert(D.LayLuot(0).BenhNhan == 0);
    assert(D.LayLuot(0).LichSu[0].Tuoi == -1);
}
void DanhGiaLai(DemoTriageService &D) {
    D.XoaDuLieu();
    Tao(D, 3);
    Tao(D, 4);
    const auto Luc = D.LayLuot(1).VaoHangLuc;
    const int Stt = D.LayLuot(1).SoThuTu;
    assert(D.XacNhan(1, SoDo, 1).ThanhCong);
    assert(D.LayHangCho().CacLuot[0] == 1);
    assert(D.LayLuot(1).VaoHangLuc == Luc && D.LayLuot(1).SoThuTu == Stt);
    assert(D.LayLuot(1).SoDanhGia == 2);
    assert(D.XacNhan(1, SoDo, 3).ThanhCong);
    assert(D.LayHangCho().CacLuot[0] == 0);
}
void BaMot(DemoTriageService &D) {
    D.NapMau(true);
    const auto Hang = D.LayHangCho();
    const TrangThai Mong[] = {TrangThai::QuayLai, TrangThai::QuayLai, TrangThai::QuayLai, TrangThai::LanDau,
                              TrangThai::QuayLai, TrangThai::QuayLai, TrangThai::LanDau};
    for (int I = 0; I < 7; ++I) {
        assert(D.LayLuot(Hang.CacLuot[I]).TinhTrang == Mong[I]);
        D.LayHangCho();
        assert(D.GoiTiep().ChiSo == Hang.CacLuot[I]);
    }
    assert(!D.GoiTiep().ThanhCong);
}
void UuTienVaBoDem(DemoTriageService &D) {
    D.NapMau(true);
    D.GoiTiep();
    const int Khan = Tao(D, 1);
    assert(D.GoiTiep().ChiSo == Khan);
    D.GoiTiep();
    D.GoiTiep();
    assert(D.LayLuot(D.LayHangCho().CacLuot[0]).TinhTrang == TrangThai::LanDau);
    D.NapMau(true);
    D.GoiTiep();
    assert(D.DongLuot(5, true).ThanhCong);
    assert(D.DongLuot(6, true).ThanhCong);
    Tao(D, 3);
    for (int I = 0; I < 3; ++I) {
        const int So = D.LayHangCho().CacLuot[0];
        assert(D.LayLuot(So).TinhTrang == TrangThai::QuayLai);
        D.GoiTiep();
    }
    assert(D.LayLuot(D.LayHangCho().CacLuot[0]).TinhTrang == TrangThai::LanDau);
}
void CanhBao(DemoTriageService &D) {
    D.XoaDuLieu();
    Tao(D, 2);
    D.TienThoiGian();
    assert(!D.CanDanhGia(0));
    D.TienThoiGian();
    assert(D.CanDanhGia(0));
    assert(D.LayLuot(0).Muc == 2);
    assert(D.XacNhan(0, SoDo, 2).ThanhCong);
    assert(!D.CanDanhGia(0));
    D.DongLuot(0, true);
    D.TienThoiGian();
    D.TienThoiGian();
    assert(!D.CanDanhGia(0));
}
void ThieuTuoi() {
    assert(DemoTriageService::DeXuatMuc(SoDo, -1).Muc == 0);
    assert(DemoTriageService::DeXuatMuc(SoDo, 0).Muc == 1);
    auto S = SoDo;
    S.SpO2 = 89;
    assert(DemoTriageService::DeXuatMuc(S, -1).Muc == 1);
    assert(!DemoTriageService::DeXuatMuc(S, -1).DayDu);
}
void DuLieuSai(DemoTriageService &D) {
    D.XoaDuLieu();
    assert(D.TiepNhan(HoSoThu(), "Thử").ThanhCong);
    SinhHieu S;
    assert(!D.XacNhan(0, S, 3).ThanhCong);
    S = SoDo;
    S.SpO2 = std::numeric_limits<double>::quiet_NaN();
    assert(!D.XacNhan(0, S, 3).ThanhCong);
    assert(!D.XacNhan(0, SoDo, 0).ThanhCong);
    assert(D.LayLuot(0).SoDanhGia == 0);
    assert(D.LayLuot(0).TinhTrang == TrangThai::SangLoc);
    auto H = HoSoThu();
    H.NgaySinh = {31, 2, 2000};
    assert(!D.CapNhatHoSo(0, H).ThanhCong);
    H.NgaySinh = {1, 11, 2026};
    assert(!D.CapNhatHoSo(0, H).ThanhCong);
    assert(D.LayHoSo(0).NgaySinh.Nam == 1990);
}
void SucChua(DemoTriageService &D) {
    D.XoaDuLieu();
    Tao(D);
    for (int I = 1; I < GioiHanDanhGia; ++I)
        assert(D.XacNhan(0, SoDo, 3).ThanhCong);
    assert(!D.XacNhan(0, SoDo, 3).ThanhCong);
    assert(D.LayLuot(0).SoDanhGia == GioiHanDanhGia);
    D.XoaDuLieu();
    for (int I = 0; I < GioiHanHoSo; ++I)
        assert(D.TiepNhan(HoSoThu(), "Thử sức chứa").ThanhCong);
    assert(!D.TiepNhan(HoSoThu(), "Tràn").ThanhCong);
    assert(D.LaySoHoSo() == GioiHanHoSo);
}
} // namespace
int main() {
    auto *D = new DemoTriageService;
    D->NapMau();
    assert(D->LaySoHoSo() == 14 && D->LaySoLuot() == 14 && D->LayHangCho().SoLuong == 8);
    assert(D->LayNgay().Ngay == 30 && D->LayNgay().Thang == 10 && D->LayNgay().Nam == 2026);
    LuongChinh(*D);
    CanLamSangLap(*D);
    DanhTinh(*D);
    DanhGiaLai(*D);
    BaMot(*D);
    UuTienVaBoDem(*D);
    CanhBao(*D);
    ThieuTuoi();
    DuLieuSai(*D);
    SucChua(*D);
    delete D;
    std::cout << "PASS: 12 nhom kiem tra mo phong C++\n";
}
