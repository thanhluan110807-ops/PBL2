# Bộ demo có sẵn hồ sơ, hàng chờ và lịch trực

Toàn bộ họ tên, CCCD, sinh hiệu, kết quả và phân công là **dữ liệu hư cấu**.
Mốc trình diễn cố định: **12/11/2026**, lịch tuần **09–15/11/2026**, giờ UTC+7.
Sinh hiệu chỉ phục vụ luật phân loại mô phỏng trong đồ án.
Ba file được lưu trực tiếp trong **data/** ở gốc repository.

## Mở chương trình và liên kết CSV cố định

Chạy trực tiếp chương trình C++ đã build, không dùng script Python. Trong WSL,
nếu thư mục build của bạn là `build-qt-wsl`, chạy từ gốc repository:

```bash
cmake -S . -B build-qt-wsl -DPBL2_BUILD_QT_DEMO=ON
cmake --build build-qt-wsl --target pbl2_qt_demo --parallel 2
./build-qt-wsl/demo/qt-triage/pbl2_qt_demo --du-lieu-mau
```

`--du-lieu-mau` dùng một lần để chuyển liên kết từ bộ cũ sang ba CSV ở ngay
thư mục này. Chương trình nhớ đường dẫn; những lần sau chỉ mở bình thường:

```bash
./build-qt-wsl/demo/qt-triage/pbl2_qt_demo
```

Lần đầu chưa có cấu hình liên kết, chương trình tự dùng bộ CSV này. Nếu đã có
bộ cũ, vẫn giữ liên kết cũ đến khi bạn chọn `--du-lieu-mau` hoặc
`--chon-du-lieu`. Chọn dữ liệu mẫu không ghi đè dữ liệu của bộ cũ.
Nếu trước đó đã liên kết thư mục `demo/qt-triage/data/demo-day-du/`, sau khi
chuyển file sang `data/` hãy chạy lại `--du-lieu-mau` một lần để nhớ đường dẫn mới.

**Sửa và lưu vào chính ba CSV này**, không tạo phiên tạm hoặc chép lại dữ liệu
mẫu mỗi lần mở. Patient/Visit tự lưu sau thao tác; nhân sự/lịch ghi khi bấm nút
xanh **Lưu thay đổi**. Đóng và mở lại vẫn giữ các thay đổi đã lưu. Chạy lại
`--du-lieu-mau` cũng chỉ đọc dữ liệu hiện tại, không đặt lại về mẫu ban đầu.
Không chạy đồng thời nhiều chương trình cùng sửa một bộ file.

## Các file

| File | Nội dung |
|---|---|
| [patient.csv](patient.csv) | 16 bệnh nhân, có danh tính đầy đủ và hồ sơ vô danh có tuổi ước lượng |
| [visit.csv](visit.csv) | 17 lượt khám, lịch sử đánh giá, bác sĩ khám, cận lâm sàng và trạng thái hai hàng chờ |
| [nhan-su-lich.csv](nhan-su-lich.csv) | 10 bác sĩ, 10 điều dưỡng, 14 ca, 49 bản ghi phân công và 4 nhân viên dự phòng |

Patient/Visit phải được nạp cùng nhau. Giao diện tự liên kết `nhan-su-lich.csv`
nằm cạnh Patient CSV. File nhân sự/lịch là CSV thật với từng cột riêng, tiêu đề
`PBL2_ADMIN_CSV,1,2026-11-09`; xem [định dạng](../docs/PHAN-B.md#csv-nhân-sự-và-lịch-trực).
Các mã bác sĩ/điều dưỡng trong lịch sử đều có trong danh sách nhân sự;
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
Các số lượng và thứ tự trên mô tả dữ liệu ban đầu, sẽ đổi khi bạn thao tác.
