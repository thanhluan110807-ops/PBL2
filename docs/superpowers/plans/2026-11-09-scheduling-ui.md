# Scheduling UI Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Hoàn thiện AdminPanel Qt nối demo An Tâm theo bản thiết kế đã duyệt.

**Architecture:** Giữ AdminPanel và các service/file hiện có; thay đổi phần lịch ở widget, QSS và test. Theo yêu cầu bổ sung, DemoWindow nhúng AdminPanel trong vùng nội dung chính.

**Tech Stack:** C++17, Qt Widgets/QSS, CMake/CTest, assert.

**Spec:** `docs/superpowers/specs/2026-11-09-scheduling-ui-design.md`

## Global Constraints

- Giữ tên class và API nghiệp vụ; không thêm thư viện UI.
- Không triển khai đăng nhập/phân quyền tương lai.
- Giữ định dạng snapshot phần B; theo yêu cầu bổ sung, tự nạp nguồn liên kết và ghi khi bấm Lưu thay đổi.
- Giữ file cục bộ không liên quan và chỉ commit/push khi người dùng yêu cầu.

## Review Focus

- Panel trống hướng dẫn được bước đầu và hiện số 0 đúng.
- Số liệu nhân sự/ca/phân công đúng sau nạp mẫu, xếp lịch và tải file.
- Chọn nhân sự/ca/phân công hiện chi tiết hữu ích, không chỉ tooltip.
- Người thay hiển thị tên và mã, thao tác thay vẫn truyền mã đúng.
- Cửa sổ hẹp còn truy cập được nút và biểu mẫu.

---

### Task 1: Tổng quan và trạng thái

**Files:** `demo/qt-triage/ui/AdminPanel.h/.cpp`, `demo/qt-triage/tests/test_admin_widgets.cpp`

- [x] Thêm assert widget cho tổng quan rỗng và tổng quan sau nạp/xếp lịch; chạy thấy fail.
- [x] Thêm tiêu đề, số liệu thật, hướng dẫn bước tiếp và thông báo thao tác/lưu; chạy test đạt.

### Task 2: Bố cục và chi tiết từng tab

**Files:** `demo/qt-triage/ui/AdminPanel.cpp`, `demo/qt-triage/tests/test_admin_widgets.cpp`

- [x] Thêm assert chi tiết nhân sự/ca/phân công và người thay có tên + mã; chạy thấy fail.
- [x] Bố cục nhóm rõ theo luồng, điều khiển có nhãn/buddy, vùng chi tiết đọc được, nút ưu tiên hợp lý; chạy test đạt.

### Task 3: Tích hợp và kiểm chứng

**Files:** `demo/qt-triage/ui/AdminPanel.cpp`, `demo/qt-triage/tests/test_admin_widgets.cpp`, `demo/qt-triage/README.md`, spec.

- [x] Kiểm tra bố cục 580×680 và 1050×760, test thao tác Qt và lưu/nạp qua demo.
- [x] Cập nhật tài liệu, build và CTest; kiểm tra diff/status.

### Điều chỉnh đã triển khai theo yêu cầu bổ sung

- [x] Chuyển nhân sự/lịch thành trang trong cửa sổ chính, giữ dữ liệu khi đổi trang và cảnh báo khi đóng với thay đổi chưa lưu.
- [x] Bỏ hộp chọn file và nút nhập/xuất khỏi GUI, dùng một nút xanh Lưu thay đổi ghi nguồn liên kết.
- [x] Kiểm tra cửa sổ chính ở 520×800 và 1440×980, widget lịch độc lập ở 580×680 và 1050×760.
