# Đọc/ghi Patient, Visit và tự động lưu

Người dùng yêu cầu hoàn thiện quản lý file và tự lưu sau mọi thay đổi. Người
dùng đã chọn cập nhật trực tiếp bộ Patient/Visit đã chọn, có bản dự phòng.
Thiết kế lưu bằng nút trước đây bị bỏ; không yêu cầu duyệt lại hành vi tự lưu
đã được chỉ định. Commit/push lên work theo yêu cầu mới nhất. Không tự sinh mã.

- PatientFileManager đọc/ghi CSV Patient v1. VisitFileManager đọc/ghi CSV Visit
  v1 dạng bản ghi có loại, đủ lịch sử và trạng thái hàng chờ/FIFO/lượt hòa.
- Khôi phục Visit bằng snapshot có kiểm tra và staging Registry; không tạo
  các thao tác khám giả để dựng lịch sử. Không memory/lớp trừu tượng mới.
- SessionFileManager sao lưu byte gốc rồi tạo snapshot đầy đủ. Ghi bền pending
  journal trước khi thay hai file chính; đồng bộ và xóa journal sau khi đủ.
- Khôi phục ghi dở tự động từ snapshot trước khi đọc file chính; không journal
  thì dùng file chính, không âm thầm thay bằng dữ liệu cũ hoặc Registry rỗng.
- GUI bỏ nút lưu; gọi đồng bộ sau thay đổi. Chưa ghi được thì báo rõ, giữ RAM,
  chặn thay đổi tiếp, timer tự thử3giây; không cho đóng khi còn chưa lưu.
- Cấu hình chỉ nhớ đường dẫn bộ dữ liệu bằng CSV ghi bền; không chuyển Patient
  vào kho riêng. Mở lại tự nạp bộ gần nhất; --chon-du-lieu cho chọn bộ khác.
- Bắt đầu rỗng chọn hai file một lần, có xác nhận nếu tên đã tồn tại. Nhập
  Patient lần đầu chưa có Visit tạo visit.csv cạnh Patient.
- Khóa một tiến trình ở cấu hình ứng dụng tránh hai cửa sổ ghi đồng thời.
- Dữ liệu nghiệp vụ dùng DSA tự viết, độc lập Qt. I/O chuẩn và native filesystem
  dùng cho flush/fsync/atomic replace theo yêu cầu chống gián đoạn; Windows
  có nhánh riêng nhưng chưa được chạy kiểm chứng tại môi trường Linux.

Kiểm chứng: Patient/Visit round-trip, đủ6trạng thái và lịch sử, FIFO/tie sau
reload, I/O lỗi và giữ bản cũ, SIGKILL trước journal/sau journal/sau Patient,
GUI tự ghi và tự retry/block thay đổi/close, cấu hình nhớ đường dẫn.
Không hứa ghi sau khi đã mất điện; công bố sau đồng bộ, giới hạn bởi filesystem
và phần cứng thực tế. Không tự xóa bản sao lưu cũ.
