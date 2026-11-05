# Thiết kế PatientRegistry

Cập nhật ngày 02/11/2026 theo ba quy tắc người dùng đã xác nhận và yêu cầu tiếp tục
triển khai. Thay thế bản nháp khôi phục từ bàn giao.

## Mục đích và phạm vi

Patient là một người bệnh, giữ cùng mã nội bộ qua nhiều lần khám. Visit là một lần
khám có mã riêng và liên kết đúng Patient. Registry quản lý hồ sơ trong bộ nhớ,
độc lập Qt; chưa thêm lưu file, sinh mã, gộp hồ sơ hoặc GUI.
Giữ C++17 và DSA tự viết; không sửa file của thành viên khác ở gốc repository.

## Quy tắc đã chốt

1. Bên gọi cung cấp mã; Patient vô danh cũng phải có mã. Mã duy nhất trong từng
   loại Patient/Visit, mã trùng bị từ chối, không ghi đè. Hai loại có không gian
   mã riêng.
2. CCCD không bắt buộc; CCCD rỗng không trùng nhau. Khi có CCCD, bên gọi tìm Patient
   theo CCCD và xác nhận đúng người trước khi dùng lại mã đã có. Không thay mã cũ
   bằng mã mới đề xuất, không tự nhận diện qua tên, không tự gộp hồ sơ. CCCD trùng
   khi tạo hoặc thuộc Patient khác khi bổ sung danh tính đều báo lỗi để kiểm tra,
   không ghi đè và không tạo hồ sơ trùng. Người dùng quyết định tái sử dụng Patient
   qua thao tác tìm kiếm riêng; hàm thêm không tự tái sử dụng.
3. Mỗi Patient tối đa một Visit đang mở. Chỉ DaKetThuc/DaHuy là đóng; mọi trạng
   thái còn lại chặn tạo Visit mới. Đóng Visit không xóa Patient/lịch sử.
4. Registry sở hữu, tự tạo và giải phóng cả Patient và Visit; bên gọi chỉ mượn.

## Phương án và lựa chọn triển khai

Hai HashTable<string, Patient*> và HashTable<string, Visit*> tự viết. Cấp phát từng
đối tượng riêng để địa chỉ không đổi khi rehash. Duyệt Patient khi tìm CCCD/tên,
duyệt Visit khi lấy lịch sử/kiểm tra Visit mở. Phương án thêm chỉ mục CCCD/lịch sử
nhanh hơn nhưng cần đồng bộ và hoàn tác nhiều chỉ mục; quy mô hiện tại ưu tiên
hai bảng đơn giản, tránh dữ liệu chỉ mục cũ khi bổ sung danh tính.

Các chi tiết tiếp theo là lựa chọn triển khai của trợ lý trong phạm vi đã chốt:

- Public API trả const Patient* (kể cả Registry không const), không phát Patient*
  có thể sửa CCCD ngoài Registry. Việc bổ sung danh tính phải đi qua Registry.
  Visit có con trỏ mượn cho phép dùng API trạng thái hiện có; bản const trả const Visit*.
- Có overload tạo Patient bằng dữ liệu hành chính, có/không có ngày sinh, CCCD
  tùy chọn. Patient constructor nhận CCCD tùy chọn để không ép biết ngày sinh.
  Có overload ThemBenhNhan(const Patient&) sao chép hồ sơ đầu vào vào một Patient
  do Registry cấp phát; không nhận quyền sở hữu đối tượng đầu vào. Cách này giữ
  được dữ liệu y tế ban đầu, kể cả tuổi ước lượng cho bệnh nhân vô danh.
- Tìm CCCD so khớp chính xác chuỗi không rỗng; không tự sửa số/định dạng, không
  ép 12 chữ số hoặc bỏ số 0 đầu. CCCD rỗng trả nullptr.
- Tìm tên trả Vector<const Patient*>; tìm một phần, không phân biệt hoa/thường
  ASCII và chữ tiếng Việt Unicode dựng sẵn, giữ dấu. Chuỗi tìm rỗng trả rỗng,
  tránh nhận diện bệnh nhân vô danh. Không tự chuẩn hóa Unicode tổ hợp.
- Tra mã Visit rồi tra LayMaBenhNhan() để lấy Patient liên quan; có hàm tiện lợi
  TimBenhNhanTheoMaLanKham. Tên trùng trả nhiều kết quả, không gộp hay tự chọn.
- Kết quả tên/lịch sử không cam kết thứ tự. Mã bệnh nhân không tồn tại khi lấy
  lịch sử hoặc tạo Visit/bổ sung danh tính báo lỗi; bệnh nhân chưa khám trả rỗng.
- Visit cho phép copy để xem lịch sử nhưng cấm move construction, vì move lấy
  mất các Vector khỏi đối tượng đang được Registry giữ và cho mượn.
- Giữ ranh giới Registry hiện tại: chưa thêm API sửa từng dữ liệu y tế sau khi
  thêm hồ sơ. Hồ sơ mẫu có thể chuẩn bị trước khi Registry sao chép.

## API

Header include/repository/PatientRegistry.h, source src/repository/PatientRegistry.cpp.

- const Patient* ThemBenhNhan(const Patient& HoSo)
- const Patient* ThemBenhNhan(MaBenhNhan, HoTen="", GioiTinh=ChuaXacDinh, SoCanCuoc="")
- const Patient* ThemBenhNhan(MaBenhNhan, HoTen, GioiTinh, NgaySinh, SoCanCuoc="")
- const Patient* TimBenhNhan(MaBenhNhan) const
- const Patient* TimBenhNhanTheoCanCuoc(SoCanCuoc) const
- Vector<const Patient*> TimBenhNhanTheoTen(TenCanTim) const
- const Patient* TimBenhNhanTheoMaLanKham(MaLanKham) const
- void BoSungDanhTinh(MaBenhNhan, SoCanCuoc, HoTen, GioiTinh, NgaySinh)
- Visit* ThemLanKham(MaLanKham, MaBenhNhan, ThoiDiemTiepNhan, TrieuChungChinh)
- Visit* TimLanKham(MaLanKham); const Visit* TimLanKham(MaLanKham) const
- Vector<Visit*> LayLichSuLanKham(MaBenhNhan)
- Vector<const Visit*> LayLichSuLanKham(MaBenhNhan) const
- int SoBenhNhan() const; int SoLanKham() const

Tra mã trung bình O(1); tìm CCCD/tên O(số Patient × độ dài văn bản), lịch sử/Visit
mở O(số Visit). Không thêm interface lưu trữ file cho bộ quản lý trong bộ nhớ.

## Bộ nhớ và lỗi

Không copy/move Registry, không xóa riêng hồ sơ. Destructor hủy Visit trước
Patient rồi các bảng tự hủy nút. Con trỏ mượn sống đến khi Registry bị hủy;
Vector kết quả không sở hữu đối tượng đích. Không trả địa chỉ ô trong HashTable.

Kiểm tra trùng trước HashTable::Them vì hàm này có thể ghi đè. new/delete tại
Registry là nơi sở hữu đã chốt. Nếu thêm chỉ mục ném lỗi, delete đối tượng vừa
tạo rồi ném lại; số lượng/dữ liệu cũ giữ nguyên. Constructor ném lỗi thì C++ tự
thu hồi phần dựng chưa xong. Không thêm smart pointer/container chuẩn.

My_stdexcept: mã/CCCD trùng = DoiSoKhongHopLe; Patient bắt buộc nhưng không tồn
tại = NgoaiPhamVi; Patient còn Visit mở = TrangThaiKhongHopLe. CCCD trùng có
thông báo yêu cầu kiểm tra và dùng lại đúng mã Patient. Model giữ trách nhiệm
kiểm tra dữ liệu cá nhân; cập nhật danh tính sai không được làm đổi hồ sơ cũ.

## Kiểm chứng

Test riêng cho CCCD tùy chọn ở Patient; test Registry cho trùng mã/CCCD, CCCD
rỗng, tái sử dụng Patient sau xác nhận, xung đột dữ liệu không ghi đè, tìm mã
Visit liên quan Patient, tìm tên ASCII/tiếng Việt và tên trùng, đủ trạng thái
Visit mở/đóng, giữ lịch sử, địa chỉ sau rehash và cập nhật thất bại. Kiểm tra
API const, cấm copy/move, nhiều vòng đời Registry. Kiểm tra lỗi cấp phát bằng
chương trình test riêng để không đưa cơ chế lỗi giả vào mã nghiệp vụ.

Chạy CMake/CTest Release và Debug ASan/UBSan, kiểm tra header độc lập và nguồn.
Không commit/push. Không dùng kết quả cũ làm xác nhận cho thay đổi mới.
