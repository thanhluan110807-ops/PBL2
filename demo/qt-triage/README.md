# An Tâm — giao diện Qt nối code lõi

Mục **Nhân sự và lịch trực** trong thanh điều hướng mở phần B ngay trong vùng nội dung chính, không bật hộp thoại riêng. Ba tab Nhân sự,
Ca trực, Lịch và thay ca dùng trực tiếp dữ liệu nhân sự/lịch và `SchedulerService`.
Các thẻ tổng quan lấy số lượng từ dữ liệu hiện tại; vùng chi tiết hiện thông tin
người, ca hoặc phân công được chọn. Khi tìm người thay, danh sách hiện tên kèm
mã nhân viên; thao tác vẫn sử dụng mã. Phần này tự nạp `nhan-su-lich.csv` cạnh
Patient CSV nếu có; bộ cũ tiếp tục dùng `nhan-su-lich.pbl2` trong thư mục dữ liệu
ứng dụng nếu chưa liên kết CSV. Nút **Lưu thay đổi** màu xanh ghi thẳng vào file
đó, lưu chung nhân sự, ca trực, phân công và dự phòng. Không có hộp chọn file
hay nút nhập/xuất riêng trong các tab. Nút lưu được bật khi có thay đổi; ghi lỗi
giữ dữ liệu để lưu lại, còn nguồn có sẵn bị lỗi đọc sẽ chặn lưu đè.
Phần này lưu khi bấm nút, còn Patient/Visit tự lưu sau mỗi thay đổi.
Trạng thái lưu hồ sơ được tóm tắt ở thanh đầu;
không hiển thị đường dẫn file và ghi chú kỹ thuật ở cuối trang.

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

## Demo với dữ liệu có sẵn

Trong WSL, sau khi build vào `build-qt-wsl`, liên kết bộ CSV mẫu một lần:

```bash
./build-qt-wsl/demo/qt-triage/pbl2_qt_demo --du-lieu-mau
```

Tự mở 16 hồ sơ, 17 lượt khám (8 đang chờ), 10 bác sĩ, 10 điều dưỡng và lịch
trực có sẵn. Sửa và lưu trực tiếp vào ba CSV cố định; lần sau mở chương trình
bình thường tự dùng lại bộ đã liên kết và giữ thay đổi, không cần Python hay
tạo phiên tạm. Lần đầu chưa có cấu hình cũng tự dùng bộ này. Xem
[bộ CSV và kịch bản trình diễn](data/demo-day-du/README.md) để biết chi tiết.

## Luồng dùng thử

1. Khởi động: tự dùng bộ file đã nhớ, hoặc bộ CSV mẫu nếu chưa có cấu hình.
   Chạy `--chon-du-lieu` để chọn CSV khác hoặc bấm **Bắt đầu rỗng**. Có file hư cấu
   [patients.csv](data/patients.csv) đúng lược đồ v1. CSV lỗi sẽ báo lỗi và giữ
   hộp thoại để chọn lại; đóng hộp thoại thì thoát. Có thể chọn thêm Visit CSV để khôi phục lần khám/hàng chờ. Nếu chưa có Visit, tạo visit.csv cạnh Patient. Lần sau tự mở lại bộ đã chọn. Không nạp lại CSV giữa phiên.
2. Tiếp nhận: hệ thống tự cấp mã Patient mới dạng BN001 và mã Visit dạng LK001; hoặc tìm tên/CCCD/mã rồi chọn
   hồ sơ cũ. Chỉ khi chọn **Chọn hồ sơ đã có** mới hiện tìm kiếm và danh sách
   bệnh nhân. Chọn người sẽ điền thông tin đã có vào ô xám chỉ đọc; ô trắng
   cho phép bổ sung mục còn thiếu. Phần bổ sung được lưu vào cùng hồ sơ khi
   tiếp nhận, không thay mã hay ghi đè dữ liệu đã có. Mã lượt khám và triệu
   chứng nhập riêng cho lần mới. Chữ nội dung 18px, tiêu đề 24px.
   Hồ sơ cùng tên không tự gộp. Mã hoặc CCCD trùng bị từ chối; mỗi Patient chỉ
   có một Visit đang mở. Patient mới có thể chưa rõ tên/tuổi.
3. Đánh giá: mở chi tiết Visit ở trang **Tiếp nhận**, nhập mạch, huyết áp tâm
   thu, nhịp thở, SpO₂ và GCS. Tạm bỏ nhập mã điều dưỡng khi chưa có đăng nhập;
   đánh giá mới lưu mã rỗng, lịch sử đã có mã vẫn giữ nguyên. Khi triển khai
   đăng nhập theo vai trò, mã sẽ lấy từ tài khoản điều dưỡng đang đăng nhập.
   Patient cần ngày sinh hoặc tuổi
   ước lượng. Dòng chữ nhỏ ngay dưới từng ô ghi khoảng giá trị nhập hợp lệ,
   đơn vị và cách nhập số thập phân cho SpO₂. Chữ nội dung 18px, tiêu đề 24px;
   hướng dẫn 14px. Mức cấp cứu được lõi tự tính;
   lưu thành công mới vào hàng khám.
4. Gọi khám: nhập mã bác sĩ và bấm **Gọi lượt tiếp theo**. Bộ lọc chỉ ảnh hưởng
   hiển thị. Mức cao hơn luôn đi trước; khi hai đầu hàng cùng mức, luân phiên
   quay lại/lần đầu, lần hòa đầu tiên ưu tiên quay lại.
5. Chi tiết: bác sĩ đã tham gia có thể chỉ định cận lâm sàng, nhận kết quả và
   đưa Visit vào hàng quay lại. Có thể kết thúc khi đang khám hoặc hủy Visit
   đang mở. Đánh giá lại chỉ hiện khi đang chờ khám. Lịch sử đánh giá, bác sĩ
   tham gia và cận lâm sàng đọc từ Visit thật. Thông tin được xếp thành từng
   dòng nhãn/giá trị từ trên xuống; nội dung dài cuộn trong hộp thoại.
6. Tra cứu trong tab **Lượt khám**: ô tìm trong danh sách hỗ trợ tên/mã/CCCD
   không phân biệt hoa/thường và dấu. Bộ lọc ngày tiếp nhận UTC+7 và mã Visit
   chính xác đi qua `VisitSearchService`, kết hợp với từ khóa để thu hẹp kết
   quả. Patient chưa khám nằm trong **Hồ sơ bệnh nhân**.
7. Hồ sơ: mặc định xem từng dòng **Họ tên: …**, **CCCD: …**, ngày sinh, giới
   tính, nhóm máu, dị ứng, tiền sử và các Visit liên kết. Giá trị chưa biết để
   trống. Bấm **Bổ sung / cập nhật thông tin** mới hiện ô nhập tên, giới tính,
   ngày sinh, CCCD. **Hủy chỉnh sửa** trở lại phần xem; **Lưu thay đổi** cập
   nhật qua Registry, giữ nguyên mã, dữ liệu y tế và lịch sử. Lưu thành công
   trở lại phần xem thông tin mới. Hồ sơ và chi tiết lần khám dùng chữ nội
   dung 18px, tiêu đề 24px (tăng khoảng 30% so với trước).

## Bộ dữ liệu kiểm tra mở rộng

Dùng [patient.csv](data/patient.csv) gồm 30 hồ sơ hư cấu và làm theo
[kịch bản kiểm tra](data/README.md) để thử tra cứu, 5 mức triage, luân phiên
hai hàng, cận lâm sàng, bổ sung danh tính và các tình huống bị từ chối.

## Giới hạn hiện tại

- Tự ghi vào đúng file Patient/Visit đã chọn sau mỗi thay đổi. Không cần nút
  lưu riêng. Có bản gốc và các bản dự phòng trong `<patient-path>.pbl2/`.
- Mở lại tự dùng bộ file gần nhất; nếu bị dừng giữa hai lần ghi, tự hoàn tất
  từ bản chụp đầy đủ. Ghi lỗi giữ RAM, tạm chặn thay đổi, tự thử lại mỗi 3 giây;
  không đóng cửa sổ khi chưa ghi được. [Chi tiết](../../docs/repository/visit-files.md).
- Chưa nối danh sách nhân viên; mã bác sĩ/điều dưỡng do người dùng nhập.
- Chưa tính áp lực hàng đợi. Mục **Nhân sự và lịch trực** hiển thị phần B trong
  cửa sổ chính, tự đọc file đã liên kết và ghi khi bấm **Lưu thay đổi**.
- Bỏ mức phân loại thủ công, cảnh báo chờ quá giờ, đồng hồ mô phỏng, mẫu 3:1.
  Thời gian là giờ hệ thống; màn hình hiển thị UTC+7. Cột **Tiếp nhận** không
  phải thời gian vào hàng (API chưa trả thời điểm vào hàng).
- Khi tiếp nhận lại, có thể điền thông tin còn thiếu, gồm dị ứng, tiền sử và
  tuổi ước lượng. Các mục đã có giữ nguyên; chưa có chức năng sửa tự do dữ
  liệu y tế đã ghi. Màn hình hồ sơ vẫn cho cập nhật danh tính qua Registry.
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
- `tests/test_patient_detail_widgets.cpp`: xem/sửa/hủy hồ sơ, dữ liệu trống,
  ngày sai/CCCD trùng, lưu và đọc lại, bố cục/cỡ chữ rộng-hẹp và lịch sử khám.
- `tests/test_reception_widgets.cpp`: tạo mới/chọn hồ sơ cũ, tự điền/khóa ô,
  bổ sung phần thiếu, kiểm tra trùng/đang khám, hủy và lưu/đọc lại đúng hồ sơ.
- `tests/test_demo_persistence.cpp`: tự ghi sau thao tác GUI, đọc lại, ghi lỗi,
  tự thử lại và chặn đóng khi chưa ghi được.

Bộ dev Qt cục bộ dùng trong workspace không thuộc repository. Hướng dẫn build
trên áp dụng với Qt development kit đã cài của người chạy.

Muốn chọn bộ dữ liệu khác lúc khởi động, chạy:

```bash
./build-qt-demo/demo/qt-triage/pbl2_qt_demo --chon-du-lieu
```

Khi bắt đầu rỗng, chọn nơi đặt hai file một lần. Chương trình cập nhật trực tiếp
hai file đó về sau; các file mẫu đã chọn cũng sẽ được cập nhật, bản gốc được
sao lưu trước. Muốn giữ file mẫu trong Git sạch, sao chép mẫu ra thư mục thử rồi
chọn bản sao. Không sửa CSV bên ngoài khi ứng dụng đang chạy.

Sau khi hợp nhất dev/work, Visit ghi CSV v2 để giữ thời điểm/vai trò bác sĩ, vẫn đọc được v1. Xem [định dạng Visit](../../docs/repository/visit-files.md).

## Tự động cấp mã khi tiếp nhận

Hồ sơ mới nhận mã BN001, BN002...; mỗi lần khám mới nhận LK001, LK002...
Hồ sơ cũ giữ nguyên mã. Hai ô mã chỉ đọc; mã được cấp lúc bấm tiếp nhận.
Registry tìm số lớn nhất trong từng dãy hiện có, kể cả Visit đã đóng/hủy;
sau khi nạp CSV hoặc mở lại ứng dụng, tiếp tục từ dữ liệu đó. API nhập mã
trực tiếp vẫn dùng cho nạp file và các tích hợp cũ. Mã khác định dạng được giữ nguyên.
Mã chỉ duy nhất trong bộ dữ liệu hiện tại: phục hồi bản sao cũ hoặc xóa lịch sử
có thể khiến mã từng cấp trong phần dữ liệu đã mất được dùng lại. Không có bộ đếm ngoài file.
