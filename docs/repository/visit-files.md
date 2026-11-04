# Đọc/ghi Patient, Visit và phiên làm việc

## API và quyền sở hữu

- `PatientFileManager::DocBenhNhan(path)` trả `PatientRegistry*` mới, bên gọi
  sở hữu. `LuuBenhNhan(registry, path)` ghi toàn bộ Patient theo CSV v1 cũ.
- `VisitFileManager::LuuLanKham(registry, triage, path)` ghi toàn bộ Visit và
  trạng thái hàng chờ. `DocLanKham(registry, path)` chỉ dùng khi Registry đã
  có Patient nhưng chưa có Visit; trả `TrangThaiHangCho` để khôi phục service.
- `TriageService::KhoiPhucHangCho(snapshot)` dựng hai heap tạm, kiểm tra đủ
  mọi Visit đang chờ, rồi mới hoán đổi; lỗi giữ nguyên heap đang dùng.
- `SessionFileManager::DocBoFile(patientPath, visitPath, hang)` đọc đúng bộ file
  đã chọn, hoàn tất journal nếu lần ghi trước bị gián đoạn. Không journal thì
  đọc file chính để tôn trọng dữ liệu sửa ngoài chương trình. Thiếu Visit chỉ
  được chấp nhận lúc nhập hồ sơ lần đầu; đã có bản lưu mà mất Visit phải báo lỗi.
- `SessionFileManager::LuuVaoBoFile(registry, triage, patientPath, visitPath)`
  sao lưu nội dung file cũ, ghi bản chụp mới, công bố journal, thay hai file
  chính rồi xóa journal sau đồng bộ. GUI gọi tự động sau mỗi thay đổi.
- `DocPhien` và `LuuPhien` là API đọc cặp CSV/tạo snapshot cấp thấp, được dùng
  trong quá trình lưu/khôi phục; không phải một nút lưu thủ công trong ứng dụng.

Không dùng `<memory>` hoặc lớp trừu tượng mới. CSV/DSA/Registry/model/service
độc lập Qt; `CsvFile` là các hàm đọc/ghi CSV dùng chung. Các thư viện chuẩn
`fstream`, stream số, locale, filesystem, chrono phục vụ I/O, định dạng số,
đường dẫn và tạo file tạm; dữ liệu nghiệp vụ vẫn dùng Vector/HashTable tự viết.

## Visit CSV v1

CSV UTF-8, dấu phẩy và quy tắc ngoặc kép giống Patient. Một Visit có thể có
nhiều dòng CSV có loại khác nhau. Không gộp Vector vào một trường phân cách
bằng dấu chấm phẩy vì nội dung văn bản có thể chứa ký tự đó.

| Loại | Các trường, theo thứ tự |
|---|---|
| `PBL2_VISIT` | phiên bản `1` (dòng đầu tiên) |
| `META` | bộ đếm thứ tự vào hàng, ưu tiên quay lại `0/1` |
| `VISIT` | mã Visit, mã Patient, tiếp nhận epoch giây, trạng thái, triệu chứng |
| `DANHGIA` | mã Visit, epoch giây, tuổi, là tuổi ước lượng `0/1`, mã điều dưỡng, mạch, HA tâm thu, nhịp thở, SpO₂, GCS, mức |
| `PHANCONG` | mã Visit, mã bác sĩ được phân công |
| `THAMGIA` | mã Visit, mã bác sĩ thực sự khám (có thể lặp) |
| `CLS` | mã Visit, chỉ định, mã bác sĩ chỉ định, có kết quả `0/1`, kết quả |
| `HANGCHO` | mã Visit, epoch vào hàng, số thứ tự vào hàng |
| `KETTHUC` | số bản ghi từ META đến trước KETTHUC, không tính dòng tiêu đề |

Mỗi dòng lịch sử đứng sau VISIT của nó. Giữ nguyên thứ tự đánh giá, phân công,
tham gia, CLS. META xuất hiện đúng một lần ở đầu. KETTHUC xuất hiện đúng một
lần ở cuối để phát hiện file ghi dở; không chấp nhận dữ liệu đi sau nó. Ví dụ:

```csv
PBL2_VISIT,1
META,1,1
VISIT,LK001,BN001,100,ChoKhamLanDau,Đau đầu
DANHGIA,LK001,101,30,1,DD001,80,120,18,98,15,5
HANGCHO,LK001,101,1
KETTHUC,4
```

Trạng thái lưu bằng tên enum hiện tại, không dùng chỉ số phụ thuộc vị trí enum.
Double ghi đủ chữ số round-trip và dùng dấu chấm, không phụ thuộc locale.
Tuổi/nguồn tuổi lấy từ từng bản đánh giá lịch sử, không tính lại bằng Patient
hiện tại. Trong v1, mức được đối chiếu với luật sinh hiệu/tuổi hiện tại; nếu sau
này đổi luật, phải nâng phiên bản và thiết kế tương thích thay vì âm thầm sửa
mức lịch sử. Thời gian lưu epoch giây; GUI hiển thị UTC+7.

## Kiểm tra trước khi nạp

- Mã Visit duy nhất, Patient tồn tại, mỗi Patient tối đa một Visit mở.
- Lịch sử đánh giá hợp lệ, không lùi trước thời điểm tiếp nhận hoặc đánh giá trước.
- Bác sĩ phân công không trùng; tham gia có thể lặp. CLS thuộc bác sĩ đã khám;
  chỉ CLS cuối có thể chưa có kết quả. Giữ cả Visit đóng/hủy và Patient chưa khám.
- Trạng thái và lịch sử phải khớp. Giữ trạng thái ChoCanLamSang có kết quả cuối
  nếu được lưu giữa GhiNhanKetQua và DanhDauQuayLai; không tạo thêm bác sĩ giả.
- Mỗi Visit đã đánh giá và đang chờ xuất hiện đúng một lần trong hàng chờ.
  Mục chờ không trùng thứ tự, không có Visit đang khám/đóng, bộ đếm đủ lớn.
- Registry khôi phục bằng bảng tạm rồi hoán đổi; không đưa con trỏ tạm cho GUI.

## Tự động lưu và khôi phục

Theo lựa chọn của người dùng: cập nhật trực tiếp bộ file đã chọn, có dự phòng.

1. Lần đầu chọn Patient CSV và Visit CSV. Nếu chưa có Visit, `visit.csv` được
   tạo cạnh Patient; bắt đầu rỗng thì chọn nơi đặt cả hai file một lần.
2. Sau tiếp nhận, đánh giá, gọi khám, CLS, quay lại, kết thúc/hủy, bổ sung danh
   tính, chương trình tự ghi đồng bộ. Không có nút Lưu dữ liệu riêng.
3. Góc trạng thái chỉ báo đã tự lưu khi file đã được ghi và đồng bộ. Lỗi I/O
   giữ thay đổi trong RAM, tạm chặn thay đổi tiếp, timer tự thử mỗi3giây; đóng
   cửa sổ cũng thử lưu và không cho đóng nếu vẫn thất bại.
4. Đường dẫn bộ file được nhớ trong cấu hình CSV ghi bền của ứng dụng. Lần
   mở sau tự nạp bộ gần nhất. Có khóa một tiến trình để tránh hai cửa sổ ghi
   đồng thời. `--chon-du-lieu` mở lại hộp chọn bộ dữ liệu khác.
5. Khi không có journal, file chính là nguồn dữ liệu. Không sửa CSV bên ngoài
   trong lúc ứng dụng còn chạy, vì ứng dụng giữ bản đang làm việc trong RAM.

Cạnh file Patient có thư mục `<patient-path>.pbl2/`:

- `backup-original/`: nguyên byte file đầu vào trước lần ghi đầu, gồm metadata
  cho biết file nào có/không có lúc đó; giữ BOM, CRLF và số0đầu.
- `raw-*`: bản sao nguyên byte trước các lần ghi sau.
- `phien-*`: bản chụp đầy đủ Patient/Visit/hàng chờ của thao tác mới.
- `committed.csv`: đánh dấu đã từng ghi đủ cặp file, khác với chỉ tạo dự phòng.
- `pending.csv`: chỉ tồn tại khi việc cập nhật hai file chính chưa hoàn tất.

Trình tự ghi:

```text
Sao lưu file cũ → ghi đủ snapshot mới → ghi bền pending journal
→ thay Patient → thay Visit → đồng bộ → xóa pending và đồng bộ thư mục
```

File tạm được đồng bộ trước khi rename. POSIX dùng fsync cho file và thư mục;
nhánh Windows dùng FlushFileBuffers/MoveFileExW write-through. Nếu bị dừng
trước journal, bộ file cũ còn hiệu lực. Nếu dừng sau journal (kể cả sau khi chỉ
thay Patient), lần đọc sau kiểm tra snapshot và hoàn tất cả hai file trước khi
trả Registry. Journal hỏng/khác bộ đường dẫn/file thiếu thì báo lỗi; không tự
bắt đầu rỗng. Không tự xóa các bản dự phòng cũ.

Các kiểm tra SIGKILL mô phỏng tiến trình bị dừng, không thay thế việc thử mất
điện vật lý. Không thể ghi sau khi đã mất điện; cơ chế ghi trước xác nhận và
đồng bộ chỉ có hiệu lực trong phạm vi bảo đảm của filesystem/phần cứng. Windows
native chưa được chạy ở môi trường Linux này; Qt6 mới kiểm tra biên dịch nguồn.
Chưa bổ sung tự sinh mã Patient/Visit, xếp lịch hoặc áp lực hàng đợi.
