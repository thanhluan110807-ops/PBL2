# Thiết kế giao diện nhân sự và phân lịch trực

Trạng thái: người dùng đã duyệt; triển khai trên `dev`. Kết quả kiểm chứng xem
phần bàn giao của lượt triển khai, không suy từ thiết kế.

## Mục tiêu và căn cứ

Hoàn thiện phần giao diện của nhiệm vụ B trong ứng dụng An Tâm: xem và quản lý nhân sự, nguyện vọng trực, cấu hình ca, đề xuất lịch, xác nhận phân công, tăng cường và thay ca. Giao diện phải dễ hiểu khi trình bày đồ án, vận hành bằng Qt Widgets và gọi các service hiện có.

Căn cứ: `docs/PHAN-B.md`, `demo/qt-triage/README.md`, `AdminPanel.h/.cpp`, `StaffPresenter.cpp`, `SchedulerService.h/.cpp`, `ScheduleRegistry.h` và bộ test widget hiện có. Nền Git là dev mới nhất đã kiểm tra, gồm bản demo tích hợp; không dùng ảnh UML ngày 03/11 như báo cáo tiến độ.

## Hướng thực hiện

Đề xuất nâng cấp AdminPanel tại chỗ, giữ tên class và vị trí file. Hướng viết một bộ cửa sổ khác sẽ trùng các luồng nhân sự/lịch đã có và làm tăng phạm vi kiểm tra, nên không chọn. Chưa tách MainWindow mới hay chuyển ứng dụng sang web.

Điều chỉnh theo yêu cầu tiếp theo của người dùng: mục **Nhân sự và lịch trực** chuyển trang trong vùng nội dung chính, cùng cấp với Lượt khám/Hồ sơ bệnh nhân. Không mở hộp thoại quản trị riêng. File dữ liệu được liên kết sẵn; giao diện chỉ có nút **Lưu thay đổi** màu xanh lá để ghi trực tiếp.

## Quy tắc thiết kế từ UI/UX Pro Max

Đã chạy tìm kiếm hệ thiết kế cho healthcare scheduling và enterprise operations dashboard, sau đó tìm riêng keyboard focus modal. Kết quả thứ hai gợi ý Minimalism & Swiss Style phù hợp ứng dụng nghiệp vụ: bố cục rõ, khoảng cách đều, chữ dễ đọc, ít hiệu ứng. Các phần Hero/Testimonials/CTA của bộ sinh hướng tới trang quảng cáo nên không áp dụng; chưa tìm được mẫu cấu trúc chuyên biệt cho lịch bệnh viện từ bộ sinh. Không lưu kết quả thô đó làm chuẩn sản phẩm.

Đề xuất giữ nhận diện An Tâm hiện tại: nền #F6F5FA, bề mặt sáng, màu nhấn tím #6A47EB; dùng chữ đậm rõ và màu phụ đủ tương phản. Giữ font hệ thống hiện có hỗ trợ tiếng Việt; không tải font hoặc thêm thư viện. Đây là lựa chọn của trợ lý để đồng bộ ứng dụng, không phải màu mới do người dùng chỉ định.

- Khoảng cách theo nhịp 8/16/24; tiêu đề 24–28px, tiêu đề mục 18px, nội dung 14px.
- Một hành động chính trong từng ngữ cảnh; nút hủy/xóa đặt riêng và có nhãn rõ.
- Focus bàn phím, hover, pressed và disabled phân biệt được. Dùng nhãn chữ cho trạng thái, không chỉ màu.
- Điều khiển Qt chuẩn, label gắn buddy với ô nhập; nội dung quan trọng có thể xem bằng bàn phím và không chỉ qua tooltip.
- Đo tương phản chữ thường tối thiểu 4.5:1 và kiểm tra lại trên ảnh chụp thực tế trước bàn giao.

## Bố cục và hành vi

Đầu màn hình có tiêu đề Nhân sự và lịch trực, bộ chọn tuần và thông tin tổng quan từ dữ liệu thật: số nhân viên, số ca, số phân công dự kiến/chờ xác nhận. Tránh hiển thị số phân công như thể đó là số nhân viên.

Ba tab giữ thứ tự quy trình hiện tại:

1. **Nhân sự:** gom biểu mẫu thành thông tin cơ bản, chuyên môn, nguyện vọng và dự phòng. Xem chi tiết người đã chọn bằng vùng thông tin đọc được. Ghi rõ nguyện vọng là ưu tiên mềm. Giữ thao tác sàng lọc; bỏ nút nhập/xuất nhân viên khỏi giao diện.
2. **Ca trực:** biểu mẫu có nhãn ngắn, hướng dẫn riêng cho ca qua đêm; bảng hiển thị mã, ngày giờ bắt đầu/kết thúc, số bác sĩ/điều dưỡng và yêu cầu trưởng ca. Nút chính Thêm ca; xóa ca tách khỏi nút chính.
3. **Lịch và thay ca:** nút chính Đề xuất lịch; bảng có ca, thời gian ca, nhân viên, vai trò và trạng thái. Chọn phân công mới hiện rõ hành động xác nhận/bắt đầu/hủy; khu vực thay ca hiển thị tên cùng mã người thay. Giữ đề xuất tăng cường; bỏ nút nhập/xuất lịch khỏi giao diện. Không đổi thuật toán hay tự xác nhận các đề xuất.

Cuối trang phân lịch là phản hồi thao tác, trạng thái chưa lưu/đã lưu và nút **Lưu thay đổi**. Chương trình tự nạp snapshot `nhan-su-lich.pbl2` trong thư mục dữ liệu ứng dụng khi mở; bấm lưu ghi vào đúng nguồn đó, không mở hộp chọn đường dẫn. Có thay đổi thì bật nút; ghi lỗi giữ thay đổi để lưu lại. Nguồn có sẵn không đọc được phải báo lỗi và chặn lưu đè. Các API file riêng vẫn được giữ ở tầng code. Patient/Visit tiếp tục tự lưu theo cơ chế đã có. Các ghi chú kỹ thuật và đường dẫn dài ở cuối trang chính đã được bỏ; thanh đầu chỉ báo trạng thái lưu hồ sơ ngắn.

Trạng thái rỗng giải thích bước tiếp theo: thêm/nạp nhân sự → tạo ca → đề xuất lịch → xác nhận. Lỗi nhập giữ nguyên dữ liệu người dùng để sửa; không âm thầm thay bằng dữ liệu mẫu. Giữ cảnh báo mất thay đổi và xác nhận thao tác phá hủy đã có.

## Màn hình hẹp và kiểm chứng

Mục tiêu kiểm tra trang tích hợp ở 520×800 và 1440×980, widget độc lập ở 580×680 và 1050×760: biểu mẫu/hàng nút xuống dòng hoặc chuyển dọc, nội dung dài được cuộn phù hợp; bảng dữ liệu được cuộn ngang khi thực sự cần và không ép cửa sổ rộng hơn màn hình. Nút lưu và thông báo còn truy cập được.

Kiểm tra widget với dữ liệu thật: tiếp nhận nhân sự/nguyện vọng, tạo ca qua đêm, đề xuất/xác nhận lịch, thay người, lưu và mở lại. Giữ test phần B hiện có; thêm kiểm tra có ý nghĩa cho các hành vi được thay đổi. Chụp ảnh rộng/hẹp và rà trực quan. Chạy build/test Qt và các kiểm tra nguồn phù hợp; chỉ công bố kết quả thực chạy, không dùng lại số test trong bàn giao cũ.

## Ranh giới phạm vi

Thiết kế giao diện giữ công thức/ràng buộc xếp lịch hiện có. Chưa thêm QueuePressureCalculator, tự sinh mã hay quy tắc nghiệp vụ mới. Nguồn lực/tăng cường được nhập và gọi API hiện có. Không cam kết mức tối ưu lịch hoặc tự thỏa toàn bộ nguyện vọng.

Bản thiết kế đã được người dùng duyệt trước khi triển khai và được cập nhật theo các yêu cầu điều chỉnh tiếp theo. Xem lịch sử Git và bản bàn giao để xác định trạng thái triển khai/kiểm chứng hiện tại trên `dev`.
