# An Tâm — giao diện Qt nối code lõi

Nút **Nhân sự và lịch trực** trong cửa sổ chính mở phần B. Ba tab Nhân sự,
Ca trực, Lịch và thay ca dùng trực tiếp dữ liệu nhân sự/lịch và `SchedulerService`.
Các thẻ tổng quan lấy số lượng từ dữ liệu hiện tại; vùng chi tiết hiện thông tin
người, ca hoặc phân công được chọn. Khi tìm người thay, danh sách hiện tên kèm
mã nhân viên; thao tác vẫn sử dụng mã. Phần này lưu/mở file riêng, không dùng
cơ chế tự lưu Patient/Visit.

Demo Qt Widgets/QSS dùng trực tiếp `PatientRegistry`, `TriageService`,
`VisitSearchService`, `PatientFileManager`, `VisitFileManager` và `SessionFileManager`. Nghiệp vụ và DSA vẫn độc lập Qt.

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
   hộp thoại để chọn lại; đóng hộp thoại thì thoát. Có thể chọn thêm Visit CSV để khôi phục lần khám/hàng chờ. Nếu chưa có Visit, tạo visit.csv cạnh Patient. Lần sau tự mở lại bộ đã chọn. Không nạp lại CSV giữa phiên.
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

## Bộ dữ liệu kiểm tra mở rộng

Dùng [patient.csv](data/patient.csv) gồm 30 hồ sơ hư cấu và làm theo
[kịch bản kiểm tra](data/README.md) để thử tra cứu, 5 mức triage, luân phiên
hai hàng, cận lâm sàng, bổ sung danh tính và các tình huống bị từ chối.

## Giới hạn hiện tại

- Tự ghi vào đúng file Patient/Visit đã chọn sau mỗi thay đổi. Không cần nút
  lưu riêng. Có bản gốc và các bản dự phòng trong `<patient-path>.pbl2/`.
- Mở lại tự dùng bộ file gần nhất; nếu bị dừng giữa hai lần ghi, tự hoàn tất
  từ bản chụp đầy đủ. Ghi lỗi giữ RAM, tạm chặn thay đổi, tự thử lại mỗi3giây;
  không đóng cửa sổ khi chưa ghi được. [Chi tiết](../../docs/repository/visit-files.md).
- Chưa nối danh sách nhân viên; mã bác sĩ/điều dưỡng do người dùng nhập.
- Chưa tính áp lực hàng đợi. Nút **Nhân sự và lịch trực** mở AdminPanel của phần B; dữ liệu nhân sự/lịch có cơ chế nhập/xuất riêng.
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
- `ui/DemoStartup.h/.cpp`: chọn Patient/Visit CSV hoặc Registry rỗng trước khi tạo service.
- `ui/DemoStyle.h/.cpp`: QSS, icon, nhãn trạng thái và thời gian.
- `main.cpp`: sở hữu Registry bằng con trỏ, tạo service/cửa sổ sau Registry
  và hủy chúng trước khi delete Registry.
- `tests/test_demo_widgets.cpp`: thao tác Qt thật kết hợp kiểm tra Registry;
  lưu ảnh rộng/hẹp tại thư mục chạy test.
- `tests/test_demo_startup.cpp`: chọn rỗng, CSV hợp lệ/lỗi, hủy chọn file/thoát.

Bộ dev Qt cục bộ dùng trong workspace không thuộc repository. Hướng dẫn build
trên áp dụng với Qt development kit đã cài của người chạy.

- `tests/test_demo_persistence.cpp`: tự ghi sau thao tác GUI, đọc lại, ghi lỗi, tự thử lại và chặn đóng khi chưa ghi được.

Muốn chọn bộ dữ liệu khác lúc khởi động, chạy:

```bash
./build-qt-demo/demo/qt-triage/pbl2_qt_demo --chon-du-lieu
```

Khi bắt đầu rỗng, chọn nơi đặt hai file một lần. Chương trình cập nhật trực tiếp
hai file đó về sau; các file mẫu đã chọn cũng sẽ được cập nhật, bản gốc được
sao lưu trước. Muốn giữ file mẫu trong Git sạch, sao chép mẫu ra thư mục thử rồi
chọn bản sao. Không sửa CSV bên ngoài khi ứng dụng đang chạy.

Sau khi hợp nhất dev/work, Visit ghi CSV v2 để giữ thời điểm/vai trò bác sĩ, vẫn đọc được v1. Xem [định dạng Visit](../../docs/repository/visit-files.md).
