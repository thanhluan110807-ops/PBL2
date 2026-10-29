# Cấu trúc dữ liệu dùng chung

## Vị trí hiện tại

| Thành phần | Header | Vai trò |
| --- | --- | --- |
| `LinkedList<T>` | `include/dsa/LinkedList.h` | Danh sách liên kết đơn, có head/tail và iterator |
| `HashTable<K, V>` | `include/dsa/HashTable.h` | Bảng băm dùng chaining bằng `LinkedList<Entry>` |

`Vector` và `PriorityQueue` chưa được tích hợp vào `dev`. Vị trí đích của chúng là `include/dsa/Vector.h` và `include/dsa/PriorityQueue.h`; không tạo header rỗng để giả lập thành phần đã có.

Các lớp template chứa khai báo và cài đặt trong header. Không cần tạo `.cpp` rỗng cho chúng. DSA độc lập với model bệnh viện và Qt.

## Cách sử dụng

Include theo đường dẫn tính từ thư mục `include/`:

```cpp
#include "dsa/LinkedList.h"
#include "dsa/HashTable.h"
```

Khi biên dịch trực tiếp bằng g++, thêm `-std=c++17 -Wall -Wextra -I include`. Khi bổ sung CMake, khai báo `include/` là thư mục include của target sử dụng các header này.

Tên API dưới đây phản ánh mã hiện có. Quy ước tiếng Việt PascalCase trong `CLAUDE.md` áp dụng cho tên mới; tác vụ tổ chức thư mục chưa đổi tên API cũ.

## Giao diện và độ phức tạp

- `LinkedList`: `Themdau`, `Themcuoi`, `Xoadau` có chi phí O(1); `Timkiem`, `Timtheo`, `Xoagiatri` và truy cập theo chỉ số có chi phí O(n) ở trường hợp xấu nhất.
- `HashTable`: `Them`, `Timkiem`, `Xoa` có chi phí kỳ vọng O(1) với phân bố băm phù hợp và hệ số tải được kiểm soát; xấu nhất O(n). `Them` cập nhật giá trị khi khóa đã tồn tại. Rehash phải phân phối lại các phần tử; chi phí phụ thuộc phân bố khóa và cách xử lý bucket.
- `HashTable::Timkiem` trả `V*`, tức con trỏ tới giá trị lưu trong bảng. Nếu `V` là `Patient*`, kết quả có kiểu `Patient**`; cần phân biệt với chính con trỏ hồ sơ.
- Cả hai container có copy/move. Copy danh sách tạo node mới; với phần tử là con trỏ, đối tượng được trỏ tới vẫn được chia sẻ, không được nhân bản tự động.
- Container giữ con trỏ không tự sở hữu đối tượng được trỏ tới. Bên quản lý hồ sơ phải quy định vòng đời và tránh con trỏ treo. Rehash hoặc xóa phần tử có thể làm mất hiệu lực con trỏ tới Entry/giá trị bên trong bảng.

## Kiểm thử

`tests/dsa/` là nơi đặt các bộ kiểm thử độc lập khi chúng được bổ sung. Hiện thư mục mới có `.gitkeep`, chưa có bốn tệp kiểm thử dự kiến trong cây ở `CLAUDE.md`.

Khi tích hợp thêm, cần kiểm tra: danh sách rỗng, thêm/xóa đầu–cuối, copy/move; khóa trùng, collision, rehash và tra cứu không thấy. Mỗi test cần thất bại thực sự khi kết quả sai.
