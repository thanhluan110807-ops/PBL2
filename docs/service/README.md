# TriageService

`TriageService` điều phối lượt khám trong bộ nhớ trên `PatientRegistry`. Registry
sở hữu Patient và Visit, phải tồn tại lâu hơn service; các con trỏ và tham chiếu
trả ra chỉ được mượn, hết hiệu lực khi Registry bị hủy. Không tạo hai service
cùng quản lý các Visit của một Registry. Mọi thay đổi Visit đã vào hàng phải đi
qua service; sửa trực tiếp qua con trỏ mượn từ Registry có thể làm lệch hàng.
Service kiểm tra mức/trạng thái đã thay đổi trước khi chọn hoặc xem hàng và báo
lỗi khi phát hiện lệch; chưa có chức năng tự phục hồi sau thao tác ngoài service.

## Cách dùng

1. Tạo hoặc tìm Patient qua Registry, sau đó gọi `TiepNhan` với mã Visit, mã
   Patient, thời điểm tiếp nhận và triệu chứng. Registry kiểm tra mã và Visit mở.
2. Gọi `DanhGia` với sinh hiệu, thời điểm và mã điều dưỡng. Lần đánh giá đầu
   đưa Visit vào hàng lần đầu; đánh giá lại cập nhật mức nhưng giữ thời điểm và
   thứ tự vào hàng. Model hiện yêu cầu có ngày sinh hoặc tuổi ước lượng.
3. `LayDanhSachCho` trả danh sách Visit chỉ đọc theo thứ tự dự kiến; xem bao
   nhiêu lần cũng không tiêu thụ lượt ưu tiên. `GoiTiep` nhận mã bác sĩ, chuyển
   Visit được chọn sang đang khám và trả tham chiếu chỉ đọc.
4. `ChuyenCanLamSang`, `QuayLai` (gồm kết quả và thời điểm vào hàng mới),
   `KetThuc`, `Huy` điều phối trạng thái; lỗi được báo bằng `My_stdexcept`.

Mức 1 được gọi trước mức 2, v.v. Trong một hàng và một mức, lượt vào sớm hơn
được gọi trước; cùng thời điểm dùng thứ tự thêm. Khi hai đầu hàng cùng mức,
bắt đầu chọn quay lại rồi luân phiên lần đầu/quay lại ở lần trùng tiếp theo.
Lượt luân phiên dùng chung cho tất cả mức. Lần gọi vì mức khác nhau hoặc chỉ có
một hàng không đổi lượt này; gọi thất bại cũng không đổi.

Thời điểm truyền bằng `std::time_t` theo đơn vị của hệ thống; thông thường là
giây Unix. Đánh giá không được trước tiếp nhận, đánh giá gần nhất hoặc lần vào
hàng hiện tại; quay lại không được trước tiếp nhận hoặc đánh giá gần nhất.
Model chưa lưu thời điểm chuyển cận lâm sàng nên chưa thể kiểm tra toàn bộ thứ
tự thời gian của quá trình khám. Các mốc chờ tối đa vẫn chưa được chốt.

## Giới hạn hiện tại

- `Visit::DanhGia` tính mức theo luật cũ; chưa hỗ trợ điều dưỡng chọn mức xác
  nhận và cần biết tuổi hoặc tuổi ước lượng. Không xem đó là luật lâm sàng đã
  được thẩm định.
- Chưa phục hồi hàng chờ hoặc lượt luân phiên từ file. Visit đã đánh giá trước
  khi service được tạo không thể đăng ký lại vào hàng qua API hiện tại. Chưa
  thể nối demo hiện có trực tiếp vào service mà không bổ sung luồng tích hợp.
- Không tính áp lực, xếp lịch hay ghi file. `LayDanhSachCho` tạo bản sao heap để
  dự tính thứ tự; danh sách dài sẽ cần thêm bộ nhớ và thời gian xử lý.

Build theo README gốc; chạy riêng bằng `ctest --test-dir build -R
'^test_triage_(service|allocation)$' --output-on-failure --no-tests=error`.
