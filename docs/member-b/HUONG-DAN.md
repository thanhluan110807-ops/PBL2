> Nhận diện nhanh: [bản đồ file phần B và chức năng chính](BAN-DO-FILE-PHAN-B.md).

> Cập nhật mới: [các lớp trong sáu ảnh UML, file lịch và trạng thái phân công](HOAN-THIEN-CAC-LOP-TRONG-ANH-UML.md).

# Phần B: cách chạy, cách hiểu và chuẩn bị kiểm tra

## Chức năng Staff sau lần bổ sung mới

Xem [chức năng đã có, vừa bổ sung và phần còn lại](CHUC-NANG-STAFF-DA-CO-VA-BO-SUNG.md). Bản mới dùng định dạng file version 3 và có thao tác sàng lọc theo đồng hồ.

## Bản tách Staff theo lớp và trách nhiệm

Xem [Tách Staff theo UML: cấu trúc file và bảng chuyển API từ bản gốc](TACH-STAFF-THEO-UML.md).

## Checklist theo UML mới

Xem [đối chiếu đầy đủ UML và công việc còn thiếu](DOI-CHIEU-UML-VA-CONG-VIEC.md). Hai ví dụ học riêng dưới đây không thay thế các module đầy đủ trong checklist.

## Hai bài học đã tách riêng

Nếu chưa hiểu code tổng thể, học từng bài độc lập trước:

1. [Đọc file: ví dụ nhỏ, giải thích và lệnh chạy riêng](doc-file/README.md).
2. [Backtracking: hai bác sĩ, hai ca và bảng theo dõi quay lui](backtracking/README.md).

Mỗi thư mục có header, file cài đặt và chương trình kiểm tra riêng; không cần Qt hoặc CMake. Đây là ví dụ học tập rút gọn, không thay thế trực tiếp định dạng file hay thuật toán đầy đủ trong ứng dụng.

## Mục tiêu bản kiểm tra trong 2 ngày

Có một luồng thực thi thật: nhân sự → ca trực → Backtracking → phân công → tìm người thay → lưu file → đọc lại và đồng bộ bảng Qt. Nhân sự/lịch dùng model và service mới; các màn bệnh nhân cũ vẫn sử dụng service demo trong bộ nhớ. Không mô tả hai phần này là đã tích hợp nghiệp vụ hoàn chỉnh.

Bản này không phải toàn bộ kiến trúc mục tiêu trên UML. Các lớp đã tích hợp dùng tên PascalCase nhất quán (`NhanVienYTe`, `BacSi`, `DieuDuong`); file Staff bạn đính kèm được giữ nguyên ở attachments, không bị ghi đè. Không đưa các lớp cũ vào build song song vì sẽ tạo hai mô hình nhân sự. `Schedule` hiện gom ca và phân công theo giá trị; chưa cần một `ScheduleRegistry` riêng. Một `AdminFileManager` lưu cả hai nhóm để không ghép nhầm hai phiên bản file.

## Chạy trong cloud đã chuẩn bị

Tại `/workspace/PBL2`:

```bash
export PATH="/workspace/.pbl2-tools/cmake/data/bin:$PATH"
cmake -S . -B build-qt-admin -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DPBL2_BUILD_QT_DEMO=ON -DCMAKE_PREFIX_PATH=/workspace/.pbl2-qt/root/usr
cmake --build build-qt-admin --parallel 2
ctest --test-dir build-qt-admin --output-on-failure --no-tests=error
./build-qt-admin/demo/qt-triage/pbl2_qt_demo
```

Lệnh cuối cần phiên desktop để thao tác; test widget chạy được bằng Qt offscreen. Không dùng đường dẫn Qt cloud trên máy cá nhân. Máy cá nhân cần CMake >=3.20, compiler C++17, Qt Widgets development kit; xem `demo/qt-triage/README.md`. Windows cần Qt kit khớp compiler. Chưa kiểm thử bản Windows.

Test bộ nhớ cho phần lõi:

```bash
cmake -S . -B build-sanitized -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DPBL2_ENABLE_SANITIZERS=ON -DPBL2_BUILD_QT_DEMO=OFF
cmake --build build-sanitized --parallel 2
ctest --test-dir build-sanitized --output-on-failure --no-tests=error
```

Nếu LeakSanitizer báo không chạy được dưới ptrace của sandbox, dùng cơ chế execution escalation cho CTest; không tắt leak checking để coi như test đã đạt.

## Kịch bản trình diễn 5 phút

1. Mở nút **Nhân sự và lịch trực** ở thanh bên; màn hình hẹp cũng có mục này trong menu.
2. Bấm **Nạp mẫu nhân sự + ca** khi chưa có dữ liệu. Có hai bác sĩ chính, hai điều dưỡng, một ca sáng và một ca đêm.
3. Trong tab Nhân sự, thử thêm lại mã `BS01`: hệ thống phải báo lỗi trùng mã, bảng không tăng dòng.
4. Sang tab Lịch, bấm **Xếp lịch hợp lệ bằng Backtracking**: xem bốn phân công. Ca sáng và ca đêm cùng ngày cần các nhân viên khác nhau vì khoảng nghỉ chưa đủ.
5. Thêm bác sĩ `BS03`, cấp bác sĩ chính, chuyên khoa `Cấp cứu`, tích đăng ký dự phòng; chọn dòng trưởng ca, tìm ứng viên rồi xác nhận thay. Giờ và điểm dự kiến được tính lại từ lịch.
6. Bấm **Lưu file**, giữ tên mặc định `nhan-su-lich.pbl2` nếu muốn lần mở ứng dụng sau tự nạp ở cùng thư mục mặc định. Nếu lưu sang tên khác, lần sau dùng **Mở file** để chọn lại.
7. Mở file vừa lưu. Bảng nhân sự, ca và lịch phải khớp dữ liệu trước đó.
8. Thử mở file hỏng: nhận thông báo lỗi, dữ liệu trên bảng vẫn được giữ.
9. Cập nhật hoặc xóa nhân viên, xóa/thêm ca: có xác nhận khi loại phân công cũ. Phải xếp lịch lại trước khi lưu snapshot có ca trực.

Phần B chưa có tự động lưu khi đóng; ứng dụng cảnh báo khi còn thay đổi chưa lưu. File mặc định được thử đọc khi mở bảng nhân sự lần đầu. Không tự xóa file người dùng.

## Bài 1 — Model, inheritance và polymorphism

**Tại sao cần lớp cha?** Bác sĩ và điều dưỡng đều có mã, họ tên, cấp bậc và kinh nghiệm. Gộp dữ liệu chung giúp không phải sửa hai nơi khi quy tắc thay đổi.

Đọc theo thứ tự: `include/model/Person.h` → `include/model/NhanVienYTe.h` → `include/model/BacSi.h` và `include/model/DieuDuong.h`, sau đó đọc các `.cpp` cùng tên trong `src/model/`.

```cpp
class NhanVienYTe : public Person {
public:
    virtual bool CoTheLamTruongCa() const = 0;
};
```

- `public Person`: nhân viên là một người, dùng thông tin định danh đã có.
- `virtual`: chọn đúng hành vi theo đối tượng thật khi gọi qua lớp cha.
- `= 0`: hàm thuần ảo, không tạo trực tiếp nhân viên chung chung.
- `const`: kiểm tra điều kiện không làm thay đổi đối tượng.
- Destructor của `Person` là `virtual`, nên xóa qua con trỏ lớp cha vẫn gọi đúng destructor lớp con.

Bác sĩ chính hoặc trưởng khoa được làm trưởng ca; nội trú và điều dưỡng không được. Constructor chặn cấp bậc sai nghề nghiệp, kinh nghiệm âm và điểm NaN/vô cực.

**Tự kiểm tra hiểu bài:** Nếu bỏ `virtual` ở hành vi cần ghi đè, lời gọi qua con trỏ lớp cha có còn chọn đúng lớp con không? Phân biệt việc này với hàm thuần ảo `= 0`.

## Bài 2 — Ai sở hữu bộ nhớ?

`StaffRegistry` sở hữu từng nhân viên được tạo qua `SaoChep()`. `HashTable<string, NhanVienYTe*>` và `Vector<NhanVienYTe*>` cùng trỏ đến nhân viên đó, nhưng DSA chỉ sở hữu các ô/nút chứa con trỏ. Chỉ destructor Registry xóa nhân viên.

`SaoChep()` là virtual để sao chép đúng bác sĩ/điều dưỡng; nếu chép chỉ phần lớp cha sẽ mất dữ liệu riêng, gọi là object slicing.

Không cho copy Registry vì hai Registry cùng giữ một con trỏ có thể `delete` hai lần. Hàm cập nhật tạo bản mới trước rồi mới thay con trỏ cũ. Hàm xóa dựng danh sách còn lại trước, sau đó mới xóa chỉ mục và đối tượng.

`AdminData` dùng `std::unique_ptr<StaffRegistry>` chỉ để quản lý một owner trong quá trình đọc file: lỗi giữa chừng sẽ tự giải phóng. Đây là tiện ích ownership chuẩn, không thay thế các custom container của đồ án. Staff không tự quản lý mảng động nên không viết Rule of Five thủ công. DSA quản lý bộ nhớ mới cần xử lý copy/move rõ ràng.

HashTable dùng chaining: nhiều khóa cùng bucket vẫn nằm ở các node khác nhau. O(1) là độ phức tạp kỳ vọng khi băm tốt; trường hợp mọi khóa cùng bucket có thể O(n). Test `test_admin_containers` cố tình dùng hàm băm luôn trả 0 để chứng minh xử lý collision.

## Bài 3 — Ca trực và ràng buộc

`Shift` lưu phút từ 00:00 thứ Hai. Ca thứ Hai 22:00–thứ Ba 06:00 có `[1320, 1800)`. Dùng phút giúp so sánh ca vắt ngày đơn giản. Ngày thứ Hai của tuần được lưu riêng trong file.

Hai ca trùng nếu `A.BatDau < B.KetThuc && B.BatDau < A.KetThuc`. Hai ca liền nhau không trùng khoảng, nhưng vẫn có thể vi phạm khoảng nghỉ.

Các ràng buộc cứng:

- Đúng nghề nghiệp và điều kiện trưởng ca.
- Không phân một người vào hai vị trí trong cùng ca; không trùng thời gian.
- Nghỉ tối thiểu 12 giờ giữa ca; sau ca đêm tối thiểu 24 giờ.
- Tổng giờ của mỗi người không quá 48 trong tuần đang lập.
- Điểm mệt mỏi dự kiến không quá 100.
- Đúng số bác sĩ, điều dưỡng và trưởng ca yêu cầu.

Ngưỡng là cấu hình mô phỏng phục vụ đồ án, chưa phải bộ quy định lao động/y khoa đã thẩm định. Ca giao với 22:00–06:00 được tính ca đêm. Đúng mốc 24 giờ được nhận, thiếu một phút bị loại.

Điểm ca = số giờ × (1.5 nếu ca đêm, ngược lại 1.0) + 2 nếu giao cuối tuần. Điểm dự kiến = điểm nền + tổng điểm các ca được phân. Không cộng ngược vào nhân viên khi chỉ xem bảng hoặc thử nhánh.

## Bài 4 — Backtracking theo một ví dụ nhỏ

**Tại sao không chọn người đầu tiên rồi giữ luôn?** Người đó có thể là bác sĩ chính duy nhất, cần dành cho ca bắt buộc có trưởng ca.

Ví dụ hai ca trùng giờ: A cần một bác sĩ thường, B cần trưởng ca; có một bác sĩ chính và một nội trú. Nếu ca A chọn bác sĩ chính, B không có ai. Thuật toán phải quay lại, đưa nội trú vào A và bác sĩ chính vào B.

Đọc `src/service/SchedulerService.cpp`, hàm `TimKiem::Thu`:

```cpp
// Thử một người cho vị trí hiện tại.
KQ.Lich.PhanCong.ThemCuoi(...);
if (Thu(O + 1)) return true;
KQ.Lich.PhanCong.XoaCuoi(); // Sai nhánh: trả trạng thái về trước khi thử.
```

- **Recursion:** hàm gọi chính nó cho vị trí tiếp theo.
- **Base case:** hết vị trí cần điền → tìm được lịch.
- **Pruning:** loại ngay người sai nghề, trùng ca, quá giờ, quá điểm hoặc thiếu nghỉ.
- **Backtracking:** bỏ lựa chọn vừa thử nếu phần còn lại không xếp được.

Ưu tiên thử người ít giờ/ít ca đêm-cuối tuần hơn; ca mong muốn chỉ là ưu tiên mềm. Thuật toán dừng ở lịch hợp lệ đầu tiên, không chứng minh tối ưu hoặc công bằng tuyệt đối. Giới hạn 20.000 lượt trên GUI; hết giới hạn khác với đã chứng minh không tồn tại lịch. Trường hợp xấu có số nhánh tăng theo cấp số nhân, gần dạng n^m với n ứng viên, m vị trí, cộng chi phí kiểm tra ràng buộc.

## Bài 5 — File I/O và exception safety

Định dạng TXT có version, các dòng `NV`, `CA`, `PC`, `DP` (đăng ký dự phòng), cuối cùng `END`. Chuỗi dùng `std::quoted` để xử lý khoảng trắng, dấu nháy và dấu phân cách. Không hỗ trợ xuống dòng trong một trường. File được giới hạn số dòng/nhân viên/ca/phân công.

**Tại sao không ghi địa chỉ con trỏ?** Địa chỉ ở lần chạy trước không còn ý nghĩa khi khởi động lại. File chỉ lưu mã nhân viên/mã ca; sau khi đọc, Registry dựng đối tượng và chỉ mục mới.

**Tại sao đọc vào biến tạm?** Nếu dòng cuối hỏng, dữ liệu đang làm việc vẫn phải còn. `Doc()` tạo `AdminData` mới, kiểm tra trùng mã, enum, dữ liệu tham chiếu và lịch đầy đủ, rồi mới giao kết quả. GUI chỉ thay `DuLieu` khi đọc thành công.

**Tại sao ghi file tạm?** Ghi lỗi giữa chừng không được phá file tốt. Ghi/flush/close thành công mới thay file đích. POSIX dùng rename cùng thư mục; Windows dùng MoveFileExW thay file hiện có. Nhánh Windows chưa được thực thi trong cloud Linux. Đây không phải giao dịch database hoặc bảo đảm chống mất điện tuyệt đối.

## Bài 6 — Đồng bộ Qt với model

`AdminPanel` sở hữu một `AdminData`. Sau thêm, sửa, xóa, xếp lịch, thay ca hoặc nạp file thành công, `LamMoi()` đọc lại model để cập nhật tất cả bảng. Bảng không phải nguồn dữ liệu nghiệp vụ.

`connect(button, &QPushButton::clicked, ...)` nối signal người dùng bấm nút với handler. Handler lấy dữ liệu form, gọi model/service, bắt lỗi và hiển thị. Không đặt thuật toán Backtracking trong code của nút bấm.

Các test widget tạo QApplication thật ở chế độ offscreen, bấm nút, kiểm tra số dòng, thay người, lưu và nạp lại. Không chỉ kiểm tra rằng cửa sổ đã mở.

## Giới hạn cần nói rõ với thầy

- Lập lịch một tuần, chưa kiểm tra ca cuối tuần trước/đầu tuần sau, chưa quản lý tích lũy tháng hoặc phục hồi mệt mỏi theo thời gian.
- Tìm lịch hợp lệ, ưu tiên cân bằng khi chọn ứng viên; chưa tối ưu toàn cục, chưa bảo đảm công bằng tuyệt đối.
- StandbyPool lưu mã đăng ký bằng LinkedList; tìm người thay trong pool, cùng cấp bậc/chuyên khoa và đủ ràng buộc, ưu tiên ít giờ hơn. Nhân viên phải xác nhận thay ca, chưa tự thay khi báo ốm.
- Chưa nối áp lực hàng bệnh nhân của A vào yêu cầu tăng nhân lực; chưa tự xoay tua điều dưỡng sàng lọc 4 giờ.
- File phần B chỉ chứa nhân sự và lịch; chưa lưu/khôi phục bệnh nhân, Visit hoặc dữ liệu service demo của A.
- Snapshot chỉ lưu khi lịch đầy đủ, hoặc chưa có ca nào; chưa lưu nháp lịch thiếu người. Ca cần sửa có thể xóa rồi thêm lại; nhân sự có thao tác cập nhật theo mã.
- Đây là phạm vi đủ minh họa luồng B trong buổi kiểm tra; chưa được coi là hoàn thành toàn bộ UML.

## Kế hoạch học và hoàn thiện

Ngày đầu: chạy demo; hiểu Staff, ownership, HashTable; tự thêm 3 test dữ liệu sai. Ngày thứ hai: tự mô tả ví dụ Backtracking hai ca, chạy lại file round-trip và luyện demo 5 phút.

Tuần 1: chốt với A dữ liệu truyền giữa triage và scheduler, hoàn thiện yêu cầu tối thiểu thầy xác nhận. Tuần 2: liên kết service thật và kiểm thử tích hợp, xử lý lịch liên tuần/standby nếu thuộc phạm vi bắt buộc. Tuần 3: đóng băng chức năng, sửa lỗi, hoàn thiện báo cáo/slide và luyện bảo vệ.

Báo cáo phần B: bài toán → mô hình lớp → custom DSA và ownership → ràng buộc lịch → Backtracking/pruning → định dạng file → test và kết quả → giới hạn. Slide phần B nên có một sơ đồ luồng, ví dụ quay lui hai ca và bảng test; tránh chỉ chụp code dài.

Chưa cần học thuộc mọi dòng. Trước hết hãy tự trả lời: “Tại sao sau khi gọi `Thu(O + 1)` thất bại phải gọi `XoaCuoi()`?” Sau đó mới chuyển sang phân tích độ phức tạp và exception safety.

## Kết quả kiểm chứng của bản bàn giao

Ngày 03/11/2026, cloud Linux, GCC 14.2, Python 3.12, CMake 3.31.6, Qt 6.8.2:

- Debug bật Qt: 13/13 CTest đạt, gồm test widget hành chính và widget bệnh nhân cũ.
- Debug lõi ASan/UBSan: 10/10 CTest đạt; LeakSanitizer hoạt động ngoài sandbox ptrace, không tắt kiểm tra leak.
- Build biên dịch độc lập 16 header lõi/repository/service.
- `git diff --check` đạt.
- Chưa kiểm chứng trên Windows hoặc với lịch/số nhân sự lớn của nhóm.

Các con số là số CTest được đăng ký; một executable test có nhiều assertion. Test GUI chạy offscreen, không thay thế buổi chạy thử thực tế trên máy bạn trước khi gặp thầy.
