# An Tâm — demo Qt Widgets/C++

Demo native mô phỏng điều phối cấp cứu, định kiểu bằng QSS. Target `pbl2_qt_demo` chỉ được tạo khi bật `PBL2_BUILD_QT_DEMO=ON`; tùy chọn mặc định tắt. Phần lõi và CI hiện có tiếp tục không cần Qt. Không dùng WebEngine, QML hoặc JavaScript trong demo này.

## Build và chạy trên máy cá nhân

Cần CMake >= 3.20, compiler C++17 và **Qt Widgets development kit**. Hỗ trợ tìm Qt 6 hoặc Qt 5; phiên này đã kiểm tra Qt **6.8.2**, GCC 14.2 và CMake 3.31.10 trên Linux. Chọn Qt kit tương thích compiler: MSVC với MSVC, MinGW với MinGW.

Từ thư mục gốc PBL2:

```bash
cmake -S . -B build-qt-demo -DPBL2_BUILD_QT_DEMO=ON -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-qt-demo --parallel 2
ctest --test-dir build-qt-demo --output-on-failure --no-tests=error
./build-qt-demo/demo/qt-triage/pbl2_qt_demo
```

Chạy tuần tự; chỉ sang bước sau khi bước trước thành công. Nếu chỉ chạy demo và không có Python, đặt `-DBUILD_TESTING=OFF`.

Nếu Qt cài bằng Qt Online Installer, thêm đường dẫn kit vào bước configure, ví dụ Linux:

```bash
cmake -S . -B build-qt-demo -DPBL2_BUILD_QT_DEMO=ON -DCMAKE_PREFIX_PATH="$HOME/Qt/6.8.3/gcc_64"
```

Đường dẫn trên là ví dụ từ cấu hình `patient-class`; dùng đúng thư mục Qt đang có trên máy. Branch Git chứa cấu hình/mã nguồn, không chứa bộ cài Qt. Khi đổi compiler/kit, chọn thư mục build mới.

Windows PowerShell, với kit MSVC đã cài (đổi đường dẫn cho đúng phiên bản):

```powershell
cmake -S . -B build-qt-demo -DPBL2_BUILD_QT_DEMO=ON -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
cmake --build build-qt-demo --config Debug --parallel 2
ctest --test-dir build-qt-demo -C Debug --output-on-failure --no-tests=error
$env:PATH = "C:\Qt\6.8.3\msvc2022_64\bin;" + $env:PATH
.\build-qt-demo\demo\qt-triage\Debug\pbl2_qt_demo.exe
```

Có thể mở `CMakeLists.txt` gốc bằng Qt Creator, chọn kit, thêm biến CMake `PBL2_BUILD_QT_DEMO=ON`, rồi chạy target `pbl2_qt_demo`. Windows/Qt 5 chưa được chạy thực tế trong phiên này.

## Chạy trong workspace hiện tại

Qt runtime 6.8.2 đã tồn tại; header/công cụ phát triển đã được giải nén riêng dưới `/workspace/.pbl2-qt/root/usr`. Bộ cài này và các build output nằm ngoài phần mã nguồn bàn giao.

```bash
export PATH="/workspace/.pbl2-tools/cmake/data/bin:$PATH"
cmake -S /workspace/PBL2 -B /workspace/PBL2/build-qt-demo -DPBL2_BUILD_QT_DEMO=ON -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/workspace/.pbl2-qt/root/usr
cmake --build /workspace/PBL2/build-qt-demo --parallel 2
ctest --test-dir /workspace/PBL2/build-qt-demo --output-on-failure --no-tests=error
```

Môi trường không có desktop gắn trực tiếp để chuyển cửa sổ đến máy người dùng. Đã chạy native qua **offscreen** và **X11/Xvfb**, có ảnh chụp; đây không phải kiểm thử tương tác thủ công trên desktop cá nhân.

Tạo ảnh không cần màn hình:

```bash
QT_QPA_PLATFORM=offscreen ./build-qt-demo/demo/qt-triage/pbl2_qt_demo --screenshot /tmp/an-tam-qt.png
```

`--screenshot` chụp cửa sổ rồi thoát; không phải luồng tương tác console. Muốn mở cửa sổ bình thường trên desktop, chạy chương trình không có tùy chọn này và không đặt `QT_QPA_PLATFORM=offscreen`.

## Thử luồng

- **Tiếp nhận**: tạo hồ sơ, chọn hồ sơ có sẵn hoặc người chưa rõ danh tính. Một bệnh nhân chỉ có một lượt chưa đóng. Lượt mới chờ sàng lọc và sinh hiệu để trống.
- **Phân loại**: nhập đủ sinh hiệu, xem đề xuất, chọn mức điều dưỡng xác nhận, đưa vào hàng. Huyết áp là **tâm thu**, đúng cấu trúc luật nhánh cũ.
- **Hàng chờ**: lọc luồng/mức, tìm tên/mã; gọi theo toàn bộ hàng hiệu lực. Cùng mức ưu tiên tối đa ba lượt quay lại rồi một lượt mới, bộ đếm riêng theo mức. STT hiển thị là dự kiến toàn hàng.
- **Chi tiết**: hồ sơ lâu dài tách biệt lượt khám. Có lịch sử sinh hiệu, tuổi/nguồn tuổi tại lần đánh giá, người xác nhận và timeline.
- **Đang khám**: có thể chuyển cận lâm sàng, nhập kết quả, vào hàng quay lại cùng Visit, lặp nhiều vòng; hoặc kết thúc. Hủy và kết thúc có xác nhận tên/mã.
- **Đánh giá lại**: lưu bản ghi mới, đổi mức nhưng giữ thời điểm/STT lần vào hàng hiện tại.
- **Hồ sơ**: bổ sung danh tính giữ mã và các lượt liên kết. Tuổi chưa biết dùng `-1` trong mô phỏng, hiển thị “Chưa rõ tuổi”.
- **Điều khiển demo**: đặt lại 14 hồ sơ; nạp 5 lượt quay lại + 2 lượt mới cùng mức 3; tiến 10 phút. Đồng hồ chung bắt đầu 09:30 ngày 30/10/2026, UTC+7, chỉ chạy khi bấm nút.
- Cảnh báo sau khi **vượt** 0/10/30/60/120 phút từ lần xác nhận gần nhất. Không tự nâng mức. Nhóm cần đánh giá lại có thể trùng nhóm đang chờ.
- Cửa sổ hẹp chuyển hàng chờ thành thẻ, KPI thành hai cột và điều hướng thành hộp thoại. Cửa sổ desktop tối thiểu 480×600; chưa build Android/iOS. Nút/biểu mẫu dùng focus, Tab và Esc của Qt.

## Tổ chức mã

| Thành phần | Trách nhiệm |
|---|---|
| `model/DemoTriageService.h/.cpp` | Trạng thái, hàng chờ, lịch sử, validation, luật C++ và đồng hồ; không phụ thuộc Qt |
| `model/DemoSeed.cpp` | Dữ liệu mẫu và kịch bản 3:1 |
| `ui/DemoWindow.h/.cpp` | Sidebar, KPI, tìm kiếm, danh sách/bảng/thẻ |
| `ui/DemoDialogs.cpp` | Form tiếp nhận, đánh giá, hồ sơ, chi tiết, cận lâm sàng |
| `ui/DemoStyle.h/.cpp` | QSS, màu, icon vector vẽ bằng QPainter, helper widget |
| `tests/test_demo_state.cpp` | Assert cho trạng thái, ưu tiên, giới hạn và lịch sử |
| `tests/test_demo_widgets.cpp` | QApplication thật, thao tác widget và hộp thoại native |
| `tests/check_rule_parity.py` | Biên dịch đối chiếu C++ gốc và C++ demo, không qua JavaScript |

Service sở hữu dữ liệu theo giá trị trong mảng cố định có kiểm tra giới hạn: 64 hồ sơ, 96 lượt, 32 đánh giá và 16 vòng cận lâm sàng mỗi lượt, 128 mốc timeline. Không dùng STL container thay DSA. Đây là giới hạn rõ ràng của prototype; đạt giới hạn sẽ báo lỗi và giữ dữ liệu, có thể đặt lại demo. Sau này thay service mô phỏng và mảng bằng API/DSA đã thống nhất. UI chỉ đọc dữ liệu const và gọi lệnh service.

Widget dùng parent/child ownership của QObject, dialog có vòng đời theo phạm vi hàm. Service không phụ thuộc widget, không chứa `QString`, container Qt, `cout`/`cin` hay lưu trữ thật. API C++ hiện có trên dev không bị đổi.

## Luật thật và phần mô phỏng

Các điều kiện phân loại C++ được đọc từ `Patient::evaluateTriageLevel` tại commit `95e145f9e95b1f3eb59df6810c5d2341dfe84dc3` của `origin/patient-class`, giữ nguyên thứ tự điều kiện/ngưỡng. Không merge Person/Patient cũ. Luật này là luật đồ án, không được khẳng định là thuật toán lâm sàng đã thẩm định.

Khác biệt có chủ đích: thiếu tuổi không áp nhóm sơ sinh; chỉ chạy các điều kiện GCS/SpO₂ toàn cục và báo đề xuất chưa đầy đủ. Điều dưỡng vẫn phải chọn mức xác nhận. Các bản ghi seed là dữ liệu hư cấu, mức xác nhận có thể khác đề xuất để minh họa điều phối.

Tiếp nhận, vòng đời Visit, quy tắc hai hàng 3:1, đồng hồ, tài khoản và hồ sơ đều là mô phỏng trong bộ nhớ. Chưa kết nối các service/repository thật. Đóng ứng dụng mất các thao tác thử; mở lại nạp seed. Bảng màu mức 1–5 theo đặc tả demo, không được trình bày như bảng màu chuẩn lâm sàng.

Thiết kế theo ảnh FlowMail và đặc tả. Đã đọc [UI UX Pro Max](https://github.com/nextlevelbuilder/ui-ux-pro-max-skill/blob/main/.claude/skills/ui-ux-pro-max/SKILL.md) từ nguồn chính thức vì phiên này không mount bản cài của skill. Áp dụng hướng dẫn về nhãn, tương phản, bàn phím, lỗi cạnh trường và responsive. Các gợi ý trang giới thiệu trong kết quả search không phù hợp dashboard; bố cục/màu theo yêu cầu người dùng.

## Kiểm chứng

Đã chạy Debug thường và Debug ASan/UBSan: **6/6 CTest** ở mỗi cấu hình, không báo lỗi sanitizer (4 kiểm tra sẵn có + 2 kiểm tra demo), với test widget sử dụng Qt offscreen. Các luồng đã kiểm tra: tiếp nhận/phân loại/gọi/kết thúc; cận lâm sàng lặp cùng Visit; bổ sung danh tính; đổi ưu tiên giữ mốc hàng; 3:1; mức khẩn hơn; cảnh báo; chống trùng và chuyển sai trạng thái; bộ lọc không ảnh hưởng gọi; Esc và cửa sổ hẹp.

Đối chiếu 3.936 trường hợp tuổi hợp lệ gồm biên ngưỡng và ngẫu nhiên với hàm C++ gốc:

```bash
python3 demo/qt-triage/tests/check_rule_parity.py
```

Lệnh bổ sung này cần g++, Python và commit gốc còn trong Git local; không phải dependency runtime. Thiếu tuổi được kiểm tra riêng trong `test_demo_state`.

Bật sanitizer bằng thư mục build riêng, thêm `-DPBL2_ENABLE_SANITIZERS=ON`. CTest giữ assertion trong Release và đặt UBSan dừng khi gặp lỗi. Trong sandbox dùng ptrace, LeakSanitizer có thể không chạy được; phiên này đã chạy baseline ngoài lớp sandbox đó, không tắt leak checking để coi như đạt.

Muốn lưu ảnh từ test widget, đặt `PBL2_DEMO_CAPTURE_DIR` đến thư mục muốn lưu trước khi chạy CTest. Test bình thường không ghi ảnh.

Công cụ `tools/check_project_sources.py` hiện quét include/src/tests gốc theo phạm vi có sẵn. Mã demo nằm riêng nên không thuộc phạm vi đó; build/test demo và kiểm tra dependency riêng phục vụ xác minh phần mới. Workflow CI hiện tại chưa bật tùy chọn demo Qt.
