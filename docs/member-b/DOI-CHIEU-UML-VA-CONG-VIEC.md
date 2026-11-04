# Đối chiếu UML toàn dự án và công việc cần hoàn thành

Ngày đối chiếu: 05/11/2026. Căn cứ: ảnh `PBL2-UML-tong-quan-toan-du-an.png` người dùng gửi, code trong checkout `/workspace/PBL2`, và kế hoạch DOCX đã gửi trước đó. UML mới là căn cứ thiết kế cho lần đối chiếu này; các phần cũ của DOCX như ConsoleUI không được dùng để thay Qt Widgets trong UML.

Đây là bản phân tích yêu cầu, chưa phải thông báo đã cài đặt các phần còn thiếu. “Có một phần” nghĩa là có code liên quan nhưng chưa đáp ứng toàn bộ trách nhiệm và liên kết trong UML. Kết quả test cũ xác nhận hành vi đã viết, không chứng minh đã phủ hết UML. Hai bài `doc-file` và `backtracking` chỉ là bài học minh họa, không phải module bàn giao đầy đủ.

**Cập nhật sau lần đối chiếu:** Staff đã được tách thành các lớp, FatigueService, StaffFileManager và presenter; đã bổ sung trạng thái nhân viên/sàng lọc và lưu version 2. Xem [bảng chuyển API và phạm vi mới](TACH-STAFF-THEO-UML.md). Các bảng dưới đây ghi hiện trạng tại thời điểm đối chiếu trước lần tách, không phải mọi dòng còn thiếu đều giữ nguyên.

## 1. Những quy tắc phải giữ xuyên suốt

1. Giao diện Qt gọi service; model không gọi Qt hay xuất menu console.
2. Registry sở hữu đối tượng. Hàng đợi, kết quả tra cứu và StandbyPool chỉ mượn đối tượng; phải kiểm soát vòng đời.
3. Patient là hồ sơ lâu dài; Visit là một lần khám. Patient không sở hữu Visit theo quy tắc ghi ở sơ đồ; PatientRegistry quản lý cả hai nhóm.
4. Mã Patient và mã Visit giữ ổn định. Bổ sung CCCD/tên/ngày sinh không được đổi mã làm đứt liên kết.
5. Sau cận lâm sàng, bệnh nhân quay lại trong cùng Visit; một đợt đến viện mới sau khi kết thúc mới tạo Visit khác.
6. Một Visit có thể có nhiều bác sĩ được phân công và nhiều bác sĩ thực sự tham gia. Hai tập này không đồng nghĩa.
7. Cập nhật mức triage phải sửa vị trí heap; không tự nâng mức chỉ vì chờ lâu. Cảnh báo tái đánh giá khác với kết luận lâm sàng.
8. Thử lịch không làm thay đổi giờ làm/điểm thực tế. Lịch đề xuất, phân công được xác nhận và ca đã thực hiện cần phân biệt.
9. File lưu mã và giá trị, không lưu địa chỉ RAM. Phục hồi phải dựng lại chỉ mục, liên kết và hàng đợi.
10. Dấu `*` gắn bên tên một số lớp trong sơ đồ biểu thị tên/trách nhiệm còn cần chốt theo ghi chú, không phải bằng chứng đã có implementation. Kiểu thuộc tính có `Patient*`, `Visit*` là con trỏ và cần quy tắc mượn/sở hữu.

## 2. Toàn bộ các khối trên UML

### 2.1. Tầng giao diện

| Khối | Trách nhiệm phải thực hiện | Code hiện tại / khoảng thiếu |
|---|---|---|
| MainWindow | Hiển thị hàng chờ, lịch trực và cảnh báo; dùng service thật chung | Có DemoWindow và AdminPanel, nhưng clinical và lịch còn là hai nguồn dữ liệu riêng |
| AddEditPatientDialog | Thu thập đầu vào, gọi service thêm/cập nhật hồ sơ, hiển thị lỗi | Có form demo; chưa nối toàn bộ vào PatientRegistry/service thật |
| VisitSearchPanel | Đổi chế độ bệnh nhân/bác sĩ, chỉ hiện bộ lọc phù hợp, hiển thị kết quả Visit | Chưa có panel/service đúng thiết kế tra cứu trong UML |

UML không vẽ chi tiết mọi form nhân sự/lịch. Việc bổ sung widget hỗ trợ các thao tác phần B là triển khai UI, không thay đổi các tầng nghiệp vụ. Không cần tạo lớp rỗng chỉ để đủ tên trên sơ đồ.

### 2.2. Tầng service

| Khối | Trách nhiệm phải thực hiện | Code hiện tại / khoảng thiếu |
|---|---|---|
| TriageService | Đăng ký bệnh nhân/lần khám, đánh giá, gọi khám, cận lâm sàng, quay lại, cập nhật hàng chờ | Có DemoTriageService với dữ liệu mô phỏng, chưa là service tích hợp các registry/core model |
| VisitSearchService | Tìm bác sĩ theo tên/mã, lọc Visit, lấy thông tin bác sĩ tham gia | Chưa có |
| QueuePressureCalculator | Tính áp lực hàng chờ và xác định cần tăng cường | Chưa có; công thức/đơn vị/ngưỡng chưa được UML xác định |
| SchedulerService | Lập lịch dự kiến, đề xuất tăng cường, tìm người thay | Có Backtracking, kiểm tra lịch và tìm người thay; thiếu kết nối ScheduleRegistry, đề xuất tăng cường và quy trình xác nhận/áp dụng đầy đủ |
| FatigueService | Kiểm tra nhận ca, dự báo điểm và cập nhật sau ca; hỗ trợ kiểm soát sàng lọc | Đang khai báo chung trong SchedulerService.h; mới tính điểm/giờ dự kiến từ lịch, thiếu cập nhật sau thực hiện ca và theo dõi sàng lọc |

### 2.3. Tầng quản lý dữ liệu và file

| Khối | Trách nhiệm phải thực hiện | Code hiện tại / khoảng thiếu |
|---|---|---|
| PatientRegistry | Sở hữu Patient và Visit; tra cứu mã/tên/lịch sử; giữ liên kết | Chưa có registry này trong core |
| PatientFileManager | Lưu/đọc hồ sơ, Visit và các lịch sử liên quan | Chưa có bản đầy đủ |
| StaffRegistry | Sở hữu nhân viên, chỉ mục theo mã; tìm bác sĩ theo tên | Có thêm/sửa/xóa/tìm mã; thiếu tìm tên và hợp đồng vòng đời tương thích kết quả tra cứu mượn |
| StaffFileManager | Đọc/ghi đúng loại nhân viên và mọi trạng thái cần phục hồi | Chưa tách; AdminFileManager hiện chỉ lưu một phần trạng thái theo mô hình rút gọn |
| ScheduleRegistry | Quản lý ca và phân công theo mã, tra cứu và xác nhận phân công | Chưa có; Schedule hiện chỉ là hai Vector |
| ScheduleFileManager | Lưu/đọc ca, phân công và trạng thái liên quan | Chưa tách khỏi AdminFileManager; phân công hiện chưa có trạng thái để lưu |
| StandbyPool | LinkedList tham chiếu nhân viên dự phòng, thêm/loại/tìm ứng viên phù hợp | Có LinkedList mã và lọc ứng viên ở SchedulerService; khác cách biểu diễn con trỏ trên UML, cần chốt giữ mã hay dùng con trỏ mượn |

Việc lưu ID trong pool không tự nó là lỗi. Nhưng nếu giữ khác UML cần ghi rõ lý do và thống nhất thiết kế; không được gọi là khớp nguyên trạng. Tương tự, một snapshot tổng có thể hữu ích để phối hợp lưu nhất quán, nhưng không thay thế ba trách nhiệm FileManager được yêu cầu.

### 2.4. Tầng model/value object

| Khối | Yêu cầu | Hiện trạng |
|---|---|---|
| Person | Mã ổn định, thông tin cá nhân, tính tuổi theo ngày | Có; cần bảo toàn toàn bộ thông tin khi lưu/đọc lớp con |
| Patient | CCCD tùy chọn, nhóm máu, vô danh, bổ sung danh tính | Có model; tích hợp registry/file còn thiếu |
| Visit | Mã lần khám/bệnh nhân, tiếp nhận, trạng thái, đánh giá, cận lâm sàng, lịch sử bác sĩ | Có nhiều hành vi; chưa dùng DoctorParticipation như UML |
| TriageAssessment | Sinh hiệu, tuổi tại đánh giá, mức phân loại và lịch sử cần lưu | Có; cần persistence và tích hợp |
| VitalSigns | Các chỉ số, kiểm tra miền giá trị | Có; cần test/serialization đủ các trường |
| BanGhiCanLamSang | Chỉ định, kết quả, người chỉ định, trạng thái có kết quả | Có; cần persistence và kiểm tra liên kết |
| DoctorParticipation | Mã bác sĩ, thời điểm, vai trò trực tiếp khám; phân biệt với được phân công | Chưa có; Visit đang dùng hai Vector<string> cho phân công/lịch sử tham gia |
| VisitSearchFilter | Chế độ và điều kiện lọc; điều kiện không rỗng kết hợp AND | Chưa có |
| KetQuaTraCuuLanKham | Một hàng gồm const Patient* và const Visit* mượn | Chưa có |
| NhanVienYTe | Lớp trừu tượng, cấp bậc/kinh nghiệm, ghi nhận mệt mỏi, ca mong muốn | Có kế thừa Person; thiếu cập nhật trạng thái nghiệp vụ đầy đủ |
| Bacsi | Chuyên khoa, thông tin nội trú/loại nhân viên, điều kiện trưởng ca | Có BacSi, dùng cấp bậc thể hiện nội trú; cần thống nhất cách biểu diễn với UML và các điều kiện nhận ca |
| Dieuduong | Bắt đầu/kết thúc sàng lọc, giờ liên tục, kiểm tra xoay tua | Có DieuDuong nhưng chưa có trạng thái/hành vi sàng lọc này |
| Catrucmongmuon | Khung giờ mong muốn, cuối tuần, kiểm tra hợp lệ | Có CaTrucMongMuon; cần thống nhất tên và đơn vị thời gian |
| Shift | Mã ca ổn định, ngày giờ bắt đầu/kết thúc, thời lượng, giao nhau, đêm/cuối tuần | Có phút tương đối trong một tuần và kiểm tra giao nhau ở service; thiếu API/biểu diễn ngày giờ đáp ứng lịch xuyên tuần |
| ShiftAssignment | Mã phân công, mã nhân viên, mã ca, vai trò; xác nhận/hủy/ghi nhận bắt đầu | Chỉ có mã nhân viên, mã ca, vai trò; thiếu mã phân công và vòng đời |

Tên khác nhau như `Bacsi`/`BacSi` cần mapping rõ ràng trước khi sửa. Không tạo hai lớp cùng đại diện một đối tượng nghiệp vụ.

### 2.5. Custom DSA và lượt chờ

| Khối | Yêu cầu | Hiện trạng |
|---|---|---|
| Vector<T> | Mảng động, thêm/lấy/xóa, copy/move và quản lý bộ nhớ | Có; cần tiếp tục dùng/test phù hợp kiểu dữ liệu thật |
| LinkedList<T> | Node/list, thêm/tìm/xóa, deep copy node, move | Có; đã dùng cho chaining và pool |
| HashTable<K,V,Hash> | Bucket LinkedList, collision, tra cứu/rehash/copy/move | Có; còn phải tích hợp đủ registry |
| PriorityQueue<T,SoSanh> | Heap trên Vector, thêm/lấy/sửa ưu tiên | Có container; còn phải nối với QueueEntry và TriageService thật |
| QueueEntry | Mã lượt chờ và Visit* mượn, vòng đời tham chiếu rõ ràng | Chưa có kiểu core đúng UML |
| QueuePriorityComparator | So sánh ưu tiên; chính sách giữa luồng mới/quay lại được chốt | Chưa có kiểu core đúng UML; demo có cách xử lý mô phỏng riêng |

Không dùng việc demo chạy đúng một luồng làm bằng chứng rằng core đã dùng custom heap đúng UML.

## 3. Phần B cần hoàn thiện

Phân công B chính từ kế hoạch trước: Staff, LinkedList, HashTable, Backtracking. Người dùng bổ sung phạm vi file và đồng bộ GUI. UML là toàn dự án và không gán tác giả cho từng lớp, nên các điểm giao với A dưới đây là công việc phải phối hợp, không tự quy toàn bộ clinical sang B.

### B01 — Hoàn thiện mô hình nhân sự

- Chốt tên lớp và API tương thích Person.
- Nhân viên có mã ổn định, dữ liệu cá nhân, cấp bậc, kinh nghiệm và nguyện vọng ca.
- Định nghĩa rõ trạng thái mệt mỏi/giờ làm lưu trực tiếp và trạng thái suy ra từ lịch; tránh hai nguồn dữ liệu mâu thuẫn.
- Bacsi kiểm tra chuyên khoa, nội trú/cấp bậc và quyền trưởng ca.
- Dieuduong ghi thời điểm bắt đầu, kết thúc sàng lọc, tính liên tục và báo cần xoay tua.
- Dữ liệu sai phải báo lỗi, không âm thầm đổi về cấp bậc mặc định.

**Hoàn thành khi:** khởi tạo/cập nhật/đa hình/khôi phục đều giữ đúng nghề nghiệp và trạng thái; test mốc sàng lọc trước/đúng/sau 4 giờ.

### B02 — Hoàn thiện StaffRegistry và DSA

- Thêm/tìm mã/cập nhật/tìm tên; trả mọi kết quả trùng tên để người dùng chọn.
- Quy định hoa thường, dấu và tìm gần đúng; UML chưa chỉ định thuật toán chuẩn hóa.
- Kiểm soát nhân viên còn được tham chiếu bởi pool, phân công và lịch sử khám trước khi loại khỏi hệ thống.
- Chọn chính sách cập nhật giữ địa chỉ hoặc vô hiệu hóa kết quả mượn có kiểm soát. Hiện CapNhat thay đối tượng rồi delete bản cũ, có thể làm const pointer tra cứu tương lai bị treo.
- Test collision, rehash, copy/move node và quyền sở hữu nhân viên.

**Hoàn thành khi:** tra cứu tên/mã đúng; cập nhật/xóa không tạo con trỏ treo hoặc xóa mất lịch sử.

### B03 — Hoàn thiện Shift và ShiftAssignment

- Shift có ngày giờ rõ ràng, qua ngày/tuần, ca đệm, phân loại đêm/cuối tuần và nhu cầu nhân sự.
- Mã ca ổn định; tính số giờ và giao nhau tại một nơi thống nhất.
- Assignment có mã riêng, mã nhân viên, mã ca, vai trò và trạng thái nghiệp vụ.
- Có quy trình xác nhận, hủy và ghi nhận bắt đầu theo UML; chốt các trạng thái bổ sung cần lưu, không tự đoán enum cuối cùng.
- Không xóa toàn bộ lịch sử phân công chỉ để sửa thông tin nhân viên.

**Hoàn thành khi:** phân công được truy tìm bằng mã; chuyển trạng thái hợp lệ; ca qua ngày và tuần vẫn kiểm tra đúng khoảng nghỉ.

### B04 — Tạo ScheduleRegistry

- Quản lý hai tập ca/phân công bằng chỉ mục HashTable theo UML.
- Thêm/tìm ca, tìm phân công và truy lịch theo người/thời gian cho service/GUI.
- Kiểm tra ID trùng, nhân viên/ca tham chiếu tồn tại.
- Xác nhận/hủy qua nghiệp vụ, tách lịch dự kiến khỏi lịch chính thức.

**Hoàn thành khi:** scheduler, file manager và GUI sử dụng cùng một nguồn lịch, không mỗi nơi giữ một bản tự sửa.

### B05 — Tách và hoàn thiện FatigueService

- API riêng cho kiểm tra nhận ca, dự báo mệt mỏi và cập nhật sau ca.
- Tập trung công thức/chính sách tại một nơi; nhân viên lưu trạng thái, không chép lại công thức khác.
- Phân biệt điểm dự báo khi thử lịch với điểm ghi nhận sau ca thật.
- Nhận lịch sử liên quan để kiểm tra giờ tuần và nghỉ sau ca đêm qua biên tuần.
- Theo dõi sàng lọc liên tục và chỉ đề xuất/thực hiện xoay tua khi có người đủ điều kiện.
- Chốt nguồn áp lực hàng chờ, công thức, ngưỡng, thời điểm hồi phục/reset. Chưa có các con số này trong UML.

**Hoàn thành khi:** xem bảng/lập lại lịch/đọc lại file không làm cộng điểm lặp; trạng thái sau ca được phục hồi đúng.

### B06 — Hoàn thiện Backtracking trong SchedulerService

- Đầu vào gồm nhân sự, ca theo ngày giờ, lịch đã xác nhận, yêu cầu nhân lực, nguyện vọng và trạng thái mệt mỏi.
- Các vị trí trong một ca có vai trò/chuyên môn yêu cầu rõ ràng.
- Cắt nhánh sai nghề/chuyên môn/trưởng ca, trùng giờ, thiếu nghỉ, quá giờ tuần, quá ngưỡng mệt mỏi.
- Bảo toàn trạng thái ban đầu; hoàn tác mọi thay đổi tạm trong nhánh thất bại.
- Có ca cố định/phân công đã xác nhận; không tự viết đè chúng khi lập lại lịch.
- Chốt mục tiêu mềm: công bằng ca đêm/cuối tuần giữa người cùng cấp bậc, nguyện vọng, đáp ứng cao điểm.
- Phân biệt “sắp ứng viên rồi lấy lời giải đầu” với “tìm lời giải tốt nhất theo hàm mục tiêu”. Nếu yêu cầu tối ưu, phải so sánh lời giải và chọn thuật toán/cắt cận phù hợp.
- Giới hạn tìm kiếm, phân biệt không khả thi với chưa tìm xong; báo thiếu người/ca/vai trò và lý do phù hợp bằng chứng.
- Kết quả là đề xuất; chỉ áp dụng sau bước xác nhận qua ScheduleRegistry.

**Hoàn thành khi:** test một lịch buộc quay lui, dữ liệu nhiều ca, ca qua tuần, ca đã cố định, bài toán vô nghiệm và hết giới hạn; kiểm tra lại toàn bộ ràng buộc của lịch đầu ra.

### B07 — Tăng cường nhân lực và thay ca

- Nhận kết quả QueuePressureCalculator qua hợp đồng đã chốt với A.
- Đề xuất ca đệm/nhân sự tăng cường theo khung thời gian và nhu cầu, không chỉ đưa cảnh báo chung.
- Pool thêm/loại người dự phòng; quyết định dùng mã hoặc con trỏ mượn rõ ràng.
- Lọc theo vai trò, chuyên môn, trạng thái sẵn sàng, nghỉ, giờ và mệt mỏi; chốt “tương đương” cấp bậc thay vì tự áp bằng tuyệt đối trong mọi trường hợp.
- Xếp ứng viên theo tiêu chí rõ ràng; xác nhận thay, giữ dấu vết phân công cũ và tính lại phần trạng thái liên quan.
- Không có người thay phải báo rõ, không nới ràng buộc âm thầm.

**Hoàn thành khi:** thiếu người dự phòng, người đang bận, người quá mệt và thay thành công đều có kết quả đúng; lịch sử không mất.

## 4. Module file đầy đủ phải gồm những gì?

### 4.1. Cấu trúc trách nhiệm

- `StaffFileManager.h/.cpp`: nhân sự và trạng thái cần phục hồi.
- `ScheduleFileManager.h/.cpp`: ca, phân công và trạng thái/lịch sử đã thống nhất.
- `PatientFileManager.h/.cpp`: Patient, Visit, đánh giá, cận lâm sàng và lịch sử bác sĩ; phối hợp với A về API model.
- Có thể có hàm hỗ trợ chung đọc trường/chuyển số, nhưng không dồn mọi loại bản ghi vào một hàm dài khó học.
- Có lớp/hàm điều phối nạp/lưu toàn hệ thống để giữ cùng phiên bản snapshot nếu dùng nhiều file.

### 4.2. Dữ liệu phải bảo toàn

| Nhóm | Những dữ liệu không được bỏ |
|---|---|
| Nhân viên | Loại thực tế bác sĩ/điều dưỡng, mã, thông tin Person có sử dụng, cấp bậc, kinh nghiệm, chuyên khoa/nội trú, nguyện vọng, trạng thái mệt mỏi/giờ và sàng lọc đã chốt |
| Ca trực | Mã, bắt đầu/kết thúc có ngày, nhu cầu vai trò/chuyên môn, thông tin phân loại cần thiết hoặc quy tắc tái tính |
| Phân công | Mã riêng, mã nhân viên, mã ca, vai trò, trạng thái và mốc thời gian/lịch sử theo quy trình |
| Dự phòng | Nhân viên đã đăng ký và trạng thái đăng ký cần giữ; kiểm tra nhân viên tồn tại |
| Patient | Mã, danh tính thật/vô danh, CCCD tùy chọn, ngày sinh/tuổi ước lượng, nhóm máu, dị ứng, tiền sử |
| Visit | Mã lần khám/bệnh nhân, tiếp nhận, triệu chứng, trạng thái, toàn bộ đánh giá và kết quả cận lâm sàng |
| Bác sĩ tham gia | Người được phân công và DoctorParticipation thực sự tham gia, vai trò/thời điểm theo mô hình thống nhất |
| Hàng chờ | Dữ liệu đủ để phục hồi luồng, mốc vào hàng/thứ tự phá hòa và trạng thái; không suy ra gần đúng nếu làm đổi thứ tự |

### 4.3. Thứ tự khôi phục

1. Đọc vào vùng dữ liệu tạm; xác minh version và cấu trúc file.
2. Dựng Patient và nhân viên.
3. Dựng Visit, lịch sử và ca trực.
4. Dựng phân công, pool và các liên kết mã; kiểm tra tham chiếu thiếu/trùng.
5. Dựng chỉ mục tìm kiếm và các hàng chờ đang hiệu lực; kiểm tra không trùng lượt.
6. Kiểm tra tính nhất quán toàn hệ thống, sau đó mới công bố dữ liệu cho GUI.

Bước 3–5 có thể điều chỉnh theo dependency thực tế, nhưng không được truy con trỏ đến đối tượng chưa dựng.

### 4.4. Xử lý lỗi và lưu an toàn

- File mất, rỗng, không có quyền, bị cắt ngang, version sai.
- Trường bắt buộc thiếu, số/enum/ngày sai, ID trùng và mã tham chiếu không tồn tại.
- Tên tiếng Việt, khoảng trắng, dấu phân cách/dấu nháy và ký tự xuống dòng phải có quy tắc định dạng rõ ràng.
- Báo file/dòng/trường và nguyên nhân đủ để sửa. Chốt dừng toàn bộ hay bỏ dòng; không tự gọi “nạp đầy đủ” khi đã bỏ dữ liệu.
- Không mất dữ liệu đang làm nếu nạp lỗi.
- Ghi file tạm và chỉ công bố snapshot khi thành công. Với nhiều file, cần phiên bản chung/manifest hoặc cơ chế tương đương để không đọc lẫn hai lần lưu.
- Save→load phải giữ giá trị và quan hệ, không đòi địa chỉ con trỏ mới giống địa chỉ trước khi đóng chương trình.
- Chốt có hỗ trợ lưu nháp/chưa đủ nhân lực không. Bản hiện tại chặn lưu khi có ca nhưng chưa đủ phân công; không nên tự coi đó là yêu cầu từ UML.

**Hoàn thành khi:** mở ứng dụng mới khôi phục đúng dữ liệu, lịch sử, liên kết, hàng chờ và bảng GUI; lỗi giữa chừng không làm hỏng snapshot tốt.

## 5. Các điểm tích hợp A–B không được quên

### 5.1. Tra cứu Visit

- VisitSearchFilter và chế độ bệnh nhân/bác sĩ; chỉ dùng các bộ lọc phù hợp.
- Tìm tên bệnh nhân là luồng chính; hiển thị mã, ngày sinh, CCCD để phân biệt người trùng tên.
- Bác sĩ có thể tìm bằng tên/mã; tên trùng phải chọn đúng người.
- Các bộ lọc kết hợp AND; mã khớp chính xác; ngày lọc là ngày tiếp nhận theo UTC+7.
- Một hàng là một Patient–Visit; không lặp cùng Visit, gần nhất trước.
- Patient chưa có Visit vẫn tìm được ở tiếp nhận, không bịa một hàng lịch sử Visit.
- Lọc bác sĩ dựa trên lịch sử trực tiếp tham gia; không tính chỉ được phân công nhưng chưa khám.
- Người thay có thể trực tiếp khám hoặc ghi thay; cần chốt AssignmentRole và vai trò DoctorParticipation, không tự gộp.
- Một Visit có nhiều bác sĩ chỉ xuất hiện một hàng trong kết quả; danh sách người tham gia hiển thị ở thông tin chi tiết.

### 5.2. QueueEntry và áp lực

- Service thật dùng custom heap với QueueEntry mượn Visit.
- Chốt thứ tự nguy kịch, phá hòa và chính sách luồng mới/quay lại; UML ghi chính sách còn cần chốt, không tự coi 3:1 trong demo là yêu cầu cuối cùng.
- Gọi khám/kết thúc/hủy phải loại phần tử chờ phù hợp; thay mức phải sửa heap.
- Dữ liệu tính áp lực lấy từ lượt chờ hiệu lực, không đếm trùng hoặc đếm lượt đã kết thúc.
- QueuePressureCalculator xác định dữ liệu/đơn vị đầu ra; SchedulerService dùng nó để đề xuất tăng cường.

### 5.3. Đồng bộ giao diện

- MainWindow và các panel dùng chung service/registry thật.
- Cập nhật nhân sự, xác nhận/hủy phân công, thay người, nhận ca và kết thúc ca làm mới đúng bảng/cảnh báo.
- Nạp file không chỉ đổi bảng mà phải thay nguồn dữ liệu nghiệp vụ nhất quán.
- Cập nhật nhân viên không xóa lịch sử khám đã tham chiếu mã của người đó.
- Có danh sách đề xuất, hành động xác nhận và trạng thái sau áp dụng; không coi lịch thử là đã chính thức.
- Khởi động lại phục hồi đúng trạng thái; test cần mở một instance dữ liệu mới, không chỉ refresh cửa sổ đang mở.

## 6. Thứ tự thực hiện để không phải viết lại

1. Chốt hợp đồng dữ liệu: tên lớp/API, mã ổn định, ngày giờ, enum vai trò/trạng thái, ownership.
2. Hoàn thiện Staff, Shift, ShiftAssignment, StaffRegistry và ScheduleRegistry.
3. Hoàn thiện DoctorParticipation cùng A, xác định API truy lịch sử.
4. Tách FatigueService, hoàn thiện các điều kiện nhận ca và trạng thái sàng lọc.
5. Viết StaffFileManager/ScheduleFileManager theo dữ liệu đầy đủ; nối PatientFileManager khi API clinical đã thống nhất.
6. Hoàn thiện Backtracking và xác nhận lịch; test trạng thái hoàn tác.
7. Hoàn thiện pool, thay ca, xoay tua và tăng cường theo áp lực.
8. Tra cứu Visit và đồng bộ Qt qua service thật.
9. Kiểm thử phục hồi toàn hệ thống, rà lại từng hàng của tài liệu này.

Giải thích cho sinh viên nên chia theo các module này và giải thích các hàm ngắn, nhưng module bàn giao vẫn phải đầy đủ đầu vào, nghiệp vụ, đầu ra và lỗi. Không thay module thật bằng mảng cố định hai người hoặc file ba trường.

## 7. Bộ tiêu chí nghiệm thu tối thiểu

| Mã | Kịch bản | Kết quả phải có |
|---|---|---|
| T01 | Hai nhân viên trùng tên, khác mã | Tra cứu không chọn nhầm, giữ được cả hai |
| T02 | Collision/rehash/copy/move DSA | Không mất khóa, không leak/double-delete |
| T03 | Nhân viên sai cấp bậc/vai trò/chuyên môn | Không nhận ca sai |
| T04 | Ca qua ngày, qua tuần và vừa chạm biên | Tính giao nhau/nghỉ chính xác |
| T05 | Nghỉ sau ca đêm thiếu một phút/đúng mốc | Một trường hợp bị loại, một được nhận |
| T06 | Giờ tuần, ngưỡng mệt mỏi tại biên | Không vượt giới hạn; xem lại không cộng lặp |
| T07 | Nhánh đầu sai, nhánh sau có lời giải | Quay lui và hoàn tác đầy đủ |
| T08 | Vô nghiệm và hết giới hạn tìm | Trạng thái phân biệt, không làm hỏng lịch cũ |
| T09 | Ca/phân công đã xác nhận | Không bị ghi đè khi đề xuất lịch mới |
| T10 | Xác nhận/hủy/bắt đầu ca | Chuyển trạng thái đúng, giữ lịch sử |
| T11 | Thay ca thành công/không có người đủ điều kiện | Giữ ràng buộc và dấu vết người cũ |
| T12 | Sàng lọc trước/đúng/sau 4 giờ | Cảnh báo/đề xuất xoay tua theo chính sách |
| T13 | Nhu cầu tăng theo áp lực hàng chờ | Đề xuất tăng cường phù hợp, không vi phạm lịch |
| T14 | Visit nhiều bác sĩ, có người chỉ được phân công | Bộ lọc trực tiếp khám trả đúng, Visit không lặp |
| T15 | Patient vô danh được bổ sung danh tính | ID/Visit/hàng chờ/lịch sử giữ nguyên liên kết |
| T16 | Cận lâm sàng rồi quay lại | Cùng Visit, đúng luồng và lịch sử |
| T17 | Lưu rồi mở dữ liệu mới | Phục hồi đầy đủ các giá trị, liên kết và trạng thái |
| T18 | File hỏng/trùng mã/mã tham chiếu thiếu | Báo cụ thể; dữ liệu tốt không bị thay mất |
| T19 | Lỗi khi lưu một trong nhiều file | Không công bố snapshot chắp vá |
| T20 | GUI thêm/sửa/xác nhận/thay/nạp file | Bảng, service và registry cùng trạng thái |

Đây là checklist cần thực hiện/đối chiếu khi triển khai, không phải danh sách test đã chạy và đã đạt.

## 8. Các quyết định UML chưa đủ thông tin để tự kết luận

1. **Chính sách lịch và mệt mỏi:** công thức/ngưỡng, hồi phục/reset, mục tiêu tối ưu, nhu cầu chuyên môn/giám sát nội trú và lịch sử cần giữ ở biên tuần.
2. **Chính sách lượt khám:** thứ tự giữa hai luồng, các vai trò trực tiếp khám/ghi thay và cách đồng nhất ngày/giờ.
3. **Hợp đồng lưu trữ và phạm vi phối hợp:** định dạng/version, lỗi nào dừng hoặc bỏ dòng, lưu nháp, tính nhất quán nhiều file, đầu việc A/B cho PatientFileManager và tra cứu clinical.

Các mặc định 12 giờ nghỉ giữa ca, 48 giờ/tuần, điểm tối đa 100 và hệ số điểm của bản trước là lựa chọn cài đặt trước đây, không phải các con số được chứng minh bởi UML. Trước khi gọi bản mới “đầy đủ”, phải chốt chính sách và hoàn thành các tiêu chí liên quan.
