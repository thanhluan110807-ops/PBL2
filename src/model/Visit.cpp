#include <string>
#include <ctime>
#include "model/Patient.h"
#include "model/Date.h"
#include "dsa/Vector.h"
#include "model/Visit.h"
#include "model/DoctorParticipation.h"
#include "util/My_stdexcept.h"

namespace {
void BaoLoi(const char* ThongBao) { throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, ThongBao); }
void SaiTrangThai() { throw My_stdexcept(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, "Trang thai lan kham khong hop le"); }

Date LayNgayVietNam(std::time_t ThoiDiemDanhGia) {
    const std::tm* LichUtc = std::gmtime(&ThoiDiemDanhGia);
    if (LichUtc == nullptr) BaoLoi("Thoi diem danh gia khong hop le");
    Date Ngay{LichUtc->tm_mday, LichUtc->tm_mon + 1, LichUtc->tm_year + 1900};
    // Sau 17:00 UTC đã sang ngày mới ở Việt Nam. Tăng ngày lịch để tránh
    // cộng giây trực tiếp vào time_t (có thể tràn ở biên giá trị).
    if (LichUtc->tm_hour >= 17) {
        ++Ngay.Ngay;
        if (!Ngay.HopLe()) {
            Ngay.Ngay = 1;
            ++Ngay.Thang;
            if (Ngay.Thang > 12) {
                Ngay.Thang = 1;
                ++Ngay.Nam;
            }
        }
    }
    if (!Ngay.HopLe()) BaoLoi("Ngay danh gia khong hop le");
    return Ngay;
}

// Giữ nguyên thứ tự và các ngưỡng của evaluateTriageLevel trong patient-class.
int TinhMuc(const VitalSigns& S, int Tuoi) {
    int Tim = S.LayNhipTim(), Ap = S.LayHuyetApTamThu(), Tho = S.LayNhipTho();
    if (S.LayDiemGcs() <= 8 || S.LaySpo2() < 90) return 1;
    if (S.LayDiemGcs() <= 12 || S.LaySpo2() < 95) return 2;
    if (Tuoi < 1) {
        if (Tim < 60 || Tim > 200 || Tho < 20 || Tho > 70 || Ap < 60) return 1;
        if (Tim < 90 || Tim > 170 || Tho < 30 || Tho > 55 || Ap < 70) return 2;
        if (Tim > 160 || Tho > 50) return 3;
    } else if (Tuoi <= 5) {
        int Nguong = 70 + 2 * Tuoi;
        if (Tim < 60 || Tim > 180 || Tho < 12 || Tho > 50 || Ap < Nguong - 10) return 1;
        if (Tim < 80 || Tim > 140 || Tho < 18 || Tho > 35 || Ap < Nguong) return 2;
        if (Tim > 120 || Tho > 30) return 3;
    } else if (Tuoi <= 12) {
        if (Tim < 50 || Tim > 160 || Tho < 10 || Tho > 40 || Ap < 75) return 1;
        if (Tim < 60 || Tim > 120 || Tho < 14 || Tho > 28 || Ap < 85) return 2;
        if (Tim > 100 || Tho > 24) return 3;
    } else if (Tuoi < 65) {
        if (Tho < 10 || Tho >= 35 || Ap < 80 || Tim < 40 || Tim > 150) return 1;
        if (Tho < 12 || Tho >= 25 || Ap < 90 || Ap > 180 || Tim < 50 || Tim > 120) return 2;
        if (Tho > 20 || Ap > 140 || Tim > 100) return 3;
    } else {
        if (Tho < 10 || Tho >= 30 || Ap < 90 || Tim < 45 || Tim > 130) return 1;
        if (Tho < 12 || Tho >= 24 || Ap < 100 || Ap > 170 || Tim < 55 || Tim > 110) return 2;
        if (Tho > 20 || Ap > 140 || Tim > 90) return 3;
    }
    return S.LaySpo2() < 97 ? 4 : 5;
}

bool CoMaBacSi(const Vector<std::string>& DanhSach, const std::string& MaBacSi) {
    for (int I = 0; I < DanhSach.KichThuoc(); ++I)
        if (DanhSach[I] == MaBacSi) return true;
    return false;
}

void KiemTraLichSuDanhGia(const BanChupLanKham& BanChup) {
    std::time_t ThoiDiemTruoc = BanChup.ThoiDiemTiepNhan;
    for (int I = 0; I < BanChup.DanhGia.KichThuoc(); ++I) {
        const TriageAssessment& DanhGia = BanChup.DanhGia[I];
        // Ô mặc định của Vector không phải đánh giá thật và không được nạp.
        if (DanhGia.LayMaDieuDuong().empty() || DanhGia.LayMuc() < 1 ||
            DanhGia.LayMuc() > 5 || DanhGia.LayThoiDiemDanhGia() < ThoiDiemTruoc)
            BaoLoi("Lich su danh gia khong hop le");
        // Tuổi và nguồn tuổi thuộc thời điểm đánh giá, không tính lại từ Patient.
        ThoiDiemTruoc = DanhGia.LayThoiDiemDanhGia();
    }
}

void KiemTraLichSuBacSi(const BanChupLanKham& BanChup) {
    for (int I = 0; I < BanChup.BacSiPhanCong.KichThuoc(); ++I) {
        if (BanChup.BacSiPhanCong[I].empty()) BaoLoi("Ma bac si phan cong bi rong");
        for (int J = 0; J < I; ++J)
            if (BanChup.BacSiPhanCong[I] == BanChup.BacSiPhanCong[J])
                BaoLoi("Ma bac si phan cong bi trung");
    }
    // Bác sĩ tham gia có thể lặp: mỗi phần tử là một lượt khám thực tế.
    for (int I = 0; I < BanChup.BacSiThamGia.KichThuoc(); ++I)
        if (BanChup.BacSiThamGia[I].empty()) BaoLoi("Ma bac si tham gia bi rong");
    if (!BanChup.BacSiThamGia.Rong() && BanChup.DanhGia.Rong())
        BaoLoi("Bac si tham gia truoc khi co danh gia");
}

void KiemTraThamGiaChiTiet(const BanChupLanKham& BanChup) {
    const auto& ChiTiet = BanChup.BacSiThamGiaChiTiet;
    if (ChiTiet.Rong()) return; // Định dạng cũ chỉ lưu mã bác sĩ.
    if (ChiTiet.KichThuoc() != BanChup.BacSiThamGia.KichThuoc())
        BaoLoi("So ban ghi bac si tham gia khong khop");
    std::time_t ThoiDiemTruoc = BanChup.ThoiDiemTiepNhan;
    for (int I = 0; I < ChiTiet.KichThuoc(); ++I) {
        const auto& P = ChiTiet[I];
        if (P.LayMaBacSi() != BanChup.BacSiThamGia[I])
            BaoLoi("Ma bac si tham gia khong khop chi tiet");
        // Dựng lại để loại ô mặc định và kiểm tra vai trò/mã người được thay.
        DoctorParticipation HopLe(P.LayMaBacSi(), P.LayThoiDiemThamGia(),
                                  P.LayVaiTro(), P.LayMaBacSiDuocThay());
        // 0 là thời điểm chưa biết của v1; không bịa giờ khám khi nâng lên v2.
        if (P.LayThoiDiemThamGia() != 0) {
            if (P.LayThoiDiemThamGia() < ThoiDiemTruoc)
                BaoLoi("Thoi diem tham gia khong hop le");
            ThoiDiemTruoc = P.LayThoiDiemThamGia();
        }
        if (P.LayVaiTro() == DoctorRole::ThayThe) {
            bool DaThamGia = false;
            for (int J = 0; J < I; ++J)
                if (ChiTiet[J].LayMaBacSi() == P.LayMaBacSiDuocThay()) DaThamGia = true;
            if (!DaThamGia) BaoLoi("Bac si duoc thay chua tham gia lan kham");
        }
    }
}

void KiemTraLichSuCanLamSang(const BanChupLanKham& BanChup) {
    for (int I = 0; I < BanChup.CanLamSang.KichThuoc(); ++I) {
        const BanGhiCanLamSang& BanGhi = BanChup.CanLamSang[I];
        if (BanGhi.LayChiDinh().empty() || BanGhi.LayMaBacSiChiDinh().empty() ||
            !CoMaBacSi(BanChup.BacSiThamGia, BanGhi.LayMaBacSiChiDinh()) ||
            BanGhi.DaCoKetQua() != !BanGhi.LayKetQua().empty())
            BaoLoi("Lich su can lam sang khong hop le");
        // Muốn có chỉ định tiếp theo thì chỉ định trước đã phải có kết quả.
        if (!BanGhi.DaCoKetQua() && I + 1 < BanChup.CanLamSang.KichThuoc())
            BaoLoi("Chi dinh truoc chua co ket qua");
    }
}

void KiemTraTrangThaiBanChup(const BanChupLanKham& BanChup) {
    const bool DaKham = !BanChup.DanhGia.Rong() && !BanChup.BacSiThamGia.Rong();
    const bool CoCls = !BanChup.CanLamSang.Rong();
    const bool CoKetQuaCuoi = CoCls &&
        BanChup.CanLamSang[BanChup.CanLamSang.KichThuoc() - 1].DaCoKetQua();
    switch (BanChup.TrangThai) {
    case VisitStatus::ChoKhamLanDau:
        if (!BanChup.BacSiThamGia.Rong() || CoCls) SaiTrangThai();
        break;
    case VisitStatus::DangKham:
    case VisitStatus::DaKetThuc:
        if (!DaKham || (CoCls && !CoKetQuaCuoi)) SaiTrangThai();
        break;
    case VisitStatus::ChoCanLamSang:
        // Có thể đã ghi kết quả nhưng chưa thực hiện DanhDauQuayLai.
        if (!DaKham || !CoCls) SaiTrangThai();
        break;
    case VisitStatus::ChoKhamQuayLai:
        if (!DaKham || !CoKetQuaCuoi) SaiTrangThai();
        break;
    case VisitStatus::DaHuy:
        break; // Có thể hủy trước đánh giá, khi khám hoặc khi đang chờ CLS.
    default:
        SaiTrangThai();
    }
}
}

VitalSigns::VitalSigns(int Tim, int Ap, int Tho, double Oxy, int Gcs)
    : NhipTim(Tim), HuyetApTamThu(Ap), NhipTho(Tho), DiemGcs(Gcs), Spo2(Oxy) {
    // Viết dạng phủ định để NaN cũng bị từ chối vì mọi so sánh với NaN đều sai.
    if (Tim < 0 || Tim > 300 || Ap < 0 || Ap > 300 || Tho < 0 || Tho > 120 ||
        !(Oxy >= 0 && Oxy <= 100) || Gcs < 3 || Gcs > 15) BaoLoi("Sinh hieu khong hop le");
}
TriageAssessment::TriageAssessment() : SinhHieu(0,0,0,0,3), Muc(0), Tuoi(0), TuoiUocLuong(false), ThoiDiemDanhGia(0) {}
TriageAssessment::TriageAssessment(const VitalSigns& SinhHieu, int Tuoi, bool TuoiUocLuong,
                                   std::time_t ThoiDiemDanhGia, const std::string& MaDieuDuong)
    : SinhHieu(SinhHieu), Muc(0), Tuoi(Tuoi), TuoiUocLuong(TuoiUocLuong), ThoiDiemDanhGia(ThoiDiemDanhGia), MaDieuDuong(MaDieuDuong) {
    if (Tuoi < 0 || Tuoi > 150 || MaDieuDuong.empty()) BaoLoi("Du lieu danh gia khong hop le");
    Muc = TinhMuc(SinhHieu, Tuoi);
}
BanGhiCanLamSang::BanGhiCanLamSang(const std::string& ChiDinh,
                                   const std::string& MaBacSiChiDinh,
                                   bool CoKetQua, const std::string& KetQua)
    : ChiDinh(ChiDinh), KetQua(KetQua), MaBacSiChiDinh(MaBacSiChiDinh), CoKetQua(CoKetQua) {
    if (ChiDinh.empty() || MaBacSiChiDinh.empty() || CoKetQua != !KetQua.empty())
        BaoLoi("Ban ghi can lam sang khong hop le");
}
void BanGhiCanLamSang::GhiKetQua(const std::string& KetQua) {
    if (CoKetQua) SaiTrangThai();
    if (KetQua.empty()) BaoLoi("Ket qua khong duoc rong");
    this->KetQua = KetQua;
    CoKetQua = true;
}
Visit::Visit(const std::string& MaLanKham, const std::string& MaBenhNhan,
             std::time_t ThoiDiemTiepNhan, const std::string& TrieuChungChinh)
    : MaLanKham(MaLanKham), MaBenhNhan(MaBenhNhan), TrieuChungChinh(TrieuChungChinh),
      ThoiDiemTiepNhan(ThoiDiemTiepNhan), TrangThaiLanKham(VisitStatus::ChoKhamLanDau) {
    if (MaLanKham.empty() || MaBenhNhan.empty()) BaoLoi("Ma lan kham va ma benh nhan la bat buoc");
}
BanChupLanKham Visit::LayBanChup() const {
    BanChupLanKham BanChup;
    BanChup.MaLanKham = MaLanKham;
    BanChup.MaBenhNhan = MaBenhNhan;
    BanChup.TrieuChungChinh = TrieuChungChinh;
    BanChup.ThoiDiemTiepNhan = ThoiDiemTiepNhan;
    BanChup.TrangThai = TrangThaiLanKham;
    BanChup.DanhGia = LichSuDanhGia;
    BanChup.CanLamSang = LichSuCanLamSang;
    BanChup.BacSiPhanCong = DanhSachBacSiDuocPhanCong;
    BanChup.BacSiThamGiaChiTiet = LichSuBacSiThamGiaKham;
    for (int I = 0; I < LichSuBacSiThamGiaKham.KichThuoc(); ++I)
        BanChup.BacSiThamGia.ThemCuoi(LichSuBacSiThamGiaKham[I].LayMaBacSi());
    return BanChup;
}

Visit* Visit::TuBanChup(const BanChupLanKham& BanChup) {
    KiemTraLichSuDanhGia(BanChup);
    KiemTraLichSuBacSi(BanChup);
    KiemTraThamGiaChiTiet(BanChup);
    KiemTraLichSuCanLamSang(BanChup);
    KiemTraTrangThaiBanChup(BanChup);
    Visit* Moi = new Visit(BanChup.MaLanKham, BanChup.MaBenhNhan,
                           BanChup.ThoiDiemTiepNhan, BanChup.TrieuChungChinh);
    try {
        // Chép trực tiếp để giữ tuổi, nguồn tuổi và thứ tự lịch sử nguyên gốc.
        Moi->LichSuDanhGia = BanChup.DanhGia;
        Moi->LichSuCanLamSang = BanChup.CanLamSang;
        Moi->DanhSachBacSiDuocPhanCong = BanChup.BacSiPhanCong;
        if (!BanChup.BacSiThamGiaChiTiet.Rong())
            Moi->LichSuBacSiThamGiaKham = BanChup.BacSiThamGiaChiTiet;
        else {
            // v1 không có giờ/vai trò: 0 và KhamChinh chỉ là giá trị tương thích.
            for (int I = 0; I < BanChup.BacSiThamGia.KichThuoc(); ++I)
                Moi->LichSuBacSiThamGiaKham.ThemCuoi(DoctorParticipation(BanChup.BacSiThamGia[I], 0));
        }
        Moi->TrangThaiLanKham = BanChup.TrangThai;
    } catch (...) {
        delete Moi;
        throw;
    }
    return Moi;
}
void Visit::KiemTraDangMo() const {
    if (TrangThaiLanKham == VisitStatus::DaKetThuc || TrangThaiLanKham == VisitStatus::DaHuy) SaiTrangThai();
}
const TriageAssessment& Visit::LayDanhGiaGanNhat() const {
    if (LichSuDanhGia.Rong()) SaiTrangThai();
    return LichSuDanhGia.Lay(LichSuDanhGia.KichThuoc() - 1);
}
std::string Visit::LayKetQuaCanLamSang() const {
    if (LichSuCanLamSang.Rong()) return "";
    return LichSuCanLamSang.Lay(LichSuCanLamSang.KichThuoc() - 1).LayKetQua();
}
void Visit::DanhGia(const Patient& BenhNhan, const VitalSigns& SinhHieu,
                    std::time_t ThoiDiemDanhGia, const std::string& MaDieuDuong) {
    KiemTraDangMo();
    if (BenhNhan.LayMaDinhDanh() != MaBenhNhan || ThoiDiemDanhGia < ThoiDiemTiepNhan)
        BaoLoi("Thong tin danh gia khong hop le");
    const Date NgayDanhGia = LayNgayVietNam(ThoiDiemDanhGia);
    if (!BenhNhan.CoNgaySinh() && !BenhNhan.CoTuoiUocLuong()) SaiTrangThai();
    bool UocLuong = !BenhNhan.CoNgaySinh();
    int Tuoi;
    if (UocLuong) {
        Tuoi = BenhNhan.LayTuoiUocLuong();
    } else {
        Tuoi = BenhNhan.TinhTuoi(NgayDanhGia);
    }
    LichSuDanhGia.ThemCuoi(TriageAssessment(SinhHieu, Tuoi, UocLuong, ThoiDiemDanhGia, MaDieuDuong));
}
void Visit::PhanCongBacSi(const std::string& MaBacSi) {
    KiemTraDangMo();
    if (MaBacSi.empty()) BaoLoi("Ma bac si khong duoc rong");
    for (int I = 0; I < DanhSachBacSiDuocPhanCong.KichThuoc(); ++I) {
        if (DanhSachBacSiDuocPhanCong.Lay(I) == MaBacSi) return;
    }
    DanhSachBacSiDuocPhanCong.ThemCuoi(MaBacSi);
}
void Visit::GhiNhanBacSiThamGiaKham(const std::string& MaBacSi) {
    GhiNhanBacSiThamGiaKham(DoctorParticipation(MaBacSi, std::time(nullptr)));
}
void Visit::KiemTraThamGia(const DoctorParticipation& P) const {
    if (P.LayMaBacSi().empty() || P.LayThoiDiemThamGia() < ThoiDiemTiepNhan ||
        (!LichSuBacSiThamGiaKham.Rong() && P.LayThoiDiemThamGia() < LichSuBacSiThamGiaKham.Lay(LichSuBacSiThamGiaKham.KichThuoc()-1).LayThoiDiemThamGia()))
        BaoLoi("Thoi diem tham gia khong hop le");
    if (P.LayVaiTro() == DoctorRole::ThayThe && !DaThamGiaKham(P.LayMaBacSiDuocThay()))
        BaoLoi("Bac si duoc thay chua tham gia lan kham");
}
void Visit::GhiNhanBacSiThamGiaKham(const DoctorParticipation& P) {
    if (TrangThaiLanKham != VisitStatus::DangKham) SaiTrangThai();
    KiemTraThamGia(P);
    // Mỗi lời gọi tương ứng một lượt khám thực tế, kể cả khi cùng bác sĩ quay lại.
    LichSuBacSiThamGiaKham.ThemCuoi(P);
}
bool Visit::DaThamGiaKham(const std::string& MaBacSi) const {
    for (int I = 0; I < LichSuBacSiThamGiaKham.KichThuoc(); ++I) {
        if (LichSuBacSiThamGiaKham.Lay(I).LayMaBacSi() == MaBacSi) return true;
    }
    return false;
}
void Visit::BatDauKham(const std::string& MaBacSi) {
    BatDauKham(DoctorParticipation(MaBacSi, std::time(nullptr)));
}
void Visit::BatDauKham(const DoctorParticipation& P) {
    if (LichSuDanhGia.Rong()) SaiTrangThai();
    if (TrangThaiLanKham != VisitStatus::ChoKhamLanDau && TrangThaiLanKham != VisitStatus::ChoKhamQuayLai) SaiTrangThai();
    KiemTraThamGia(P);
    // Ghi nhận bác sĩ trước; nếu Vector cấp phát thất bại thì trạng thái vẫn là chờ khám.
    LichSuBacSiThamGiaKham.ThemCuoi(P);
    TrangThaiLanKham = VisitStatus::DangKham;
}
void Visit::ChuyenDiCanLamSang(const std::string& ChiDinh, const std::string& MaBacSiChiDinh) {
    if (TrangThaiLanKham != VisitStatus::DangKham) SaiTrangThai();
    if (ChiDinh.empty() || MaBacSiChiDinh.empty()) BaoLoi("Chi dinh va ma bac si khong duoc rong");
    if (!DaThamGiaKham(MaBacSiChiDinh)) SaiTrangThai();
    // Lưu mã ngay khi chỉ định; thay đổi phân công sau đó không sửa bản ghi cũ.
    LichSuCanLamSang.ThemCuoi(BanGhiCanLamSang(ChiDinh, MaBacSiChiDinh));
    TrangThaiLanKham = VisitStatus::ChoCanLamSang;
}
void Visit::GhiNhanKetQua(const std::string& KetQua) {
    if (TrangThaiLanKham != VisitStatus::ChoCanLamSang) SaiTrangThai();
    LichSuCanLamSang.Lay(LichSuCanLamSang.KichThuoc() - 1).GhiKetQua(KetQua);
}
void Visit::DanhDauQuayLai() {
    if (TrangThaiLanKham != VisitStatus::ChoCanLamSang ||
        !LichSuCanLamSang.Lay(LichSuCanLamSang.KichThuoc() - 1).DaCoKetQua()) SaiTrangThai();
    TrangThaiLanKham = VisitStatus::ChoKhamQuayLai;
}
void Visit::KetThucLanKham() {
    if (TrangThaiLanKham != VisitStatus::DangKham || LichSuBacSiThamGiaKham.Rong()) SaiTrangThai();
    TrangThaiLanKham = VisitStatus::DaKetThuc;
}
void Visit::HuyLanKham() { KiemTraDangMo(); TrangThaiLanKham = VisitStatus::DaHuy; }
