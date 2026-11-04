# PatientRegistry

`PatientRegistry` sở hữu hồ sơ Patient và Visit trong bộ nhớ. Patient đại diện
một người bệnh có mã dùng xuyên suốt; Visit đại diện một lần khám có mã riêng.
Registry không tự đọc/ghi file; `PatientFileManager` hiện chỉ đọc Patient CSV.
Chưa lưu file Visit, chưa tự sinh mã hoặc gộp hồ sơ. Các mã do bên gọi cung cấp.

## Đọc hồ sơ Patient từ CSV

`PatientFileManager::DocBenhNhan(DuongDan)` đọc file UTF-8 có tiêu đề cố định
và một dòng cho mỗi Patient, rồi trả con trỏ `PatientRegistry*` mới. Bên gọi
sở hữu con trỏ và phải `delete` sau khi các service mượn Registry đã bị hủy.
Gọi trực tiếp lớp `PatientFileManager`; không có lớp trừu tượng trung gian.

```cpp
PatientFileManager QuanLyFile;
PatientRegistry* SoBo = QuanLyFile.DocBenhNhan("patients.csv");
// Tạo và dùng các service mượn *SoBo sau khi đọc thành công.
// Hủy các service trước, rồi giải phóng Registry:
delete SoBo;
```

Tiêu đề v1 là:

```csv
MaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong
```

Ngày sinh dùng `YYYY-MM-DD`; ngày hoặc tuổi ước lượng chưa biết để trống, còn
tuổi `0` là một giá trị hợp lệ. CCCD được giữ dạng chuỗi để không mất số 0 đầu.
Các trường chứa dấu phẩy, dấu ngoặc kép hoặc xuống dòng dùng quy tắc CSV có
ngoặc kép; bộ đọc chấp nhận BOM UTF-8 đầu file. Xem [thiết kế CSV](../superpowers/specs/2026-11-04-patient-csv-read-design.md)
để biết các giá trị giới tính/nhóm máu và ví dụ đầy đủ.

File thiếu, tiêu đề hoặc dữ liệu sai, mã/CCCD trùng đều báo lỗi; lỗi dữ liệu có
số dòng bắt đầu bản ghi. Nếu lỗi giữa file, Registry tạm được hủy và không có
Registry mới được trả ra. Bộ đọc tự `delete` Registry tạm khi gặp lỗi giữa file.
Chỉ thay dữ liệu đang dùng lúc khởi động, trước khi
tạo service mượn Registry. Chưa có hàm ghi CSV, đọc Visit hoặc khôi phục hàng
chờ, nên bộ đọc này chưa phục hồi toàn bộ phiên làm việc.

## Tiếp nhận đúng hồ sơ

1. Nếu có CCCD, gọi `TimBenhNhanTheoCanCuoc`. Chuỗi CCCD được so khớp chính xác;
   không tự đổi định dạng và giữ số 0 đầu. CCCD rỗng trả `nullptr`.
2. Nếu tìm thấy, người tiếp nhận xác nhận đúng người rồi dùng mã Patient tìm được.
   Không tạo Patient mới hoặc thay mã cũ bằng mã mới đề xuất. Nếu thông tin khác,
   cần kiểm tra trước khi thao tác; Registry không tự xác nhận hoặc ghi đè.
3. Nếu chưa có, gọi `ThemBenhNhan` với mã Patient mới. Có thể chưa có CCCD hoặc
   ngày sinh. Mã/CCCD trùng làm thao tác thêm thất bại, kể cả trùng thông tin.
4. Gọi `ThemLanKham` bằng mã Patient đã xác định và mã Visit mới. Nếu Patient còn
   Visit mở, hàm báo lỗi; tìm thấy Patient cũ chưa có nghĩa được tạo Visit mới.

Chỉ `DaKetThuc` và `DaHuy` là đóng. Kết thúc/hủy Visit giữ nguyên hồ sơ và lịch sử.
Hai loại mã có không gian riêng, nhưng mỗi mã phải duy nhất trong cùng loại.

```cpp
PatientRegistry SoBo;
const Patient* BenhNhan = SoBo.TimBenhNhanTheoCanCuoc("001234567890");
if (BenhNhan == nullptr) {
    BenhNhan = SoBo.ThemBenhNhan("BN001", "Nguyen Van A", Gender::Nam,
                                Date{1, 1, 2000}, "001234567890");
}
// Ở luồng GUI thật, xác nhận đúng người trước khi dùng lại hồ sơ tìm thấy.
Visit* LanKham = SoBo.ThemLanKham("LK001", BenhNhan->LayMaDinhDanh(), 1767225600, "Dau bung");
```

## Bổ sung danh tính và dữ liệu ban đầu

`BoSungDanhTinh` sửa Patient hiện có, giữ mã, địa chỉ và dữ liệu y tế. CCCD không
rỗng phải chưa thuộc người khác; dữ liệu sai/CCCD trùng không được đổi hồ sơ cũ.
Patient mượn luôn chỉ đọc để không thể bỏ qua kiểm tra CCCD của Registry.

Với bệnh nhân vô danh cần tuổi ước lượng hoặc dữ liệu y tế ban đầu, chuẩn bị hồ
sơ mẫu rồi gọi overload nhận `const Patient&`. Registry tự tạo bản sao, không
sở hữu hồ sơ mẫu, không nhận con trỏ từ bên gọi:

```cpp
Patient HoSo("BN002");
HoSo.DatTuoiUocLuong(40);
HoSo.CapNhatThongTinDiUng("Penicillin");
const Patient* BenhNhanVoDanh = SoBo.ThemBenhNhan(HoSo);
// Sửa/hủy HoSo sau đó không đổi BenhNhanVoDanh.
```

Bản này chưa cung cấp API sửa từng dữ liệu y tế sau khi thêm Patient. Không dùng
`const_cast` hoặc tự delete con trỏ mượn. Visit mượn cho phép dùng các hàm đánh
giá, khám, cận lâm sàng và đổi trạng thái hiện có; Registry const chỉ trả Visit
chỉ đọc. Có thể copy Visit để xem lịch sử, nhưng không move từ Visit mượn vì
move sẽ lấy mất lịch sử khỏi đối tượng gốc. Con trỏ đối tượng giữ nguyên qua rehash và hết hạn khi Registry bị hủy.

## Tra cứu

- `TimBenhNhan`: tìm chính xác mã Patient, không thấy trả `nullptr`.
- `TimLanKham`: tìm chính xác mã Visit; `TimBenhNhanTheoMaLanKham` lấy Patient liên
  quan. Không thấy mã Visit trả `nullptr`.
- `TimBenhNhanTheoTen`: trả `Vector<const Patient*>` chứa mọi tên phù hợp, hỗ trợ
  một phần tên và không phân biệt hoa/thường ASCII/chữ tiếng Việt UTF-8 dựng sẵn.
  Giữ dấu, không tự đồng nhất Unicode tổ hợp, không tự chọn khi trùng tên. Chuỗi
  tìm rỗng trả danh sách rỗng.
- `LayLichSuLanKham`: trả các Visit của Patient, kể cả đã đóng/hủy; không cam kết
  thứ tự. Patient chưa khám trả rỗng, mã Patient không tồn tại báo lỗi.

Vector kết quả chỉ mượn các đối tượng; hủy Vector không hủy hồ sơ. Không copy/move
Registry, không xóa từng hồ sơ. Destructor hủy Visit trước Patient, sau đó các
HashTable tự thu hồi nút. Không giữ con trỏ tới ô bên trong bảng băm.

Tra mã trung bình O(1); tìm CCCD/tên duyệt Patient, lấy lịch sử và kiểm tra Visit
mở duyệt Visit. Chưa có chỉ mục phụ để giữ đồng bộ bộ nhớ và xử lý ngoại lệ đơn giản.
`new`/`delete` chỉ nằm ở nơi sở hữu; thêm vào bảng thất bại sẽ hủy đối tượng mới.

## Tra cứu lần khám theo tên và ngày (bản thử phần lõi)

**Ghi nhận riêng:** workspace đang có code thử `TraCuuLanKham` và các test liên
quan. Giữ nguyên các thay đổi này; chưa coi bản thử là thiết kế hoặc bản triển
khai đã được chốt cho đề xuất tương lai bên dưới. Lần cập nhật này chỉ sửa tài liệu,
không hoàn tác, chỉnh code thử hoặc xác nhận lại kết quả chạy test của bản thử.

`TraCuuLanKham` trả `Vector<KetQuaTraCuuLanKham>`. Mỗi dòng chứa hai con trỏ
chỉ đọc `BenhNhan` và `LanKham`, nên GUI có thể hiện tên, thông tin đối chiếu,
mã bệnh nhân và mã lần khám rồi mở đúng hồ sơ khi chọn dòng. Một người có nhiều
lần khám sẽ xuất hiện nhiều dòng; Patient chưa có Visit không nằm trong kết quả.
Để tìm hồ sơ người chưa khám, dùng `TimBenhNhanTheoTen` hiện có.

```cpp
Date Ngay{2, 11, 2026};
auto TheoTen = SoBo.TraCuuLanKham("An");
auto TheoNgay = SoBo.TraCuuLanKham("", &Ngay);
auto KetHop = SoBo.TraCuuLanKham("An", &Ngay);
auto DuBaDieuKien = SoBo.TraCuuLanKham("An", &Ngay, "LK025");
auto TatCa = SoBo.TraCuuLanKham();
```

Các điều kiện kết hợp bằng AND: dòng phải thỏa mọi điều kiện được nhập.
Tên tìm một phần, không phân biệt hoa/thường như `TimBenhNhanTheoTen`;
mã lần khám khớp chính xác. Chuỗi rỗng bỏ lọc; ngày `nullptr` bỏ lọc ngày.
Ngày là **ngày tiếp nhận** theo giờ Việt Nam UTC+7, không phải ngày kết thúc khám;
ngày không hợp lệ báo `DoiSoKhongHopLe`, không khớp trả danh sách rỗng.
Kết quả gồm cả Visit đang mở, đã kết thúc hoặc đã hủy, xếp theo thời điểm tiếp
nhận giảm dần; cùng thời điểm thì mã lần khám tăng dần để có thứ tự ổn định.
Không tự chọn hay gộp người trùng tên.

Đây là API phần lõi, chưa có màn hình Qt cho tính năng này. GUI có thể lấy tên
làm ô tìm chính, dùng ngày/mã lần khám làm bộ lọc bổ sung mà không yêu cầu nhập
mã bệnh nhân. API tìm mã Patient vẫn phục vụ liên kết và xử lý bên trong.
Chưa thêm chỉ mục phụ: không nhập mã thì duyệt Visit, lấy Patient theo mã;
nhập mã thì tra trực tiếp Visit trước khi kiểm tra các điều kiện còn lại.
Sắp xếp chèn tự viết có chi phí O(k²) với k dòng kết quả, phù hợp bản thử nhỏ;
cần đánh giá và thay thuật toán khi có nhiều kết quả. Con trỏ kết quả hết hạn
khi Registry bị hủy; hủy Vector không hủy Patient/Visit.

## Đề xuất nâng cấp tra cứu trong tương lai

Cập nhật ngày 02/11/2026. **Trạng thái: đề xuất tương lai, chưa phải yêu cầu triển
khai ngay.** Nội dung này thay thế hướng đề xuất tra cứu trước đó trong mục này;
không sửa code, API hoặc xác nhận code thử thành bản đã chốt.

### 1. Hai chế độ trên cùng một màn hình

- Theo bệnh nhân.
- Theo bác sĩ đã trực tiếp khám.
- Chỉ hiển thị các bộ lọc phù hợp với chế độ đang chọn.

### 2. Tra cứu theo bệnh nhân

- Lấy tìm theo tên làm cách tìm chính; hỗ trợ tìm một phần tên, không phân biệt
  hoa/thường. Cho phép lọc thêm mã lần khám và ngày tiếp nhận.
- Hiển thị đủ thông tin đối chiếu để người dùng chọn đúng người, chẳng hạn mã
  bệnh nhân, họ tên, ngày sinh và CCCD nếu có; thể hiện rõ thông tin chưa biết.
  Không tự chọn hoặc gộp hồ sơ chỉ vì trùng tên.
- Không yêu cầu người dùng nhớ mã bệnh nhân. Vẫn giữ mã bệnh nhân làm định danh
  ổn định và giữ các hàm tra cứu theo mã bên trong để quản lý, liên kết Patient
  với các Visit. Tên và CCCD không thay thế mã bệnh nhân.
- Giữ cách tìm hồ sơ Patient chưa có lần khám để phục vụ tiếp nhận, qua luồng
  tìm/chọn hồ sơ Patient. Patient chưa có Visit không tạo ra dòng lần khám giả
  trong danh sách kết quả Visit.

### 3. Tra cứu theo bác sĩ đã trực tiếp khám

- Cho phép tìm bác sĩ bằng tên hoặc mã bác sĩ. Khi tìm bằng tên, hiển thị danh
  sách bác sĩ phù hợp kèm mã và thông tin phân biệt để chọn đúng người.
- Trả về các lần khám mà bác sĩ đã thực sự trực tiếp khám. Bác sĩ chỉ được phân
  công nhưng chưa trực tiếp khám không được tính vào kết quả này.
- Cho phép kết hợp thêm ngày tiếp nhận, mã lần khám và tên bệnh nhân.

### 4. Ghi chú vai trò bác sĩ

- Bác sĩ được phân công và đã trực tiếp khám: hiện “Tên bác sĩ (được phân công)”.
- Bác sĩ trực tiếp khám thay cho bác sĩ khác không thể khám: hiện
  “Tên bác sĩ (thay thế)”. Khi xem chi tiết, có thể hiện
  “Tên bác sĩ (thay thế Tên bác sĩ được thay)”.
- Cần lưu vai trò tại thời điểm tham gia khám và thông tin thay cho ai nếu có;
  không suy ra vai trò chỉ từ danh sách phân công hiện tại. Thay đổi phân công
  về sau không được làm đổi ghi chú vai trò đã lưu trong lịch sử.
- Một lần khám có thể có nhiều bác sĩ trực tiếp tham gia, mỗi người có ghi chú
  vai trò tương ứng.

### 5. Kết hợp bộ lọc

- Có thể dùng nhiều bộ lọc cùng lúc. Kết quả phải thỏa tất cả điều kiện được
  nhập (AND); điều kiện để trống thì không áp dụng.
- Mã bệnh nhân, mã lần khám và mã bác sĩ được so khớp chính xác khi dùng để xác
  định đối tượng.
- Ngày lọc là ngày tiếp nhận của Visit theo giờ Việt Nam UTC+7, kể cả khi tra
  cứu theo bác sĩ. Không đồng nhất ngày tiếp nhận với ngày bác sĩ trực tiếp khám.

### 6. Danh sách kết quả

- Mỗi dòng là một lần khám, kèm Patient tương ứng. Xếp theo thời điểm tiếp nhận
  giảm dần, gần nhất lên đầu.
- Hiển thị họ tên bệnh nhân, thông tin đối chiếu, mã bệnh nhân, mã lần khám,
  thời điểm tiếp nhận và bác sĩ trực tiếp khám kèm vai trò.
- Chọn một dòng để mở đúng lần khám và hồ sơ bệnh nhân liên quan.
- Một bệnh nhân khám nhiều lần có thể xuất hiện nhiều dòng. Mỗi Visit chỉ xuất
  hiện một dòng trong một danh sách kết quả, dù có nhiều bác sĩ cùng tham gia.

### 7. Khi triển khai sau

- Đọc lại model và thiết kế hiện có để xác định dữ liệu cần bổ sung cho lịch sử
  bác sĩ trực tiếp khám và việc thay thế; chưa chốt model/API mới trong tài liệu này.
- Đánh giá cấu trúc hỗ trợ tìm kiếm và thuật toán sắp xếp theo quy mô dữ liệu.
  Hiện tìm tên/CCCD phải duyệt các Patient; khi dữ liệu hoặc nhu cầu tăng, xem
  xét cấu trúc hỗ trợ tìm tên và bảng băm CCCD → Patient, cùng chỉ mục cho các
  điều kiện tra cứu nếu cần. Chưa chốt cấu trúc hoặc thuật toán cụ thể.
- Bảo đảm các chỉ mục bổ sung luôn đồng bộ khi thêm/cập nhật Patient hoặc lịch
  sử bác sĩ tham gia khám. Khi đổi tên/CCCD, cần loại bỏ mục cũ và cập nhật mục
  mới; nếu thất bại giữa chừng, không để dữ liệu và chỉ mục lệch nhau. CCCD rỗng
  không tạo mục trong chỉ mục CCCD; giữ quy tắc từ chối CCCD trùng và quyền sở
  hữu Patient của Registry.
- Tuân thủ C++17, DSA tự viết và quy ước dự án, gồm include trực tiếp và không
  dùng `<cstddef>`.
- Kiểm tra tên bệnh nhân/bác sĩ trùng, Patient chưa có Visit, lọc kết hợp và
  điều kiện trống, so khớp mã chính xác, thứ tự kết quả và không lặp Visit.
  Kiểm tra chuyển ngày UTC+7, phân biệt ngày tiếp nhận với ngày trực tiếp khám,
  phân biệt bác sĩ được phân công với bác sĩ đã trực tiếp khám hoặc khám thay,
  và bảo toàn ghi chú vai trò lịch sử khi đổi phân công.

Code thử đang có trong workspace được ghi nhận riêng ở mục “Tra cứu lần khám
theo tên và ngày (bản thử phần lõi)”. Bảo toàn code thử, không tự hoàn tác hoặc
coi đó là bản đã được chốt. Chưa commit hoặc push khi cập nhật đề xuất này.

## Lỗi và kiểm tra

Bắt `const My_stdexcept&`: mã/CCCD trùng là `DoiSoKhongHopLe`, Patient bắt buộc
nhưng không có là `NgoaiPhamVi`, còn Visit mở là `TrangThaiKhongHopLe`. Lỗi cấp
phát được ném lại cho bên gọi; dữ liệu và số lượng cũ giữ nguyên.

Build theo README gốc, chạy toàn bộ CTest hoặc riêng hai test Registry:

```bash
ctest --test-dir build -R '^test_patient_registry(_allocation)?$' --output-on-failure --no-tests=error
```

Test thường kiểm tra nghiệp vụ, con trỏ const, địa chỉ qua rehash và vòng đời.
Test allocation là binary riêng thay new/delete để lần lượt làm lỗi các điểm
cấp phát, kiểm tra giữ dữ liệu cũ và số khối nhớ trở về ban đầu sau khi hủy.
Chạy thêm ASan/UBSan với phát hiện rò rỉ theo cấu hình CMake khi runtime hỗ trợ.

Trong môi trường Codex hiện tại, LeakSanitizer báo fatal do giới hạn ptrace.
Debug ASan/UBSan được kiểm tra với ASAN_OPTIONS=detect_leaks=0:halt_on_error=1;
đây không phải kết quả LeakSanitizer. Test allocation riêng đếm khối nhớ để
kiểm tra thu hồi qua từng lỗi cấp phát và destructor Registry.

GUI dùng `LayDanhSachBenhNhan()` để liệt kê cả hồ sơ chưa có Visit. Kết quả
`Vector<const Patient*>` chỉ mượn hồ sơ; không tự delete và không cam kết thứ tự.
