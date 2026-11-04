// Phần B — Chức năng chính: Quản lý chuyên khoa, nội trú và khả năng làm trưởng ca.
#pragma once
#include "model/NhanVienYTe.h"

class BacSi : public NhanVienYTe {
    std::string ChuyenKhoa;
    bool LaNoiTru;
    void HoanDoiDuLieu(NhanVienYTe& Khac) override;
public:
    BacSi(const std::string& Ma, const std::string& Ten, CapBac Bac,
          int KinhNghiem, const std::string& ChuyenKhoa,
          double DiemNen = 0, const CaTrucMongMuon& MongMuon = {});
    // Bản có cờ nội trú bảo toàn thông tin riêng trong Staff.cpp gốc.
    BacSi(const std::string& Ma, const std::string& Ten, CapBac Bac,
          int KinhNghiem, const std::string& ChuyenKhoa, bool NoiTru,
          double DiemNen, const CaTrucMongMuon& MongMuon = {});
    StaffType LayLoaiNhanVien() const override { return StaffType::BacSi; }
    bool CoTheLamTruongCa() const override;
    NhanVienYTe* SaoChep() const override { return new BacSi(*this); }
    const std::string& LayChuyenKhoa() const { return ChuyenKhoa; }
    void DatChuyenKhoa(const std::string& Ten);
    bool LaBacSiNoiTru() const;
};
