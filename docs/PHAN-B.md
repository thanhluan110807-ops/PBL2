# Bản đồ file phần B

Giữ nguyên tên class và cấu trúc dự án; chú thích đầu file ghi **Phần B — Chức năng chính** để nhận diện. File `.h` khai báo cách sử dụng; `.cpp` cài đặt cách xử lý.

| Thư mục | Chức năng chính |
|---|---|
| `include/model/`, `src/model/` | Mô hình nhân viên, ca, phân công; chi tiết xem README của thư mục |
| `include/repository/`, `src/repository/` | Registry trong RAM và FileManager đọc/ghi dữ liệu |
| `include/service/`, `src/service/` | Xếp lịch, tìm người thay và tính mệt mỏi |
| `include/dsa/` | DSA tự viết dùng chung; B phụ trách HashTable/LinkedList |
| `demo/qt-triage/ui/AdminPanel.*` | GUI nhân sự/lịch, tự nạp dữ liệu liên kết và nút lưu chung |
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

## Lưu và nạp dữ liệu trên giao diện

Theo yêu cầu cập nhật, phần B hiện trong vùng nội dung chính của ứng dụng. Khi mở, chương trình ưu tiên `nhan-su-lich.csv` cạnh Patient CSV đã liên kết; nếu không có thì bộ cũ tiếp tục dùng `nhan-su-lich.pbl2` từ thư mục dữ liệu ứng dụng. Nút **Lưu thay đổi** màu xanh lưu trực tiếp vào nguồn đã liên kết, gồm nhân sự, tuần, ca trực, phân công và dự phòng; người dùng không cần chọn file. Nút được bật khi dữ liệu đã thay đổi. Ghi lỗi giữ thay đổi trong RAM để lưu lại. Nếu nguồn có sẵn không đọc được, báo lỗi và chặn chỉnh sửa/lưu đè cho đến khi dữ liệu được khôi phục. Đóng ứng dụng khi còn thay đổi chưa lưu vẫn có cảnh báo.

Các nút **Lưu/Mở file**, **Xuất/Mở file nhân viên** và **Xuất/Mở file lịch riêng** đã được bỏ khỏi giao diện. Các API của `StaffFileManager`, `ScheduleFileManager`, `AdminPanel::LuuNhanSu/NapNhanSu` vẫn phục vụ code và kiểm thử. Khi nạp nhân viên qua API, chương trình kiểm tra liên kết ca, phân công và dự phòng trước khi thay dữ liệu; lỗi giữ nguyên dữ liệu hiện tại. Xuất riêng nhân viên không được coi là đã lưu toàn bộ snapshot.

## Vì sao đọc vào dữ liệu tạm?

Nếu đọc thẳng vào registry đang dùng, lỗi ở dòng cuối có thể để lại dữ liệu nửa cũ, nửa mới. Quy trình là **mở → kiểm tra header/version → đọc từng dòng → kiểm tra liên kết → thay registry sau khi thành công**. `unique_ptr` tự thu hồi dữ liệu tạm khi lỗi; `std::quoted` giữ được tên có khoảng trắng/dấu nháy. Đây là RAII và an toàn ngoại lệ, không phải backtracking.

Lỗi parse được ghi số dòng vật lý (header là dòng 1) và đường dẫn file. Lỗi liên kết PC trong file lịch ghi dòng PC gốc dù kiểm tra sau khi đọc hết file. Lỗi kiểm tra toàn snapshot có thể báo vị trí cuối file. File trắng giữa các bản ghi được bỏ qua; dòng khác trắng sau END bị từ chối. Lưu ra file tạm cùng thư mục rồi thay thế sau khi ghi/đóng thành công để bảo toàn file cũ khi ghi lỗi.

Mỗi API đọc file cần đúng loại header tương ứng. Các file nhân viên/lịch riêng vẫn dùng định dạng văn bản cũ; snapshot chung `AdminFileManager` hỗ trợ thêm CSV bên dưới. Không bỏ qua bản ghi lỗi. Tệp dùng UTF-8; ca dùng thời điểm Unix, giờ Việt Nam UTC+7. Các module tương ứng hai lớp StaffFileManager và ScheduleFileManager trong UML.

## CSV nhân sự và lịch trực

`AdminFileManager` đọc/ghi đường dẫn có đuôi `.csv` bằng CSV UTF-8; các đường dẫn
`.pbl2`/`.txt` tiếp tục đọc/ghi định dạng cũ. Dòng đầu là
`PBL2_ADMIN_CSV,1,YYYY-MM-DD`, ngày phải là thứ Hai. Mỗi dòng có loại và các
cột riêng, dùng quy tắc ngoặc kép như CSV Patient/Visit, không gói dòng văn bản
cũ vào một ô. Dòng `END` kết thúc file; dữ liệu sau đó bị từ chối.

| Loại | Các cột sau loại |
|---|---|
| `NV` | loại nhân viên, mã, họ tên, cấp bậc, kinh nghiệm, điểm nền, chuyên khoa, giờ mong muốn đầu/cuối, mong cuối tuần, điểm hiện tại, giờ làm tuần, số ca đêm/cuối tuần tháng, vừa trực đêm, nội trú, đang sàng lọc, giờ sàng lọc liên tục, giới tính, có ngày sinh, ngày/tháng/năm sinh, theo đồng hồ, bắt đầu sàng lọc, mốc đồng hồ |
| `CA` | mã ca, bắt đầu/kết thúc Unix giây, số bác sĩ, số điều dưỡng, cần trưởng ca |
| `PC` | mã phân công, mã nhân viên, mã ca, vai trò, trạng thái, thời điểm bắt đầu, lý do hủy |
| `DP` | mã nhân viên dự phòng |

Các enum/cờ lưu bằng số như snapshot v4 hiện có; bộ đọc kiểm tra giới hạn,
liên kết, giờ nghỉ, mệt mỏi và tính hợp lệ của phân công. CSV thiếu cột, có giá
trị sai hoặc bị cắt ngang sẽ báo lỗi, không thay dữ liệu đang dùng. Lưu CSV
vẫn ghi file tạm rồi thay file đích sau khi ghi/đóng thành công.

Kiểm thử Linux sau bổ sung: **17/17** khi bật Qt; **14/14** core với ASan/UBSan. Chưa kiểm thử Windows.
