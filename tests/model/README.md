# Kiểm thử Person

Chạy từ thư mục gốc dự án; chưa yêu cầu Qt hoặc CMake:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -g -I include src/model/Person.cpp tests/model/test_person.cpp -o /tmp/pbl2-test-person
/tmp/pbl2-test-person
```

Bộ kiểm thử dùng `assert`, cần giữ assertions được bật (không thêm `-DNDEBUG`). Nó kiểm tra dữ liệu không hợp lệ, ngày sinh chưa biết, tính tuổi quanh sinh nhật, giới hạn tuổi đầu vào, cập nhật thất bại giữ nguyên dữ liệu và destructor ảo.

`Person` dùng `std::string` để lưu văn bản, `My_stdexcept` tự viết để báo lỗi cho GUI, và `Date::HomNay()` đã có để kiểm tra ngày sinh. Không dùng Qt trong model, không dùng `std::optional`, thuật toán STL hay quản lý bộ nhớ động riêng trong lớp Person.

Kiểm tra `CoNgaySinh()` trước khi gọi `LayNgaySinh()` hoặc `TinhTuoi(...)` nếu dữ liệu có thể chưa biết. Các hàm này báo `My_stdexcept` với mã `TrangThaiKhongHopLe` khi thiếu ngày sinh; dữ liệu đầu vào sai báo `My_stdexcept` với mã `DoiSoKhongHopLe`.

`CapNhatThongTinCaNhan(HoTen, GioiTinh)` giữ nguyên ngày sinh; bản nhận thêm `Date` kiểm tra và thay cả ba thông tin. `XoaNgaySinh()` đánh dấu ngày sinh chưa xác định. Mã định danh là `const`, không có setter; phép gán cả đối tượng bị vô hiệu hóa. Việc kiểm tra mã trùng giữa nhiều hồ sơ thuộc Registry.
