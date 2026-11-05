# TriageService Implementation Plan

> Thực hiện theo TDD trong workspace hiện có; không commit hoặc push.

**Mục tiêu:** Điều phối Visit, đánh giá và hai hàng khám với luật luân phiên 1:1 đã duyệt.
**Kiến trúc:** Registry sở hữu Patient/Visit; service mượn Registry và quản lý hai heap của lượt chờ, lưu mức và thứ tự vào hàng. Không tính áp lực hoặc lưu file.
**Công nghệ:** C++17, CMake, DSA tự viết, assert; lõi độc lập Qt.

## Quy tắc

- Mức 1 ưu tiên cao nhất; trong cùng hàng và cùng mức ưu tiên lượt vào trước.
- Khi hai đầu hàng cùng mức, lần đầu chọn quay lại, sau đó luân phiên lần đầu/quay lại trên toàn hệ thống. Chỉ lần trùng thực sự làm đổi lượt; hai mức khác nhau hoặc một hàng rỗng không thay đổi lượt.
- Mỗi Visit có tối đa một lượt chờ; đánh giá lại đổi vị trí nhưng giữ thứ tự vào hàng. Lượt chưa đánh giá chưa được gọi khám.
- Bác sĩ bắt đầu khám theo API Visit; xét nghiệm, kết quả, quay lại, kết thúc, hủy giữ các điều kiện model. Lỗi không làm mất lượt chờ hoặc thay đổi lượt ưu tiên.
- Chưa triển khai xác nhận mức thủ công, QueuePressureCalculator, lưu file, khôi phục và nối demo.

## Công việc

1. Tạo `tests/service/test_triage_service.cpp`: tái hiện quy tắc cùng mức, xen kẽ mức khác, một hàng rỗng, FIFO và trường hợp gọi thất bại; xem test thất bại vì service chưa có.
2. Tạo `include/service/TriageService.h`, `src/service/TriageService.cpp`, nối `CMakeLists.txt`: quản lý hàng chờ và trạng thái Visit, kiểm soát quyền sở hữu và an toàn khi cấp phát lỗi. Chạy test mới.
3. Thêm kiểm thử cho tái đánh giá, chuyển xét nghiệm/quay lại nhiều vòng, kết thúc, hủy, ID sai, lượt chưa đánh giá và không tạo trùng. Hoàn thiện API cho các hành vi này.
4. Chạy build và toàn bộ CTest trong thư mục mới ngoài các build hiện có; rà diff và ghi README service về API, giới hạn, thời gian và quyền sở hữu.

## Ghi nhận thực hiện

- Dùng `TiepNhan`, `DanhGia`, `GoiTiep`, `LayDanhSachCho`, `ChuyenCanLamSang`, `QuayLai`, `KetThuc`, `Huy`; xem header để biết chữ ký cụ thể.
- Chỉ đổi lượt sau khi `BatDauKham` thành công. Việc xem hàng mô phỏng trên bản sao, không đổi hàng hoặc lượt thật.
- Rà soát độc lập chỉ ra thời gian lùi và nguy cơ sửa Visit bên ngoài service: thêm kiểm tra thời gian và phát hiện sai lệch mức/trạng thái; chưa tự phục hồi khi bị sửa ngoài service.
- Thêm test lỗi cấp phát riêng và tài liệu về các giới hạn phục hồi, tuổi, thời gian và giao diện.
