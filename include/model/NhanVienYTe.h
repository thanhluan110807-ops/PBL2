#pragma once
#include "model/Person.h"
#include "model/CapBac.h"
#include "model/CaTrucMongMuon.h"

// Dữ liệu phục hồi từ file; không phải một loại nhân viên mới.
struct TrangThaiNhanVien {
    double DiemMetMoi = 0;
    double GioLamTuan = 0;
    int SoCaDemThang = 0;
    int SoCaCuoiTuanThang = 0;
    bool VuaTrucDem = false;
};

// Dữ liệu nhân viên chung; Person đã giữ mã và họ tên nên không khai báo lặp.
class NhanVienYTe : public Person {
    CapBac Bac;
    int KinhNghiem;
    double DiemNen;
    double DiemMetMoi;
    double GioLamTuan = 0;
    int SoCaDemThang = 0;
    int SoCaCuoiTuanThang = 0;
    bool VuaTrucDem = false;
    CaTrucMongMuon MongMuon;
protected:
    void HoanDoiThongTinChung(NhanVienYTe& Khac) noexcept;
    virtual void HoanDoiDuLieu(NhanVienYTe& Khac) = 0;
    NhanVienYTe(const std::string& Ma, const std::string& Ten, CapBac Bac,
               int KinhNghiem, double DiemNen, const CaTrucMongMuon& MongMuon);
public:
    // Không tự cấp phát bộ nhớ: compiler sao chép các trường giá trị đúng cách.
    NhanVienYTe(const NhanVienYTe&) = default;
    virtual ~NhanVienYTe() = default;
    // Mã Person bất biến: cập nhật bằng các hàm nghiệp vụ, không gán đè danh tính.
    NhanVienYTe& operator=(const NhanVienYTe&) = delete;
    virtual StaffType LayLoaiNhanVien() const = 0;
    virtual bool CoTheLamTruongCa() const = 0;
    virtual NhanVienYTe* SaoChep() const = 0;
    CapBac LayCapBac() const { return Bac; }
    int LaySoNamKinhNghiem() const { return KinhNghiem; }
    double LayDiemMetMoiNen() const { return DiemNen; }
    double LayDiemMetMoi() const { return DiemMetMoi; }
    double LayGioLamTuan() const { return GioLamTuan; }
    int LaySoCaDemThang() const { return SoCaDemThang; }
    int LaySoCaCuoiTuanThang() const { return SoCaCuoiTuanThang; }
    bool DaVuaTrucDem() const { return VuaTrucDem; }
    const CaTrucMongMuon& LayCaMongMuon() const { return MongMuon; }
    void DatHoTen(const std::string& Ten);
    void DatCapBac(CapBac BacMoi);
    void DatSoNamKinhNghiem(int SoNam);
    void DatCaMongMuon(const CaTrucMongMuon& Ca);
    void CongDonGioLam(double SoGio);
    void GhiNhanCaTruc(bool LaCaDem, bool LaCuoiTuan);
    void DatLaiTuanMoi();
    bool CanNghiNgoiBatBuoc() const;
    bool VuotNguongMetMoi(double Nguong) const;
    // Chỉ nhận điểm do service tính; model không chứa công thức chính sách.
    void GhiNhanDiemMetMoi(double Diem);
    void CapNhatTu(const NhanVienYTe& Nguon);
    void KhoiPhucTrangThai(const TrangThaiNhanVien& TrangThai);
};
