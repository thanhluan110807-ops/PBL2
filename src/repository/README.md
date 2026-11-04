# src/repository

Chức năng chính: quản lý dữ liệu trong RAM và đọc/ghi file.

Các file thuộc phạm vi phần B đã triển khai:

| File | Chức năng chính |
|---|---|
| `StaffRegistry.cpp` | Quản lý nhân viên trong RAM, tra cứu theo mã và tên bác sĩ. |
| `ScheduleRegistry.cpp` | Quản lý ca và phân công trong RAM, kiểm tra liên kết và trạng thái. |
| `StaffFileManager.cpp` | Đọc/ghi file nhân viên và phục hồi đúng lớp BacSi/DieuDuong. |
| `ScheduleFileManager.cpp` | Đọc/ghi file ca trực và phân công, giữ lịch sử trạng thái. |
| `AdminFileManager.cpp` | Đọc/ghi snapshot chung nhân sự, lịch và dự phòng cho GUI. |

Các lớp dùng chung như `Person`, `Patient`, `Visit` là điểm phối hợp; bảng này không gán toàn bộ thư mục cho thành viên B. Xem [bản đồ phần B](../../docs/member-b/BAN-DO-FILE-PHAN-B.md).
