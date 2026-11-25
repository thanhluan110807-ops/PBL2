#include "service/VisitSearchService.h"

#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Visit.h"
#include "repository/PatientRegistry.h"

VisitSearchService::VisitSearchService(const PatientRegistry& Registry) : Registry(Registry) {}

Vector<KetQuaTraCuuLanKham> VisitSearchService::TimTheoBenhNhan(
    const std::string& TenCanTim, const Date* NgayTiepNhan,
    const std::string& MaLanKham) const {

    return Registry.TraCuuLanKham(TenCanTim, NgayTiepNhan, MaLanKham);
}

Vector<KetQuaTraCuuLanKham> VisitSearchService::TimTheoBacSi(const std::string& MaBacSi) const {
    Vector<KetQuaTraCuuLanKham> KetQua;
    if (MaBacSi.empty()) return KetQua;
    const auto DanhSach = TimTheoBenhNhan();
    for (int Dong = 0; Dong < DanhSach.KichThuoc(); ++Dong) {
        const auto& HoSo = DanhSach[Dong];
        const auto& LanKham = *HoSo.LanKham;
        for (int I = 0; I < LanKham.SoLanBacSiThamGiaKham(); ++I) {
            if (LanKham.LayBacSiThamGiaKhamTai(I) != MaBacSi) continue;
            KetQua.ThemCuoi(HoSo);
            break;
        }
    }
    return KetQua;
}

Vector<KetQuaTraCuuDanhGia> VisitSearchService::TimDanhGiaTheoDieuDuong(const std::string& MaDieuDuong) const {
    Vector<KetQuaTraCuuDanhGia> KetQua;
    if (MaDieuDuong.empty()) return KetQua;
    const auto DanhSach = TimTheoBenhNhan();
    for (int Dong = 0; Dong < DanhSach.KichThuoc(); ++Dong) {
        const auto& HoSo = DanhSach[Dong];
        const auto& LanKham = *HoSo.LanKham;
        for (int I = LanKham.SoDanhGia() - 1; I >= 0; --I) {
            const auto& DanhGia = LanKham.LayDanhGiaTai(I);
            if (DanhGia.LayMaDieuDuong() == MaDieuDuong) KetQua.ThemCuoi({HoSo, &DanhGia});
        }
    }
    return KetQua;
}
