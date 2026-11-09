# Thiết kế giao diện nhân sự và phân lịch trực

Trạng thái: người dùng đã duyệt; triển khai trên `dev`. Kết quả kiểm chứng xem
phần bàn giao của lượt triển khai, không suy từ thiết kế.

## Mục tiêu và căn cứ

Hoàn thiện phần giao diện của nhiệm vụ B trong ứng dụng An Tâm: xem và quản lý nhân sự, nguyện vọng trực, cấu hình ca, đề xuất lịch, xác nhận phân công, tăng cường và thay ca. Giao diện phải dễ hiểu khi trình bày đồ án, vận hành bằng Qt Widgets và gọi các service hiện có.

Căn cứ: `docs/PHAN-B.md`, `demo/qt-triage/README.md`, `AdminPanel.h/.cpp`, `StaffPresenter.cpp`, `SchedulerService.h/.cpp`, `ScheduleRegistry.h` và bộ test widget hiện có. Nền Git là dev mới nhất đã kiểm tra, gồm bản demo tích hợp; không dùng ảnh UML ngày 03/11 như báo cáo tiến độ.

## Hướng thực hiện

Đề xuất nâng cấp AdminPanel tại chỗ, giữ tên class và vị trí file. Hướng viết một bộ cửa sổ khác sẽ trùng các luồng nhân sự/lịch đã có và làm tăng phạm vi kiểm tra, nên không chọn. Chưa tách MainWindow mới hay chuyển ứng dụng sang web.

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

1. **Nhân sự:** gom biểu mẫu thành thông tin cơ bản, chuyên môn, nguyện vọng và dự phòng. Bảng là vùng chính; xem chi tiết người đã chọn bằng vùng thông tin đọc được. Ghi rõ nguyện vọng là ưu tiên mềm. Giữ thao tác sàng lọc và nhập/xuất nhân viên.
2. **Ca trực:** biểu mẫu có nhãn ngắn, hướng dẫn riêng cho ca qua đêm; bảng hiển thị mã, ngày giờ bắt đầu/kết thúc, số bác sĩ/điều dưỡng và yêu cầu trưởng ca. Nút chính Thêm ca; xóa ca tách khỏi nút chính.
3. **Lịch và thay ca:** nút chính Đề xuất lịch; bảng có ca, thời gian ca, nhân viên, vai trò và trạng thái. Chọn phân công mới hiện rõ hành động xác nhận/bắt đầu/hủy; khu vực thay ca hiển thị tên cùng mã người thay. Giữ đề xuất tăng cường và nhập/xuất lịch. Không đổi thuật toán hay tự xác nhận các đề xuất.

Cuối màn hình là phản hồi thao tác và trạng thái lưu. Giữ cơ chế lưu/mở snapshot phần B cùng các file nhân viên/lịch riêng; không tự chuyển phần B sang autosave Patient/Visit.

Trạng thái rỗng giải thích bước tiếp theo: thêm/nạp nhân sự → tạo ca → đề xuất lịch → xác nhận. Lỗi nhập giữ nguyên dữ liệu người dùng để sửa; không âm thầm thay bằng dữ liệu mẫu. Giữ cảnh báo mất thay đổi và xác nhận thao tác phá hủy đã có.

## Màn hình hẹp và kiểm chứng

Mục tiêu kiểm tra 580×680 và 1050×760: biểu mẫu/hàng nút xuống dòng hoặc chuyển dọc, nội dung dài được cuộn phù hợp; bảng dữ liệu được cuộn ngang khi thực sự cần và không ép cửa sổ rộng hơn màn hình. Nút đóng/lưu và thông báo còn truy cập được.

Kiểm tra widget với dữ liệu thật: tiếp nhận nhân sự/nguyện vọng, tạo ca qua đêm, đề xuất/xác nhận lịch, thay người, lưu và mở lại. Giữ test phần B hiện có; thêm kiểm tra có ý nghĩa cho các hành vi được thay đổi. Chụp ảnh rộng/hẹp và rà trực quan. Chạy build/test Qt và các kiểm tra nguồn phù hợp; chỉ công bố kết quả thực chạy, không dùng lại số test trong bàn giao cũ.

## Ranh giới phạm vi

Thiết kế giao diện giữ công thức/ràng buộc xếp lịch hiện có. Chưa thêm QueuePressureCalculator, tự sinh mã hay quy tắc nghiệp vụ mới. Nguồn lực/tăng cường được nhập và gọi API hiện có. Không cam kết mức tối ưu lịch hoặc tự thỏa toàn bộ nguyện vọng.

Bản thiết kế đã được người dùng duyệt trước khi triển khai. Phần giao diện và kiểm thử tương ứng được lưu cùng bản thiết kế trong lần cập nhật tiếp theo trên nhánh `dev`.
