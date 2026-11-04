# Thiết kế VisitSearchService — tra cứu lần khám theo bệnh nhân

## Mục đích và phạm vi

`VisitSearchService` là điểm gọi tra cứu **Visit** cho giao diện. Người dùng nhập
thông tin bệnh nhân và, nếu cần, ngày tiếp nhận hoặc mã lần khám; kết quả là các
lần khám kèm đúng Patient liên quan. Thiết kế này chỉ dùng dữ liệu đang có trong
`PatientRegistry`, không đọc file và không thay đổi dữ liệu.

Phạm vi này **không** gồm tìm theo bác sĩ, `StaffRegistry`, vai trò khám thay,
`SearchMode`, chỉ mục tìm kiếm mới hoặc các tính năng được đánh dấu “đề xuất nâng
cấp tra cứu trong tương lai” ở `docs/repository/README.md`. Chỉ mở rộng khi người
dùng yêu cầu và duyệt riêng.

## Các cách đã cân nhắc

1. Giao diện gọi thẳng `PatientRegistry::TraCuuLanKham`: ít mã nhất, nhưng không
   có `VisitSearchService` như kiến trúc mục tiêu.
2. Tạo service và viết lại lọc/sắp xếp: có lớp service, nhưng trùng quy tắc với
   Registry, dễ cho kết quả khác nhau.
3. **Chọn:** service nhận tham chiếu chỉ đọc tới Registry và chuyển yêu cầu tới
   `TraCuuLanKham` hiện có. Một nơi duy nhất giữ quy tắc lọc, service là điểm
   vào cho giao diện và có thể được xem xét mở rộng sau khi có yêu cầu mới.

## Hợp đồng tra cứu hiện tại được giữ nguyên

- Đầu vào: một phần tên bệnh nhân (`TenCanTim`), ngày tiếp nhận tùy chọn
  (`NgayTiepNhan`), mã Visit tùy chọn (`MaLanKham`). Chuỗi rỗng hoặc ngày null
  nghĩa là không áp dụng điều kiện đó. Không nhập điều kiện nào trả tất cả Visit.
- Các điều kiện cùng có mặt kết hợp theo AND. Tên được so khớp một phần, không
  phân biệt hoa/thường theo cách Registry đang hỗ trợ; mã Visit khớp chính xác.
  Ngày là ngày tiếp nhận tại Việt Nam (UTC+7), không phải ngày khám hay kết thúc.
- Kết quả là `Vector<KetQuaTraCuuLanKham>`; mỗi dòng mượn một `const Patient*`
  và một `const Visit*`. Mỗi Visit xuất hiện một lần; Patient có nhiều Visit có
  nhiều dòng. Patient chưa có Visit không tạo dòng giả và vẫn có thể được tìm
  riêng bằng `PatientRegistry::TimBenhNhanTheoTen` trong luồng tiếp nhận.
- Kết quả gồm Visit đang mở, đã kết thúc và đã hủy; sắp xếp thời điểm tiếp nhận
  giảm dần, cùng thời điểm thì mã Visit tăng dần. Tên trùng không tự gộp hồ sơ.
- Ngày không hợp lệ báo lỗi `DoiSoKhongHopLe`; không tìm thấy trả danh sách rỗng.
  Service không sở hữu hoặc xóa Patient/Visit; Registry phải sống lâu hơn service
  và các con trỏ kết quả.

## Luồng hoạt động

1. Giao diện gửi các điều kiện tới `VisitSearchService`.
2. Service gọi `PatientRegistry::TraCuuLanKham` một lần, không tự lọc hoặc sắp
   xếp lại và không lưu bản sao của Patient/Visit.
3. Giao diện hiển thị mỗi dòng với mã Visit, thời điểm tiếp nhận và thông tin
   Patient đủ để phân biệt người trùng tên; chọn một dòng bằng mã Visit để mở
   đúng lần khám. Dữ liệu cụ thể của màn hình sẽ được quyết định khi làm GUI.

Chữ ký dự kiến để triển khai, chưa phải code đã có:

```cpp
class VisitSearchService {
    const PatientRegistry& Registry;
public:
    explicit VisitSearchService(const PatientRegistry& Registry);
    Vector<KetQuaTraCuuLanKham> TimTheoBenhNhan(
        const std::string& TenCanTim = "", const Date* NgayTiepNhan = nullptr,
        const std::string& MaLanKham = "") const;
};
```

## Đường mở rộng trong tương lai — chưa triển khai

- Đặt tên `TimTheoBenhNhan` theo chế độ tìm, thay vì một hàm chung nhận
  `SearchMode`. Sau này có thể **thêm** hàm tìm theo bác sĩ mà không đổi chữ ký
  hoặc hành vi của hàm tìm theo bệnh nhân. Không thêm chế độ, bộ lọc hay phụ
  thuộc nhân sự vào code của giai đoạn này.
- Giữ `PatientRegistry::TraCuuLanKham` là nguồn duy nhất cho các điều kiện chung
  đã có: tên bệnh nhân, ngày tiếp nhận và mã Visit. Nếu sau này có điều kiện
  bác sĩ, service có thể lấy danh sách Visit từ lời gọi này rồi lọc tiếp theo
  mã bác sĩ đã thực sự tham gia. Như vậy không cần viết lại quy tắc ngày UTC+7,
  kết hợp AND hoặc thứ tự kết quả. Chi tiết lọc bác sĩ chỉ quyết định sau khi
  đối chiếu code nhân sự và lịch sử tham gia khám ở thời điểm triển khai.
- Kết quả cặp `Patient`/`Visit` chỉ chứa thông tin chung cho một dòng lần khám.
  Nếu sau này cần ghi chú vai trò bác sĩ, thiết kế **bổ sung** dữ liệu kết quả
  riêng cho chế độ đó; không suy vai trò từ danh sách phân công và không đổi ý
  nghĩa hai con trỏ đang dùng cho tra cứu bệnh nhân.
- Service hiện chỉ giữ tham chiếu tới `PatientRegistry`; khi có chế độ bác sĩ
  mới xem xét thêm cách nhận `StaffRegistry`, đồng thời giữ được cách tạo và
  gọi chế độ bệnh nhân. Không giả định sẵn API của nhánh nhân sự. Phần lưu file
  nạp dữ liệu vào Registry trước khi tra cứu, không trở thành phụ thuộc của
  service.

Đây là các ranh giới để giảm việc sửa lại, không phải cam kết rằng tính năng
chưa chốt có thể được thêm mà không cần thay đổi code hoặc thiết kế sau này.

## Kiểm chứng khi triển khai

Kiểm tra service trả đúng kết quả Registry cho bộ lọc tên/ngày/mã và bộ lọc kết
hợp, tên trùng, Visit đã hủy, không có kết quả và ngày không hợp lệ. Không lặp
lại toàn bộ bài test thuật toán tra cứu của Registry nếu service chỉ chuyển lời
gọi. Thiết kế đã được triển khai ở phần lõi; chưa có GUI hoặc lưu file cho phần
tra cứu này. Các đề xuất tương lai ở trên vẫn chưa được triển khai.
