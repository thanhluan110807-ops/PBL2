# Bài riêng 2: Backtracking — thử và quay lui

## Tại sao phải quay lui?

Giả sử hai ca A và B trùng giờ. Mỗi ca cần một bác sĩ, nên một người không thể nhận cả hai.

| Bác sĩ | Làm ca thường | Làm trưởng ca |
|---|---|---|
| 0: bác sĩ chính | Có | Có |
| 1: bác sĩ nội trú | Có | Không |

Ca A không bắt buộc trưởng ca; ca B bắt buộc trưởng ca. Nếu chọn bác sĩ 0 cho A rồi không đổi nữa thì B không xếp được. Nhưng bài toán vẫn có lời giải: **A → bác sĩ 1, B → bác sĩ 0**.

Backtracking giúp quay lại lựa chọn trước để tìm lời giải đó.

Bài này không đọc file, không có Qt, giờ tuần hoặc điểm mệt mỏi. Các ràng buộc đó sẽ thêm sau khi hiểu cơ chế quay lui. `main` chỉ là chương trình kiểm tra.

## Các biến cần hiểu

```cpp
const bool BacSiLamTruong[2] = {true, false};
const bool CaCanTruong[2] = {false, true};
int PhanCong[2] = {-1, -1};
```

- `BacSiLamTruong[0]` là `true`: bác sĩ 0 đủ điều kiện làm trưởng.
- `CaCanTruong[1]` là `true`: ca 1, tức B, cần trưởng ca.
- `PhanCong[0] = 1` nghĩa là ca A giao bác sĩ 1. Đây là chỉ số mảng, không phải mã bác sĩ trong ứng dụng.
- `-1` nghĩa là chưa chọn bác sĩ.

Hàm bắt đầu bằng `XepCa(0, ...)`: xếp từ ca A. Điều kiện đầu vào của bài học: hai ô phân công đều là -1 và gọi với chỉ số ca 0.

## Theo dõi bằng tay

| Bước | Thao tác | Phân công `[A, B]` |
|---|---|---|
| 1 | Thử bác sĩ 0 cho A | `[0, -1]` |
| 2 | Xếp B: bác sĩ 0 đã bận, bác sĩ 1 không làm trưởng được | `[0, -1]` |
| 3 | B thất bại; bỏ lựa chọn bác sĩ 0 ở A | `[-1, -1]` |
| 4 | Thử bác sĩ 1 cho A | `[1, -1]` |
| 5 | Thử bác sĩ 0 cho B | `[1, 0]` |
| 6 | Không còn ca cần xếp | Thành công |

## Đọc code theo bốn ý

### 1. Khi nào dừng thành công?

```cpp
if (CaHienTai == 2) return true;
```

Hai ca có chỉ số 0 và 1. Đến chỉ số 2 nghĩa là hai ca trước đã xếp được. Đây là **base case** của đệ quy.

**Đệ quy** là hàm gọi lại chính nó để giải phần việc nhỏ hơn: lần này xếp A, lần gọi tiếp theo xếp B.

### 2. Thử những ai?

```cpp
for (int BacSi = 0; BacSi < 2; ++BacSi)
```

Lần lượt thử bác sĩ 0 rồi bác sĩ 1. Trước khi chọn phải kiểm tra bác sĩ chưa được xếp vào ca trước, và đủ điều kiện trưởng ca nếu ca hiện tại yêu cầu.

`continue` bỏ ứng viên hiện tại để thử ứng viên tiếp theo. Loại ứng viên không hợp lệ sớm gọi là **pruning — cắt nhánh**.

### 3. Chọn và đi tiếp thế nào?

```cpp
PhanCong[CaHienTai] = BacSi;
if (XepCa(CaHienTai + 1, BacSiLamTruong, CaCanTruong, PhanCong))
    return true;
```

Ghi người đang thử vào ca hiện tại. Gọi lại hàm để xếp ca kế tiếp. Nếu phần còn lại xếp được thì trả `true` và giữ lời giải.

Mảng `PhanCong` được dùng chung giữa các lời gọi; trong tham số hàm C++, ký hiệu mảng ở đây được điều chỉnh thành con trỏ. Vì vậy sửa một ô sẽ tác động đến cùng mảng ban đầu, không tạo mảng mới ở mỗi lần gọi. Số 2 trong khai báo tham số không tự kiểm tra kích thước; bài này luôn truyền mảng đúng 2 ô.

### 4. Khi đi tiếp không được thì sao?

```cpp
PhanCong[CaHienTai] = -1;
```

Bỏ lựa chọn vừa thử. Sau đó vòng `for` thử bác sĩ khác. Đây là thao tác **hoàn tác** của Backtracking.

Cuối hàm `return false` nghĩa là đã thử hết người cho ca này mà vẫn bế tắc. Hàm gọi trước đó nhận `false`, rồi hoàn tác lựa chọn của nó.

## Chạy riêng

Từ thư mục gốc PBL2, Linux:

```bash
cd docs/member-b/backtracking
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror Backtracking.cpp kiem_tra.cpp -o /tmp/pbl2-hoc-backtracking
/tmp/pbl2-hoc-backtracking
```

Kết quả đầu tiên:

```text
Ca A: bac si 1; Ca B: bac si 0
```

Windows có MinGW g++ C++17, chạy tại thư mục bài:

```powershell
g++ -std=c++17 -Wall -Wextra Backtracking.cpp kiem_tra.cpp -o kiem_tra.exe
.\kiem_tra.exe
```

Đã kiểm tra trên Linux; chưa chạy Windows. Không bật `-DNDEBUG` vì `assert` đang kiểm tra kết quả.

## Ba tình huống được kiểm tra

1. Một bác sĩ chính, một nội trú; chỉ ca B cần trưởng: phải quay lui để tìm `[1, 0]`.
2. Không ai đủ điều kiện làm trưởng mà B vẫn yêu cầu trưởng: trả `false`, hoàn tác về `[-1, -1]`.
3. Không ca nào cần trưởng: tìm được hai người khác nhau cho hai ca.

## Liên hệ code thật

| Bài học nhỏ | Trong `src/service/SchedulerService.cpp` |
|---|---|
| `XepCa()` | `TimKiem::Thu()` |
| Mảng đúng 2 ca | Các vị trí bác sĩ/điều dưỡng cần điền |
| Kiểm tra bận và trưởng ca | `CoThePhanCong()` kiểm tra thêm nghỉ, giờ, mệt mỏi |
| Gán bác sĩ vào mảng | `PhanCong.ThemCuoi(...)` |
| Gán lại `-1` | `PhanCong.XoaCuoi()` |

Bản đầy đủ còn giới hạn số lượt thử. Hết giới hạn chỉ có nghĩa chưa tìm xong, không chứng minh bài toán không có lời giải. Backtracking tìm lời giải hợp lệ đầu tiên, chưa bảo đảm tối ưu.

## Bài tự làm

Giữ nguyên hai bác sĩ, đổi cả A và B thành cần trưởng ca. Trước khi chạy, tự giải thích vì sao không có lời giải. Sau đó đổi cả hai bác sĩ thành có thể làm trưởng: kết quả sẽ khác thế nào?
