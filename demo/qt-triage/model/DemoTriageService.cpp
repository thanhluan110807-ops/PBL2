#include "model/DemoTriageService.h"
#include <cmath>
#include <ctime>

namespace Demo {
namespace {
KetQua Loi(const std::string &ThongBao, const std::string &Truong = "") {
    return {false, ThongBao, Truong, -1};
}
bool Rong(const std::string &Chuoi) {
    for (char KyTu : Chuoi)
        if (KyTu != ' ' && KyTu != '\t' && KyTu != '\n' && KyTu != '\r')
            return false;
    return true;
}
std::string Ma(const char *TienTo, int So) {
    std::string SoChu = std::to_string(So);
    while (SoChu.size() < 4)
        SoChu = "0" + SoChu;
    return TienTo + SoChu;
}
} // namespace
const HoSo &DemoTriageService::LayHoSo(int ChiSo) const {
    if (ChiSo < 0 || ChiSo >= SoHoSo)
        throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Ho so khong ton tai");
    return BenhNhan[ChiSo];
}
const LuotKham &DemoTriageService::LayLuot(int ChiSo) const {
    if (ChiSo < 0 || ChiSo >= SoLuot)
        throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Luot khong ton tai");
    return CacLuot[ChiSo];
}
Date DemoTriageService::LayNgay() const {
    // Mốc UTC tương ứng 00:00 ngày 30/10; dịch theo ngày mô phỏng, độc lập múi giờ máy.
    const std::time_t MocNgay = 1793318400LL + (BayGio / 1440) * 86400;
    const std::tm *Lich = std::gmtime(&MocNgay);
    if (!Lich)
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi dong ho mo phong");
    return {Lich->tm_mday, Lich->tm_mon + 1, Lich->tm_year + 1900};
}
int DemoTriageService::LayTuoi(const HoSo &DuLieu) const {
    if (!DuLieu.CoNgaySinh)
        return DuLieu.TuoiUocLuong;
    const Date Ngay = LayNgay();
    int Tuoi = Ngay.Nam - DuLieu.NgaySinh.Nam;
    if (Ngay.Thang < DuLieu.NgaySinh.Thang ||
        (Ngay.Thang == DuLieu.NgaySinh.Thang && Ngay.Ngay < DuLieu.NgaySinh.Ngay))
        --Tuoi;
    return Tuoi;
}
bool DemoTriageService::DangCho(const LuotKham &Luot) {
    return Luot.TinhTrang == TrangThai::LanDau || Luot.TinhTrang == TrangThai::QuayLai;
}
bool DemoTriageService::DaDong(const LuotKham &Luot) {
    return Luot.TinhTrang == TrangThai::KetThuc || Luot.TinhTrang == TrangThai::Huy;
}
const char *DemoTriageService::TenTrangThai(TrangThai Loai) {
    switch (Loai) {
    case TrangThai::SangLoc:
        return "Chờ sàng lọc";
    case TrangThai::LanDau:
        return "Chờ khám lần đầu";
    case TrangThai::QuayLai:
        return "Chờ khám quay lại";
    case TrangThai::DangKham:
        return "Đang khám";
    case TrangThai::CanLamSang:
        return "Chờ cận lâm sàng";
    case TrangThai::KetThuc:
        return "Đã kết thúc";
    case TrangThai::Huy:
        return "Đã hủy";
    }
    return "";
}
KetQua DemoTriageService::KiemTraHoSo(const HoSo &DuLieu) const {
    if (!DuLieu.ChuaRo && Rong(DuLieu.HoTen))
        return Loi("Nhập họ tên hoặc chọn chưa rõ danh tính.", "HoTen");
    if (DuLieu.TuoiUocLuong < -1 || DuLieu.TuoiUocLuong > 150)
        return Loi("Tuổi ước lượng phải từ 0 đến 150, hoặc để trống.", "Tuoi");
    if (DuLieu.CoNgaySinh &&
        (!DuLieu.NgaySinh.HopLe() || LayNgay().Truoc(DuLieu.NgaySinh) || LayTuoi(DuLieu) > 150))
        return Loi("Ngày sinh không hợp lệ, ở tương lai hoặc tuổi vượt 150.", "NgaySinh");
    if (DuLieu.GioiTinh != "Nam" && DuLieu.GioiTinh != "Nữ" && DuLieu.GioiTinh != "Khác" &&
        DuLieu.GioiTinh != "Chưa xác định")
        return Loi("Giới tính không hợp lệ.", "GioiTinh");
    return {};
}
int DemoTriageService::TimLuotDangMo(int HoSoSo) const {
    for (int ViTri = 0; ViTri < SoLuot; ++ViTri)
        if (CacLuot[ViTri].BenhNhan == HoSoSo && !DaDong(CacLuot[ViTri]))
            return ViTri;
    return -1;
}
void DemoTriageService::GhiMoc(LuotKham &Luot, const std::string &NoiDung) {
    Luot.CacMoc[Luot.SoMoc++] = {BayGio, NoiDung};
}
void DemoTriageService::VaoHang(LuotKham &Luot, TrangThai Loai) {
    Luot.TinhTrang = Loai;
    Luot.VaoHangLuc = BayGio;
    Luot.SoThuTu = ++ThuTu;
}
KetQua DemoTriageService::TiepNhan(const HoSo &DuLieu, const std::string &TrieuChung, int DaCo) {
    if (Rong(TrieuChung))
        return Loi("Nhập triệu chứng hoặc lý do đến khám.", "TrieuChung");
    if (SoLuot >= GioiHanLuot || (DaCo < 0 && SoHoSo >= GioiHanHoSo))
        return Loi("Đã đạt sức chứa demo. Hãy đặt lại dữ liệu.");
    if (DaCo >= 0) {
        if (DaCo >= SoHoSo)
            return Loi("Chọn một hồ sơ hợp lệ.");
        if (TimLuotDangMo(DaCo) >= 0)
            return Loi("Bệnh nhân đang có lượt chưa đóng. Hãy mở lượt hiện tại.");
    } else {
        const KetQua KiemTra = KiemTraHoSo(DuLieu);
        if (!KiemTra.ThanhCong)
            return KiemTra;
        DaCo = SoHoSo++;
        BenhNhan[DaCo] = DuLieu;
        BenhNhan[DaCo].Ma = Ma("BN-", SoHoSo);
    }
    LuotKham &Luot = CacLuot[SoLuot];
    Luot = LuotKham{};
    Luot.Ma = Ma("LK-", SoLuot + 1);
    Luot.BenhNhan = DaCo;
    Luot.TrieuChung = TrieuChung;
    Luot.TiepNhanLuc = BayGio;
    GhiMoc(Luot, "Tiếp nhận · chờ sàng lọc");
    return {true, "", "", SoLuot++};
}
KetQua DemoTriageService::CapNhatHoSo(int ChiSo, const HoSo &DuLieu) {
    LayHoSo(ChiSo);
    const KetQua KiemTra = KiemTraHoSo(DuLieu);
    if (!KiemTra.ThanhCong)
        return KiemTra;
    const std::string MaCu = BenhNhan[ChiSo].Ma;
    BenhNhan[ChiSo] = DuLieu;
    BenhNhan[ChiSo].Ma = MaCu;
    return {};
}
KetQua DemoTriageService::KiemTraSinhHieu(const SinhHieu &SoDo) {
    if (SoDo.Mach < 0 || SoDo.Mach > 300)
        return Loi("Nhập số nguyên 0–300 lần/phút.", "Mach");
    if (SoDo.HuyetAp < 0 || SoDo.HuyetAp > 300)
        return Loi("Nhập số nguyên 0–300 mmHg.", "HuyetAp");
    if (SoDo.NhipTho < 0 || SoDo.NhipTho > 120)
        return Loi("Nhập số nguyên 0–120 lần/phút.", "NhipTho");
    if (!std::isfinite(SoDo.SpO2) || SoDo.SpO2 < 0 || SoDo.SpO2 > 100)
        return Loi("Nhập SpO₂ từ 0 đến 100%.", "SpO2");
    if (SoDo.GCS < 3 || SoDo.GCS > 15)
        return Loi("Nhập GCS nguyên từ 3 đến 15.", "GCS");
    return {};
}
DeXuat DemoTriageService::DeXuatMuc(const SinhHieu &SoDo, int Tuoi) {
    const KetQua KiemTra = KiemTraSinhHieu(SoDo);
    if (!KiemTra.ThanhCong || Tuoi < -1 || Tuoi > 150)
        return {};
    // Giữ nguyên thứ tự điều kiện evaluateTriageLevel @95e145f trên patient-class.
    const int Mach = SoDo.Mach, HuyetAp = SoDo.HuyetAp, NhipTho = SoDo.NhipTho;
    int Muc = 0;
    if (SoDo.GCS <= 8 || SoDo.SpO2 < 90)
        Muc = 1;
    else if (SoDo.GCS <= 12 || SoDo.SpO2 < 95)
        Muc = 2;
    else if (Tuoi == -1)
        return {}; // Không dùng nhóm dưới 1 tuổi khi chưa biết tuổi.
    else if (Tuoi < 1) {
        if (Mach < 60 || Mach > 200 || NhipTho < 20 || NhipTho > 70 || HuyetAp < 60)
            Muc = 1;
        else if (Mach < 90 || Mach > 170 || NhipTho < 30 || NhipTho > 55 || HuyetAp < 70)
            Muc = 2;
        else if (Mach > 160 || NhipTho > 50)
            Muc = 3;
    } else if (Tuoi <= 5) {
        const int Nguong = 70 + 2 * Tuoi;
        if (Mach < 60 || Mach > 180 || NhipTho < 12 || NhipTho > 50 || HuyetAp < Nguong - 10)
            Muc = 1;
        else if (Mach < 80 || Mach > 140 || NhipTho < 18 || NhipTho > 35 || HuyetAp < Nguong)
            Muc = 2;
        else if (Mach > 120 || NhipTho > 30)
            Muc = 3;
    } else if (Tuoi <= 12) {
        if (Mach < 50 || Mach > 160 || NhipTho < 10 || NhipTho > 40 || HuyetAp < 75)
            Muc = 1;
        else if (Mach < 60 || Mach > 120 || NhipTho < 14 || NhipTho > 28 || HuyetAp < 85)
            Muc = 2;
        else if (Mach > 100 || NhipTho > 24)
            Muc = 3;
    } else if (Tuoi < 65) {
        if (NhipTho < 10 || NhipTho >= 35 || HuyetAp < 80 || Mach < 40 || Mach > 150)
            Muc = 1;
        else if (NhipTho < 12 || NhipTho >= 25 || HuyetAp < 90 || HuyetAp > 180 || Mach < 50 || Mach > 120)
            Muc = 2;
        else if (NhipTho > 20 || HuyetAp > 140 || Mach > 100)
            Muc = 3;
    } else {
        if (NhipTho < 10 || NhipTho >= 30 || HuyetAp < 90 || Mach < 45 || Mach > 130)
            Muc = 1;
        else if (NhipTho < 12 || NhipTho >= 24 || HuyetAp < 100 || HuyetAp > 170 || Mach < 55 || Mach > 110)
            Muc = 2;
        else if (NhipTho > 20 || HuyetAp > 140 || Mach > 90)
            Muc = 3;
    }
    if (!Muc)
        Muc = SoDo.SpO2 < 97 ? 4 : 5;
    return {Muc, Tuoi != -1};
}
KetQua DemoTriageService::XacNhan(int ChiSo, const SinhHieu &SoDo, int Muc) {
    LayLuot(ChiSo);
    LuotKham &Luot = CacLuot[ChiSo];
    if (DaDong(Luot))
        return Loi("Lượt đã đóng, không thể đánh giá.");
    if (Luot.SoDanhGia >= GioiHanDanhGia || Luot.SoMoc >= GioiHanMoc)
        return Loi("Đã đạt giới hạn lịch sử của demo.");
    const KetQua KiemTra = KiemTraSinhHieu(SoDo);
    if (!KiemTra.ThanhCong)
        return KiemTra;
    if (Muc < 1 || Muc > 5)
        return Loi("Chọn mức điều dưỡng xác nhận từ 1 đến 5.", "Muc");
    const HoSo &Benh = LayHoSo(Luot.BenhNhan);
    DanhGia BanGhi;
    BanGhi.SoDo = SoDo;
    BanGhi.Tuoi = LayTuoi(Benh);
    BanGhi.TuNgaySinh = Benh.CoNgaySinh;
    BanGhi.GoiY = DeXuatMuc(SoDo, BanGhi.Tuoi);
    BanGhi.Muc = Muc;
    BanGhi.Luc = BayGio;
    Luot.LichSu[Luot.SoDanhGia++] = BanGhi;
    Luot.Muc = Muc;
    if (Luot.TinhTrang == TrangThai::SangLoc)
        VaoHang(Luot, TrangThai::LanDau);
    GhiMoc(Luot, "Xác nhận phân loại · Mức " + std::to_string(Muc));
    DongBoBoDem();
    return {};
}
// Hai đầu hàng của từng mức được tìm bằng vòng lặp, xếp theo thời điểm rồi STT.
// Chỉ tăng bộ đếm khi thực sự có hai luồng cùng mức cạnh tranh.
int DemoTriageService::ChonLuot(const bool *DaLay, int *Dem) const {
    int Dau[6][2];
    for (int Muc = 0; Muc <= 5; ++Muc)
        Dau[Muc][0] = Dau[Muc][1] = -1;
    for (int ViTri = 0; ViTri < SoLuot; ++ViTri) {
        const LuotKham &Luot = CacLuot[ViTri];
        if (DaLay[ViTri] || !DangCho(Luot))
            continue;
        int &HienTai = Dau[Luot.Muc][Luot.TinhTrang == TrangThai::QuayLai ? 1 : 0];
        if (HienTai == -1 || Luot.VaoHangLuc < CacLuot[HienTai].VaoHangLuc ||
            (Luot.VaoHangLuc == CacLuot[HienTai].VaoHangLuc && Luot.SoThuTu < CacLuot[HienTai].SoThuTu))
            HienTai = ViTri;
    }
    for (int Muc = 1; Muc <= 5; ++Muc)
        if (Dau[Muc][0] < 0 || Dau[Muc][1] < 0)
            Dem[Muc] = 0;
    for (int Muc = 1; Muc <= 5; ++Muc) {
        const int Moi = Dau[Muc][0], Lai = Dau[Muc][1];
        if (Moi >= 0 && Lai >= 0) {
            if (Dem[Muc] < 3) {
                ++Dem[Muc];
                return Lai;
            }
            Dem[Muc] = 0;
            return Moi;
        }
        if (Moi >= 0)
            return Moi;
        if (Lai >= 0)
            return Lai;
    }
    return -1;
}
void DemoTriageService::DongBoBoDem() {
    for (int Muc = 1; Muc <= 5; ++Muc) {
        bool Moi = false, Lai = false;
        for (int ViTri = 0; ViTri < SoLuot; ++ViTri)
            if (CacLuot[ViTri].Muc == Muc) {
                Moi = Moi || CacLuot[ViTri].TinhTrang == TrangThai::LanDau;
                Lai = Lai || CacLuot[ViTri].TinhTrang == TrangThai::QuayLai;
            }
        if (!Moi || !Lai)
            BoDem[Muc] = 0;
    }
}
HangCho DemoTriageService::LayHangCho() const {
    HangCho Hang;
    bool DaLay[GioiHanLuot]{};
    int Dem[6];
    for (int Muc = 0; Muc <= 5; ++Muc)
        Dem[Muc] = BoDem[Muc];
    // Xem và lọc chỉ dùng bản sao bộ đếm; không tiêu thụ lần gọi 3:1.
    for (int ViTri = ChonLuot(DaLay, Dem); ViTri >= 0; ViTri = ChonLuot(DaLay, Dem)) {
        Hang.CacLuot[Hang.SoLuong++] = ViTri;
        DaLay[ViTri] = true;
    }
    return Hang;
}
KetQua DemoTriageService::GoiTiep() {
    bool DaLay[GioiHanLuot]{};
    int Dem[6];
    for (int Muc = 0; Muc <= 5; ++Muc)
        Dem[Muc] = BoDem[Muc];
    const int ChiSo = ChonLuot(DaLay, Dem);
    if (ChiSo < 0)
        return Loi("Hàng chờ đang trống.");
    LuotKham &Luot = CacLuot[ChiSo];
    if (Luot.SoMoc >= GioiHanMoc)
        return Loi("Đã đạt giới hạn lịch sử của demo.");
    for (int Muc = 0; Muc <= 5; ++Muc)
        BoDem[Muc] = Dem[Muc];
    Luot.TinhTrang = TrangThai::DangKham;
    Luot.VaoHangLuc = -1;
    Luot.SoThuTu = 0;
    GhiMoc(Luot, "Được gọi · đang khám");
    DongBoBoDem();
    return {true, "", "", ChiSo};
}
KetQua DemoTriageService::ChuyenCanLamSang(int ChiSo, const std::string &ChiDinh) {
    LayLuot(ChiSo);
    LuotKham &Luot = CacLuot[ChiSo];
    if (Luot.TinhTrang != TrangThai::DangKham)
        return Loi("Chỉ chuyển cận lâm sàng khi đang khám.");
    if (Rong(ChiDinh))
        return Loi("Nhập nội dung chỉ định.", "NoiDung");
    if (Luot.SoCanLamSang >= GioiHanCanLamSang || Luot.SoMoc >= GioiHanMoc)
        return Loi("Đã đạt giới hạn lịch sử demo.");
    Luot.CacLanCanLamSang[Luot.SoCanLamSang++] = {ChiDinh, "", BayGio, -1};
    Luot.TinhTrang = TrangThai::CanLamSang;
    GhiMoc(Luot, "Chuyển cận lâm sàng · " + ChiDinh);
    return {};
}
KetQua DemoTriageService::QuayLai(int ChiSo, const std::string &KetQuaXetNghiem) {
    LayLuot(ChiSo);
    LuotKham &Luot = CacLuot[ChiSo];
    if (Luot.TinhTrang != TrangThai::CanLamSang)
        return Loi("Lượt chưa ở trạng thái chờ cận lâm sàng.");
    if (Rong(KetQuaXetNghiem))
        return Loi("Nhập kết quả cận lâm sàng.", "NoiDung");
    if (Luot.SoMoc >= GioiHanMoc)
        return Loi("Đã đạt giới hạn lịch sử demo.");
    CanLamSang &Lan = Luot.CacLanCanLamSang[Luot.SoCanLamSang - 1];
    Lan.KetQua = KetQuaXetNghiem;
    Lan.TraLuc = BayGio;
    VaoHang(Luot, TrangThai::QuayLai);
    GhiMoc(Luot, "Có kết quả · vào hàng quay lại");
    DongBoBoDem();
    return {};
}
KetQua DemoTriageService::DongLuot(int ChiSo, bool Huy) {
    LayLuot(ChiSo);
    LuotKham &Luot = CacLuot[ChiSo];
    if (DaDong(Luot) || (!Huy && Luot.TinhTrang != TrangThai::DangKham))
        return Loi("Không thể đóng lượt với trạng thái hiện tại.");
    if (Luot.SoMoc >= GioiHanMoc)
        return Loi("Đã đạt giới hạn lịch sử demo.");
    Luot.TinhTrang = Huy ? TrangThai::Huy : TrangThai::KetThuc;
    Luot.VaoHangLuc = -1;
    Luot.SoThuTu = 0;
    GhiMoc(Luot, TenTrangThai(Luot.TinhTrang));
    DongBoBoDem();
    return {};
}
bool DemoTriageService::CanDanhGia(int ChiSo) const {
    const LuotKham &Luot = LayLuot(ChiSo);
    const int MocPhut[] = {0, 0, 10, 30, 60, 120};
    return !DaDong(Luot) && Luot.SoDanhGia > 0 &&
           BayGio - Luot.LichSu[Luot.SoDanhGia - 1].Luc > MocPhut[Luot.Muc];
}
ThongKe DemoTriageService::LayThongKe() const {
    ThongKe So;
    for (int ViTri = 0; ViTri < SoLuot; ++ViTri) {
        switch (CacLuot[ViTri].TinhTrang) {
        case TrangThai::SangLoc:
            ++So.SangLoc;
            break;
        case TrangThai::LanDau:
            ++So.LanDau;
            break;
        case TrangThai::QuayLai:
            ++So.QuayLai;
            break;
        default:
            break;
        }
        if (CanDanhGia(ViTri))
            ++So.CanDanhGia;
    }
    return So;
}
void DemoTriageService::XoaDuLieu() {
    for (int ViTri = 0; ViTri < SoHoSo; ++ViTri)
        BenhNhan[ViTri] = HoSo{};
    for (int ViTri = 0; ViTri < SoLuot; ++ViTri)
        CacLuot[ViTri] = LuotKham{};
    SoHoSo = SoLuot = ThuTu = 0;
    BayGio = 570;
    for (int &Dem : BoDem)
        Dem = 0;
}
} // namespace Demo
