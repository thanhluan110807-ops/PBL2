#pragma once

#include <ctime>
#include <string>
#include "dsa/Vector.h"
#include "model/Patient.h"
#include "model/DoctorParticipation.h"

// Một bộ sinh hiệu hợp lệ, lưu giá trị gốc để đánh giá và xem lại.
class VitalSigns {
    int NhipTim, HuyetApTamThu, NhipTho, DiemGcs;
    double Spo2;
public:
    VitalSigns(int NhipTim, int HuyetApTamThu, int NhipTho, double Spo2, int DiemGcs);
    int LayNhipTim() const { return NhipTim; }
    int LayHuyetApTamThu() const { return HuyetApTamThu; }
    int LayNhipTho() const { return NhipTho; }
    double LaySpo2() const { return Spo2; }
    int LayDiemGcs() const { return DiemGcs; }
};

enum class VisitStatus { ChoKhamLanDau, DangKham, ChoCanLamSang, ChoKhamQuayLai, DaKetThuc, DaHuy };

// Vector cần kiểu có constructor mặc định; bản mặc định chỉ dùng cho ô trống.
class TriageAssessment {
    VitalSigns SinhHieu;
    int Muc, Tuoi;
    bool TuoiUocLuong;
    std::time_t ThoiDiemDanhGia;
    std::string MaDieuDuong;
public:
    TriageAssessment();
    TriageAssessment(const VitalSigns& SinhHieu, int Tuoi, bool TuoiUocLuong,
                     std::time_t ThoiDiemDanhGia, const std::string& MaDieuDuong);
    const VitalSigns& LaySinhHieu() const { return SinhHieu; }
    int LayMuc() const { return Muc; }
    int LayTuoi() const { return Tuoi; }
    bool LaTuoiUocLuong() const { return TuoiUocLuong; }
    std::time_t LayThoiDiemDanhGia() const { return ThoiDiemDanhGia; }
    const std::string& LayMaDieuDuong() const { return MaDieuDuong; }
};

// Một chỉ định và kết quả trong cùng lần khám; Vector cần constructor mặc định.
class BanGhiCanLamSang {
    std::string ChiDinh, KetQua, MaBacSiChiDinh;
    bool CoKetQua = false;
    friend class Visit;
    void GhiKetQua(const std::string& KetQua);
public:
    BanGhiCanLamSang() = default;
    BanGhiCanLamSang(const std::string& ChiDinh, const std::string& MaBacSiChiDinh)
        : ChiDinh(ChiDinh), MaBacSiChiDinh(MaBacSiChiDinh) {}
    // Dựng bản ghi từ file, kiểm tra chỉ định và cờ kết quả khớp nội dung.
    BanGhiCanLamSang(const std::string& ChiDinh, const std::string& MaBacSiChiDinh,
                    bool CoKetQua, const std::string& KetQua);
    const std::string& LayChiDinh() const { return ChiDinh; }
    const std::string& LayKetQua() const { return KetQua; }
    const std::string& LayMaBacSiChiDinh() const { return MaBacSiChiDinh; }
    bool DaCoKetQua() const { return CoKetQua; }
};

// Dữ liệu sao chép độc lập dùng để lưu/khôi phục, không giữ con trỏ Patient.
struct BanChupLanKham {
    std::string MaLanKham, MaBenhNhan, TrieuChungChinh;
    std::time_t ThoiDiemTiepNhan = 0;
    VisitStatus TrangThai = VisitStatus::ChoKhamLanDau;
    Vector<TriageAssessment> DanhGia;
    Vector<BanGhiCanLamSang> CanLamSang;
    Vector<std::string> BacSiPhanCong, BacSiThamGia;
    // Giữ danh sách mã cho bên gọi cũ; lịch sử mới lưu đầy đủ sự kiện khám.
    // Nếu chi tiết rỗng, đây là bản cũ chỉ biết mã, thời điểm được khôi phục là 0.
    Vector<DoctorParticipation> BacSiThamGiaChiTiet;
};

class Visit {
    const std::string MaLanKham, MaBenhNhan;
    std::string TrieuChungChinh;
    std::time_t ThoiDiemTiepNhan;
    VisitStatus TrangThaiLanKham;
    Vector<TriageAssessment> LichSuDanhGia;
    Vector<BanGhiCanLamSang> LichSuCanLamSang;
    Vector<std::string> DanhSachBacSiDuocPhanCong;
    Vector<DoctorParticipation> LichSuBacSiThamGiaKham;
    bool DaThamGiaKham(const std::string& MaBacSi) const;
    void KiemTraDangMo() const;
public:
    Visit(const std::string& MaLanKham, const std::string& MaBenhNhan,
          std::time_t ThoiDiemTiepNhan, const std::string& TrieuChungChinh);
    // Có thể chép lịch sử để xem, nhưng không được move lấy mất dữ liệu
    // của một Visit đang được Registry giữ và cho thành phần khác mượn.
    Visit(const Visit&) = default;
    Visit(Visit&&) = delete;
    BanChupLanKham LayBanChup() const;
    // Trả đối tượng mới do bên gọi sở hữu; chỉ công bố khi toàn bộ lịch sử hợp lệ.
    static Visit* TuBanChup(const BanChupLanKham& BanChup);
    const std::string& LayMaLanKham() const { return MaLanKham; }
    const std::string& LayMaBenhNhan() const { return MaBenhNhan; }
    const std::string& LayTrieuChungChinh() const { return TrieuChungChinh; }
    std::string LayKetQuaCanLamSang() const;
    std::time_t LayThoiDiemTiepNhan() const { return ThoiDiemTiepNhan; }
    VisitStatus LayTrangThai() const { return TrangThaiLanKham; }
    int SoDanhGia() const { return LichSuDanhGia.KichThuoc(); }
    const TriageAssessment& LayDanhGiaTai(int ChiSo) const { return LichSuDanhGia.Lay(ChiSo); }
    const TriageAssessment& LayDanhGiaGanNhat() const;
    int SoLanCanLamSang() const { return LichSuCanLamSang.KichThuoc(); }
    const BanGhiCanLamSang& LayCanLamSangTai(int ChiSo) const { return LichSuCanLamSang.Lay(ChiSo); }
    int SoBacSiDuocPhanCong() const { return DanhSachBacSiDuocPhanCong.KichThuoc(); }
    const std::string& LayBacSiDuocPhanCongTai(int ChiSo) const { return DanhSachBacSiDuocPhanCong.Lay(ChiSo); }
    int SoLanBacSiThamGiaKham() const { return LichSuBacSiThamGiaKham.KichThuoc(); }
    const std::string& LayBacSiThamGiaKhamTai(int ChiSo) const { return LichSuBacSiThamGiaKham.Lay(ChiSo).LayMaBacSi(); }
    // Tự xác định ngày Việt Nam (UTC+7) từ thời điểm đánh giá để tính tuổi.
    void DanhGia(const Patient& BenhNhan, const VitalSigns& SinhHieu,
                 std::time_t ThoiDiemDanhGia, const std::string& MaDieuDuong);
    void PhanCongBacSi(const std::string& MaBacSi);
    void GhiNhanBacSiThamGiaKham(const std::string& MaBacSi);
    void BatDauKham(const std::string& MaBacSi);
    const DoctorParticipation& LayThamGiaTai(int I) const { return LichSuBacSiThamGiaKham.Lay(I); }
    void GhiNhanBacSiThamGiaKham(const DoctorParticipation& ThamGia);
    void BatDauKham(const DoctorParticipation& ThamGia);
    void KiemTraThamGia(const DoctorParticipation& ThamGia) const;
    void ChuyenDiCanLamSang(const std::string& ChiDinh, const std::string& MaBacSiChiDinh);
    void GhiNhanKetQua(const std::string& KetQua);
    void DanhDauQuayLai();
    void KetThucLanKham();
    void HuyLanKham();
};
