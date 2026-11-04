# include/model

Chức năng chính: mô tả dữ liệu nghiệp vụ.

Các file thuộc phạm vi phần B đã triển khai:

| File | Chức năng chính |
|---|---|
| `CapBac.h` | Định nghĩa cấp bậc và loại nhân viên. |
| `CaTrucMongMuon.h` | Lưu và kiểm tra nguyện vọng ca trực. |
| `NhanVienYTe.h` | Lớp trừu tượng chứa thông tin và trạng thái nhân viên y tế. |
| `BacSi.h` | Quản lý chuyên khoa, nội trú và khả năng làm trưởng ca. |
| `DieuDuong.h` | Quản lý điều dưỡng và thời gian trực sàng lọc. |
| `Shift.h` | Lưu ca trực và kiểm tra thời gian, ca đêm, cuối tuần. |
| `ShiftAssignment.h` | Quản lý mã, vai trò và trạng thái một phân công. |
| `DoctorParticipation.h` | Ghi bác sĩ thực sự tham gia khám, thời điểm và vai trò. |

Các lớp dùng chung như `Person`, `Patient`, `Visit` là điểm phối hợp; bảng này không gán toàn bộ thư mục cho thành viên B. Xem [bản đồ phần B](../../docs/PHAN-B.md).
