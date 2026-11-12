# Bộ demo có sẵn hồ sơ, hàng chờ và lịch trực

Toàn bộ họ tên, CCCD, sinh hiệu, kết quả và phân công là **dữ liệu hư cấu**.
Mốc trình diễn cố định: **12/11/2026**, lịch tuần **09–15/11/2026**, giờ UTC+7.
Sinh hiệu chỉ phục vụ luật phân loại mô phỏng trong đồ án.

## Mở chương trình với dữ liệu sẵn trong WSL

Từ thư mục gốc repository, sau khi build Qt demo:

```bash
python3 demo/qt-triage/chay_demo_mau.py
```

Script tìm binary trong `build-qt-wsl`, `build-qt-demo`, `build-demo` và `build`.
Chỉ tự chọn khi tìm được đúng một bản chạy được. Nếu có nhiều bản, script báo
các đường dẫn để bạn chỉ định bản vừa build, tránh mở nhầm binary cũ. Đường dẫn
thực thi được in rõ trước khi mở cửa sổ. Với thư mục `build-qt-wsl`, chạy:

```bash
python3 demo/qt-triage/chay_demo_mau.py ./build-qt-wsl/demo/qt-triage/pbl2_qt_demo
```

Nếu dùng thư mục build khác, truyền đường dẫn chương trình Linux đã build:

```bash
python3 demo/qt-triage/chay_demo_mau.py ./build-cua-ban/demo/qt-triage/pbl2_qt_demo
```

Chương trình tự mở màn hình chính với dữ liệu sẵn, không cần chọn file hoặc bấm
nạp mẫu. Cần Python 3 (thư viện chuẩn), WSLg/desktop Linux hoạt động và bản build
hiện tại có hỗ trợ mã điều dưỡng rỗng. Script không build chương trình.

Mỗi lần chạy tạo bản sao mới dưới `/tmp/pbl2-demo-*`, dùng cấu hình và dữ liệu
riêng qua `XDG_CONFIG_HOME`/`XDG_DATA_HOME`. File đang dùng bình thường và file
mẫu gốc được giữ nguyên. Khi thử thao tác, Patient/Visit tự lưu vào **bản sao**;
nhân sự/lịch lưu khi bấm **Lưu thay đổi**. Bản sao còn sau khi đóng, nhưng hệ thống
có thể dọn `/tmp`; lần chạy script tiếp theo bắt đầu lại từ mẫu ban đầu.
Mở binary theo cách thường vẫn dùng bộ dữ liệu bình thường trước đó.

## Các file

| File | Nội dung |
|---|---|
| [patient.csv](patient.csv) | 16 bệnh nhân, có danh tính đầy đủ và hồ sơ vô danh có tuổi ước lượng |
| [visit.csv](visit.csv) | 17 lượt khám, lịch sử đánh giá, bác sĩ khám, cận lâm sàng và trạng thái hai hàng chờ |
| [nhan-su-lich.pbl2](nhan-su-lich.pbl2) | 10 bác sĩ, 10 điều dưỡng, 14 ca, 49 bản ghi phân công và 4 nhân viên dự phòng |

Hai CSV phải được nạp cùng nhau. `nhan-su-lich.pbl2` dùng định dạng Admin v4,
được đặt tại đúng thư mục dữ liệu ứng dụng bởi script. Không chép mẫu đè lên
file thật. Các mã bác sĩ/điều dưỡng trong lịch sử đều có trong danh sách nhân sự;
đánh giá mới để trống mã điều dưỡng theo giai đoạn chưa có đăng nhập.

## Kịch bản trình diễn gợi ý

1. **Tiếp nhận:** có 2 lượt chưa đánh giá, 5 chờ khám lần đầu, 3 chờ quay lại
   và 2 đang khám. LK009 có đủ tuổi; LK010 là bệnh nhân chưa rõ tên, tuổi ước
   lượng 40. Có thể mở một lượt và nhập sinh hiệu để đưa vào hàng chờ.
2. **Hàng chờ:** có đủ mức 1–5, hai hàng lần đầu/quay lại. Khi chưa thay đổi
   dữ liệu, thứ tự gọi là **LK001 → LK006 → LK002 → LK003 → LK007 → LK004 →
   LK008 → LK005**. Dùng mã bác sĩ **BS04** khi gọi khám. Mức 1 đi trước;
   khi bằng mức thì hai hàng luân phiên theo quy tắc hiện có.
3. **Chi tiết:** LK011/LK012 đang khám bởi BS04/BS08; LK013 đang chờ siêu âm
   do BS04 chỉ định. Có thể nhập kết quả cho LK013 để đưa lại vào hàng chờ.
   LK006–LK008 đã có kết quả mô phỏng và đang chờ bác sĩ xem lại.
4. **Hồ sơ/lượt khám:** BN001 có lần trước LK016 và lượt hiện tại LK001;
   BN011 có LK017 đã kết thúc và LK011 đang khám. Tổng cộng có 3 lượt đã kết
   thúc, 1 đã hủy; BN016 chưa có lượt, có thể dùng để demo tiếp nhận hồ sơ cũ.
   Mã tiếp theo lúc chưa thao tác là **BN017** và **LK018**. Khi lọc ngày dùng
   **12/11/2026**; lịch sử trước đó ở 11/11 và 05/11, không tự đổi theo ngày máy.
5. **Nhân sự/ca trực:** xem BS01–BS10 và DD01–DD10, cấp bậc, chuyên khoa,
   điểm mệt mỏi và giờ đã xếp. Có ca sáng **07:00–15:00** và đêm
   **22:00–06:00 hôm sau**, mỗi ca cần 2 bác sĩ, 2 điều dưỡng, 1 trưởng ca.
   Đây là hai khung ca minh họa, không phải lịch phủ đủ 24 giờ mỗi ngày.
6. **Đề xuất lịch:** đã có 48 phân công hiệu lực cho 12 ca thứ Hai–thứ Bảy.
   Hai ca Chủ nhật còn trống. Bấm **Đề xuất lịch trực** để bổ sung **8 phân
   công dự kiến** mà vẫn giữ lịch cũ, rồi chọn dòng và xác nhận. Bộ lập lịch
   kiểm tra giờ nghỉ, ca đêm, giới hạn giờ và mệt mỏi như bình thường.
7. **Thay ca/lưu:** chọn **PC002** (BS05, T2-SANG), bấm tìm người thay;
   BS09/BS10 là bác sĩ dự phòng cùng cấp bậc/chuyên khoa, có thể xác nhận thay.
   Mẫu đã có PC-HUY-001 để xem lịch sử hủy. Bấm nút xanh **Lưu thay đổi**.

Ban đầu 49 bản ghi phân công gồm **8 dự kiến, 36 đã xác nhận, 4 đang trực,
1 đã hủy**. Nhân viên dự phòng là BS09, BS10, DD09, DD10. Các mốc đang trực
là trạng thái của kịch bản cố định, không phải chấm công theo ngày mở ứng dụng.
Mỗi lần muốn trình diễn lại từ đầu, đóng chương trình và chạy lại script.
