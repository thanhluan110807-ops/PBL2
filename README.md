# PBL2 — Smart Hospital Triage & Flexible Shift Scheduler

Dự án C++17 mô phỏng phân loại cấp cứu và xếp lịch trực. Giao diện người dùng theo định hướng Qt Widgets; quy ước lập trình nằm trong [CLAUDE.md](CLAUDE.md).

## Phạm vi hiện tại trên dev

Repository hiện có `Person`, `Date`, `Gender`, ngoại lệ tự viết `My_stdexcept`, `LinkedList`, `HashTable` và hai chương trình kiểm thử bên dưới.

Chưa có `CMakeLists.txt`, chương trình GUI hoặc target `hospital_gui`/`run_tests` trên `dev`. Hướng dẫn hiện tại chỉ build và chạy các test đã có bằng g++; không xác nhận toàn bộ ứng dụng bệnh viện đã hoạt động. Đầu ra console của test phục vụ kiểm tra, không phải giao diện người dùng của ứng dụng.

## Yêu cầu

- Môi trường Linux hoặc WSL với Bash, `mktemp` và g++ hỗ trợ C++17.
- Các lệnh dưới đây dùng AddressSanitizer và UndefinedBehaviorSanitizer, cần compiler/runtime hỗ trợ hai công cụ này. Đã xác minh với g++ 14.2.0 trong môi trường Codex hiện tại.
- Không cần Qt, CMake, dịch vụ nền, secret hoặc thư viện bổ sung để chạy hai test này.

Mở terminal tại thư mục gốc repository, nơi có `include/`, `src/`, `tests/`. Với môi trường Codex của dự án:

```bash
cd /workspace/PBL2
```

Trên máy cá nhân, mở thư mục checkout tương ứng thay vì dùng đường dẫn Codex.

## Build và chạy toàn bộ test hiện có

Chạy nguyên khối sau từ thư mục gốc repository:

```bash
bash <<'BASH'
set -eu
ThuMucKiemThu=$(mktemp -d "${TMPDIR:-/tmp}/pbl2-tests.XXXXXX")

g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -UNDEBUG \
    -fsanitize=address,undefined -fno-omit-frame-pointer -g -I include \
    src/model/Person.cpp tests/model/test_person.cpp \
    -o "$ThuMucKiemThu/test_person"

g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -UNDEBUG \
    -fsanitize=address,undefined -fno-omit-frame-pointer -g -I include \
    tests/util/test_my_stdexcept.cpp \
    -o "$ThuMucKiemThu/test_my_stdexcept"

"$ThuMucKiemThu/test_person"
"$ThuMucKiemThu/test_my_stdexcept"
BASH
```

- Mỗi lần chạy tạo thư mục tạm mới bên ngoài repository để không dùng nhầm chương trình biên dịch từ lần trước.
- `-I include` giúp tìm header qua các đường dẫn như `model/Person.h`, `dsa/HashTable.h` và `util/My_stdexcept.h`.
- Test dùng `assert`; `-UNDEBUG` giữ assertions hoạt động. Không thêm `-DNDEBUG` vào lệnh kiểm thử.
- `set -e` dừng khi biên dịch hoặc kiểm thử thất bại. Không bỏ qua mã thoát lỗi hay chỉ dựa vào một chương trình test cũ còn trên máy.

## Kết quả và phạm vi kiểm tra

Lệnh hoàn tất thành công phải có **mã thoát 0**, cả hai dòng sau và không có báo lỗi sanitizer:

```text
PASS: Person - construction, validation, age, updates, identity, virtual destructor
PASS: My_stdexcept - owned message, copy, bounds, catch, DSA integration
```

| Test | Nội dung được kiểm tra |
| --- | --- |
| `tests/model/test_person.cpp` | Tạo người chưa biết thông tin, kiểm tra đầu vào, tính tuổi quanh sinh nhật, giới hạn tuổi, cập nhật thất bại giữ nguyên dữ liệu và destructor ảo |
| `tests/util/test_my_stdexcept.cpp` | Vòng đời và sao chép thông báo lỗi, giới hạn bộ đệm, bắt lỗi theo mã; tích hợp lỗi vượt phạm vi trong LinkedList và HashTable |

Đây là hai chương trình test, không phải kiểm thử đầy đủ mọi hành vi DSA hoặc kiểm thử GUI. `tests/dsa/` hiện chưa có bộ test độc lập cho từng container.

## Khởi động trong Codex

1. Dùng checkout hiện có tại `/workspace/PBL2`; đọc `CLAUDE.md` và kiểm tra `git status --short --branch` trước khi làm việc.
2. Chạy khối build/test ở trên. Chương trình test kết thúc sau khi kiểm tra, không cần giữ tiến trình chạy nền.
3. Chỉ ghi nhận đạt khi cả hai chương trình được biên dịch từ source hiện tại và chạy thành công. Khi lỗi, giữ kết quả thất bại để chẩn đoán thay vì mô tả môi trường đã sẵn sàng.

Tác vụ cloud đã được cách ly; dùng checkout sẵn có, chỉ tạo Git worktree nếu người dùng yêu cầu. Thay đổi cấu hình khởi động Codex được lưu trong environment settings, riêng với commit tài liệu trong Git; cần review/lưu và Publish trong giao diện để kích hoạt phiên bản môi trường mới.

## Tài liệu liên quan

- [Kiểm thử và giao diện Person](tests/model/README.md)
- [Ngoại lệ My_stdexcept](docs/util/README.md)
- [Cấu trúc dữ liệu dùng chung](docs/dsa/README.md)
