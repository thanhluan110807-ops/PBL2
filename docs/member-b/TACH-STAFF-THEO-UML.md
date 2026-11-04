# Tách Staff gốc thành các nhóm theo UML

**Bản cập nhật kế tiếp:** đã thêm tìm bác sĩ theo tên, cập nhật giữ địa chỉ, sàng lọc theo timestamp và file version 3. Xem [bản trạng thái mới](CHUC-NANG-STAFF-DA-CO-VA-BO-SUNG.md). Nội dung version 2 phía dưới mô tả lần tách trước.

## 1. Phạm vi của lần tách này

Đối chiếu trực tiếp `staff.h` và `Staff.cpp` người dùng đính kèm. Các nhóm chức năng trong hai file gốc được chuyển vào model, service, file manager và phần trình bày Qt. Đây là code đã tích hợp vào CMake của dự án, không phải ví dụ hai người hoặc file ba trường.

Bản tích hợp trước dùng các tên `NhanVienYTe`, `BacSi`, `DieuDuong`, `CaTrucMongMuon`, `CapBac`; lần này tiếp tục dùng các tên đó. Tên trong file gốc là `Nhanvienyte`, `Bacsi`, `Dieuduong`, `Catrucmongmuon`, `Capbac`. Bảng phía dưới chỉ rõ cách chuyển API; đây không phải bản thay thế tương thích từng tên hàm cũ.

Các file gốc đính kèm được giữ nguyên. Mã nguồn bản gốc cũng được đặt trong thư mục `ban-goc-doi-chieu` trong gói tải riêng, chỉ để so sánh, không biên dịch cùng bản mới.

## 2. File nào làm gì?

| Header và .cpp tương ứng | Chức năng từ Staff gốc |
|---|---|
| `include/model/CapBac.h`, `src/model/CapBac.cpp` | Enum cấp bậc và đổi cấp bậc ↔ chuỗi |
| `include/model/CaTrucMongMuon.h`, `src/model/CaTrucMongMuon.cpp` | Khung giờ/nguyện vọng cuối tuần, kiểm tra hợp lệ |
| `include/model/NhanVienYTe.h`, `src/model/NhanVienYTe.cpp` | Nhân viên chung, cấp bậc, kinh nghiệm, giờ tuần, ca đêm/cuối tuần, cờ vừa trực đêm, điểm đã ghi nhận |
| `include/model/BacSi.h`, `src/model/BacSi.cpp` | Chuyên khoa, nội trú, điều kiện trưởng ca |
| `include/model/DieuDuong.h`, `src/model/DieuDuong.cpp` | Trạng thái sàng lọc, giờ liên tục, bắt đầu/kết thúc, mốc xoay tua |
| `include/service/FatigueService.h`, `src/service/FatigueService.cpp` | Chính sách tính điểm, dự báo và công thức mệt mỏi gốc |
| `include/repository/StaffFileManager.h`, `src/repository/StaffFileManager.cpp` | Tạo/đọc bản ghi và lưu/đọc danh sách nhân viên |
| `demo/qt-triage/ui/StaffPresenter.h/.cpp` | Chuyển thông tin nhân viên thành nội dung hiển thị Qt; dùng làm tooltip trong bảng nhân sự |

`Person` và `StaffRegistry` dùng lại trong dự án. Bản bàn giao lên GitHub giữ việc xóa `staff.h`/`Staff.cpp` cũ; không thêm header hay source chuyển tiếp cùng tên. Include trực tiếp `NhanVienYTe.h`, `BacSi.h`, `DieuDuong.h` khi cần.

UML không có ô riêng cho enum CapBac hoặc presenter. Enum được tách để các lớp cùng dùng; presenter là chi tiết triển khai UI, không phải thêm nghiệp vụ mới. Những lớp UML không có implementation trong Staff gốc như ScheduleRegistry không được tạo rỗng trong lần tách này.

## 3. Vì sao tách như vậy?

- Một bác sĩ và một điều dưỡng có dữ liệu/hành vi riêng: mỗi lớp có file riêng.
- Nguyện vọng ca không phải lịch trực thật: không gộp CaTrucMongMuon với Shift.
- Công thức mệt mỏi là chính sách nghiệp vụ: đặt ở service. Model chỉ giữ điểm và nhận cập nhật hợp lệ.
- Định dạng file có thể đổi: không đặt `ostringstream`/`ifstream` trong lớp nhân viên.
- Giao diện Qt quyết định cách trình bày: model không in `cout` hoặc giữ widget.

## 4. Bảng chuyển toàn bộ nhóm hàm từ file gốc

| API/trường trong bản gốc | Nơi tương ứng sau khi tách |
|---|---|
| `Capbac`, `Capbactostring`, `stringtoCapbac` | `CapBac`, `CapBacThanhChuoi`, `ChuoiThanhCapBac` |
| `Catrucmongmuon` và ba trường | `CaTrucMongMuon`: `GioBatDau`, `GioKetThuc`, `MongMuonCuoiTuan` |
| `Manhanvien`, `Laymanhanvien` | Mã bất biến của Person, đọc bằng `LayMaDinhDanh()` |
| `Hoten`, `LayHoten`, `Dathoten` | Person giữ tên; `LayHoTen()`, `DatHoTen()` |
| `LayCapbac`, `Datcapbac` | `LayCapBac()`, `DatCapBac()` |
| `Laysonamkinhnghiem`, `Datsonamkinhnghiem` | `LaySoNamKinhNghiem()`, `DatSoNamKinhNghiem()` |
| `Laycatrucmongmuon`, `DatCatrucmongmuon` | `LayCaMongMuon()`, `DatCaMongMuon()` |
| `LayFatigueScore` | `LayDiemMetMoi()` là điểm đã ghi nhận; `LayDiemMetMoiNen()` là giá trị khởi tạo cho cơ chế dự báo trước đây |
| `LayWeeklyHours`, `CongDonGioLam` | `LayGioLamTuan()`, `CongDonGioLam()` |
| `LaySocatrucdemthang`, `LaySocatruccuoituanthang` | `LaySoCaDemThang()`, `LaySoCaCuoiTuanThang()` |
| `Ghinhancatruc`, `Davuatrucdem` | `GhiNhanCaTruc()`, `DaVuaTrucDem()` |
| `Datlaituanmoi` | `DatLaiTuanMoi()`; chỉ reset giờ tuần như bản gốc, không tự xóa bộ đếm tháng |
| `Cannghinghoibatbuoc` | `CanNghiNgoiBatBuoc()`; trả cờ cũ, chưa tự chứng minh đủ/thiếu 24 giờ |
| `Vuotnguongmetmoi` | `VuotNguongMetMoi()` |
| `Cothelamtruongca` | `CoTheLamTruongCa()`; virtual, lớp con thực hiện |
| `Layloainhanvien` | `LayLoaiNhanVien()` trả enum StaffType để tránh so sánh chuỗi |
| Hai bản `CapnhatFatigueScore` | `FatigueService::CapNhatTheoCongThucGoc()` phân biệt bác sĩ/điều dưỡng |
| `Laychuyenkhoa`, `Datchuyenkhoa`, `Kiemtranoitru` | `LayChuyenKhoa()`, `DatChuyenKhoa()`, `LaBacSiNoiTru()` |
| `Kiemtradangtrucsangloc` | `DangSangLoc()` |
| `Batdautrucsangloc`, `Ketthuctrucsangloc` | `BatDauTrucSangLoc()`, `KetThucTrucSangLoc()` |
| `Congdongiotrucsanglong`, `Laysogiotrucsangloclentuc` | `CongDonGioSangLoc()`, `LayGioSangLocLienTuc()` |
| `Canxoaytuasangloc` | `CanXoayTuaSangLoc()` |
| `Xuatdongfile` ở ba lớp | `StaffFileManager::GhiDong()`; dùng dữ liệu qua getter, ghi đúng loại nhân viên |
| `Hienthithongtin` ở ba lớp | `StaffPresenter::MoTa()` trả QString cho Qt; không in console trong model |
| Destructor ảo | Destructor mặc định virtual qua Person và NhanVienYTe |
| Constructor sao chép | Compiler sao chép các trường giá trị; `SaoChep()` tạo đúng lớp con để Registry dùng |
| Copy/move assignment tự viết | Không giữ phép gán đổi mã trong bản tích hợp, vì Person có mã bất biến. Cập nhật thông tin bằng hàm nghiệp vụ/Registry; đây là khác biệt có chủ đích, không phải quên tách |
| Move constructor tự viết | Không cần thao tác chuyển con trỏ sở hữu vì model không cấp phát tài nguyên thô; không hứa tối ưu move/noexcept như các hàm cũ |

Không tạo setter cho mã để bắt chước phép gán cũ: đổi mã của người đã được liên kết với phân công/Visit có thể làm sai chỉ mục.

## 5. Công thức gốc được giữ ở đâu?

`FatigueService::CapNhatTheoCongThucGoc(NV, HeSoApLuc)` giữ phép tính gốc:

- Bác sĩ: hệ số áp lực × (số ca đêm tháng × 1.5 + giờ tuần × 0.12), nhân thêm 1.2 nếu nội trú.
- Điều dưỡng: hệ số áp lực × giờ tuần × 0.15; nếu đang sàng lọc thì cộng hệ số áp lực × giờ sàng lọc liên tục × 0.8.
- Kết quả được cộng vào điểm đã ghi nhận qua `GhiNhanDiemMetMoi()`.

Đây là công thức mô phỏng bạn đã viết, chưa được xác nhận là MBI. Gọi hai lần vẫn cộng hai lần như bản gốc; không gọi khi chỉ mở bảng hoặc thử nhánh Backtracking. Hàm này được tách để bảo toàn chức năng, chưa được tự động nối vào vòng đời ca trực của scheduler.

Các hàm `TinhDiem/TinhGio/DiemCa` đã có của scheduler vẫn là cơ chế dự báo từ lịch; không dùng lẫn hai cơ chế để cộng điểm hai lần. Chính sách thống nhất sau ca cần chốt ở bước hoàn thiện scheduler, không sửa âm thầm trong việc tách file.

## 6. Các sửa lỗi được thực hiện cùng lần tách

1. Không chấp nhận chuỗi cấp bậc sai rồi tự biến thành điều dưỡng trẻ.
2. Setter/constructor kiểm tra dữ liệu không hợp lệ, NaN/vô cực và sai nghề nghiệp; báo My_stdexcept rõ ràng.
3. Bắt đầu sàng lọc lặp lại không reset giờ đang tích lũy. Kết thúc mới đưa số giờ về 0.
4. Cờ nội trú và cấp bậc nội trú đều làm bác sĩ không được làm trưởng ca. Có constructor riêng nhận cờ nội trú để bảo toàn dữ liệu gốc.
5. Không lưu mã/tên lặp ở NhanVienYTe: dùng Person theo UML.
6. Bổ sung lưu trạng thái từng lớp con, ngày sinh/giới tính và nguyện vọng; không để tách file xong nhưng round-trip mất dữ liệu.

## 7. Định dạng file và tương thích

- File riêng nhân viên: `PBL2_STAFF 2`, các dòng NV, kết thúc END.
- Snapshot tổng: ghi `PBL2_ADMIN 2`, dùng lại StaffFileManager cho dòng NV.
- Cả hai có thể đọc cấu trúc nhân viên version 1 của bản tích hợp trước; các trạng thái không có trong version 1 nhận giá trị ban đầu.
- Version 2 lưu điểm hiện tại, giờ tuần, bộ đếm ca, cờ ca đêm/nội trú/sàng lọc, giờ sàng lọc, giới tính và ngày sinh ngoài các trường trước đây.
- Đây không phải parser cho các dòng `Xuatdongfile()` kiểu dấu chấm phẩy của file gốc. File định dạng cũ đó thiếu loại nhân viên, nguyện vọng và một số trạng thái nên không thể tự khôi phục đầy đủ nếu không có hợp đồng bổ sung.
- Lưu qua file tạm; đọc vào Registry mới, chỉ giao kết quả khi thành công. Không ghi địa chỉ con trỏ.

## 8. Cách dùng ngắn gọn

```cpp
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "service/FatigueService.h"

DieuDuong Lan("DD01", "Le Lan", CapBac::DieuDuongChinh, 4);
Lan.BatDauTrucSangLoc();
Lan.CongDonGioSangLoc(4);
bool CanThay = Lan.CanXoayTuaSangLoc(); // true
```

```cpp
#include "repository/StaffFileManager.h"

StaffRegistry NhanSu;
NhanSu.Them(Lan); // Registry tạo bản sao đúng lớp con và sở hữu bản sao.
StaffFileManager::LuuNhanVien("staff.pbl2", NhanSu);
auto DocLai = StaffFileManager::DocNhanVien("staff.pbl2");
```

`auto DocLai` là unique_ptr sở hữu Registry đã đọc, tự giải phóng khi ra khỏi scope. Custom HashTable/Vector vẫn quản lý dữ liệu; unique_ptr chỉ bảo vệ owner tạm khi đọc lỗi.

## 9. Biên dịch và học theo thứ tự

Dùng CMake của dự án vì file .cpp này phụ thuộc Person, Registry, ngoại lệ và custom DSA. Không biên dịch mỗi BacSi.cpp đơn lẻ thành chương trình có main.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DPBL2_BUILD_QT_DEMO=OFF
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure --no-tests=error
```

Trong cloud, nếu cần kích hoạt CMake: `export PATH="/workspace/.pbl2-tools/cmake/data/bin:$PATH"`.

Đọc: CapBac → CaTrucMongMuon → NhanVienYTe → BacSi → DieuDuong → FatigueService → StaffFileManager → StaffPresenter.

- `.h` trả lời lớp có dữ liệu/hàm gì, lớp khác được gọi gì.
- `.cpp` trả lời mỗi hàm thực hiện như thế nào.
- `#include` cho compiler biết khai báo; CMake phải liệt kê các .cpp để linker có định nghĩa.

## 10. Giới hạn còn giữ rõ

Tách đúng trách nhiệm các chức năng trong Staff không có nghĩa toàn UML đã hoàn thành. Lần này chưa tạo ScheduleRegistry, vòng đời phân công, công thức áp lực, tự xoay tua khi đủ 4 giờ hoặc dữ liệu clinical còn thiếu. `CanXoayTuaSangLoc()` chỉ phát hiện điều kiện cần; service sau này vẫn phải tìm người thay đủ điều kiện. Cờ vừa trực đêm không thay thế kiểm tra khoảng nghỉ theo timestamp.

Các giới hạn đó đã có trong checklist toàn dự án. Code ở đây là bản nhân sự tích hợp có đầy đủ các nhóm chức năng gốc và những thay đổi API nêu rõ phía trên.


## 11. Kết quả kiểm tra bản tách ngày 05/11/2026

- Linux/GCC: 11/11 CTest lõi với ASan/UBSan đạt; leak checking chạy ngoài ptrace.
- Debug bật Qt: 14/14 CTest đạt, có kiểm tra cập nhật tên trên GUI không làm mất giờ tuần, ngày sinh hoặc trạng thái sàng lọc.
- 23 header lõi/service/repository biên dịch độc lập; `git diff --check` đạt.
- Nhánh Windows chưa được chạy thực tế.
