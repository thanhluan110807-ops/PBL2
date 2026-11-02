# PatientRegistry Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [x]`) syntax for tracking.

**Goal:** Tạo PatientRegistry theo ba quy tắc và quyền sở hữu người dùng đã chốt.

**Architecture:** Hai HashTable tự viết sở hữu các đối tượng cấp phát riêng; tra
CCCD/tên/lịch sử bằng duyệt. Patient public chỉ đọc; cập nhật danh tính qua Registry.

**Tech Stack:** C++17, CMake, assert, DSA tự viết, không thêm thư viện.

**Spec:** docs/superpowers/specs/2026-11-01-patient-registry-design.md

## Global Constraints

- C++17; không Qt trong lõi; không STL container thay DSA.
- Tên biến/hàm tiếng Việt không dấu PascalCase; comment tiếng Việt.
- Bảo toàn file khác; không commit/push; làm tại workspace hiện có theo yêu cầu.
- Không hỏi lại quy tắc đã chốt; triển khai trực tiếp theo xác nhận của người dùng.

## Review Focus

- CCCD của Patient chưa biết ngày sinh vẫn được lưu, CCCD rỗng không trùng.
- Không cho bên gọi sửa CCCD qua con trỏ mượn để vượt kiểm tra của Registry.
- Tìm tên tiếng Việt có dấu phải không phân biệt hoa/thường, không gộp tên trùng.
- Khi cấp phát lỗi, hồ sơ/chỉ mục cũ giữ nguyên và không rò rỉ đối tượng mới.
- CCCD cũ được giải phóng khi cập nhật, CCCD mới trùng bị chặn trước sửa hồ sơ.

### Task 1: CCCD tùy chọn ở Patient

**Files:** include/model/Patient.h, src/model/Patient.cpp, tests/model/test_patient.cpp.
**Interfaces:** Giữ constructor cũ qua tham số mặc định; thêm SoCanCuoc tùy chọn sau
GioiTinh hoặc NgaySinh; không ép ngày sinh/tên khi chỉ biết CCCD.

- [x] Viết test constructor có CCCD, không ngày sinh, có ngày sinh; assert giữ số 0 đầu.
- [x] Build test_patient và thấy thất bại do chưa có overload.
- [x] Thêm tham số và khởi tạo SoCanCuoc; chạy test_patient đạt.

### Task 2: Registry và tích hợp kiểm thử

**Files:** include/repository/PatientRegistry.h, src/repository/PatientRegistry.cpp,
tests/repository/test_patient_registry.cpp, CMakeLists.txt,
docs/repository/README.md.
**Interfaces:** Chính xác API trong spec; dùng constructor Patient từ Task 1.

- [x] Viết test các trường hợp trong spec/Review Focus, compile thấy thiếu Registry.
- [x] Cài API, quyền sở hữu, kiểm tra trùng, tìm tên, trạng thái mở và lịch sử.
- [x] Đăng ký source/test với CMake, -UNDEBUG, timeout và sanitizer giống test hiện có.
- [x] Chạy test Registry đạt, ghi hướng dẫn tiếp nhận tìm CCCD -> xác nhận -> dùng mã
      hiện có -> tạo Visit nếu không còn Visit mở; không tự tái sử dụng trong hàm thêm.

### Task 3: Cấp phát thất bại và kiểm chứng toàn bộ

**Files:** tests/repository/test_patient_registry_allocation.cpp, CMakeLists.txt.
**Interfaces:** Cùng API Task 2; global new thay thế chỉ trong binary test riêng.

- [x] Test lặp ngắt cấp phát khi tạo Patient/Visit, kể cả rehash, và bổ sung danh tính;
      assert số lượng, con trỏ, CCCD và thông tin cũ giữ nguyên sau lỗi.
- [x] Chạy Release build và toàn bộ CTest, yêu cầu tất cả đạt.
- [x] Chạy Debug ASan/UBSan với phát hiện rò rỉ nếu runtime hỗ trợ; kiểm tra output.
- [x] Review độc lập cuối cùng theo skill; sửa lỗi có tác động thực tế và kiểm tra lại.
- [x] Ghi kết quả thực tế và file/dòng thay đổi; giữ thay đổi chưa commit.

## Tiến độ và quyết định thực thi

- Baseline Release trước sửa: 8/8 CTest đạt, HEAD ee7d1c3, nhánh work.
- Người dùng yêu cầu cập nhật thiết kế rồi tiếp tục triển khai, nên không thêm
  vòng xin duyệt kế hoạch/quy tắc đã được xác nhận.
- Dùng workspace hiện tại; chỉ file nháp thiết kế chưa theo dõi từ lượt trước.

- Task 1: complete — test_patient thất bại thiếu constructor rồi đạt sau sửa.
- Task 2: complete — test Registry thất bại thiếu header rồi đạt; Release 10/10 đạt.
- Pre-flight: Task 2 dùng constructor Task 1; Task 3 dùng API Task 2, chữ ký khớp spec.
- Ruling: Patient trả chỉ đọc để bảo vệ CCCD duy nhất; dữ liệu y tế ban đầu dùng
  bản sao hồ sơ, chưa thêm API sửa y tế sau khi đăng ký vì ngoài phạm vi hiện tại.

- Task 3: Release build/CTest 10/10 đạt; Debug ASan/UBSan 10/10 đạt với
  ASAN_OPTIONS=detect_leaks=0:halt_on_error=1, UBSAN_OPTIONS do CMake đặt.
- LeakSanitizer detect_leaks=1: mọi test C++ báo fatal ptrace của môi trường;
  không kết luận LSan đạt. Test allocation kiểm tra số khối cấp phát trở về
  ban đầu sau mỗi lỗi/thành công và hủy Registry.
- Kiểm chứng test allocation: bản sao tạm bỏ delete trong catch bị assert
  SoKhoiDangSong == SoKhoiBanDau; không sửa source dự án để thử.

- Final review: reviewer độc lập xác nhận kiểm tra trùng/tìm kiếm/hoàn tác;
  phát hiện P2 Visit move construction lấy mất lịch sử khỏi đối tượng được mượn.
- Final: fixed Visit move — tái hiện mất SoDanhGia bằng chương trình riêng;
  static_assert cấm move RED trước sửa; thêm Visit copy default và move delete;
  test copy snapshot giữ lịch sử gốc.
- Final: Ruling: chưa thêm bổ sung CCCD khi chưa biết ngày sinh vào hồ sơ cũ —
  giữ chữ ký BoSungDanhTinh theo thiết kế đã đọc; tạo hồ sơ mới có CCCD và chưa
  biết ngày sinh đã hỗ trợ. Giới hạn: hồ sơ cũ cần đủ tên/ngày sinh khi bổ sung.

- Task 3: complete — sau sửa review, Release 10/10 và Debug ASan/UBSan 10/10
  (detect_leaks=0) đạt; header độc lập và kiểm tra nguồn đạt. git diff --check sạch.
- Giữ nhánh work, HEAD ee7d1c3; không stage, commit, push hoặc merge.
