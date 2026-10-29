# PBL2 — Smart Hospital Triage & Flexible Shift Scheduler

Dự án C++17 mô phỏng phân loại cấp cứu và xếp lịch trực. Giao diện người dùng theo định hướng Qt Widgets; quy ước lập trình nằm trong [CLAUDE.md](CLAUDE.md).

## Phạm vi hiện tại trên dev

Repository có `Person`, `Date`, `Gender`, ngoại lệ tự viết `My_stdexcept`, `LinkedList`, `HashTable` và hai chương trình kiểm thử. CMake biên dịch thư viện lõi và các test; CTest chạy các test đã đăng ký.

Chưa có chương trình GUI hoặc target `hospital_gui`. Phần lõi và kiểm thử **không cần Qt**. Khi bổ sung GUI, ứng dụng sẽ dùng điểm vào riêng và liên kết phần lõi, không dùng `main` của test. Đầu ra console của test phục vụ kiểm tra, không phải giao diện người dùng.

## Yêu cầu

- CMake **3.20 trở lên**, công cụ build tương ứng (Make/Ninja…) và compiler hỗ trợ C++17.
- Cấu hình sanitizer cần GCC/Clang và runtime hỗ trợ AddressSanitizer/UndefinedBehaviorSanitizer. Tùy chọn này mặc định tắt; không hỗ trợ MSVC trong cấu hình hiện tại.
- Khi `BUILD_TESTING=ON`, cần Python **3.8 trở lên** cho kiểm tra nguồn và test của công cụ kiểm tra; chỉ dùng thư viện chuẩn Python, không cài gói pip. Build riêng `pbl2_core` với `BUILD_TESTING=OFF` không yêu cầu Python.
- Không cần dịch vụ nền, secret hoặc thư viện nghiệp vụ bổ sung để chạy phần lõi/test.

Mở terminal tại thư mục gốc repository. Trong môi trường Codex của dự án, dùng `/workspace/PBL2`. Nếu CMake chưa có trong PATH nhưng bản cài đặt môi trường đã tồn tại, kích hoạt trước:

```bash
export PATH="/workspace/.pbl2-tools/cmake/data/bin:$PATH"
cmake --version
```

Đây là đường dẫn riêng của môi trường Codex; trên máy cá nhân, cài CMake và compiler bằng công cụ phù hợp của máy.

## Build và chạy toàn bộ test hiện có

Chạy từ thư mục gốc repository:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DPBL2_ENABLE_SANITIZERS=OFF
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure --no-tests=error
```

Chỉ chuyển bước sau khi bước trước thành công. CTest trả mã lỗi nếu test thất bại hoặc không có test nào được tìm thấy. Không cần chạy từng file thực thi bằng tay.

### Bật sanitizer

Dùng thư mục build riêng để không nhầm cấu hình thường với cấu hình kiểm tra bộ nhớ. CTest đặt `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1` để lỗi UBSan làm test thất bại thay vì chỉ in cảnh báo:

```bash
cmake -S . -B build-sanitized -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DPBL2_ENABLE_SANITIZERS=ON
cmake --build build-sanitized --parallel 2
ctest --test-dir build-sanitized --output-on-failure --no-tests=error
```

### Kiểm tra Release

Assertions trong test vẫn hoạt động khi Release mặc định định nghĩa `NDEBUG`: target test dùng `-UNDEBUG` (GCC/Clang) hoặc `/UNDEBUG` (MSVC) để bỏ macro này. Không ghi đè tùy chọn đó khi chạy test.

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DPBL2_ENABLE_SANITIZERS=OFF
cmake --build build-release --parallel 2
ctest --test-dir build-release --output-on-failure --no-tests=error
```

Các lệnh trên dành cho generator một cấu hình như Make/Ninja trên Linux hoặc WSL. Với generator nhiều cấu hình như Visual Studio, chọn cấu hình bằng `cmake --build build --config Debug` và `ctest --test-dir build -C Debug --output-on-failure --no-tests=error`.

### Chỉ build phần lõi

```bash
cmake -S . -B build-core -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DPBL2_ENABLE_SANITIZERS=OFF
cmake --build build-core --target pbl2_core --parallel 2
```

Cấu hình này chủ ý không tạo test. Để kiểm tra chất lượng, dùng cấu hình `BUILD_TESTING=ON` ở trên.

## Target và phạm vi kiểm tra

| Target / tên CTest | Nội dung |
| --- | --- |
| `pbl2_core` | Thư viện tĩnh chứa `src/model/Person.cpp`, công khai thư mục header `include/` |
| `test_person` | Dữ liệu đầu vào, ngày sinh chưa biết, tính tuổi quanh sinh nhật, cập nhật, mã định danh và destructor ảo |
| `test_my_stdexcept` | Vòng đời/sao chép thông báo, giới hạn bộ đệm, bắt lỗi; tích hợp lỗi vượt phạm vi của LinkedList/HashTable |
| `check_project_sources` | Kiểm tra include trực tiếp `stdexcept` và dấu hiệu Qt trong mã lõi |
| `test_project_checks` | Kiểm tra công cụ bằng các tình huống vi phạm, comment/literal và thư mục bị loại trừ |
| `check_core_headers` (target build) | Biên dịch từng header lõi độc lập; cũng được thực hiện khi build toàn bộ |

Hai test C++ là hai chương trình riêng. CTest hiện đăng ký thêm hai kiểm tra Python, tổng cộng **4 kiểm tra**. Kết quả đạt phải có `100% tests passed, 0 tests failed out of 4`, mã thoát 0 và không có lỗi sanitizer. Đây chưa phải bộ kiểm thử đầy đủ mọi hành vi DSA hoặc kiểm thử GUI; `tests/dsa/` chưa có các bộ test độc lập.

Có thể chạy một test từ cấu hình đã build bằng `ctest --test-dir build -R '^test_person$' --output-on-failure --no-tests=error`. Dùng `ctest --test-dir build -V` để xem đầu ra chi tiết.

CMake xuất `compile_commands.json` trong thư mục build cho công cụ đọc mã như clangd. Thư mục `build/`, `build-*/` được Git bỏ qua; không commit output biên dịch.

## Kiểm tra quy ước và header lõi

Các kiểm tra được bật cùng `BUILD_TESTING=ON`. Sau configure, có thể chạy riêng:

```bash
cmake --build build --target check_project_sources check_core_headers --parallel 2
ctest --test-dir build -R '^(check_project_sources|test_project_checks)$' --output-on-failure --no-tests=error
```

- **Include stdexcept:** báo lỗi khi có chỉ thị include trực tiếp `<stdexcept>` hoặc `"stdexcept"` trong mã C/C++ dưới `include/`, `src/`, `tests/`. Dùng ngoại lệ dự án `My_stdexcept`.
- **Qt trong lõi:** kiểm tra các nhóm `model/`, `dsa/`, `service/`, `repository/`, `util/` dưới ba gốc trên. Nhận biết include Qt dạng `QtCore/...`, `QObject`, `qobject.h`…, các kiểu `Q...`, namespace `Qt`, macro `Q_...`/`QT_...` và từ khóa signals/slots/emit. GUI ở `include/ui/`, `src/ui/`, `tests/ui/` được dùng Qt, nhưng vẫn thuộc quy định không include trực tiếp stdexcept.
- **Header độc lập:** chọn `.h`, `.hh`, `.hpp`, `.hxx` thuộc năm nhóm lõi dưới `include/`. Mỗi header được include một mình trong một tệp `.cpp` sinh tại thư mục build và biên dịch thành object riêng, không dùng header được include trước để che dependency thiếu. Hiện có **6 header lõi**. Các mảnh cài đặt `.tpp`/`.inl`/`.ipp` vẫn được kiểm tra quy ước, không coi là header độc lập.

Cùng một công cụ [check_project_sources.py](tools/check_project_sources.py) xác định phạm vi nguồn và danh sách header cho CMake. Nó bỏ qua thư mục `third_party`, `third-party`, `thirdparty`, `vendor`, `external`, `build`, `_deps`, `CMakeFiles`, `.git` và tên bắt đầu bằng `build-`/`cmake-build-` (không phân biệt hoa/thường). Không đi theo symlink, không quét thư viện hệ thống, không truy tìm nội dung include bên ngoài. Các file ngoài ba gốc mã nguồn trên không được quét.

Script bỏ qua comment, string/character literal, kể cả raw string; không mở rộng macro include và không đánh giá nhánh `#if`. Đây là kiểm tra quy ước trực tiếp, không thay thế compiler hay test nghiệp vụ. Nếu đọc nguồn không đầy đủ hoặc nguồn không phải UTF-8 thì báo lỗi thay vì im lặng bỏ qua.

Chạy lại configure khi thêm/xóa header hoặc di chuyển thư mục để CMake cập nhật danh sách. CI luôn configure từ checkout mới. Build Linux trên GitHub thực sự biên dịch các đường dẫn include, nhờ đó phát hiện lỗi chữ hoa/thường; không dựa vào việc Windows có thể chấp nhận đường dẫn sai case.

Có thể chạy công cụ độc lập bằng `python3 tools/check_project_sources.py`, xem danh sách bằng `python3 tools/check_project_sources.py --list-core-headers`, hoặc chạy test công cụ bằng `python3 -m unittest discover -s tests/checks -p 'test_*.py' -v`.

## Kiểm tra tự động trên GitHub

Workflow [Build and test](.github/workflows/build-test.yml) chạy khi mở/cập nhật pull request vào `dev`, push lên `dev`, hoặc chạy thủ công. Ba cấu hình trên Ubuntu 24.04:

- Debug, sanitizer OFF.
- Release, sanitizer OFF (assertions vẫn bật trong test).
- Debug, sanitizer ON.

Mỗi cấu hình thực hiện configure → kiểm tra quy ước nguồn → build lõi, test và từng header độc lập → CTest, không cài hoặc dò Qt. Muốn bắt buộc kiểm tra đạt trước khi merge, chủ repo cần chọn các check tương ứng trong branch protection/ruleset của GitHub; việc thêm workflow không tự bật quy định chặn merge.

## Khởi động trong Codex

1. Dùng checkout hiện có; đọc `CLAUDE.md`, README và kiểm tra `git status --short --branch` trước khi sửa.
2. Kích hoạt CMake nếu cần, rồi chạy cấu hình Debug có sanitizer và CTest theo hướng dẫn trên.
3. Chỉ ghi nhận đạt khi toàn bộ kiểm tra CTest và các header độc lập từ source hiện tại đã đạt. Không cần tiến trình nền.

Tác vụ cloud đã được cách ly; chỉ tạo Git worktree nếu người dùng yêu cầu. Hướng dẫn khởi động trong environment settings được lưu riêng với Git; review/lưu và Publish trong giao diện để kích hoạt phiên bản môi trường mới.

## Tài liệu liên quan

- [Kiểm thử và giao diện Person](tests/model/README.md)
- [Ngoại lệ My_stdexcept](docs/util/README.md)
- [Cấu trúc dữ liệu dùng chung](docs/dsa/README.md)
