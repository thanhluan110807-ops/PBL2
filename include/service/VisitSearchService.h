#pragma once

#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "repository/PatientRegistry.h"

// Tra cứu các lần khám theo thông tin bệnh nhân; Registry giữ quyền sở hữu dữ liệu.
class VisitSearchService {
    const PatientRegistry& Registry;

public:
    // Registry phải sống lâu hơn service và các con trỏ trong kết quả tra cứu.
    explicit VisitSearchService(const PatientRegistry& Registry);

    // Điều kiện trống được bỏ qua; ngày là ngày tiếp nhận theo giờ Việt Nam.
    Vector<KetQuaTraCuuLanKham> TimTheoBenhNhan(
        const std::string& TenCanTim = "", const Date* NgayTiepNhan = nullptr,
        const std::string& MaLanKham = "") const;
};
