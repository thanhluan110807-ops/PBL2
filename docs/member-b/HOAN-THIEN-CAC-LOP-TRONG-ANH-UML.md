# Hoàn thiện các lớp trong sáu ảnh UML

Tài liệu này cập nhật sau `CHUC-NANG-STAFF-DA-CO-VA-BO-SUNG.md`. Phạm vi là các lớp bạn vừa khoanh trong ảnh, không phải toàn bộ hệ thống bệnh viện.

## 1. Trước đây có gì, lần này bổ sung gì?

| Lớp trong ảnh | Trước lần sửa này | Hiện tại |
|---|---|---|
| `NhanVienYTe` | Có cấp bậc, kinh nghiệm, điểm mệt mỏi, hàm ảo lấy loại; `BacSi`/`DieuDuong` kế thừa | Giữ các chức năng và kiểm thử đã có |
| `StaffRegistry` | Có bảng băm mã nhân viên, thêm/tìm, tìm bác sĩ theo tên | Thêm tên gọi `ThemNhanVien`, `TimNhanVien` tương ứng UML; API cũ vẫn dùng được |
| `StaffFileManager` | Có đọc/ghi nhân viên, kiểm tra dữ liệu, phiên bản 3 | Giữ cách lưu cả trạng thái và đồng hồ sàng lọc |
| `DoctorParticipation` | `Visit` chỉ lưu mã bác sĩ dạng chuỗi | Lớp riêng có mã bác sĩ, thời điểm, vai trò, mã bác sĩ được thay; `Visit` lưu `Vector<DoctorParticipation>` |
| `Shift` | Mã công khai, thời gian tính bằng phút trong tuần | Mã được đọc qua getter; lưu hai mốc `time_t`; tính giờ, giao nhau, ca đêm, cuối tuần theo UTC+7 |
| `ShiftAssignment` | Chỉ có mã nhân viên, mã ca, vai trò | File riêng, mã phân công riêng, xác nhận/hủy/bắt đầu, lý do hủy, thời điểm bắt đầu |
| `ScheduleRegistry` | Chưa có, dùng hai Vector trực tiếp | Hai HashTable trỏ tới ca và phân công, quản lý bộ nhớ, chống trùng mã, kiểm tra liên kết và thao tác trạng thái |
| `ScheduleFileManager` | Lịch nằm trong snapshot nhân sự | Có file lịch riêng, đọc/ghi cả trạng thái, kiểm tra mã tham chiếu, chỉ giao registry mới khi đọc thành công |
| `SchedulerService` | Có quay lui và tìm người thay qua API static | Thêm API gắn với hai registry: `LapLichDuKien`, `DeXuatTangCuong`, `TimNguoiThayCa`; giữ phân công hiện có |

Các hàm trong ảnh đã có cài đặt ở tầng core. Đây không phải xác nhận mọi lớp/mọi luồng của UML tổng thể đã hoàn thành. Giao diện bệnh nhân demo vẫn dùng model mô phỏng riêng; `DoctorParticipation` đã nối vào `Visit` lõi và kiểm thử, chưa được đưa vào form khám bệnh của demo.

## 2. Mở những file nào để học?

Mỗi header dưới đây có `.cpp` cùng tên trong `src/` tương ứng:

- `include/model/DoctorParticipation.h`: sự kiện bác sĩ thực sự tham gia khám.
- `include/model/Shift.h`: thông tin một ca trực; vẫn chứa `Schedule` để tương thích GUI cũ.
- `include/model/ShiftAssignment.h`: một người được phân vào một ca.
- `include/repository/ScheduleRegistry.h`: quản lý nhiều ca và phân công, tìm theo mã.
- `include/repository/ScheduleFileManager.h`: chuyển registry thành file và ngược lại.
- `include/service/SchedulerService.h`: tìm người phù hợp bằng quay lui.
- `tests/service/test_schedule_uml.cpp`: ví dụ chạy được cho toàn bộ các phần trên.

Thứ tự đọc: **Shift → ShiftAssignment → ScheduleRegistry → ScheduleFileManager → SchedulerService**. Đọc `DoctorParticipation` cùng `Visit` ở một buổi riêng; đừng học file và backtracking cùng lúc.

## 3. Vì sao cần ba mã khác nhau?

Ví dụ `PC01` là mã phân công, `BS01` là mã nhân viên, `CA01` là mã ca. Bác sĩ BS01 trực nhiều ca sẽ có nhiều phân công. Khi thay người, bản PC01 bị hủy, bản PC02 được tạo cho người thay. Nhờ mã phân công riêng, ta vẫn biết ai từng được xếp vào ca và vì sao họ được thay.

```cpp
ShiftAssignment P("PC01", "BS01", "CA01", AssignmentRole::TruongCa);
P.XacNhan();
// Thời điểm thực tế được truyền vào khi bắt đầu; Registry kiểm tra nó nằm trong ca.
```

Khái niệm: **constructor**, **enum class**, **đóng gói**. `AssignmentRole` biểu diễn nhiệm vụ; `AssignmentStatus` biểu diễn tiến độ. Hai thông tin này khác nhau.

Quy tắc cài đặt được lựa chọn cho bản này:

```text
Dự kiến ──XacNhan──> Đã xác nhận ──GhiNhanBatDau──> Đang trực
   └──Huy──> Đã hủy       └──Huy──> Đã hủy
```

- Xác nhận lặp lại một bản đã xác nhận không tạo thêm bản.
- Hủy cần lý do khác rỗng. Đã hủy không được xác nhận trở lại: tạo bản mới.
- Chỉ bắt đầu sau khi xác nhận và trong khoảng `[bắt đầu ca, kết thúc ca)`.
- Không hủy một ca đã bắt đầu bằng chức năng này. Bàn giao giữa ca cần quy trình khác để không xóa dấu vết thời gian đã làm.
- UML chưa có hàm kết thúc ca: bản này chưa bổ sung tính lương, chốt giờ đã làm hay cộng mệt mỏi cuối ca. `DangTruc` chưa đồng nghĩa đã hoàn thành.

## 4. Vì sao dùng time_t?

Chỉ lưu “7 giờ sáng” không phân biệt được ngày hôm nay với ngày mai. `time_t` lưu một thời điểm; `std::difftime` tính khoảng cách giữa hai thời điểm. Giờ Việt Nam dùng UTC+7 cố định để kết quả không phụ thuộc timezone máy chạy.

```cpp
const std::time_t ThuHai = 1793552400; // 02/11/2026 00:00, UTC+7
Shift Ca = Shift::TuThoiDiem("CA01", ThuHai + 7*3600, ThuHai + 15*3600,
                            1, 1, true);
// Ca.TinhSoGio() == 8; ca cần 1 bác sĩ, 1 điều dưỡng và có trưởng ca.
```

Khái niệm: **static factory method**, kiểu thời gian, **getter const**. Không có setter đổi mã ca; đổi mã phải tạo ca mới. Vector cần phép gán bản sao nên mã không khai báo `const std::string`; quyền chỉ đọc được thể hiện bằng API và registry chỉ trả `const Shift*`.

Giới hạn của đồ án: thời điểm căn theo phút, ca tối đa 24 giờ, ngày từ 05/01/1970 đến trước/đến mốc 01/01/2100. Một registry quản lý một tuần theo ngày bắt đầu ca; ca Chủ nhật có thể kết thúc sáng thứ Hai. Kiểm tra giao giờ dùng thời điểm tuyệt đối, nhưng chưa quản lý lịch sử tuần trước để kiểm tra nghỉ xuyên hai registry.

Constructor cũ `Shift("CA",420,900,...)` vẫn dành cho test/API nhập phút tương đối, quy về tuần 05/01/1970. Khi nhập dữ liệu thật dùng `TuThoiDiem`. GUI đã gắn phút nhập vào ngày của tuần đang chọn; file snapshot cũ được chuyển theo ngày thứ Hai trong header khi đọc.

Ca liền nhau không giao giờ: `[7h,15h)` và `[15h,23h)` không chồng nhau. Tuy vậy cùng một người vẫn có thể không được nhận cả hai do quy tắc nghỉ 12 giờ. **Không giao giờ chỉ là một trong nhiều điều kiện.**

## 5. Registry khác FileManager thế nào?

`ScheduleRegistry` giữ dữ liệu trong RAM. `ScheduleFileManager` đọc/ghi trên ổ đĩa. Tách riêng giúp bạn học và kiểm thử mỗi việc độc lập.

Registry sở hữu các đối tượng `Shift` và `ShiftAssignment`. `HashTable<string, Shift*>` giữ địa chỉ để tìm nhanh; Vector con trỏ dùng để duyệt và giải phóng. Thêm một bản ghi:

1. Kiểm tra mã và các liên kết.
2. Tạo bản sao, dùng `unique_ptr` giữ tạm để lỗi giữa chừng không rò bộ nhớ.
3. Thêm vào Vector rồi HashTable; nếu bảng băm lỗi thì gỡ phần tử vừa thêm.
4. Chuyển quyền sở hữu cho registry. Destructor của registry xóa các đối tượng.

Khái niệm: **con trỏ**, **quyền sở hữu**, **RAII**, **exception safety**. Không copy registry vì sao chép các con trỏ rồi cùng `delete` sẽ gây giải phóng hai lần. Con trỏ trả ra được mượn để đọc, không được tự `delete`; hết hạn khi registry bị hủy/thay thế.

File riêng có dạng:

```text
PBL2_SCHEDULE 1
CA "CA01" 1793577600 1793606400 1 1 1
PC "PC01" "BS01" "CA01" 1 1 0 ""
END
```

- `CA`: mã, thời điểm bắt đầu/kết thúc, số bác sĩ/điều dưỡng, cờ trưởng ca.
- `PC`: mã phân công, mã nhân viên, mã ca, vai trò, trạng thái, thời điểm bắt đầu thực tế, lý do hủy.
- Vai trò: 0 bác sĩ, 1 trưởng ca, 2 điều dưỡng. Trạng thái: 0 dự kiến, 1 xác nhận, 2 đang trực, 3 hủy.
- Chuỗi dùng `std::quoted` để giữ dấu cách/dấu nháy. Không tự ghép chuỗi rồi tách bằng khoảng trắng.
- File lịch không chứa nhân viên: khi đọc phải cung cấp `StaffRegistry` có đầy đủ các mã liên quan, kể cả bản lịch sử bị hủy.
- Lịch dự kiến chưa đủ người vẫn được lưu. File đúng cấu trúc không đồng nghĩa lịch đã đủ nhu cầu.
- Đọc vào registry mới, kiểm tra toàn bộ rồi mới trao cho người gọi. File hỏng không xóa registry đang dùng.
- Ghi ra file tạm cùng thư mục, đóng thành công rồi đổi tên thay file cũ. Windows dùng `MoveFileExW`; chưa chạy kiểm thử trên Windows.
- Snapshot chung nâng lên `PBL2_ADMIN 4`, vẫn đọc bản 1/2/3. File nhân viên riêng vẫn phiên bản 3. Bản chương trình cũ không đọc được snapshot 4.

## 6. Backtracking đang làm gì?

Ví dụ ca cần một trưởng ca và một điều dưỡng:

1. Tạo hai vị trí còn thiếu.
2. Thử một nhân viên đủ loại/vai trò vào vị trí đầu.
3. Kiểm tra trùng ca, giao giờ, nghỉ giữa ca, giờ tuần, mệt mỏi.
4. Nếu hợp lệ thì thêm **phân công tạm** và chuyển đến vị trí tiếp.
5. Nếu vị trí sau không tìm được người, xóa lựa chọn tạm vừa thêm rồi thử người khác.

Đây là **đệ quy + thử lựa chọn + hoàn tác**. Không tăng giờ/điểm trên nhân viên thật trong lúc thử; nếu không, quay lui sẽ để lại dữ liệu sai.

`LapLichDuKien()` chỉ điền các chỗ còn thiếu, không xóa phân công đã có. `DeXuatTangCuong(MaCa, BS, DD)` thêm đúng số chỗ yêu cầu so với số người hiện có; cũng dùng quay lui. Hai hàm trả Vector đề xuất; caller thêm vào registry sau khi xem xét. GUI thêm chúng ở trạng thái dự kiến để người dùng xác nhận từng dòng.

`TimNguoiThayCa(MaPhanCong)` trả danh sách con trỏ nhân viên phù hợp: cùng cấp bậc, cùng chuyên khoa nếu là bác sĩ, đáp ứng các ràng buộc, và có trong danh sách dự phòng nếu cung cấp danh sách này. GUI cho người dùng chọn rồi hủy bản cũ và xác nhận bản mới, giữ lịch sử.

Các luật tiếp tục dùng cấu hình mô phỏng đã có: 48 giờ/tuần, nghỉ 12 giờ thường/24 giờ sau ca đêm, trần mệt mỏi 100. Không có chứng minh tối ưu; chỉ tìm một phương án hợp lệ. Hết lượt tìm khác với vô nghiệm, và báo hai thông báo khác nhau. GUI giới hạn 20.000 lượt thử; API mặc định 200.000.

**Chưa có công thức tự quyết định số người tăng cường theo áp lực hàng chờ**, vì ảnh chưa định nghĩa ngưỡng đó. Người dùng nhập số lượng; không coi đây là luật vận hành bệnh viện thực tế.

## 7. DoctorParticipation: được phân công chưa phải đã khám

```cpp
DoctorParticipation Luot("BS02", ThoiDiem, DoctorRole::ThayThe, "BS01");
LanKham.GhiNhanBacSiThamGiaKham(Luot);
```

Khái niệm: **value object**, **composition**, lịch sử sự kiện. `Visit` chứa bản sao mỗi lượt tham gia; không sở hữu bác sĩ. Ba vai trò hiện dùng: `KhamChinh`, `HoTro`, `ThayThe` (UML chỉ ghi kiểu `DoctorRole`, các giá trị là lựa chọn cài đặt được ghi rõ ở đây).

`Visit` kiểm tra đang khám, mã khác rỗng, thời điểm không trước tiếp nhận hoặc bản tham gia trước, và bác sĩ được thay phải từng tham gia lần khám. Không tự nhận BS01 đã khám chỉ vì họ được phân công. Một bác sĩ quay lại khám có thể tạo nhiều bản ghi ở các thời điểm khác nhau. Caller/service phải kiểm tra mã bác sĩ tồn tại trong nhân sự; model không phụ thuộc repository.

## 8. Thao tác GUI để trình bày

1. Mở **Nhân sự và lịch trực**, nạp mẫu hoặc nhập nhân viên và ca, chọn đúng thứ Hai.
2. Tab **Lịch và thay ca** → **Xếp lịch**: xem mã phân công và trạng thái **Dự kiến**.
3. Chọn dòng → **Xác nhận phân công**. Bấm **Ghi nhận bắt đầu** chỉ thành công nếu thời điểm máy đang nằm trong ca.
4. Chọn một phân công chưa bắt đầu → hủy và nhập lý do. Xếp lịch lại để bù vị trí thiếu; bản hủy vẫn còn.
5. Chọn dòng → **Đề xuất tăng cường** → nhập số bác sĩ/điều dưỡng cần thêm; xem các dòng dự kiến mới.
6. Thử **Tìm người thay** → chọn người → **Xác nhận thay**. Quan sát bản cũ bị hủy và bản mới được xác nhận.
7. **Lưu file** lưu chung nhân sự/lịch; **Xuất file lịch riêng** chỉ lưu lịch. Mở lại và kiểm tra mã, trạng thái, lý do hủy.

GUI dùng adapter chuyển `Schedule` cũ sang `ScheduleRegistry` trước mỗi thao tác nghiệp vụ, rồi lấy snapshot để hiển thị. Đây là cách tích hợp vào GUI hiện có; không phải hai nguồn dữ liệu đồng thời. Khi có lịch sử xác nhận/hủy/bắt đầu, các thao tác cũ đổi tuần/xóa ca/sửa hoặc xóa nhân viên làm mất lịch sử bị chặn.

## 9. Kiểm thử

- `test_schedule_uml`: mốc giờ, ca qua đêm/cuối tuần, hai ca tiếp giáp, giới hạn ca; trạng thái sai/thao tác lặp; trùng mã, thiếu nhân viên, nhiều tuần; dự kiến/tăng cường/thay người; hết lượt thử; không làm thay đổi registry khi đề xuất; lịch sử sau hủy; file round-trip và dữ liệu hỏng; snapshot v4; vai trò/thời điểm tham gia khám.
- `test_admin_widgets`: thao tác Qt, thay người giữ lịch sử, lưu/mở lại; bắt đầu trước xác nhận bị chặn; xác nhận rồi bắt đầu ca hiện tại thành công.
- Toàn bộ bộ test Qt: **16/16 đạt trên Linux**.
- Bộ test core với AddressSanitizer/UndefinedBehaviorSanitizer (có kiểm tra rò bộ nhớ): **13/13 đạt trên Linux**. Chưa chạy trên Windows.

Các phần ngoài ảnh như kết thúc ca/cập nhật mệt mỏi đúng một lần, lịch sử xuyên tuần, file hồ sơ bệnh nhân và đồng bộ model khám bệnh vào GUI demo vẫn cần công việc riêng.
