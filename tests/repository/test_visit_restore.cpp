#include <cassert>
#include <ctime>
#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "model/DoctorParticipation.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"

namespace {
template <typename ThaoTac>
void PhaiLoi(ThaoTac Chay) {
    bool CoLoi = false;
    try { Chay(); } catch (const My_stdexcept&) { CoLoi = true; }
    assert(CoLoi);
}

BanChupLanKham TaoBanChup(const std::string& Ma = "LK") {
    BanChupLanKham BanChup;
    BanChup.MaLanKham = Ma;
    BanChup.MaBenhNhan = "BN";
    BanChup.ThoiDiemTiepNhan = 100;
    BanChup.TrieuChungChinh = "Dau, sot\n\"kho chiu\"";
    return BanChup;
}

void ThemDanhGia(BanChupLanKham& BanChup, std::time_t ThoiDiem = 101) {
    BanChup.DanhGia.ThemCuoi(TriageAssessment(VitalSigns(75, 120, 16, 98.125, 15),
                                             28, true, ThoiDiem, "DD"));
}

void KiemTraGiongNhau(const BanChupLanKham& Truoc, const BanChupLanKham& Sau) {
    assert(Truoc.MaLanKham == Sau.MaLanKham && Truoc.MaBenhNhan == Sau.MaBenhNhan);
    assert(Truoc.ThoiDiemTiepNhan == Sau.ThoiDiemTiepNhan);
    assert(Truoc.TrieuChungChinh == Sau.TrieuChungChinh && Truoc.TrangThai == Sau.TrangThai);
    assert(Truoc.DanhGia.KichThuoc() == Sau.DanhGia.KichThuoc());
    for (int I = 0; I < Truoc.DanhGia.KichThuoc(); ++I) {
        const TriageAssessment& A = Truoc.DanhGia[I];
        const TriageAssessment& B = Sau.DanhGia[I];
        assert(A.LayTuoi() == B.LayTuoi() && A.LaTuoiUocLuong() == B.LaTuoiUocLuong());
        assert(A.LayMuc() == B.LayMuc() && A.LayMaDieuDuong() == B.LayMaDieuDuong());
        assert(A.LayThoiDiemDanhGia() == B.LayThoiDiemDanhGia());
        assert(A.LaySinhHieu().LayNhipTim() == B.LaySinhHieu().LayNhipTim());
        assert(A.LaySinhHieu().LayHuyetApTamThu() == B.LaySinhHieu().LayHuyetApTamThu());
        assert(A.LaySinhHieu().LayNhipTho() == B.LaySinhHieu().LayNhipTho());
        assert(A.LaySinhHieu().LaySpo2() == B.LaySinhHieu().LaySpo2());
        assert(A.LaySinhHieu().LayDiemGcs() == B.LaySinhHieu().LayDiemGcs());
    }
    assert(Truoc.CanLamSang.KichThuoc() == Sau.CanLamSang.KichThuoc());
    for (int I = 0; I < Truoc.CanLamSang.KichThuoc(); ++I) {
        assert(Truoc.CanLamSang[I].LayChiDinh() == Sau.CanLamSang[I].LayChiDinh());
        assert(Truoc.CanLamSang[I].LayMaBacSiChiDinh() == Sau.CanLamSang[I].LayMaBacSiChiDinh());
        assert(Truoc.CanLamSang[I].DaCoKetQua() == Sau.CanLamSang[I].DaCoKetQua());
        assert(Truoc.CanLamSang[I].LayKetQua() == Sau.CanLamSang[I].LayKetQua());
    }
    assert(Truoc.BacSiPhanCong.KichThuoc() == Sau.BacSiPhanCong.KichThuoc());
    for (int I = 0; I < Truoc.BacSiPhanCong.KichThuoc(); ++I)
        assert(Truoc.BacSiPhanCong[I] == Sau.BacSiPhanCong[I]);
    assert(Truoc.BacSiThamGia.KichThuoc() == Sau.BacSiThamGia.KichThuoc());
    for (int I = 0; I < Truoc.BacSiThamGia.KichThuoc(); ++I)
        assert(Truoc.BacSiThamGia[I] == Sau.BacSiThamGia[I]);
}

void KiemTraVongDoi() {
    Patient BenhNhan("BN", "An", Gender::Nam);
    BenhNhan.DatTuoiUocLuong(28);
    Visit LanKham("LK", "BN", 100, "Dau, sot\n\"kho chiu\"");
    auto KiemTra = [&] {
        const BanChupLanKham Truoc = LanKham.LayBanChup();
        Visit* KhoiPhuc = Visit::TuBanChup(Truoc);
        KiemTraGiongNhau(Truoc, KhoiPhuc->LayBanChup());
        delete KhoiPhuc;
    };
    LanKham.PhanCongBacSi("BS1");
    KiemTra(); // Có thể phân công trước khi đánh giá hoặc bắt đầu khám.
    LanKham.DanhGia(BenhNhan, VitalSigns(75, 120, 16, 98.125, 15), 101, "DD1");
    LanKham.BatDauKham("BS1");
    KiemTra();
    LanKham.ChuyenDiCanLamSang("Xet nghiem", "BS1");
    KiemTra();
    LanKham.GhiNhanKetQua("On");
    KiemTra(); // Kết quả có thể đã ghi trong khi chưa bấm quay lại.
    LanKham.DanhDauQuayLai();
    KiemTra();
    // Danh tính được sửa sau lần đầu: giữ nguyên tuổi ước lượng đã lưu trước đó.
    BenhNhan.BoSungDanhTinh("", "An", Gender::Nam, Date{1, 1, 1970});
    LanKham.DanhGia(BenhNhan, VitalSigns(80, 125, 18, 97, 15), 102, "DD2");
    LanKham.BatDauKham("BS1");
    LanKham.GhiNhanBacSiThamGiaKham("BS1");
    LanKham.PhanCongBacSi("BS2");
    LanKham.ChuyenDiCanLamSang("Sieu am", "BS1");
    KiemTra();
    LanKham.GhiNhanKetQua("Binh thuong");
    LanKham.DanhDauQuayLai();
    LanKham.BatDauKham("BS2");
    LanKham.KetThucLanKham();
    KiemTra();
    Visit HuySom("HUY1", "BN", 100, "");
    HuySom.HuyLanKham();
    Visit* BanHuySom = Visit::TuBanChup(HuySom.LayBanChup());
    delete BanHuySom;
    BanChupLanKham HuyCls = LanKham.LayBanChup();
    HuyCls.TrangThai = VisitStatus::DaHuy;
    HuyCls.CanLamSang.ThemCuoi(BanGhiCanLamSang("CT", "BS2", false, ""));
    Visit* BanHuyCls = Visit::TuBanChup(HuyCls);
    delete BanHuyCls;
}

void KiemTraBanChupSai() {
    auto TuChoi = [](const BanChupLanKham& BanChup) {
        PhaiLoi([&] { Visit* Sai = Visit::TuBanChup(BanChup); delete Sai; });
    };
    BanChupLanKham B = TaoBanChup();
    B.MaLanKham = ""; TuChoi(B);
    B = TaoBanChup(); B.MaBenhNhan = ""; TuChoi(B);
    B = TaoBanChup(); B.TrangThai = static_cast<VisitStatus>(99); TuChoi(B);
    B = TaoBanChup(); ThemDanhGia(B, 99); TuChoi(B);
    B = TaoBanChup(); ThemDanhGia(B, 102); ThemDanhGia(B, 101); TuChoi(B);
    B = TaoBanChup(); B.DanhGia.ThemCuoi(TriageAssessment()); TuChoi(B);
    B = TaoBanChup(); B.BacSiPhanCong.ThemCuoi(""); TuChoi(B);
    B = TaoBanChup(); B.BacSiPhanCong.ThemCuoi("BS"); B.BacSiPhanCong.ThemCuoi("BS"); TuChoi(B);
    B = TaoBanChup(); B.BacSiThamGia.ThemCuoi("BS"); TuChoi(B);
    B = TaoBanChup(); B.TrangThai = VisitStatus::DangKham; TuChoi(B);
    ThemDanhGia(B); TuChoi(B);
    B.BacSiThamGia.ThemCuoi(""); TuChoi(B);
    B.BacSiThamGia[0] = "BS";
    B.CanLamSang.ThemCuoi(BanGhiCanLamSang("CT", "KHAC", true, "On")); TuChoi(B);
    B.CanLamSang[0] = BanGhiCanLamSang("CT", "BS", false, ""); TuChoi(B);
    B.TrangThai = VisitStatus::ChoCanLamSang;
    B.CanLamSang.ThemCuoi(BanGhiCanLamSang("CT2", "BS", true, "On")); TuChoi(B);
    B = TaoBanChup(); ThemDanhGia(B); B.BacSiThamGia.ThemCuoi("BS");
    B.TrangThai = VisitStatus::ChoKhamQuayLai; TuChoi(B);
    B.CanLamSang.ThemCuoi(BanGhiCanLamSang()); TuChoi(B);
    PhaiLoi([] { BanGhiCanLamSang("", "BS", false, ""); });
    PhaiLoi([] { BanGhiCanLamSang("CT", "", false, ""); });
    PhaiLoi([] { BanGhiCanLamSang("CT", "BS", true, ""); });
    PhaiLoi([] { BanGhiCanLamSang("CT", "BS", false, "Ket qua le"); });
}

// Mất vai trò/thời điểm khi nối hai nhánh sẽ làm các kiểm tra này thất bại.
void KiemTraLichSuThamGiaChiTiet() {
    Patient BenhNhan("BN", "An", Gender::Nam);
    BenhNhan.DatTuoiUocLuong(28);
    Visit LanKham("LK", "BN", 100, "Dau");
    LanKham.DanhGia(BenhNhan, VitalSigns(75, 120, 16, 98, 15), 101, "DD");
    LanKham.BatDauKham(DoctorParticipation("BS1", 102));
    LanKham.GhiNhanBacSiThamGiaKham(DoctorParticipation("BS2", 103, DoctorRole::HoTro));
    LanKham.GhiNhanBacSiThamGiaKham(DoctorParticipation("BS3", 104, DoctorRole::ThayThe, "BS1"));
    LanKham.GhiNhanBacSiThamGiaKham(DoctorParticipation("BS1", 105));
    BanChupLanKham BanChup = LanKham.LayBanChup();
    Visit* Moi = Visit::TuBanChup(BanChup);
    for (int I = 0; I < LanKham.SoLanBacSiThamGiaKham(); ++I) {
        const auto& Truoc = LanKham.LayThamGiaTai(I);
        const auto& Sau = Moi->LayThamGiaTai(I);
        assert(Truoc.LayMaBacSi() == Sau.LayMaBacSi());
        assert(Truoc.LayThoiDiemThamGia() == Sau.LayThoiDiemThamGia());
        assert(Truoc.LayVaiTro() == Sau.LayVaiTro());
        assert(Truoc.LayMaBacSiDuocThay() == Sau.LayMaBacSiDuocThay());
    }
    delete Moi;
    // Không được để danh sách mã và bản ghi chi tiết chỉ hai lịch sử khác nhau.
    BanChup.BacSiThamGia[0] = "KHAC";
    PhaiLoi([&] { Visit* Sai = Visit::TuBanChup(BanChup); delete Sai; });
    BanChup = LanKham.LayBanChup();
    BanChup.BacSiThamGiaChiTiet[1] = DoctorParticipation("BS2", 99, DoctorRole::HoTro);
    PhaiLoi([&] { Visit* Sai = Visit::TuBanChup(BanChup); delete Sai; });
    BanChup = LanKham.LayBanChup();
    BanChup.BacSiThamGiaChiTiet[1] = DoctorParticipation("BS2", 101, DoctorRole::HoTro);
    PhaiLoi([&] { Visit* Sai = Visit::TuBanChup(BanChup); delete Sai; });
    BanChup = LanKham.LayBanChup();
    BanChup.BacSiThamGiaChiTiet[2] = DoctorParticipation("BS3", 104, DoctorRole::ThayThe, "THIEU");
    PhaiLoi([&] { Visit* Sai = Visit::TuBanChup(BanChup); delete Sai; });
    // Bản chụp v1 chỉ có mã: 0 biểu diễn thời điểm không biết, không lấy giờ hiện tại.
    BanChup.BacSiThamGiaChiTiet = Vector<DoctorParticipation>();
    Moi = Visit::TuBanChup(BanChup);
    assert(Moi->LayThamGiaTai(0).LayThoiDiemThamGia() == 0);
    delete Moi;
}

void KiemTraRegistryKhongNapDo() {
    PatientRegistry SoBo;
    const Patient* BenhNhan = SoBo.ThemBenhNhan("BN");
    Vector<BanChupLanKham> DanhSach;
    DanhSach.ThemCuoi(TaoBanChup("LK1"));
    DanhSach.ThemCuoi(TaoBanChup("LK2"));
    PhaiLoi([&] { SoBo.KhoiPhucLanKham(DanhSach); }); // Hai lần mở cùng người.
    assert(SoBo.SoLanKham() == 0 && SoBo.TimBenhNhan("BN") == BenhNhan);
    DanhSach[1].MaLanKham = "LK1";
    PhaiLoi([&] { SoBo.KhoiPhucLanKham(DanhSach); });
    assert(SoBo.SoLanKham() == 0);
    DanhSach[1] = TaoBanChup("LK2"); DanhSach[1].MaBenhNhan = "THIEU";
    PhaiLoi([&] { SoBo.KhoiPhucLanKham(DanhSach); });
    assert(SoBo.SoLanKham() == 0);
    DanhSach[1] = TaoBanChup("LK2"); DanhSach[1].TrangThai = VisitStatus::DaKetThuc;
    PhaiLoi([&] { SoBo.KhoiPhucLanKham(DanhSach); });
    assert(SoBo.SoLanKham() == 0);
    // Tuổi đã lưu không cần khớp danh tính mới được bổ sung của Patient.
    ThemDanhGia(DanhSach[1]); DanhSach[1].BacSiThamGia.ThemCuoi("BS");
    SoBo.KhoiPhucLanKham(DanhSach);
    assert(SoBo.SoLanKham() == 2 && SoBo.TimBenhNhan("BN") == BenhNhan);
    assert(SoBo.TimLanKham("LK2")->LayDanhGiaGanNhat().LayTuoi() == 28);
    const Visit* LanDau = SoBo.TimLanKham("LK1");
    Vector<BanChupLanKham> Rong;
    PhaiLoi([&] { SoBo.KhoiPhucLanKham(Rong); });
    assert(SoBo.SoLanKham() == 2 && SoBo.TimLanKham("LK1") == LanDau);
    // Bản chụp độc lập: sửa nó không ảnh hưởng đối tượng đã công bố.
    DanhSach[0].TrieuChungChinh = "Da sua";
    assert(LanDau->LayTrieuChungChinh() != "Da sua");
}

void KiemTraThuHoiSauRehash() {
    PatientRegistry SoBo;
    const Patient* BenhNhan = SoBo.ThemBenhNhan("BN");
    Vector<BanChupLanKham> DanhSach;
    for (int I = 0; I < 80; ++I) {
        BanChupLanKham BanChup = TaoBanChup("LK" + std::to_string(I));
        BanChup.TrangThai = VisitStatus::DaHuy;
        DanhSach.ThemCuoi(BanChup);
    }
    BanChupLanKham Sai = TaoBanChup("SAI");
    Sai.TrangThai = VisitStatus::DaKetThuc; // Lỗi sau khi bảng tạm đã rehash nhiều lần.
    DanhSach.ThemCuoi(Sai);
    PhaiLoi([&] { SoBo.KhoiPhucLanKham(DanhSach); });
    assert(SoBo.SoLanKham() == 0 && SoBo.TimBenhNhan("BN") == BenhNhan);
    DanhSach.XoaCuoi();
    SoBo.KhoiPhucLanKham(DanhSach);
    assert(SoBo.SoLanKham() == 80 && SoBo.TimBenhNhan("BN") == BenhNhan);
}
} // namespace

int main() {
    KiemTraVongDoi();
    KiemTraLichSuThamGiaChiTiet();
    KiemTraBanChupSai();
    KiemTraRegistryKhongNapDo();
    KiemTraThuHoiSauRehash();
}
