# Bản đồ file phần B

Giữ nguyên tên class và cấu trúc dự án; chú thích đầu file ghi **Phần B — Chức năng chính** để nhận diện. File `.h` khai báo cách sử dụng; `.cpp` cài đặt cách xử lý.

| Thư mục | Chức năng chính |
|---|---|
| `include/model/`, `src/model/` | Mô hình nhân viên, ca, phân công; chi tiết xem README của thư mục |
| `include/repository/`, `src/repository/` | Registry trong RAM và FileManager đọc/ghi dữ liệu |
| `include/service/`, `src/service/` | Xếp lịch, tìm người thay và tính mệt mỏi |
| `include/dsa/` | DSA tự viết dùng chung; B phụ trách HashTable/LinkedList |
| `demo/qt-triage/ui/AdminPanel.*` | GUI nhân sự/lịch và thao tác nhập/xuất file |
| `demo/qt-triage/ui/StaffPresenter.*` | Định dạng thông tin nhân sự trên GUI |
| `tests/service/test_file_io.cpp` | Kiểm tra định dạng file, lỗi và bảo toàn file cũ |
| `tests/service/test_schedule_uml.cpp`, `test_admin.cpp` | Kiểm tra lịch, phân công và snapshot |
| `tests/model/test_staff*.cpp` | Kiểm tra mô hình nhân viên và đối chiếu UML |
| `tests/dsa/test_admin_containers.cpp` | Kiểm tra DSA phục vụ nhân sự |
| `demo/qt-triage/tests/test_admin_widgets.cpp` | Kiểm tra thao tác nhân sự/lịch/file qua Qt |
| `docs/PHAN-B.md` | Hướng dẫn học, đối chiếu UML và giới hạn triển khai |

## Riêng phần file theo UML

| Module | Chức năng chính | Dữ liệu |
|---|---|---|
| `StaffFileManager` | Đọc/lưu nhân viên ↔ `StaffRegistry` | `PBL2_STAFF 3`, đọc được bản 1/2/3 |
| `ScheduleFileManager` | Đọc/lưu lịch ↔ `ScheduleRegistry` | `PBL2_SCHEDULE 1` |
| `AdminFileManager` | Lưu/nạp đồng bộ cho GUI, bổ trợ tích hợp | `PBL2_ADMIN 4`, đọc được bản 1/2/3/4 |

`PatientFileManager` thuộc điểm phối hợp hồ sơ bệnh nhân, không được tự nhận là đã hoàn thành trong phần B. Không khôi phục `staff.h`, `Staff.cpp`, `DocFile.h`, `DocFile.cpp` cũ.

## Dùng phần đọc file

Trong tab Nhân sự, **Xuất file nhân viên** lưu nhân viên riêng; **Mở file nhân viên** nạp từ file đó. Trước khi thay dữ liệu, chương trình kiểm tra toàn bộ ca, phân công và mã dự phòng đang có. Nếu thiếu mã hoặc vi phạm ràng buộc, báo lỗi và giữ dữ liệu hiện tại; không tự xóa lịch. Nhập thành công đánh dấu snapshot chưa lưu.

Trong tab Lịch, **Xuất/Mở file lịch riêng** dùng `ScheduleFileManager` và yêu cầu nhân viên tương ứng đã được nạp. Nút **Lưu/Mở file** phía dưới quản lý snapshot chung. Xuất file nhân viên riêng không xóa dấu “chưa lưu” của snapshot, vì lịch/dự phòng chưa được lưu bởi thao tác đó.

## Vì sao đọc vào dữ liệu tạm?

Nếu đọc thẳng vào registry đang dùng, lỗi ở dòng cuối có thể để lại dữ liệu nửa cũ, nửa mới. Quy trình là **mở → kiểm tra header/version → đọc từng dòng → kiểm tra liên kết → thay registry sau khi thành công**. `unique_ptr` tự thu hồi dữ liệu tạm khi lỗi; `std::quoted` giữ được tên có khoảng trắng/dấu nháy. Đây là RAII và an toàn ngoại lệ, không phải backtracking.

Lỗi parse được ghi số dòng vật lý (header là dòng 1) và đường dẫn file. Lỗi liên kết PC trong file lịch ghi dòng PC gốc dù kiểm tra sau khi đọc hết file. Lỗi kiểm tra toàn snapshot có thể báo vị trí cuối file. File trắng giữa các bản ghi được bỏ qua; dòng khác trắng sau END bị từ chối. Lưu ra file tạm cùng thư mục rồi thay thế sau khi ghi/đóng thành công để bảo toàn file cũ khi ghi lỗi.

Mỗi file cần đúng loại header; không dùng file lịch để mở bằng nút nhân viên. Định dạng hiện chưa hỗ trợ CSV/Excel và không bỏ qua bản ghi lỗi. Tệp dùng UTF-8; ca dùng thời điểm Unix, giờ Việt Nam UTC+7. Các module tương ứng hai lớp StaffFileManager và ScheduleFileManager trong UML.

Kiểm thử Linux sau bổ sung: **17/17** khi bật Qt; **14/14** core với ASan/UBSan. Chưa kiểm thử Windows.
