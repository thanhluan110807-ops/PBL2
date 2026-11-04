// Phần B — Chức năng chính: Quản lý điều dưỡng và thời gian trực sàng lọc.
#pragma once
#include "model/NhanVienYTe.h"
#include <ctime>

class DieuDuong : public NhanVienYTe {
    bool DangTrucSangLoc = false;
    double GioSangLocLienTuc = 0;
    bool TheoDongHo = false;
    std::time_t BatDauSangLoc = 0;
    std::time_t MocDongHo = 0;
    void HoanDoiDuLieu(NhanVienYTe& Khac) override;
public:
    DieuDuong(const std::string& Ma, const std::string& Ten, CapBac Bac,
              int KinhNghiem, double DiemNen = 0, const CaTrucMongMuon& MongMuon = {});
    StaffType LayLoaiNhanVien() const override { return StaffType::DieuDuong; }
    bool CoTheLamTruongCa() const override { return false; }
    NhanVienYTe* SaoChep() const override { return new DieuDuong(*this); }
    bool DangSangLoc() const { return DangTrucSangLoc; }
    double LayGioSangLocLienTuc() const { return GioSangLocLienTuc; }
    bool DangTheoDongHo() const { return TheoDongHo; }
    std::time_t LayBatDauSangLoc() const { return BatDauSangLoc; }
    std::time_t LayMocDongHo() const { return MocDongHo; }
    // Các API cũ dành cho mô phỏng cộng giờ thủ công.
    void BatDauTrucSangLoc();
    void KetThucTrucSangLoc();
    void CongDonGioSangLoc(double SoGio);
    bool CanXoayTuaSangLoc() const;
    // Truyền thời điểm vào để dễ kiểm thử; đơn vị time_t là thời điểm hệ thống.
    void BatDauTrucSangLoc(std::time_t ThoiDiem);
    void CapNhatThoiGianSangLoc(std::time_t ThoiDiem);
    double TinhThoiGianSangLocLienTuc(std::time_t ThoiDiem) const;
    bool CanXoayTuaSangLoc(std::time_t ThoiDiem) const;
    void KetThucTrucSangLoc(std::time_t ThoiDiem);
    void KhoiPhucSangLoc(bool Dang, double SoGio, bool TheoDongHo,
                        std::time_t BatDau, std::time_t MocCuoi);
};
