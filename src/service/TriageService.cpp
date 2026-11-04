#include "service/TriageService.h"
#include <climits>
#include "model/Patient.h"
#include "model/Visit.h"
#include "util/My_stdexcept.h"

namespace {
void SaiTrangThai() {
    throw My_stdexcept(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, "Trang thai hang cho khong hop le");
}
}

Visit& TriageService::TimLanKham(const std::string& Ma) {
    Visit* LanKham = Registry.TimLanKham(Ma);
    if (LanKham == nullptr)
        throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Khong tim thay lan kham");
    return *LanKham;
}

int TriageService::TimTrongHang(const HangCho& Hang, const Visit* LanKham) const {
    for (int I = 0; I < Hang.KichThuoc(); ++I)
        if (Hang.LayTai(I).LanKham == LanKham) return I;
    return -1;
}

void TriageService::KiemTraHang() const {
    const HangCho* CacHang[] = {&HangLanDau, &HangQuayLai};
    for (int I = 0; I < 2; ++I) {
        VisitStatus TrangThai;
        if (I == 0) {
            TrangThai = VisitStatus::ChoKhamLanDau;
        } else {
            TrangThai = VisitStatus::ChoKhamQuayLai;
        }
        for (int J = 0; J < CacHang[I]->KichThuoc(); ++J) {
            const MucChoKham& Muc = CacHang[I]->LayTai(J);
            if (Muc.LanKham == nullptr || Muc.LanKham->LayTrangThai() != TrangThai ||
                Muc.LanKham->SoDanhGia() == 0 || Muc.LanKham->LayDanhGiaGanNhat().LayMuc() != Muc.Muc)
                SaiTrangThai();
        }
    }
}

bool TriageService::ChonHangQuayLai(const HangCho& Dau, const HangCho& Lai, bool UuTienLai) {
    if (Dau.Rong()) return true;
    if (Lai.Rong()) return false;
    if (Dau.LayDinh().Muc == Lai.LayDinh().Muc) return UuTienLai;
    return Lai.LayDinh().Muc < Dau.LayDinh().Muc;
}

MucChoKham TriageService::TaoMuc(Visit& LanKham, std::time_t ThoiDiem) {
    if (ThoiDiem < LanKham.LayThoiDiemTiepNhan())
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Thoi diem vao hang khong hop le");
    if (ThuTu == ULLONG_MAX)
        throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Het so thu tu hang cho");
    return {&LanKham, 5, ThoiDiem, ThuTu + 1};
}

const Visit& TriageService::TiepNhan(const std::string& MaLanKham, const std::string& MaBenhNhan,
                                     std::time_t ThoiDiem, const std::string& TrieuChung) {
    return *Registry.ThemLanKham(MaLanKham, MaBenhNhan, ThoiDiem, TrieuChung);
}

void TriageService::DanhGia(const std::string& Ma, const VitalSigns& SinhHieu,
                            std::time_t ThoiDiem, const std::string& MaDieuDuong) {
    KiemTraHang();
    Visit& LanKham = TimLanKham(Ma);
    int ViTriDau = TimTrongHang(HangLanDau, &LanKham);
    int ViTriLai = TimTrongHang(HangQuayLai, &LanKham);
    if (ViTriDau >= 0 && ViTriLai >= 0) SaiTrangThai();
    bool Moi = ViTriDau < 0 && ViTriLai < 0;
    if (Moi && (LanKham.LayTrangThai() != VisitStatus::ChoKhamLanDau || LanKham.SoDanhGia() != 0)) SaiTrangThai();
    if (!Moi && ((ViTriDau >= 0 && LanKham.LayTrangThai() != VisitStatus::ChoKhamLanDau) ||
                 (ViTriLai >= 0 && LanKham.LayTrangThai() != VisitStatus::ChoKhamQuayLai))) SaiTrangThai();
    if ((LanKham.SoDanhGia() > 0 && ThoiDiem < LanKham.LayDanhGiaGanNhat().LayThoiDiemDanhGia()) ||
        (ViTriDau >= 0 && ThoiDiem < HangLanDau.LayTai(ViTriDau).ThoiDiemVaoHang) ||
        (ViTriLai >= 0 && ThoiDiem < HangQuayLai.LayTai(ViTriLai).ThoiDiemVaoHang))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Thoi diem danh gia khong phu hop");
    const Patient* BenhNhan = Registry.TimBenhNhan(LanKham.LayMaBenhNhan());
    if (BenhNhan == nullptr) SaiTrangThai();
    // Cấp phát mục chờ trước khi lưu đánh giá; nếu đánh giá thất bại thì gỡ mục.
    if (Moi) HangLanDau.Them(TaoMuc(LanKham, ThoiDiem));
    try {
        LanKham.DanhGia(*BenhNhan, SinhHieu, ThoiDiem, MaDieuDuong);
    } catch (...) {
        if (Moi) HangLanDau.XoaTai(TimTrongHang(HangLanDau, &LanKham));
        throw;
    }
    HangCho& Hang = ViTriLai >= 0 ? HangQuayLai : HangLanDau;
    int ViTri = TimTrongHang(Hang, &LanKham);
    MucChoKham Muc = Hang.LayTai(ViTri);
    Muc.Muc = LanKham.LayDanhGiaGanNhat().LayMuc();
    Hang.SuaTai(ViTri, Muc);
    if (Moi) ++ThuTu;
}

const Visit& TriageService::GoiTiep(const std::string& MaBacSi) {
    KiemTraHang();
    if (HangLanDau.Rong() && HangQuayLai.Rong()) SaiTrangThai();
    bool TranhChap = !HangLanDau.Rong() && !HangQuayLai.Rong() &&
                     HangLanDau.LayDinh().Muc == HangQuayLai.LayDinh().Muc;
    bool ChonLai = ChonHangQuayLai(HangLanDau, HangQuayLai, UuTienQuayLai);
    HangCho& Hang = ChonLai ? HangQuayLai : HangLanDau;
    Visit* LanKham = Hang.LayDinh().LanKham;
    // Ghi nhận bác sĩ có thể lỗi; chỉ bỏ mục và đổi lượt sau khi thành công.
    LanKham->BatDauKham(MaBacSi);
    Hang.XoaDinh();
    if (TranhChap) UuTienQuayLai = !UuTienQuayLai;
    return *LanKham;
}

Vector<const Visit*> TriageService::LayDanhSachCho() const {
    KiemTraHang();
    // Dùng bản sao hai heap và lượt ưu tiên để xem mà không tiêu thụ lượt gọi.
    HangCho Dau = HangLanDau, Lai = HangQuayLai;
    bool Luot = UuTienQuayLai;
    Vector<const Visit*> KetQua;
    while (!Dau.Rong() || !Lai.Rong()) {
        bool TranhChap = !Dau.Rong() && !Lai.Rong() && Dau.LayDinh().Muc == Lai.LayDinh().Muc;
        HangCho& Hang = ChonHangQuayLai(Dau, Lai, Luot) ? Lai : Dau;
        KetQua.ThemCuoi(Hang.LayDinh().LanKham);
        Hang.XoaDinh();
        if (TranhChap) Luot = !Luot;
    }
    return KetQua;
}

void TriageService::ChuyenCanLamSang(const std::string& Ma, const std::string& ChiDinh,
                                     const std::string& MaBacSi) {
    TimLanKham(Ma).ChuyenDiCanLamSang(ChiDinh, MaBacSi);
}

void TriageService::QuayLai(const std::string& Ma, const std::string& KetQua, std::time_t ThoiDiem) {
    KiemTraHang();
    Visit& LanKham = TimLanKham(Ma);
    if (LanKham.LayTrangThai() != VisitStatus::ChoCanLamSang ||
        TimTrongHang(HangLanDau, &LanKham) >= 0 || TimTrongHang(HangQuayLai, &LanKham) >= 0) SaiTrangThai();
    if (LanKham.SoDanhGia() > 0 && ThoiDiem < LanKham.LayDanhGiaGanNhat().LayThoiDiemDanhGia())
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Thoi diem quay lai bi lui");
    MucChoKham Muc = TaoMuc(LanKham, ThoiDiem);
    Muc.Muc = LanKham.LayDanhGiaGanNhat().LayMuc();
    HangQuayLai.Them(Muc);
    try {
        LanKham.GhiNhanKetQua(KetQua);
        LanKham.DanhDauQuayLai();
    } catch (...) {
        HangQuayLai.XoaTai(TimTrongHang(HangQuayLai, &LanKham));
        throw;
    }
    ++ThuTu;
}

void TriageService::KetThuc(const std::string& Ma) { TimLanKham(Ma).KetThucLanKham(); }

void TriageService::Huy(const std::string& Ma) {
    Visit& LanKham = TimLanKham(Ma);
    int ViTriDau = TimTrongHang(HangLanDau, &LanKham);
    int ViTriLai = TimTrongHang(HangQuayLai, &LanKham);
    LanKham.HuyLanKham();
    if (ViTriDau >= 0) HangLanDau.XoaTai(ViTriDau);
    if (ViTriLai >= 0) HangQuayLai.XoaTai(ViTriLai);
}
