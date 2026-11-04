# Học phần đọc file riêng

Bản push này giữ việc bạn đã xóa `DocFile.h` và `DocFile.cpp`; không đưa lại bản mẫu cùng tên trong thư mục bài học.

Phần đọc/ghi được tách riêng trong code chính:

- `include/repository/StaffFileManager.h` và `src/repository/StaffFileManager.cpp`: file nhân viên.
- `include/repository/ScheduleFileManager.h` và `src/repository/ScheduleFileManager.cpp`: file lịch.
- `include/repository/AdminFileManager.h` và `src/repository/AdminFileManager.cpp`: snapshot chung cho giao diện.

Đọc [hướng dẫn các lớp UML](../HOAN-THIEN-CAC-LOP-TRONG-ANH-UML.md), mục Registry và FileManager. Học luồng mở file → đọc header → đọc từng dòng → kiểm tra → trả dữ liệu mới trước khi học backtracking. Các module file không gọi thuật toán quay lui.
