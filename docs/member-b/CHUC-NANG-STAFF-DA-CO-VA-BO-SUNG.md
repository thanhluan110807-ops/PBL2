> Cập nhật mới: [các lớp trong sáu ảnh UML, file lịch và trạng thái phân công](HOAN-THIEN-CAC-LOP-TRONG-ANH-UML.md).

# Staff: chức năng đã có, vừa bổ sung và phần còn lại trên UML

## 1. Phạm vi

Lần hoàn thiện này tập trung nhóm Staff vừa tách: Person/NhanVienYTe/BacSi/DieuDuong/CaTrucMongMuon, StaffRegistry, StaffFileManager và các thao tác nhân sự trong Qt. Phần lịch, phân công, clinical/tra cứu Visit vẫn phải đối chiếu riêng; tài liệu không coi hoàn thành Staff là hoàn thành toàn UML.

## 2. Những chức năng đã có trước lần bổ sung

| Nhóm | Chức năng |
|---|---|
| Person / nhân viên chung | Mã ổn định, tên, giới tính/ngày sinh khi có, cấp bậc, kinh nghiệm, nguyện vọng ca |
| Bác sĩ | Chuyên khoa, nội trú, loại nhân viên, điều kiện trưởng ca |
| Điều dưỡng | Bắt đầu/kết thúc sàng lọc, cộng giờ thủ công, phát hiện từ 4 giờ trở lên |
| Thống kê | Điểm mệt mỏi, giờ tuần, ca đêm/cuối tuần và cờ vừa trực đêm |
| Registry | Thêm/tìm mã/cập nhật/xóa; sở hữu nhân viên, chỉ mục HashTable |
| File | Đọc/ghi đúng lớp con, bảo toàn trạng thái, kiểm tra dữ liệu, ghi file tạm |
| GUI | Nhập/sửa/xóa nhân viên; xem danh sách; bảng lịch, thay ca và lưu/nạp snapshot |

Điểm chưa đầy đủ trước đây: điều dưỡng chỉ cộng giờ do người gọi đưa vào; tìm bác sĩ theo tên chưa có; cập nhật Registry xóa đối tượng cũ nên kết quả tra cứu mượn có thể bị treo.

## 3. Những chức năng được bổ sung/sửa trong lần này

### 3.1. Tìm bác sĩ theo tên

`StaffRegistry::TimBacSiTheoTen(Ten)` trả `Vector<const BacSi*>`.

- Tìm một phần tên, chỉ trả bác sĩ.
- Trả mọi người cùng tên để chọn theo mã, không tự chọn kết quả đầu.
- Tên rỗng/toàn khoảng trắng trả danh sách rỗng; GUI để ô trống thì hiện toàn bộ nhân viên.
- Bản này phân biệt hoa/thường và dấu UTF-8. Chưa tìm không dấu hoặc chuẩn hóa Unicode; đó không phải tính năng đã được xác định bởi UML.
- Có ô tìm tên bác sĩ trên tab Nhân sự.

Các con trỏ là mượn: dùng được qua cập nhật cùng nhân viên. Sau xóa nhân viên, hủy Registry hoặc nạp thay toàn bộ dữ liệu, phải bỏ kết quả cũ và tìm lại.

### 3.2. Cập nhật giữ nguyên địa chỉ

`NhanVienYTe::CapNhatTu()` và `StaffRegistry::CapNhat()` cập nhật vào đối tượng đang tồn tại.

Quy trình:

1. Kiểm tra mã giống nhau.
2. Tạo bản sao tạm của dữ liệu mới. Cấp phát lỗi thì dữ liệu đang dùng chưa bị sửa.
3. Kiểm tra đúng lớp con: không đổi một bác sĩ thành điều dưỡng bằng cùng mã.
4. Hoán đổi các trường dữ liệu với bản tạm, không đổi mã hoặc địa chỉ đối tượng thật.
5. Hủy bản tạm.

`dynamic_cast` kiểm tra con trỏ có thật sự trỏ đến lớp con yêu cầu không. Nếu không đúng kiểu, nó trả nullptr. `swap` đổi dữ liệu giữa hai biến mà không cần xóa đối tượng nhân viên. Đây là lý do con trỏ tra cứu còn hợp lệ.

Cập nhật đầy đủ dữ liệu có thể thay trạng thái theo dữ liệu nguồn. Form Qt bảo toàn trạng thái đã ghi nhận khi chỉ sửa dữ liệu hành chính. Quy trình đổi nghề nghiệp sau khi có lịch sử không thuộc API này.

### 3.3. Tính sàng lọc theo thời điểm

Thêm các hàm:

```cpp
BatDauTrucSangLoc(ThoiDiem);
TinhThoiGianSangLocLienTuc(ThoiDiem);
CapNhatThoiGianSangLoc(ThoiDiem);
CanXoayTuaSangLoc(ThoiDiem);
KetThucTrucSangLoc(ThoiDiem);
```

`time_t` là kiểu thời điểm của thư viện thời gian chuẩn. Lấy chênh lệch bằng `std::difftime`, đơn vị giây; chia 3600 để đổi sang giờ.

Ví dụ 08:00 bắt đầu, 11:59:59 chưa đủ bốn giờ, 12:00 đạt bốn giờ. Không cần ngồi chờ bốn tiếng khi test: truyền hai thời điểm đã biết vào hàm.

Trong chế độ đồng hồ:

- Giữ thời điểm bắt đầu và mốc đã ghi nhận gần nhất.
- Gọi bắt đầu lặp không reset giờ.
- Không chấp nhận thời điểm lùi so với mốc đã ghi nhận.
- Không cộng giờ thủ công đồng thời để tránh đếm hai lần.
- Truy vấn thời gian không tự thay trạng thái; hàm cập nhật mới ghi nhận mốc.

Các hàm cộng giờ cũ vẫn dùng được cho mô phỏng; kết thúc đợt cũ trước khi đổi chế độ. Kết thúc xóa trạng thái liên tục như API gốc; chưa tạo lịch sử các đợt sàng lọc.

Qt có ba nút: bắt đầu, cập nhật giờ/cảnh báo, kết thúc. Chọn dòng điều dưỡng trước. Qt truyền đồng hồ hệ thống; không đặt tính thời gian trong widget. Cảnh báo hiện khi bấm cập nhật, chưa có timer tự bật cảnh báo hoặc tự đổi người.

### 3.4. Lưu/đọc mốc thời gian

Staff và snapshot tổng chuyển sang version 3. Ngoài dữ liệu version 2, thêm cờ theo đồng hồ, thời điểm bắt đầu và mốc cập nhật cuối.

- Đọc được version 1, 2 và 3.
- Version cũ không có mốc thì giữ chế độ cộng giờ thủ công.
- Version 3 khôi phục đồng hồ đang chạy; truy vấn ở thời điểm sau sẽ tính tiếp từ mốc lưu.
- Kiểm tra mốc và số giờ khớp nhau; dữ liệu mâu thuẫn bị từ chối.
- Quy ước mô phỏng hiện tại tính thời gian trôi qua khi ứng dụng đóng là thời gian vẫn đang sàng lọc nếu chưa kết thúc. Nếu nghiệp vụ muốn tự kết thúc khi đóng, cần đổi chính sách một cách rõ ràng.

### 3.5. Kiểm tra nhận ca dùng trạng thái hiện tại

- Dự báo điểm bắt đầu từ `LayDiemMetMoi()` hiện tại, thay vì chỉ dùng điểm khởi tạo.
- Kiểm tra trần giờ dùng giờ đã làm + giờ dự kiến + ca mới.
- `Shift::CoChongGio()` gom phép kiểm tra giao nhau vào model theo UML; hai ca chạm biên không trùng nhưng vẫn phải kiểm tra nghỉ.

Schedule truyền cho scheduler ở bước này được hiểu là lịch dự kiến chưa ghi nhận giờ thực hiện. Chưa có vòng đời phân công để tự loại ca hoàn thành khỏi phép dự báo; không đưa lại giờ đã thực hiện vào lịch dự kiến rồi coi là chỉ tính một lần.

## 4. Cách tự trình diễn

1. Mở Nhân sự và lịch trực, nạp dữ liệu mẫu hoặc dữ liệu của bạn.
2. Nhập tên bác sĩ vào ô tìm; xác nhận điều dưỡng trùng tên không xuất hiện trong kết quả bác sĩ.
3. Xóa nội dung ô tìm, chọn một điều dưỡng.
4. Bấm bắt đầu sàng lọc, rồi cập nhật giờ. Di chuột lên tên để xem thông tin.
5. Nếu dữ liệu mẫu có ca trực, xếp lịch trước khi lưu snapshot tổng (giới hạn lưu nháp của bản hiện tại vẫn còn).
6. Lưu và mở lại file: điều dưỡng vẫn có mốc đồng hồ. Bấm kết thúc để chấm dứt đợt.
7. Chạy test `test_staff_uml` để kiểm tra mốc 4 giờ bằng thời gian giả lập mà không chờ thật.

## 5. Những phần UML còn thiếu ngoài nhóm Staff

| Phần | Hiện trạng và công việc còn lại |
|---|---|
| ShiftAssignment | Chưa có mã phân công riêng và vòng đời xác nhận/hủy/bắt đầu/hoàn thành |
| ScheduleRegistry | Chưa có registry chính thức quản lý ca và phân công theo UML |
| ScheduleFileManager | Chưa tách file lịch riêng; snapshot AdminFileManager vẫn lưu lịch |
| FatigueService sau ca | Có công thức gốc và dự báo; chưa có xử lý hoàn thành ca có chống ghi nhận lặp, hồi phục hoặc chuyển tuần/tháng tự động |
| Lịch xuyên tuần | Chưa tổng hợp lịch sử biên tuần để kiểm tra nghỉ liên tuần |
| Xoay tua | Có phát hiện đủ 4 giờ; chưa tự chọn/xác nhận người thay và cập nhật phân công |
| Áp lực/tăng cường | Chưa có QueuePressureCalculator và đề xuất ca tăng cường từ hàng chờ |
| Tra cứu clinical | Chưa có DoctorParticipation, VisitSearchService/Panel đầy đủ và PatientRegistry tích hợp |
| Persistence toàn hệ thống | Chưa có PatientFileManager khôi phục Patient/Visit/hàng chờ thật |

Các phần này cần hoàn thiện thành module tiếp theo, không nằm sẵn trong class Staff. Những chính sách chưa được UML xác định gồm công thức áp lực/ngưỡng tăng cường, phục hồi điểm, tiêu chí tối ưu và phân biệt vai trò khám/ghi thay. Không tự coi công thức mô phỏng là quy định y khoa.

## 6. Thứ tự tiếp theo

Để đi đến phần B đầy đủ: ShiftAssignment + ScheduleRegistry → xác nhận/hủy/hoàn thành và ghi nhận sau ca → ScheduleFileManager → xoay tua/tăng cường → tích hợp clinical và tra cứu với A.

Bản này hoàn thiện các khoảng thiếu đã xác định ở nhóm Staff và các liên kết trực tiếp nêu trên, không công bố hoàn thành toàn hệ thống.


## 7. Kết quả kiểm tra bản này

Linux/GCC: 12/12 CTest lõi chạy ASan/UBSan đạt, leak checking bật ngoài ptrace; 15/15 CTest Debug khi bật Qt đạt; 23 header độc lập biên dịch thành công; `git diff --check` đạt. Test mới kiểm tra bác sĩ trùng tên, cập nhật giữ địa chỉ và từ chối đổi loại, mốc sàng lọc trước/đúng 4 giờ, thời gian lùi, không trộn chế độ, file v3 round-trip và đọc v2. Test widget đã bấm các nút bắt đầu/cập nhật/kết thúc, tìm tên và nạp lại đồng hồ. Chưa kiểm chứng Windows hoặc toàn bộ UML.
