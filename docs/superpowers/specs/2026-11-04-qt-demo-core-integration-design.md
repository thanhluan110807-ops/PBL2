# Thiết kế nối demo Qt với phần lõi hiện tại

## Mục đích

Giữ bố cục, QSS và cách thao tác Qt Widgets của demo An Tâm, nhưng các thao tác
nghiệp vụ dùng `PatientRegistry`, `TriageService`, `VisitSearchService` và bộ đọc
`PatientFileManager` thật. Giao diện là nơi nhập/hiển thị, không tự tính mức cấp
cứu hoặc thứ tự hàng chờ. Đây là demo của phần lõi **hiện có**, không phải toàn
bộ kiến trúc mục tiêu trong UML.

## Cách thực hiện đã cân nhắc

1. Giữ `DemoTriageService` làm nguồn dữ liệu rồi chuyển đổi một phần sang lõi:
   ít sửa widget ban đầu, nhưng còn hai bộ quy tắc và dễ hiện trạng thái giả.
2. **Đã chọn:** chuyển hẳn sang Registry/Service thật; giữ phần trình bày Qt
   có thể tái sử dụng. Tạm ẩn các mục mô phỏng chưa có hợp đồng lõi. Người dùng
   đã chọn hướng này ngày 04/11/2026.

## Phạm vi giao diện

- Giữ các màn hình chính của demo: tổng quan, tiếp nhận, hàng chờ, lần khám và
  hồ sơ. Chúng đảm nhận vai trò `MainWindow`, hộp thoại Patient và vùng tra cứu
  Visit trong UML mục tiêu. Không đổi tên các class hiện có chỉ để giống UML.
- Tiếp nhận: tìm Patient theo CCCD hoặc tên để chọn đúng người; nếu chưa có,
  tạo Patient với mã do người dùng nhập, rồi tạo Visit với mã riêng. Registry
  kiểm tra mã/CCCD trùng và mỗi Patient chỉ có một Visit mở. Người vô danh vẫn
  cần mã Patient. Hồ sơ Patient có thể được nạp từ CSV **trước khi tạo service**.
- Phân loại: nhập đủ sinh hiệu và mã điều dưỡng, gọi `TriageService::DanhGia`.
  Giao diện hiển thị mức do lõi tính, không cho chọn mức khác. Trước khi đánh
  giá, Patient phải có ngày sinh hoặc tuổi ước lượng theo model hiện tại.
- Hàng chờ: hiển thị `TriageService::LayDanhSachCho`; gọi tiếp bằng
  `GoiTiep(MaBacSi)` sau khi nhập mã bác sĩ. Thứ tự đúng quy tắc lõi: mức khẩn
  hơn trước, hai đầu hàng cùng mức luân phiên 1:1, lượt trùng đầu ưu tiên quay
  lại. Bộ lọc chỉ đổi hiển thị, không đổi kết quả gọi tiếp.
- Lần khám: hiển thị hồ sơ và lịch sử từ `Visit`; cho phép cận lâm sàng, nhận
  kết quả/quay lại, kết thúc và hủy qua `TriageService`. Chỉ định cận lâm sàng
  cần mã bác sĩ đã tham gia lần khám. Các nút chỉ hiện khi trạng thái cho phép.
- Tra cứu Visit theo tên Patient, ngày tiếp nhận và mã Visit bằng
  `VisitSearchService::TimTheoBenhNhan`. Danh sách Patient chưa có Visit dùng
  API Patient riêng; cần thêm cách lấy tất cả Patient chỉ đọc để màn hồ sơ và
  hộp chọn hồ sơ hoạt động với dữ liệu đọc từ CSV.
- Nhập `patients.csv` v1 qua hộp thoại Qt lúc khởi động hoặc bắt đầu từ Registry
  rỗng do người dùng chọn. File lỗi phải báo rõ, không tự chuyển sang dữ liệu
  rỗng. Không nạp lại CSV giữa phiên khi service còn mượn Registry.

## Cấu trúc và vòng đời dữ liệu

`main.cpp` sở hữu `PatientRegistry*` (theo quyết định không dùng `<memory>`),
tạo các service sau khi Registry đã nạp xong, tạo cửa sổ cuối cùng và hủy theo
thứ tự ngược lại. Widget giữ tham chiếu hoặc mã, không sở hữu hay `delete`
Patient/Visit. Không giữ con trỏ đến đối tượng sau khi Registry bị hủy. Mọi
thay đổi Visit đi qua `TriageService`; bổ sung danh tính qua Registry.

Luồng chính:

```text
Chọn CSV hoặc bắt đầu rỗng → PatientRegistry → TriageService + VisitSearchService
                                 ↓
                        DemoWindow / dialog Qt
                    nhập lệnh ↓       ↑ xem dữ liệu
                      service + Registry
```

`PatientRegistry` hiện không có API liệt kê tất cả Patient; bổ sung một phương
thức đọc chỉ trả `Vector<const Patient*>`, không thay quyền sở hữu hay cho sửa
trực tiếp. `TraCuuLanKham()` đã lấy được toàn bộ Visit khi không có bộ lọc.

## Các mục tạm ẩn và giới hạn

- Bỏ chọn mức phân loại thủ công, cảnh báo chờ quá mốc, nút tiến đồng hồ,
  kịch bản 3:1 và các mô tả/nhãn 3:1. Không thay chúng bằng phép tính giả.
- Không triển khai QueuePressureCalculator, xác nhận mức thủ công, tìm Visit
  theo bác sĩ, xếp lịch nhân sự hoặc những đề xuất tra cứu tương lai chưa duyệt.
- `PatientFileManager` mới chỉ **đọc Patient**; chưa ghi file, chưa nạp Visit,
  hàng chờ và lượt luân phiên. Thao tác trong demo mất khi thoát; UI phải nói
  rõ điều này. Không tự ghi đè CSV.
- Registry hiện chỉ cho bổ sung danh tính của Patient đã có. Không hiện nút
  sửa tự do các trường y tế nếu chưa có API kiểm soát tương ứng.
- Mã bác sĩ/điều dưỡng trong lõi hiện là chuỗi, chưa nối `StaffRegistry`; UI
  nhập mã và không tuyên bố đã kiểm tra danh sách nhân viên thực.
- Ngày tra cứu là ngày tiếp nhận UTC+7. Mốc thời gian thao tác dùng `time_t`
  hiện tại của hệ thống; bỏ đồng hồ mô phỏng cũ để một màn hình không có hai
  nguồn thời gian.

## Kiểm chứng và môi trường

Test lõi vẫn chạy độc lập Qt. Test widget mới phải chứng minh tối thiểu: tạo
Patient/Visit, đánh giá, xem hàng và gọi đúng lượt, vòng cận lâm sàng, tra cứu
Visit theo Patient, nạp CSV Patient, và thông báo lỗi. Test không dùng luật 3:1
hoặc mức xác nhận thủ công. Build demo Qt bằng `PBL2_BUILD_QT_DEMO=ON`, chạy
offscreen nếu không có desktop.

Trong workspace đang kiểm tra, Qt development kit của bản bàn giao cũ không còn
ở `/workspace/.pbl2-qt/root/usr`; `qmake` hệ thống cũng thiếu binary đích.
Phải có Qt Widgets development kit trước khi có thể xác nhận build/test GUI;
không dùng kết quả demo cũ làm bằng chứng cho bản nối lõi.
