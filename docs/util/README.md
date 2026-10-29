# Ngoại lệ tối giản của dự án

Header: `include/util/My_stdexcept.h`. Lớp tự giữ thông báo trong mảng `char`, không include `<stdexcept>`, không dùng `std::string` và không cấp phát động riêng.

## Giao diện

- Constructor nhận `My_stdexcept::LoaiLoi` và thông báo `const char*`.
- `LayLoaiLoi()` trả mã để GUI phân biệt nguyên nhân.
- `LayThongBao()` trả chuỗi chỉ đọc, có hiệu lực khi đối tượng lỗi còn sống và chưa bị gán lại.

Bốn loại đủ cho mã hiện tại:

| Mã | Ý nghĩa |
| --- | --- |
| `DoiSoKhongHopLe` | Dữ liệu đầu vào sai, như mã rỗng hoặc ngày sinh không hợp lệ |
| `TrangThaiKhongHopLe` | Gọi thao tác khi đối tượng chưa đủ dữ liệu, như tính tuổi khi chưa có ngày sinh |
| `NgoaiPhamVi` | Truy cập ngoài danh sách hoặc lấy khóa không tồn tại trong bảng băm |
| `LoiThucThi` | Thất bại khi thực hiện thao tác, như không chuyển đổi được ngày hệ thống |

```cpp
#include "util/My_stdexcept.h"

// Ném lỗi tại model/service; GUI bắt và hiển thị.
throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,
                  "Ma dinh danh khong duoc rong");

// Trong khối catch sau thao tác cần xử lý:
// catch (const My_stdexcept& Loi) {
//     ... dùng Loi.LayLoaiLoi() và Loi.LayThongBao() ...
// }
```

Thông báo được sao chép tối đa 255 byte, phần vượt giới hạn bị cắt; luôn có ký tự kết thúc `\0`. Con trỏ thông báo đầu vào có thể là `nullptr`, khi đó dùng thông báo mặc định. Với thông báo UTF-8 quá dài, cắt theo byte có thể ngắt giữa một ký tự; thông báo lỗi ngắn hiện tại không gặp trường hợp này.

Lớp không kế thừa `std::exception`; `catch (const std::exception&)` không bắt được lỗi này. Cơ chế `throw`/`catch` vẫn là cơ chế ngoại lệ của C++, không phải tự viết lại bộ máy ngoại lệ. Các lỗi do thư viện khác ném vẫn cần xử lý theo kiểu của thư viện đó.

## Kiểm thử

Chạy từ thư mục gốc; không định nghĩa `NDEBUG` để giữ assertions:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -g -I include tests/util/test_my_stdexcept.cpp -o /tmp/pbl2-test-errors
/tmp/pbl2-test-errors
```
