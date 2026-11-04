# An Tâm — giao diện Qt nối code lõi

Demo Qt Widgets/QSS dùng trực tiếp `PatientRegistry`, `TriageService`,
`VisitSearchService` và `PatientFileManager`. Nghiệp vụ và DSA vẫn độc lập Qt.

## Build và chạy

Cần CMake >= 3.20, compiler C++17 và Qt Widgets development kit (Qt 5 hoặc 6).
Dùng kit phù hợp compiler trên Windows (MSVC hoặc MinGW).

```bash
cmake -S . -B build-qt-demo -DPBL2_BUILD_QT_DEMO=ON -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-qt-demo --parallel 2
ctest --test-dir build-qt-demo --output-on-failure --no-tests=error
./build-qt-demo/demo/qt-triage/pbl2_qt_demo
```

Nếu CMake chưa tìm thấy Qt, thêm `-DCMAKE_PREFIX_PATH=<thu-muc-kit-Qt>` khi
configure. Python chỉ cần khi `BUILD_TESTING=ON`. Để chạy test không có desktop,
CTest đã đặt `QT_QPA_PLATFORM=offscreen` cho các test widget.

## Luồng dùng thử

1. Khởi động: chọn CSV hoặc bấm **Bắt đầu rỗng**. Có file hư cấu
   [patients.csv](data/patients.csv) đúng lược đồ v1. CSV lỗi sẽ báo lỗi và giữ
   hộp thoại để chọn lại; đóng hộp thoại thì thoát. Không nạp CSV giữa phiên.
2. Tiếp nhận: nhập mã Patient và mã Visit riêng, hoặc tìm tên/CCCD/mã rồi chọn
   hồ sơ cũ. Hồ sơ cùng tên không tự gộp. Mã hoặc CCCD trùng bị từ chối; mỗi
   Patient chỉ có một Visit đang mở. Patient mới có thể chưa rõ tên/tuổi.
3. Đánh giá: mở chi tiết Visit ở trang **Tiếp nhận**, nhập mạch, huyết áp tâm
   thu, nhịp thở, SpO₂, GCS và mã điều dưỡng. Patient cần ngày sinh hoặc tuổi
   ước lượng. Mức cấp cứu được lõi tự tính; lưu thành công mới vào hàng khám.
4. Gọi khám: nhập mã bác sĩ và bấm **Gọi lượt tiếp theo**. Bộ lọc chỉ ảnh hưởng
   hiển thị. Mức cao hơn luôn đi trước; khi hai đầu hàng cùng mức, luân phiên
   quay lại/lần đầu, lần hòa đầu tiên ưu tiên quay lại.
5. Chi tiết: bác sĩ đã tham gia có thể chỉ định cận lâm sàng, nhận kết quả và
   đưa Visit vào hàng quay lại. Có thể kết thúc khi đang khám hoặc hủy Visit
   đang mở. Đánh giá lại chỉ hiện khi đang chờ khám. Lịch sử đánh giá, bác sĩ
   tham gia và cận lâm sàng đọc từ Visit thật.
6. Tra cứu: ô tên trên đầu trang cùng ngày tiếp nhận UTC+7 và mã Visit chính
   xác được kết hợp AND qua `VisitSearchService`. Tên không phân biệt hoa/
   thường nhưng giữ dấu theo lõi. Ô tìm trong danh sách hỗ trợ lọc hiển thị
   tên/mã/CCCD không dấu. Patient chưa khám nằm trong **Hồ sơ bệnh nhân**.
7. Hồ sơ: xem dữ liệu y tế và các Visit liên kết; bổ sung danh tính bằng tên,
   giới tính, ngày sinh, CCCD qua Registry, giữ nguyên mã và lịch sử.

## Giới hạn hiện tại

- Chỉ đọc CSV Patient; chưa ghi file, chưa nạp Visit/hàng chờ. Thay đổi mất
  khi thoát và không sửa CSV nguồn. Chương trình hiển thị rõ giới hạn này.
- Chưa nối danh sách nhân viên; mã bác sĩ/điều dưỡng do người dùng nhập.
- Chưa tính áp lực hàng đợi hoặc xếp lịch trực; mục lịch trực đang vô hiệu hóa.
- Bỏ mức phân loại thủ công, cảnh báo chờ quá giờ, đồng hồ mô phỏng, mẫu 3:1.
  Thời gian là giờ hệ thống; màn hình hiển thị UTC+7. Cột **Tiếp nhận** không
  phải thời gian vào hàng (API chưa trả thời điểm vào hàng).
- Hồ sơ đã tạo chỉ có API bổ sung danh tính, chưa sửa tự do thông tin y tế hay
  tuổi ước lượng. Nếu thiếu tuổi khi đánh giá, cần bổ sung ngày sinh hợp lệ.
- `model/DemoTriageService.*`, `model/DemoSeed.cpp`, `tests/test_demo_state.cpp`
  và `tests/check_rule_parity.py` được giữ làm tham khảo lịch sử, không thuộc
  target demo hoặc bộ kiểm thử hiện tại.

## Mã nguồn và kiểm thử

- `ui/DemoWindow.h/.cpp`: bố cục, thống kê, bảng/thẻ, lọc và gọi tiếp.
- `ui/DemoDialogs.cpp`: tiếp nhận, đánh giá, chi tiết, danh tính, cận lâm sàng.
- `ui/DemoStartup.h/.cpp`: chọn CSV/Registry rỗng trước khi tạo service.
- `ui/DemoStyle.h/.cpp`: QSS, icon, nhãn trạng thái và thời gian.
- `main.cpp`: sở hữu Registry bằng con trỏ, tạo service/cửa sổ sau Registry
  và hủy chúng trước khi delete Registry.
- `tests/test_demo_widgets.cpp`: thao tác Qt thật kết hợp kiểm tra Registry;
  lưu ảnh rộng/hẹp tại thư mục chạy test.
- `tests/test_demo_startup.cpp`: chọn rỗng, CSV hợp lệ/lỗi, hủy chọn file/thoát.

Bộ dev Qt cục bộ dùng trong workspace không thuộc repository. Hướng dẫn build
trên áp dụng với Qt development kit đã cài của người chạy.
