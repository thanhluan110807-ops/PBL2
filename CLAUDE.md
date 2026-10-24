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
- **Chỉ dùng thư viện chuẩn C++ (STL)**. Không thêm thư viện bên thứ ba (Boost, Qt, v.v.).
- Ưu tiên rõ ràng hơn tối ưu. Chỉ tối ưu khi thật sự cần.
- Mỗi thay đổi phải giải thích ngắn gọn bằng tiếng Việt: làm gì và vì sao.
- Không tự ý đổi cấu trúc thư mục hay đổi tên class đã có mà không hỏi.

## 3. Công nghệ

- Ngôn ngữ: C++17
- Compiler: g++ (hoặc clang++), bật `-Wall -Wextra`
- Build: CMake (tối thiểu 3.10)
- Test: viết test đơn giản bằng `assert` hoặc một file `main` test riêng, không dùng framework nặng
- Lưu trữ dữ liệu: file văn bản/CSV (không dùng database)

## 4. Cấu trúc thư mục

```
project/
├── CMakeLists.txt
├── CLAUDE.md
├── README.md
├── include/            # file header (.h)
│   ├── model/          # Patient, Doctor, Shift, TriageLevel...
│   ├── service/        # TriageService, SchedulerService, FatigueService
│   ├── repository/     # đọc/ghi dữ liệu
│   ├── ui/             # menu console
│   └── util/           # hàm tiện ích (thời gian, chuỗi)
├── src/                # file cài đặt (.cpp), cấu trúc giống include/
├── data/               # dữ liệu mẫu (CSV)
├── tests/              # file test
└── docs/               # tài liệu, sơ đồ UML
```

## 5. Kiến trúc phân tầng

```
UI (console menu)
   ↓
Service (nghiệp vụ: phân loại, xếp lịch, tính mệt mỏi)
   ↓
Repository (đọc/ghi dữ liệu)
   ↓
Model (các thực thể dữ liệu)
```

Quy tắc:
- Tầng trên được gọi tầng dưới, **không gọi ngược lại**.
- UI **không chứa logic nghiệp vụ**.
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
- Tên hàm, biến: `camelCase` (`calculateFatigue`)
- Biến thành viên: thêm hậu tố `_` (`name_`)
- Hằng số: `UPPER_SNAKE_CASE`
- File: tên trùng tên class (`Patient.h`, `Patient.cpp`)
- Mỗi header có `#pragma once`
- Một class một cặp file `.h` / `.cpp`
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

```bash
# Build
mkdir -p build && cd build
cmake ..
make

# Chạy chương trình
./build/hospital_app

# Chạy test
./build/run_tests
```

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
- Không dùng thư viện ngoài, không dùng kỹ thuật quá nâng cao.

## 12. Ngoài phạm vi (không làm)

- Giao diện đồ họa, web, hay kết nối mạng
- Cơ sở dữ liệu thật (SQL)
- Đa luồng phức tạp
- Trí tuệ nhân tạo / học máy (phần "thông minh" chỉ là luật và công thức rõ ràng)