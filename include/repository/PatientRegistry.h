#pragma once

#include <ctime>
#include <string>

#include "dsa/HashTable.h"
#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"

// Một dòng tra cứu là một lần khám kèm hồ sơ người bệnh để GUI hiển thị.
// Chỉ mượn con trỏ chỉ đọc; Registry vẫn sở hữu cả hai đối tượng.
struct KetQuaTraCuuLanKham {
    const Patient* BenhNhan = nullptr;
    const Visit* LanKham = nullptr;
};

// Sở hữu hồ sơ trong bộ nhớ; không làm giao diện, tự nhận diện hay gộp người bệnh.
class PatientRegistry {
private:
    HashTable<std::string, Patient*> DanhSachBenhNhan;
    HashTable<std::string, Visit*> DanhSachLanKham;

    Patient* LayBenhNhanBatBuoc(const std::string& MaBenhNhan) const;
    void KiemTraCanCuoc(const std::string& SoCanCuoc, const Patient* BoQua = nullptr) const;
    bool CoLanKhamDangMo(const std::string& MaBenhNhan) const;

public:
    PatientRegistry() = default;
    ~PatientRegistry();
    // Không sao chép/chuyển chủ sở hữu để tránh hai Registry cùng delete một hồ sơ.
    PatientRegistry(const PatientRegistry&) = delete;
    PatientRegistry& operator=(const PatientRegistry&) = delete;
    PatientRegistry(PatientRegistry&&) = delete;
    PatientRegistry& operator=(PatientRegistry&&) = delete;

    // Tạo bản sao do Registry sở hữu; không nhận quyền sở hữu đối tượng đầu vào.
    const Patient* ThemBenhNhan(const Patient& HoSo);
    const Patient* ThemBenhNhan(const std::string& MaBenhNhan, const std::string& HoTen = "",
                               Gender GioiTinh = Gender::ChuaXacDinh,
                               const std::string& SoCanCuoc = "");
    const Patient* ThemBenhNhan(const std::string& MaBenhNhan, const std::string& HoTen,
                               Gender GioiTinh, const Date& NgaySinh,
                               const std::string& SoCanCuoc = "");

    // Các con trỏ Patient chỉ đọc, sống đến khi Registry bị hủy; không tự delete.
    const Patient* TimBenhNhan(const std::string& MaBenhNhan) const;
    const Patient* TimBenhNhanTheoCanCuoc(const std::string& SoCanCuoc) const;
    const Patient* TimBenhNhanTheoMaLanKham(const std::string& MaLanKham) const;
    // Tìm một phần tên, giữ dấu, không phân biệt hoa/thường ASCII và tiếng Việt.
    // Chuỗi tìm rỗng trả danh sách rỗng; kết quả không cam kết thứ tự.
    Vector<const Patient*> TimBenhNhanTheoTen(const std::string& TenCanTim) const;
    // Mượn tất cả hồ sơ, gồm cả người chưa khám; không cam kết thứ tự.
    Vector<const Patient*> LayDanhSachBenhNhan() const;
    // Kết hợp các điều kiện bằng AND; tên tìm một phần, mã lần khám khớp chính xác.
    // Chuỗi rỗng/nullptr bỏ lọc; ngày là ngày tiếp nhận Việt Nam (UTC+7).
    // Mới nhất trước, cùng thời điểm thì mã lần khám tăng dần; gồm cả đã đóng/hủy.
    // Không có Visit thì không có dòng kết quả; không tự chọn người trùng tên.
    Vector<KetQuaTraCuuLanKham> TraCuuLanKham(
        const std::string& TenCanTim = "", const Date* NgayTiepNhan = nullptr,
        const std::string& MaLanKham = "") const;
    void BoSungDanhTinh(const std::string& MaBenhNhan, const std::string& SoCanCuoc,
                       const std::string& HoTen, Gender GioiTinh, const Date& NgaySinh);
    // Tiếp nhận lại: bổ sung mục còn thiếu, kiểm tra CCCD và chặn khi còn lượt đang mở.
    void BoSungThongTinConThieu(const std::string& MaBenhNhan, const Patient& ThongTin);

    // Từ chối mã trùng, Patient không tồn tại hoặc Patient còn Visit chưa đóng.
    Visit* ThemLanKham(const std::string& MaLanKham, const std::string& MaBenhNhan,
                      std::time_t ThoiDiemTiepNhan, const std::string& TrieuChungChinh);
    // Chỉ nạp vào Registry chưa có Visit; lỗi không thay đổi Registry và Patient.
    void KhoiPhucLanKham(const Vector<BanChupLanKham>& DanhSach);
    Visit* TimLanKham(const std::string& MaLanKham);
    const Visit* TimLanKham(const std::string& MaLanKham) const;
    // Chỉ mượn Visit, không xóa đối tượng khi Vector bị hủy; không cam kết thứ tự.
    // Patient tồn tại chưa khám trả rỗng, mã Patient không tồn tại báo NgoaiPhamVi.
    Vector<Visit*> LayLichSuLanKham(const std::string& MaBenhNhan);
    Vector<const Visit*> LayLichSuLanKham(const std::string& MaBenhNhan) const;
    int SoBenhNhan() const;
    int SoLanKham() const;
};
