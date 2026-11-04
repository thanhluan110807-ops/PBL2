# Qt demo core integration — Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans to implement task-by-task.

**Goal:** Nối demo Qt An Tâm với Registry, TriageService, VisitSearchService và CSV hiện có.
**Architecture:** Widget mượn Registry/service, dùng mã ổn định cho thao tác; main sở hữu Registry thô. Giữ QSS và bố cục, bỏ mô phỏng khỏi target chạy.
**Tech Stack:** C++17, Qt5/6 Widgets, CMake, assert.
**Spec:** docs/superpowers/specs/2026-11-04-qt-demo-core-integration-design.md

## Global Constraints
- Bảo toàn thay đổi cục bộ; làm trên work theo yêu cầu, không commit/push.
- Không dùng memory hoặc class trừu tượng mới; nghiệp vụ độc lập Qt.
- Chỉ đọc CSV Patient lúc khởi động, không ghi file hoặc khôi phục Visit.
- Không làm áp lực hàng đợi, lịch trực, tìm theo bác sĩ hay đề xuất tương lai.

## Review Focus
- Patient CSV chưa có Visit vẫn hiện và chọn được.
- Nhập trùng mã/CCCD, Visit đang mở không làm mất dữ liệu hoặc tạo thêm hồ sơ khi thử lại.
- Hai hàng cùng mức đổi lượt đúng lõi, xem/lọc không làm đổi lượt.
- Hủy hộp thoại, lỗi CSV, lỗi số đo giữ nguyên dữ liệu và còn sửa được.
- Ngày tiếp nhận dùng UTC+7 và danh tính bổ sung không làm mất lịch sử.

### Task 1: API liệt kê Patient
Files: include/repository/PatientRegistry.h, src/repository/PatientRegistry.cpp, tests/repository/test_patient_registry.cpp.
Interface: Vector<const Patient*> LayDanhSachBenhNhan() const; chỉ mượn, không cam kết thứ tự.
- [x] Test Registry rỗng/Patient chưa khám/con trỏ gốc và bản danh sách độc lập.
- [x] Build để thấy API thiếu, thêm API, chạy test_patient_registry.

### Task 2: Nối cửa sổ và hộp thoại
Files: demo/qt-triage/ui/DemoWindow.h/.cpp, DemoDialogs.cpp, DemoStyle.h/.cpp, tests/test_demo_widgets.cpp, CMakeLists.txt trong demo.
Interface: DemoWindow(PatientRegistry&, TriageService&, VisitSearchService&, QWidget* = nullptr); thao tác nhận const std::string& mã Patient/Visit.
- [x] Viết test widget nhận service thật, tiếp nhận, đánh giá, lỗi số đo, gọi tiếp, CLS/quay lại/đóng/hủy, tra cứu, hồ sơ chưa khám.
- [x] Chạy test trước khi đổi constructor, xác nhận chưa nối được.
- [x] Thay phụ thuộc demo state bằng pbl2_core. Giữ widget bố cục/QSS, thay nguồn dữ liệu và thao tác; bỏ số phút chờ vì API hiện chưa cung cấp thời điểm vào hàng, hiển thị thời điểm tiếp nhận có nhãn rõ.
- [x] Bộ lọc tên/ngày/mã gọi VisitSearchService; trạng thái chưa đánh giá là ChoKhamLanDau + SoDanhGia()==0.
- [x] Chạy widget offscreen, chụp và kiểm tra bố cục rộng/hẹp.

### Task 3: Khởi động CSV, kiểm chứng và tài liệu
Files: demo/qt-triage/main.cpp, ui/DemoStartup.h/.cpp, tests/test_demo_widgets.cpp, README.md của demo và gốc.
Interface: PatientRegistry* DemoUi::ChonDuLieu(QWidget* = nullptr); nullptr nếu thoát, bên gọi delete sau service.
- [x] Test chọn rỗng, hủy chọn file, CSV hợp lệ và báo lỗi CSV không tự sang dữ liệu rỗng.
- [x] Main gọi khởi động, tạo Registry trước service và hủy sau cùng, có catch thu hồi bộ nhớ.
- [x] Thêm CSV hư cấu để chạy thử, cập nhật hướng dẫn đúng giới hạn lưu dữ liệu.
- [x] Build Debug + toàn bộ CTest, test GUI offscreen; kiểm tra source, diff, review độc lập trước bàn giao.

## Progress / rulings
- User đã duyệt thiết kế và yêu cầu bắt đầu; triển khai trực tiếp, không hỏi lại kế hoạch.
- Dùng work hiện tại vì service/file reader phụ thuộc thay đổi chưa commit của người dùng; không tách checkout làm mất ngữ cảnh.
- Qt runtime có sẵn, thiếu dev kit; tải gói dev vào /workspace/.pbl2-qt để build, không đổi hệ thống.

- Task 1 complete: test build failed for missing API, implementation added; test_patient_registry passed.
- Task 2 complete: new widget test initially failed to compile against old constructor; real-core integration now passes widget tests including validation, CLS, identity, close/cancel and search.
- Task 3 complete: startup test initially failed to link ChonDuLieu; implemented and passed CSV/error/cancel tests.
- Final review: independent read-only review found no Critical/Important; KPI stale filters was reported Minor. Treated as user-visible misleading navigation, added failing regression then cleared filters; passed.
- Narrow layout: test exposed horizontal overflow at 520px; wrapped persistence/result labels; regression passed.
- Validation: Qt 5.15.15 / GCC 14.2 / CMake 3.31.10 Debug 16/16. ASan/UBSan 16/16 with ASAN_OPTIONS=detect_leaks=0. No leak-check or Qt6 result claimed.
- Not evaluated: allocation exhaustion in Qt event callbacks; no requirement added or claim made about recovery from process-wide out-of-memory.
- No commit/push; existing local PatientFileManager/VisitSearchService changes preserved.
