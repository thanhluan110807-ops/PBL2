# CLAUDE.md — Smart Hospital Triage & Flexible Shift Scheduler

## 1. Tổng quan dự án

Hệ thống C++ hướng đối tượng mô phỏng hoạt động Khoa Cấp cứu của một bệnh viện công lập hạng I.
Hệ thống giải quyết hai bài toán:

1. **Phân loại cấp cứu thông minh**: phân loại bệnh nhân theo 5 cấp độ, quản lý hàng đợi ưu tiên, kiểm soát bệnh nhân quay lại khám (reentrant patients).
2. **Phân lịch trực linh hoạt**: tự động xếp ca trực cho y bác sĩ, theo dõi chỉ số mệt mỏi tích lũy để tránh burnout.

Đây là đồ án sinh viên năm 2. Mục tiêu là **kiến trúc sạch, dễ hiểu, dễ bảo vệ trước giảng viên**, không phải code "cao siêu".

## 2. Nguyên tắc bắt buộc (QUAN TRỌNG)

- Code **đơn giản, dễ đọc**, phù hợp trình độ sinh viên năm 2. Không dùng template metaprogramming, không dùng trick phức tạp.
- Kiến trúc **chuẩn doanh nghiệp**: chia tầng rõ ràng, tách interface và implementation, mỗi class chỉ làm một việc.
- **Giao diện người dùng chỉ dùng GUI bằng Qt Widgets**, không xây dựng menu hoặc luồng tương tác console. Qt được phép dùng cho tầng giao diện; phần nghiệp vụ và cấu trúc dữ liệu độc lập với Qt. Các thư viện bên thứ ba khác cần được thống nhất trước khi thêm.
- Ưu tiên rõ ràng hơn tối ưu. Chỉ tối ưu khi thật sự cần.
- Mỗi thay đổi phải giải thích ngắn gọn bằng tiếng Việt: làm gì và vì sao.
- Không tự ý đổi cấu trúc thư mục hay đổi tên class đã có mà không hỏi.

## 3. Công nghệ

- Ngôn ngữ: C++17
- Compiler: g++ (hoặc clang++), bật `-Wall -Wextra`
- Build: CMake (tối thiểu 3.10)
- Giao diện: Qt Widgets (Qt 6 hoặc Qt 5 theo cấu hình build), chương trình GUI là điểm chạy chính
- Test: viết test đơn giản bằng `assert` hoặc một file `main` test riêng, không dùng framework nặng
- Lưu trữ dữ liệu: file văn bản/CSV (không dùng database)

## 4. Cấu trúc thư mục

Cấu trúc đích được thống nhất như sau. Việc cập nhật tài liệu không đồng nghĩa các tệp mã nguồn hiện có đã được di chuyển.

```text
PBL2/
├── include/
│   ├── dsa/
│   │   ├── Vector.h
│   │   ├── LinkedList.h
│   │   ├── HashTable.h
│   │   └── PriorityQueue.h
│   ├── model/
│   ├── service/
│   ├── repository/
│   └── ui/
│
├── src/
│   ├── model/
│   ├── service/
│   ├── repository/
│   └── ui/
│
├── tests/
│   └── dsa/
│       ├── test_vector.cpp
│       ├── test_linked_list.cpp
│       ├── test_hash_table.cpp
│       └── test_priority_queue.cpp
│
└── docs/
    └── dsa/
        └── README.md
```

- `include/dsa/`: các cấu trúc dữ liệu dùng chung, độc lập với nghiệp vụ bệnh viện và Qt. Các lớp template thường đặt cả khai báo và cài đặt trong header.
- `include/model/`, `service/`, `repository/`, `ui/`: header các lớp theo trách nhiệm; phần cài đặt `.cpp` nằm trong nhóm tương ứng dưới `src/`. `ui/` chứa giao diện Qt Widgets.
- `tests/dsa/`: kiểm thử độc lập từng cấu trúc dữ liệu; `docs/dsa/README.md` giải thích giao diện sử dụng, quan hệ phụ thuộc và độ phức tạp của chúng.
- Tên tệp và đường dẫn include phải khớp chữ hoa/thường, ví dụ `HashTable.h`, `LinkedList.h`. Khi di chuyển hoặc đổi tên tệp, cập nhật đồng bộ `#include`, cấu hình build và kiểm thử.
- Cây trên tập trung vào mã nguồn và DSA; tiếp tục giữ các tệp cấu hình/tài liệu gốc, `.agents/` và dữ liệu cần thiết như `data/` theo chức năng của chúng.

## 5. Kiến trúc phân tầng

```
UI (Qt Widgets: MainWindow, các dialog)
   ↓
Service (nghiệp vụ: phân loại, xếp lịch, tính mệt mỏi)
   ↓
Repository (đọc/ghi dữ liệu)
   ↓
Model (các thực thể dữ liệu)
```

Quy tắc:
- Tầng trên được gọi tầng dưới, **không gọi ngược lại**.
- UI **không chứa logic nghiệp vụ**; nhận thao tác qua nút bấm/biểu mẫu, gọi service và hiển thị kết quả bằng bảng, nhãn hoặc hộp thoại.
- Model và service cung cấp dữ liệu/kết quả hoặc báo lỗi cho bên gọi; không phụ thuộc widget Qt và không dùng `cout`/`cin` làm giao diện người dùng.
- Các hàm như `HienThiThongTin()` chỉ để in console được loại bỏ khi chuyển luồng tương ứng sang GUI; cập nhật đồng bộ khai báo, `override` và nơi gọi. Giữ các hàm truy xuất dữ liệu, lưu file và destructor ảo khi cần đa hình.
- Log chẩn đoán và đầu ra của chương trình kiểm thử được phép giữ riêng; chúng không phải giao diện console của ứng dụng.
- Model **không biết gì** về UI hay Repository.
- Dùng interface (lớp trừu tượng) cho Repository để dễ thay đổi nơi lưu trữ.

## 6. Nghiệp vụ chính

### 6.1. Phân loại 5 cấp độ (Triage)
| Cấp | Tên | Ý nghĩa | Thời gian chờ tối đa |
|-----|-----|---------|----------------------|
| 1 | Hồi sức | Đe dọa tính mạng ngay lập tức | 0 phút |
| 2 | Khẩn cấp | Nguy cơ cao, cần xử lý rất nhanh | ~10 phút |
| 3 | Cấp cứu | Cần xử lý sớm, tình trạng ổn định tương đối | ~30 phút |
| 4 | Bán cấp | Ít khẩn cấp | ~60 phút |
| 5 | Không cấp | Có thể chờ lâu | ~120 phút |

(Các mốc thời gian là giá trị mẫu, chỉnh lại theo yêu cầu đề tài.)

- Hàng đợi ưu tiên: cấp thấp số hơn được khám trước; cùng cấp thì ai đến trước khám trước.
- Nếu bệnh nhân chờ quá lâu so với mốc cho phép, hệ thống cảnh báo hoặc nâng ưu tiên.

### 6.2. Bệnh nhân quay lại (Reentrant)
- Lưu lịch sử các lần khám của mỗi bệnh nhân.
- Nếu bệnh nhân quay lại trong khoảng thời gian ngắn, đánh dấu và cân nhắc nâng mức ưu tiên.

### 6.3. Lịch trực linh hoạt (Flexible Shifts)
- Các loại ca: sáng, chiều, đêm (có thể điều chỉnh độ dài ca).
- Xếp ca tự động theo nhu cầu từng khung giờ và số bác sĩ khả dụng.
- Ràng buộc cứng: không xếp bác sĩ vào hai ca chồng nhau, đảm bảo thời gian nghỉ tối thiểu giữa các ca.

### 6.4. Chỉ số mệt mỏi (Fatigue Score)
- Mỗi bác sĩ có điểm mệt mỏi tích lũy, tăng theo giờ trực (ca đêm tính hệ số cao hơn), giảm khi được nghỉ.
- Vượt ngưỡng thì không xếp thêm ca, ưu tiên xếp người có điểm thấp.
- Công thức viết rõ trong code và trong `docs/`, dễ giải thích.

## 7. Quy ước code

- Tên class: `PascalCase` (`TriageService`)
- Từ nay, tên biến và hàm do dự án định nghĩa dùng **tiếng Việt không dấu**, viết hoa chữ cái đầu của từng từ theo **PascalCase**. Ví dụ biến: `TenBien`, `HoTen`, `SoGioLamViec`; ví dụ hàm: `TinhDiemMetMoi()`, `XepLichTruc()`, `LayMaNhanVien()`.
- Áp dụng cho biến cục bộ, tham số, biến thành viên, biến toàn cục, hằng số, hàm tự do và phương thức. Biến thành viên cũng dùng `PascalCase`, không thêm hậu tố `_`; dùng `this->` khi cần phân biệt với tham số cùng tên.
- Tên cần diễn đạt đầy đủ ý nghĩa nghiệp vụ; hàm dùng động từ như `Lay`, `Dat`, `Tinh`, `KiemTra`, `CapNhat`; biến điều kiện có thể dùng `La`, `Co`, `Da` như `LaCaDem`, `CoTheNhanCa`.
- Hằng số cũng dùng tiếng Việt không dấu theo `PascalCase`, ví dụ `SoGioToiDaTrongTuan`.
- Giữ đúng các tên do C++ hoặc giao diện bên ngoài quy định: `main`, toán tử `operator...`, API thư viện và phương thức phải khớp tên để override. Constructor/destructor theo tên class.
- Quy ước áp dụng cho tên mới từ thời điểm này. Khi đổi tên có sẵn, cập nhật đồng bộ khai báo, định nghĩa, nơi sử dụng và kiểm thử; việc đổi tên toàn bộ mã cũ là một thay đổi riêng.
- File: tên trùng tên class (`Patient.h`, `Patient.cpp`)
- Mỗi header có `#pragma once`
- Class thông thường có cặp file `.h` / `.cpp`; class template có thể đặt cả khai báo và cài đặt trong `.h` như các cấu trúc dữ liệu ở `include/dsa/`.
- Dùng `const` và tham chiếu `const&` khi truyền đối tượng lớn
- Ưu tiên `std::vector`, `std::map`, `std::priority_queue`, `std::string`
- Dùng smart pointer (`std::unique_ptr`, `std::shared_ptr`), **không dùng `new`/`delete` thủ công**
- Hàm ngắn (tốt nhất dưới ~30 dòng), tên hàm nói rõ việc nó làm
- Comment bằng tiếng Việt cho phần nghiệp vụ khó hiểu, không comment thừa

## 8. Nguyên tắc OOP cần thể hiện

- **Đóng gói**: dữ liệu `private`, truy cập qua getter/setter hợp lý
- **Kế thừa / đa hình**: dùng khi thật sự hợp lý (ví dụ `Staff` → `Doctor`, `Nurse`)
- **Trừu tượng**: interface cho Repository, chiến lược xếp lịch
- Áp dụng SOLID ở mức cơ bản, không lạm dụng design pattern

## 9. Lệnh thường dùng

Các lệnh dưới đây áp dụng trên branch đã tích hợp `CMakeLists.txt` và các target tương ứng. Điểm chạy ứng dụng là `hospital_gui`; `run_tests` là chương trình kiểm thử riêng.

```bash
# Cấu hình và build giao diện (cần Qt Widgets)
cmake -S . -B build
cmake --build build --target hospital_gui

# Chạy ứng dụng GUI
./build/hospital_gui

# Build và chạy kiểm thử nghiệp vụ riêng
cmake --build build --target run_tests
./build/run_tests
```

Nếu thiếu Qt hoặc target `hospital_gui`, xử lý phụ thuộc/cấu hình build trước; không coi target console là ứng dụng thay thế đã đáp ứng yêu cầu GUI.

## 10. Quy trình Git

- Branch chính: `main` (luôn chạy được)
- Branch tính năng: `feature/<ten-tinh-nang>` (ví dụ `feature/triage-queue`)
- Branch sửa lỗi: `fix/<mo-ta>`
- Commit message ngắn gọn, rõ nghĩa, ví dụ: `feat: them hang doi uu tien cho benh nhan`
- Trước khi merge vào `main`: code phải build được và test qua
- Không commit thư mục `build/` và file tạm

## 11. Cách Claude nên làm việc với dự án này

- Đọc cấu trúc hiện có trước khi viết code mới, giữ đúng phong cách sẵn có.
- Khi yêu cầu chưa rõ, hỏi lại thay vì đoán.
- Làm từng bước nhỏ: viết, build thử, rồi mới sang bước tiếp theo.
- Giải thích code bằng ngôn ngữ đơn giản, vì sinh viên cần hiểu để bảo vệ đồ án.
- Không thêm tính năng ngoài phạm vi đề tài.
- Dùng Qt Widgets cho GUI theo quy định ở mục 2; giữ giải pháp đơn giản, phù hợp trình độ đồ án.

## 12. Ngoài phạm vi (không làm)

- Giao diện console tương tác, giao diện web và chức năng kết nối mạng
- Cơ sở dữ liệu thật (SQL)
- Đa luồng phức tạp
- Trí tuệ nhân tạo / học máy (phần "thông minh" chỉ là luật và công thức rõ ràng)
