#pragma once

#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Visit.h"
#include "repository/PatientRegistry.h"


struct KetQuaTraCuuDanhGia {
    KetQuaTraCuuLanKham HoSo;
    const TriageAssessment* DanhGia = nullptr;
};


class VisitSearchService {
    const PatientRegistry& Registry;

public:

    explicit VisitSearchService(const PatientRegistry& Registry);


    Vector<KetQuaTraCuuLanKham> TimTheoBenhNhan(
        const std::string& TenCanTim = "", const Date* NgayTiepNhan = nullptr,
        const std::string& MaLanKham = "") const;

    Vector<KetQuaTraCuuLanKham> TimTheoBacSi(const std::string& MaBacSi) const;

    Vector<KetQuaTraCuuDanhGia> TimDanhGiaTheoDieuDuong(const std::string& MaDieuDuong) const;
};
