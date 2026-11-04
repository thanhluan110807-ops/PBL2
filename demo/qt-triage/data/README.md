# Bộ dữ liệu kiểm tra Patient

[patient.csv](patient.csv) có 30 hồ sơ hoàn toàn hư cấu. CCCD là chuỗi giả để
thử định dạng và tính duy nhất, không phải thông tin của người thật. File dùng
UTF-8 và lược đồ v1; `patients.csv` là bộ mẫu nhỏ cũ được giữ nguyên.

## Nạp dữ liệu

Sau khi mở demo, chọn `patient.csv` trong thư mục này rồi bấm **Đọc CSV & bắt đầu**.
Trong WSL, đường dẫn thường là `~/PBL2/demo/qt-triage/data/patient.csv`.
Trang **Hồ sơ bệnh nhân** phải có 30 hồ sơ. Hàng chờ và danh sách lần khám ban
đầu rỗng: CSV chưa lưu Visit, sinh hiệu, trạng thái khám hoặc lịch trực.

## Hồ sơ để kiểm tra

| Mã | Tình huống |
|---|---|
| BN001, BN004 | Cùng tên Nguyễn An, khác mã/CCCD/ngày sinh; phải chọn đúng người |
| BN002, BN006 | Cùng tên Trần Bình, khác giới tính và tuổi |
| BN003, BN020 | Chưa rõ danh tính, có tuổi ước lượng 40 và 0; có thể đánh giá |
| BN009, BN010 | Thiếu cả ngày sinh lẫn tuổi; đánh giá bị từ chối cho đến khi bổ sung danh tính/ngày sinh |
| BN007 | Giới tính Khac |
| BN008 | Có tên nhưng không có CCCD/ngày sinh; đánh giá dùng tuổi ước lượng 22 |
| BN011–BN018 | Tuổi ước lượng 0, 1, 5, 6, 12, 13, 64, 65 để thử biên nhóm tuổi |
| BN019 | Tuổi ước lượng 150: giá trị biên dữ liệu mô phỏng, không đại diện trường hợp lâm sàng |
| BN021 | Ngày sinh 29/02 hợp lệ của năm nhuận |
| BN023 | Dị ứng/tiền sử có dấu phẩy, được đặt trong ngoặc kép CSV |
| BN024 | Tên/tiền sử có dấu ngoặc kép, CSV escape bằng hai dấu ngoặc |
| BN025 | Tiền sử có xuống dòng trong một trường CSV; vẫn là một Patient |
| BN026, BN027 | Tìm tên không phân biệt hoa/thường; giữ nguyên dấu |
| BN028 | Hoà có vị trí dấu khác Hòa; tra cứu lõi chưa cam kết coi hai cách viết là một |
| BN029, BN030 | Có ngày sinh, tuổi do lõi tính tại thời điểm đánh giá |

## Thử phân loại 5 mức

Tiếp nhận từng hồ sơ BN001–BN005 bằng chế độ **Chọn hồ sơ đã có**, dùng các
mã Visit TEST001–TEST005. Nhập triệu chứng thử tùy ý và mã điều dưỡng DD_TEST.
Nhập các bộ số đo dưới đây trong hộp **Đánh giá**. Đây là đầu vào thử luật
mô phỏng hiện tại trong code, không phải hướng dẫn y tế.

| Patient / Visit | Mạch | HA tâm thu | Nhịp thở | SpO₂ | GCS | Mức dự kiến |
|---|---:|---:|---:|---:|---:|---:|
| BN001 / TEST001 | 80 | 120 | 18 | 89 | 15 | 1 |
| BN002 / TEST002 | 80 | 120 | 18 | 94 | 15 | 2 |
| BN003 / TEST003 | 110 | 120 | 18 | 98 | 15 | 3 |
| BN004 / TEST004 | 80 | 120 | 18 | 96 | 15 | 4 |
| BN005 / TEST005 | 80 | 120 | 18 | 98 | 15 | 5 |

Hàng chờ dự kiến TEST001 → TEST002 → TEST003 → TEST004 → TEST005. Nhập
BS_TEST rồi gọi tiếp. Bộ lọc không thay đổi người được gọi. Để thử hàng cùng
mức hoặc nhóm tuổi trẻ em, không dùng mọi bộ số đo người lớn như giá trị bình
thường; có thể dùng các ví dụ riêng bên dưới.

## Thử luân phiên hai hàng khi cùng mức

Bắt đầu một phiên mới với cùng file CSV, sau đó:

1. Tiếp nhận BN001/HOA001; đánh giá 80/120/18/98/15 (mức 5).
2. Gọi HOA001 với BS_TEST, chuyển cận lâm sàng do BS_TEST chỉ định, ghi kết quả
   thử để đưa HOA001 vào hàng quay lại.
3. Tiếp nhận BN004/HOA004, đánh giá cùng bộ số đo (mức 5).
4. Gọi tiếp: HOA001 được chọn vì đây là lần hòa đầu tiên.
5. Chuyển HOA001 đi cận lâm sàng và cho quay lại lần nữa.
6. Tiếp nhận BN002/KHAN002 với SpO₂ 89, các số đo khác 80/120/18/15 (mức 1).
7. Gọi tiếp: KHAN002 được chọn vì khẩn hơn. Kết thúc KHAN002.
8. Gọi tiếp: HOA004 được chọn ở lần hòa kế tiếp; lần gọi mức 1 không đổi lượt hòa.
9. Kết thúc HOA004, tiếp nhận BN006/HOA006 với 80/120/18/98/15 (mức 5).
10. Gọi tiếp: HOA001 được chọn ở lần hòa tiếp theo.

Thứ tự số đo ghi tắt là mạch / HA tâm thu / nhịp thở / SpO₂ / GCS.

## Thử tuổi, tra cứu và kiểm tra lỗi

- Với BN011 (tuổi 0), nhập 140/80/40/98/15: mức 5; đổi mạch thành 201: mức 1.
  Với BN012 (tuổi 1), nhập 100/100/24/98/15: mức 5.
- Với BN017 và BN018 (64 và 65 tuổi), nhập 95/120/18/98/15: lần lượt mức 5
  và mức 3; cho thấy luật nhóm tuổi khác nhau. Bản đánh giá lưu tuổi đã dùng.
- Tìm tên **nguyễn an** qua vùng tra cứu Visit sau khi tiếp nhận BN001 và
  BN004: hai Visit, không tự gộp theo tên. Thêm ngày tiếp nhận hôm nay và mã
  Visit chính xác để thử lọc AND. Ngày hiển thị/tìm là UTC+7.
- Tìm CCCD **000000000001** trong hộp chọn hồ sơ cũ: chọn được BN001 và giữ
  số 0 đầu. Tìm Patient chưa khám ở trang Hồ sơ, không phải danh sách Visit.
- Thử tạo Patient mới với mã BN001 hoặc CCCD 000000000001: bị từ chối. Thử mở
  Visit thứ hai cho BN001 khi còn Visit mở, hoặc dùng mã Visit đã có: bị từ chối.
- Thử BN010: tiếp nhận được, đánh giá bị từ chối vì chưa có tuổi; bổ sung tên,
  giới tính, ngày sinh hợp lệ rồi đánh giá lại. Mã BN010 và Visit cũ giữ nguyên.
- Sau khi gọi khám, thử chỉ định cận lâm sàng bằng mã bác sĩ chưa tham gia:
  bị từ chối. Chọn đúng BS_TEST để tiếp tục vòng cận lâm sàng/quay lại.
- Thử sinh hiệu sai (SpO₂ 101 hoặc GCS 2): báo lỗi; sửa đúng rồi lưu lại.
- Thử hủy Visit đang mở, hoặc kết thúc khi đang khám; lịch sử còn, có thể tiếp
  nhận Visit mới cho cùng Patient. Hủy hộp xác nhận thì trạng thái giữ nguyên.

File chính chỉ chứa dữ liệu hợp lệ để nạp trọn bộ. Muốn thử CSV lỗi, sao chép
ra file riêng rồi sửa một ngày sinh, mã/CCCD trùng hoặc bỏ một cột. Bộ đọc
phải báo lỗi và không trả Registry nạp dở. Không sửa trực tiếp file mẫu gốc.

Thay đổi trong demo mất khi thoát; mở lại cùng file để lặp kịch bản. Bộ dữ liệu
này chưa kiểm tra xếp lịch trực, tính áp lực hàng đợi hoặc ghi/khôi phục Visit,
vì các chức năng đó chưa được nối vào demo hiện tại.
