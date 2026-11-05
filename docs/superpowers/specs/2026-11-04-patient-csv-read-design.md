# Thiết kế đọc hồ sơ Patient từ CSV

## Mục đích và phạm vi

Đọc một file CSV chứa nhiều Patient để tạo một `PatientRegistry` mới trong RAM.
Đây là bước đầu của hướng tách file theo loại dữ liệu. Chưa ghi file, chưa đọc
Visit, chưa khôi phục hàng chờ và chưa nối GUI. Không sửa dữ liệu của một
Registry đang hoạt động nếu file lỗi.

## Cách biểu diễn đã cân nhắc

- **Một dòng cho một Patient (đã chốt):** một file `patients.csv` có dòng
  tiêu đề cố định. Dễ xem trong bảng tính, tìm lỗi theo dòng và nạp bằng API
  `ThemBenhNhan` đang có.
- Một file riêng cho từng Patient: nhiều file nhỏ, khó quản lý tên file và
  kiểm tra mã/CCCD trùng trên toàn bộ dữ liệu.
- Nhiều dòng kiểu `thuộc tính = giá trị` cho mỗi Patient: dễ thêm thuộc tính
  nhưng khó đọc như bảng CSV và phải ghép các dòng trước khi kiểm tra hồ sơ.

## Định dạng CSV đề xuất

File dùng UTF-8, dấu phẩy làm dấu phân cách, dòng đầu tiên là tiêu đề chính xác:

```csv
MaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong
BN001,"Nguyễn Văn An",Nam,2000-01-31,001234567890,A,"Penicillin, aspirin","Hen suyễn",
BN002,,ChuaXacDinh,,,ChuaXacDinh,,,40
```

Dòng tiêu đề chính xác là dấu hiệu nhận biết **lược đồ v1**. Các tên cột, thứ
tự cột và chuỗi enum trong file là hợp đồng dữ liệu ổn định, không tự đổi theo
tên biến/class trong code. Nếu về sau thay cấu trúc, tạo lược đồ v2 và quy tắc
chuyển đổi riêng; bộ đọc v1 phải từ chối tiêu đề lạ thay vì đoán vị trí cột.

Mỗi dòng dữ liệu là một Patient. `MaBenhNhan` bắt buộc; mã khác nhau trong cùng
file. `SoCanCuoc` là chuỗi để giữ số 0 đầu, có thể rỗng; CCCD không rỗng phải
duy nhất. `GioiTinh` nhận `Nam`, `Nu`, `Khac`, `ChuaXacDinh`. `NhomMau` nhận `A`,
`B`, `AB`, `O`, `ChuaXacDinh`. `NgaySinh` dùng `YYYY-MM-DD`; rỗng nghĩa là chưa
biết. `TuoiUocLuong` rỗng nghĩa là chưa có, `0` là tuổi ước lượng hợp lệ. Các
trường văn bản rỗng được giữ rỗng. Trạng thái danh tính không cần một cột riêng
vì `Patient` hiện xác định nó từ việc họ tên có rỗng hay không.

CSV dùng quy tắc dấu ngoặc kép chuẩn: trường có dấu phẩy, dấu `"` hoặc xuống
dòng phải được đặt trong `"..."`; bên trong, `"` viết thành `""`. Bộ đọc không
tự cắt khoảng trắng trong trường văn bản, không đổi chữ hoa/thường trong mã hoặc
giá trị enum. Có thể chấp nhận dấu BOM UTF-8 đầu file do phần mềm bảng tính tạo.
Không coi dấu chấm phẩy là dấu phân cách. File chỉ có tiêu đề là danh sách rỗng.

## Luồng nạp và lỗi

1. Mở file theo đường dẫn được cung cấp. Không tìm thấy hoặc không mở được file
   là lỗi rõ ràng; không âm thầm coi là danh sách rỗng.
2. Kiểm tra tiêu đề và đọc từng bản ghi CSV. Sai số cột, dấu ngoặc kép, enum,
   ngày hoặc tuổi thì báo lỗi kèm số dòng bắt đầu bản ghi.
3. Chuyển mỗi dòng thành một bản ghi trung gian nội bộ để tách việc đọc CSV khỏi
   việc dựng model. Sau khi kiểm tra trường và kiểu dữ liệu, tạo `Patient` qua
   constructor đang có, đặt nhóm máu, dị ứng, tiền sử bệnh và
   tuổi ước lượng nếu có; thêm bằng `PatientRegistry::ThemBenhNhan(const Patient&)`
   để giữ quy tắc mã và CCCD trùng. Lỗi dữ liệu của model cũng được gắn số dòng.
4. Chỉ trả Registry mới khi toàn bộ file hợp lệ. Nếu có lỗi giữa chừng, `delete`
   Registry tạm; Registry đang dùng và `TriageService` đang tham chiếu nó không
   bị tác động. Bên gọi chỉ thay Registry lúc khởi động, trước khi tạo service.

`PatientRegistry` hiện không copy/move; API trả `PatientRegistry*` theo quyết
định mới của người dùng và không dùng thư viện `<memory>`. Bên gọi sở hữu con trỏ,
phải `delete` sau khi hủy mọi service đang mượn Registry. Bộ đọc dùng `try`/`catch`
để thu hồi Registry tạm nếu nạp file thất bại. Lớp đọc
file nằm trong tầng repository và độc lập Qt. Chỉ thay Registry lúc khởi động;
nếu về sau muốn nhập file khi chương trình đang chạy, phải thay đồng thời các
service đang mượn Registry. Khi làm bước ghi CSV sau này, phải ghi đúng cùng
định dạng và bảo toàn đủ các trường của Patient.

Theo yêu cầu mới nhất, chỉ có lớp cụ thể `PatientFileManager`; không tạo lớp
trừu tượng cho bộ đọc file này.

CSV là file dữ liệu cho chương trình. Phần mềm bảng tính có thể tự đổi CCCD
thành số và làm mất số 0 đầu khi người dùng mở rồi lưu lại; khi chỉnh thủ công
phải nhập cột này dưới dạng văn bản. File chỉ có Patient nên chưa thể tái tạo
Visit hoặc hàng đợi sau khi khởi động lại.

## Kiểm chứng dự kiến

Đọc một file có hồ sơ đủ thông tin và hồ sơ vô danh, kiểm tra lại từng trường;
kiểm tra CCCD có số 0 đầu, tuổi `0` khác tuổi trống, dấu phẩy/ngoặc kép/xuống
dòng trong văn bản, file chỉ có tiêu đề, file không tồn tại, tiêu đề/số cột sai,
giá trị enum/ngày/tuổi sai, mã hoặc CCCD trùng. Mọi lỗi đều không tạo Registry
hoàn chỉnh để bên gọi sử dụng. Chưa dùng dữ liệu Patient để dựng lại Visit.
