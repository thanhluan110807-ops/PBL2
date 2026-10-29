# PBL2 — Smart Hospital Triage & Flexible Shift Scheduler

Dự án C++17 mô phỏng phân loại cấp cứu và xếp lịch trực. Giao diện người dùng theo định hướng Qt Widgets; quy ước lập trình nằm trong [CLAUDE.md](CLAUDE.md).

## Phạm vi hiện tại trên dev

Repository có `Person`, `Date`, `Gender`, ngoại lệ tự viết `My_stdexcept`, `LinkedList`, `HashTable` và hai chương trình kiểm thử. CMake biên dịch thư viện lõi và các test; CTest chạy các test đã đăng ký.

Chưa có chương trình GUI hoặc target `hospital_gui`. Phần lõi và kiểm thử **không cần Qt**. Khi bổ sung GUI, ứng dụng sẽ dùng điểm vào riêng và liên kết phần lõi, không dùng `main` của test. Đầu ra console của test phục vụ kiểm tra, không phải giao diện người dùng.

## Yêu cầu

- CMake **3.20 trở lên**, công cụ build tương ứng (Make/Ninja…) và compiler hỗ trợ C++17.
- Cấu hình sanitizer cần GCC/Clang và runtime hỗ trợ AddressSanitizer/UndefinedBehaviorSanitizer. Tùy chọn này mặc định tắt; không hỗ trợ MSVC trong cấu hình hiện tại.
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

Hai test là hai chương trình riêng. Kết quả đạt phải có `100% tests passed, 0 tests failed out of 2`, mã thoát 0 và không có lỗi sanitizer. Đây chưa phải bộ kiểm thử đầy đủ mọi hành vi DSA hoặc kiểm thử GUI; `tests/dsa/` chưa có các bộ test độc lập.

Có thể chạy một test từ cấu hình đã build bằng `ctest --test-dir build -R '^test_person$' --output-on-failure --no-tests=error`. Dùng `ctest --test-dir build -V` để xem đầu ra chi tiết.

CMake xuất `compile_commands.json` trong thư mục build cho công cụ đọc mã như clangd. Thư mục `build/`, `build-*/` được Git bỏ qua; không commit output biên dịch.

## Kiểm tra tự động trên GitHub

Workflow [Build and test](.github/workflows/build-test.yml) chạy khi mở/cập nhật pull request vào `dev`, push lên `dev`, hoặc chạy thủ công. Ba cấu hình trên Ubuntu 24.04:

- Debug, sanitizer OFF.
- Release, sanitizer OFF (assertions vẫn bật trong test).
- Debug, sanitizer ON.

Mỗi cấu hình thực hiện configure → build → CTest, không cài hoặc dò Qt. Muốn bắt buộc kiểm tra đạt trước khi merge, chủ repo cần chọn các check tương ứng trong branch protection/ruleset của GitHub; việc thêm workflow không tự bật quy định chặn merge.

## Khởi động trong Codex

1. Dùng checkout hiện có; đọc `CLAUDE.md`, README và kiểm tra `git status --short --branch` trước khi sửa.
2. Kích hoạt CMake nếu cần, rồi chạy cấu hình Debug có sanitizer và CTest theo hướng dẫn trên.
3. Chỉ ghi nhận đạt khi hai test từ source hiện tại chạy thành công. Không cần tiến trình nền.

Tác vụ cloud đã được cách ly; chỉ tạo Git worktree nếu người dùng yêu cầu. Hướng dẫn khởi động trong environment settings được lưu riêng với Git; review/lưu và Publish trong giao diện để kích hoạt phiên bản môi trường mới.

## Tài liệu liên quan

- [Kiểm thử và giao diện Person](tests/model/README.md)
- [Ngoại lệ My_stdexcept](docs/util/README.md)
- [Cấu trúc dữ liệu dùng chung](docs/dsa/README.md)
