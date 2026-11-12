#include "service/TriageService.h"
#include <cassert>
#include <ctime>
#include <string>
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Visit.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"

template <typename F> void PhaiLoi(F Ham) {
    bool DaLoi = false;
    try { Ham(); } catch (const My_stdexcept&) { DaLoi = true; }
    assert(DaLoi);
}

void KiemTraMucUuTien() {
    PatientRegistry Registry;
    Registry.ThemBenhNhan("BN1", "A", Gender::Nam, Date{1, 1, 2000});
    Registry.ThemBenhNhan("BN2", "B", Gender::Nam, Date{1, 1, 2000});
    TriageService DieuPhoi(Registry);
    DieuPhoi.TiepNhan("LK1", "BN1", 1767225600, "Dau");
    DieuPhoi.TiepNhan("LK2", "BN2", 1767225600, "Dau");
    DieuPhoi.DanhGia("LK1", VitalSigns(75,120,16,98,15), 1767225601, "DD");
    DieuPhoi.DanhGia("LK2", VitalSigns(75,120,16,89,15), 1767225602, "DD");
    assert(DieuPhoi.GoiTiep("BS").LayMaLanKham() == "LK2");
    assert(DieuPhoi.GoiTiep("BS").LayMaLanKham() == "LK1");
    PhaiLoi([&] { DieuPhoi.GoiTiep("BS"); });
}

void ThemLuot(PatientRegistry& Registry, TriageService& Service,
              const std::string& Ma, std::time_t Luc, double Oxy = 98) {
    Registry.ThemBenhNhan("BN" + Ma, "Benh nhan", Gender::Nam, Date{1, 1, 2000});
    Service.TiepNhan(Ma, "BN" + Ma, 1767225600, "Dau");
    Service.DanhGia(Ma, VitalSigns(75,120,16,Oxy,15), Luc, "DD");
}

void DuaQuayLai(TriageService& Service, const std::string& Ma, std::time_t Luc) {
    Service.ChuyenCanLamSang(Ma, "Xet nghiem", "BS");
    Service.QuayLai(Ma, "Ket qua", Luc);
}

void KiemTraLuanPhien() {
    PatientRegistry Registry;
    TriageService Service(Registry);
    ThemLuot(Registry, Service, "R", 1767225601);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "R");
    DuaQuayLai(Service, "R", 1767225602);
    ThemLuot(Registry, Service, "A", 1767225603);
    const auto DuKien = Service.LayDanhSachCho();
    assert(DuKien.KichThuoc() == 2);
    assert(DuKien[0]->LayMaLanKham() == "R");
    assert(DuKien[1]->LayMaLanKham() == "A");
    assert(Service.LayDanhSachCho()[0]->LayMaLanKham() == "R");
    // Gọi sai không được tiêu thụ lượt ưu tiên.
    PhaiLoi([&] { Service.GoiTiep(""); });
    assert(Service.GoiTiep("BS").LayMaLanKham() == "R");
    DuaQuayLai(Service, "R", 1767225604);
    ThemLuot(Registry, Service, "Khan", 1767225605, 89);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "Khan");
    // Cùng chuyển hai lượt lên mức 2: lượt luân phiên dùng chung cho mọi mức.
    Service.DanhGia("R", VitalSigns(75,120,16,94,15), 1767225606, "DD");
    Service.DanhGia("A", VitalSigns(75,120,16,94,15), 1767225606, "DD");
    // Lần gọi khẩn cấp không đổi lượt: lần trùng tiếp phải chọn lần đầu.
    assert(Service.GoiTiep("BS").LayMaLanKham() == "A");
    Service.DanhGia("R", VitalSigns(75,120,16,98,15), 1767225607, "DD");
    ThemLuot(Registry, Service, "B", 1767225608);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "R");
    assert(Service.GoiTiep("BS").LayMaLanKham() == "B");
}

void KiemTraDanhGiaVaHuy() {
    PatientRegistry Registry;
    TriageService Service(Registry);
    ThemLuot(Registry, Service, "A", 1767225601);
    ThemLuot(Registry, Service, "B", 1767225602);
    // Đánh giá lại không đưa A ra sau B khi hai lượt có cùng mức.
    Service.DanhGia("A", VitalSigns(75,120,16,98,15), 1767225603, "DD");
    assert(Service.GoiTiep("BS").LayMaLanKham() == "A");
    Service.Huy("B");
    assert(Service.SoDangCho() == 0);
    assert(Registry.TimLanKham("B")->LayTrangThai() == VisitStatus::DaHuy);
    PhaiLoi([&] { Service.Huy("B"); });
    Service.KetThuc("A");
    assert(Registry.TimLanKham("A")->LayTrangThai() == VisitStatus::DaKetThuc);
}

void KiemTraDoiMucVaGiuLuotKhiMotHangRong() {
    PatientRegistry Registry;
    TriageService Service(Registry);
    ThemLuot(Registry, Service, "R", 1767225601);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "R");
    DuaQuayLai(Service, "R", 1767225602);
    ThemLuot(Registry, Service, "A", 1767225603);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "R");
    // Chỉ còn hàng lần đầu; gọi A không thay đổi lượt chờ lần trùng tiếp.
    assert(Service.GoiTiep("BS").LayMaLanKham() == "A");
    DuaQuayLai(Service, "R", 1767225604);
    ThemLuot(Registry, Service, "B", 1767225605);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "B");
    Service.Huy("R");
    ThemLuot(Registry, Service, "C", 1767225606);
    ThemLuot(Registry, Service, "D", 1767225607);
    Service.DanhGia("D", VitalSigns(75,120,16,89,15), 1767225608, "DD");
    assert(Service.GoiTiep("BS").LayMaLanKham() == "D");
    assert(Service.GoiTiep("BS").LayMaLanKham() == "C");
}

void KiemTraLoiKhongMatHang() {
    PatientRegistry Registry;
    TriageService Service(Registry);
    Registry.ThemBenhNhan("BN", "A", Gender::Nam);
    Service.TiepNhan("LK", "BN", 1767225600, "Dau");
    // Thiếu tuổi vẫn phải gỡ mục chờ đã cấp phát khi đánh giá thất bại.
    PhaiLoi([&] { Service.DanhGia("LK", VitalSigns(75,120,16,98,15), 1767225601, ""); });
    assert(Service.SoDangCho() == 0);
    Registry.BoSungDanhTinh("BN", "", "A", Gender::Nam, Date{1,1,2000});
    // Có tuổi thì được đánh giá khi chưa xác định điều dưỡng đăng nhập.
    Service.DanhGia("LK", VitalSigns(75,120,16,98,15), 1767225601, "");
    assert(Registry.TimLanKham("LK")->LayDanhGiaGanNhat().LayMaDieuDuong().empty());
    assert(Service.GoiTiep("BS").LayMaLanKham() == "LK");
    Service.ChuyenCanLamSang("LK", "XN", "BS");
    PhaiLoi([&] { Service.QuayLai("LK", "", 1767225602); });
    assert(Service.SoDangCho() == 0);
    Service.QuayLai("LK", "Binh thuong", 1767225602);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "LK");
}

void KiemTraThoiGianVaThayDoiBenNgoai() {
    PatientRegistry Registry;
    TriageService Service(Registry);
    ThemLuot(Registry, Service, "A", 1767225610);
    // Không ghi nhận đánh giá mới có thời gian trước đánh giá gần nhất.
    PhaiLoi([&] { Service.DanhGia("A", VitalSigns(75,120,16,89,15), 1767225609, "DD"); });
    assert(Registry.TimLanKham("A")->SoDanhGia() == 1);
    assert(Service.GoiTiep("BS").LayMaLanKham() == "A");
    Service.ChuyenCanLamSang("A", "XN", "BS");
    PhaiLoi([&] { Service.QuayLai("A", "Ket qua", 1767225609); });
    assert(Service.SoDangCho() == 0);
    Service.QuayLai("A", "Ket qua", 1767225611);
    // Nếu mã khác sửa Visit trực tiếp, báo lỗi thay vì gọi sai mức ưu tiên.
    Registry.TimLanKham("A")->DanhGia(*Registry.TimBenhNhan("BNA"),
        VitalSigns(75,120,16,89,15), 1767225612, "DD");
    PhaiLoi([&] { Service.GoiTiep("BS"); });
    assert(Service.SoDangCho() == 1);
}

int main() {
    KiemTraMucUuTien();
    KiemTraLuanPhien();
    KiemTraDanhGiaVaHuy();
    KiemTraDoiMucVaGiuLuotKhiMotHangRong();
    KiemTraLoiKhongMatHang();
    KiemTraThoiGianVaThayDoiBenNgoai();
}
