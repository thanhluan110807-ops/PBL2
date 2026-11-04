#include "service/VisitSearchService.h"

#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "repository/PatientRegistry.h"

VisitSearchService::VisitSearchService(const PatientRegistry& Registry) : Registry(Registry) {}

Vector<KetQuaTraCuuLanKham> VisitSearchService::TimTheoBenhNhan(
    const std::string& TenCanTim, const Date* NgayTiepNhan,
    const std::string& MaLanKham) const {
    // Registry đã có quy tắc lọc và sắp xếp; không lặp lại ở tầng service.
    return Registry.TraCuuLanKham(TenCanTim, NgayTiepNhan, MaLanKham);
}
