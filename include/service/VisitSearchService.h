#pragma once

#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Visit.h"
#include "repository/PatientRegistry.h"

// Một dòng sinh hiệu mượn dữ liệu; thêm đánh giá vào Visit có thể làm con trỏ cũ hết hạn.
struct KetQuaTraCuuDanhGia {
    KetQuaTraCuuLanKham HoSo;
    const TriageAssessment* DanhGia = nullptr;
};

// Tra cứu lịch sử khám và đánh giá; Registry giữ quyền sở hữu dữ liệu.
class VisitSearchService {
    const PatientRegistry& Registry;

public:
    // Registry phải sống lâu hơn service và các con trỏ trong kết quả tra cứu.
    explicit VisitSearchService(const PatientRegistry& Registry);

    // Điều kiện trống được bỏ qua; ngày là ngày tiếp nhận theo giờ Việt Nam.
    Vector<KetQuaTraCuuLanKham> TimTheoBenhNhan(
        const std::string& TenCanTim = "", const Date* NgayTiepNhan = nullptr,
        const std::string& MaLanKham = "") const;
    // Chỉ lấy bác sĩ thực sự tham gia, mỗi lần khám xuất hiện một lần.
    Vector<KetQuaTraCuuLanKham> TimTheoBacSi(const std::string& MaBacSi) const;
    // Mỗi kết quả là một lần đánh giá có đúng mã điều dưỡng. Mã rỗng trả rỗng.
    Vector<KetQuaTraCuuDanhGia> TimDanhGiaTheoDieuDuong(const std::string& MaDieuDuong) const;
};
