#pragma once

#include "model/Person.h"

enum class BloodGroup { A, B, AB, O, ChuaXacDinh };
enum class IdentityStatus { ChuaXacDinh, DaXacDinh };

// Hồ sơ lâu dài; sinh hiệu và đánh giá thuộc từng Visit.
class Patient : public Person {
private:
    std::string SoCanCuoc;
    BloodGroup NhomMau = BloodGroup::ChuaXacDinh;
    std::string ThongTinDiUng;
    std::string TienSuBenh;
    IdentityStatus TrangThaiDanhTinh = IdentityStatus::ChuaXacDinh;
    int TuoiUocLuong = 0;
    bool DaCoTuoiUocLuong = false;

public:
    explicit Patient(const std::string& MaDinhDanh, const std::string& HoTen = "",
                     Gender GioiTinh = Gender::ChuaXacDinh);
    Patient(const std::string& MaDinhDanh, const std::string& HoTen,
            Gender GioiTinh, const Date& NgaySinh);

    bool LaBenhNhanVoDanh() const;
    IdentityStatus LayTrangThaiDanhTinh() const;
    const std::string& LaySoCanCuoc() const;
    BloodGroup LayNhomMau() const;
    const std::string& LayThongTinDiUng() const;
    const std::string& LayTienSuBenh() const;
    bool CoTuoiUocLuong() const;
    int LayTuoiUocLuong() const;
    void DatTuoiUocLuong(int Tuoi);
    void XoaTuoiUocLuong();
    void CapNhatNhomMau(BloodGroup NhomMau);
    void CapNhatThongTinDiUng(const std::string& ThongTin);
    void CapNhatTienSuBenh(const std::string& ThongTin);
    void BoSungDanhTinh(const std::string& SoCanCuoc, const std::string& HoTen,
                       Gender GioiTinh, const Date& NgaySinh);
};
