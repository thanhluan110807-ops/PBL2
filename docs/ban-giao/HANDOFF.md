# Bàn giao PBL2 — 03/11/2026

Đây là bản bàn giao chính; [bản trước](../handoffs/2026-11-03-chuyen-chat.md) là ảnh chụp lịch sử và có thông tin Git cũ.

Đọc [AGENTS.md](../../AGENTS.md), [CLAUDE.md](../../CLAUDE.md), tài liệu theo
tác vụ và kiểm tra Git trước khi làm việc. Đây là ảnh chụp từ phiên hiện tại,
không thay thế việc đọc code, các chỉ dẫn mới hoặc kiểm chứng mới. Nội dung của
file chưa commit sẽ không xuất hiện trong một checkout khác chỉ từ commit này.

## Lưu ý khi chuyển chat

Các file `docs/ROADMAP.md`, `docs/architecture/`, `docs/development-workflow.md`, kế hoạch retro và công cụ kiến trúc được nhắc bên dưới hiện chỉ có cục bộ, chưa được theo dõi/commit. Chúng không có trong commit bàn giao hoặc trên GitHub từ commit đó. Những sửa đổi retro trong CLAUDE, README, CMake, workflow và code/test cũng còn cục bộ. Nếu chat mới thiếu chúng, ghi nhận thiếu, không tự giả định nội dung. Gói chuyển cục bộ tại `/workspace/outputs/pbl2-chuyen-chat-2026-11-03.tar.gz` là ảnh chụp trước lượt này, không nằm trong repository và có thể không có ở môi trường khác.

## Mục tiêu và nguồn

Đồ án C++17 cho sinh viên năm 2 mô phỏng khoa cấp cứu: phân loại 5 mức, hàng
đợi và khám quay lại; đồng thời xếp lịch trực và theo dõi mệt mỏi. GUI định
hướng Qt Widgets, lưu dữ liệu file văn bản/CSV; nghiệp vụ và DSA độc lập Qt.
Đọc [CLAUDE.md](../../CLAUDE.md), [README](../../README.md),
[lộ trình](../ROADMAP.md) và [kiến trúc mục tiêu](../architecture/README.md).

Đã đọc lại bản bàn giao 02/11 trong tệp người dùng gửi `Pasted text.txt`, thiết
kế Registry, code liên quan, tài liệu retro và trạng thái Git. Bản 02/11 nói
Registry chưa code và ba quy tắc còn mở: đó là trạng thái cũ, đã được quyết
định sau và triển khai. Bản bàn giao đầu vào được nhắc trong tài liệu 02/11
không tìm thấy trong các nguồn đã kiểm tra; không suy diễn nội dung thiếu.
Bản ghi đầy đủ riêng của cuộc retro cũng không tìm thấy; kết quả áp dụng được
ghi trong [kế hoạch retro](../superpowers/plans/2026-11-03-retro-improvements.md).

## Quyết định đã chốt

- Patient là hồ sơ một người, mã nội bộ ổn định, khác CCCD; Visit là một lần
  khám có mã riêng và liên kết Patient qua mã. Bên gọi cấp mã, kể cả bệnh nhân
  vô danh; mã Patient duy nhất trong Patient và mã Visit duy nhất trong Visit,
  trùng bị từ chối, không ghi đè. Registry tự tạo, sở hữu và giải phóng cả hai.
- CCCD tùy chọn, các CCCD rỗng không trùng. Khi có CCCD, tìm trước; người tiếp
  nhận xác nhận đúng người rồi dùng mã Patient cũ. CCCD trùng với thông tin
  khác phải kiểm tra, không tự ghi đè hay tạo hồ sơ trùng. Không tự nhận diện
  hoặc gộp theo tên; chưa gộp hồ sơ. Tìm theo mã Patient, mã Visit, CCCD, tên
  một phần không phân biệt hoa/thường. Xem chi tiết và giới hạn Unicode trong
  [tài liệu Registry](../repository/README.md).
- Tối đa một Visit mở cho mỗi Patient; chỉ `DaKetThuc` và `DaHuy` là đóng.
  Đóng/hủy không xóa Patient hay lịch sử. Tìm thấy Patient cũ vẫn phải kiểm tra
  Visit mở. [Thiết kế](../superpowers/specs/2026-11-01-patient-registry-design.md)
  và [code](../../include/repository/PatientRegistry.h) mô tả các lựa chọn API.
- Visit phân biệt bác sĩ được phân công và bác sĩ thực sự tham gia; có thể có
  nhiều bác sĩ tham gia. Chỉ định cận lâm sàng thuộc bác sĩ đã tham gia; không
  có trường ghi chú lâm sàng chung. Ngày tính tuổi từ thời điểm đánh giá theo
  UTC+7; nhân viên đi khám vẫn là bệnh nhân, API liên kết nhân viên chưa chốt.
  Xem [Visit.h](../../include/model/Visit.h) và [Visit.cpp](../../src/model/Visit.cpp).
- C++17, DSA tự viết, code và chú thích tiếng Việt dễ hiểu; include trực tiếp
  header dùng trong từng file, không include `<cstddef>`. Quy tắc chi tiết và
  ngoại lệ chữ ký cấp phát test ở [CLAUDE.md](../../CLAUDE.md).

## Hiện trạng và kiến trúc mục tiêu

- **Đã triển khai:** Person, Patient, Visit, PatientRegistry và DSA dùng chung;
  `pbl2_core` và test đăng ký trong [CMakeLists.txt](../../CMakeLists.txt).
  Registry duyệt Patient khi tìm tên/CCCD, duyệt Visit khi lấy lịch sử/kiểm tra
  Visit mở; con trỏ Patient mượn chỉ đọc. Xem tài liệu Registry để biết giới hạn.
- **Bản thử đã có và đã nằm trong commit:** `TraCuuLanKham` lọc tên, ngày tiếp
  nhận UTC+7 và mã Visit, trả mỗi Visit một dòng, mới nhất trước. Đây vẫn là bản
  thử, không coi là thiết kế nâng cấp đã duyệt; Patient chưa có Visit được tìm
  qua API tìm Patient riêng. Mô tả cũ nói “workspace có code thử” là ghi nhận
  tại thời điểm viết, không có nghĩa code đó hiện chưa commit.
- **Chưa triển khai trong lõi:** QueuePressureCalculator, TriageService và các
  luồng xếp lịch tích hợp. `demo/qt-triage/` là demo mô phỏng tách biệt, không
  chứng minh service lõi đã có. Cần đọc code thực tế khi tiếp tục.
- **Đề xuất tương lai, chưa yêu cầu triển khai:** màn hình tìm theo bệnh nhân
  hoặc bác sĩ trực tiếp khám; lọc AND, ngày tiếp nhận UTC+7, một dòng mỗi Visit,
  vai trò phân công/thay thế lưu tại thời điểm tham gia. Tên trùng cần đối chiếu;
  Patient chưa khám vẫn tìm được riêng. Chỉ mục tên/CCCD và thuật toán sắp xếp
  cần đánh giá và đồng bộ khi cập nhật. Xem toàn bộ yêu cầu trong
  [đề xuất tra cứu](../repository/README.md#đề-xuất-nâng-cấp-tra-cứu-trong-tương-lai).
- [target.json](../architecture/target.json) là nguồn UML **mục tiêu**, không
  phải tiến độ; các lớp đánh dấu `*` và quan hệ kế thừa nhân viên cũ vẫn là đề
  xuất. Ảnh tổng quan đã xuất tại
  `/workspace/outputs/pbl2-uml-kien-truc-muc-tieu-2026-11-03/PBL2-UML-tong-quan-toan-du-an.png`
  cùng ảnh chi tiết và README trong thư mục đó. Đây là output ngoài repository,
  có thể không tồn tại trong môi trường mới; cách xuất lại ở README kiến trúc.

## Retro và cách áp dụng

Trợ lý từng bỏ sót TriageService rồi QueuePressureCalculator khi tư vấn bước
tiếp theo, và từng nhầm UML hiện trạng với UML toàn dự án. Các cải thiện đã làm
cục bộ: [lộ trình phụ thuộc](../ROADMAP.md), nguồn UML và generator/checker,
[quy trình](../development-workflow.md), kiểm tra include trực tiếp và cstddef,
CMake/CTest và workflow, cập nhật README/CLAUDE. Kiểm tra include chỉ nhận biết
các ký hiệu được khai báo trong checker, không phân tích đầy đủ C++. Cần đối
chiếu toàn kiến trúc, code và quyết định còn mở trước khi đề xuất thứ tự.

Theo kế hoạch retro, lần kiểm thử trước đạt Release 11/11 và Debug ASan/UBSan
11/11 với `detect_leaks=0`; LeakSanitizer chưa xác minh do giới hạn ptrace.
Đây là kết quả phiên trước trên workspace lúc đó, không phải lần chạy mới trong
lượt bàn giao, cũng không phải kết quả GitHub CI. Kiểm tra mới tùy thay đổi.

## Bước tiếp theo và điểm chưa chốt

Theo [lộ trình](../ROADMAP.md), bước gần nhất là xác định và chốt đầu vào, đơn
vị, công thức, ngưỡng và ví dụ tính áp lực hàng đợi trước khi triển khai
QueuePressureCalculator; TriageService cần bộ tính và hợp đồng hàng chờ. Thứ
tự Calculator với QueueEntry phụ thuộc Calculator nhận số liệu tổng hợp hay
QueueEntry, chưa đủ căn cứ chốt tuyệt đối. Quy tắc hàng chờ/quay lại, phá hòa,
API service, công thức mệt mỏi, lịch trực và lưu/khôi phục còn cần đối chiếu
và chốt theo phạm vi việc tiếp theo. Không tự gán công thức hay trọng số.

Nhánh `patient-class` từng được bản bàn giao cũ nêu là nguồn luật triage; chưa
đối chiếu đầy đủ các ngưỡng và tổ hợp trong lượt này. Các mốc chờ trong CLAUDE
được ghi là ví dụ. Không tuyên bố đã xác minh tương đương toàn bộ luật triage.

## Git và thay đổi cần bảo toàn

Ảnh chụp trước commit/push lượt này: nhánh `work`, HEAD `ed432d532548d1378232440994cfeaa1dbc6382f`, `origin/work` lưu cục bộ `742c995491bf9ad01a6dd54f8eb0edce158f8830`, ahead 1; index trống. Remote `origin` là `https://github.com/thanhluan110807-ops/PBL2.git`. Các thông tin này có thể thay đổi sau commit/push; kiểm tra lại Git và remote. Không suy trạng thái GitHub từ origin/dev cục bộ.

Người dùng chốt làm việc trên `work`; sau khi hoàn tất và kiểm tra phù hợp thì merge vào `dev` theo yêu cầu, đối chiếu dev mới nhất và xử lý xung đột. PatientRegistry đã có trên GitHub nhánh work, commit thêm hai file `a2821fb`; không nhầm với các cải thiện retro chưa commit.

Các thay đổi cục bộ có trước commit bàn giao, giữ nguyên và không đưa vào commit:

- Đã theo dõi: `.github/workflows/build-test.yml`, `CLAUDE.md`, `CMakeLists.txt`,
  `README.md`, `include/dsa/PriorityQueue.h`, `src/model/Patient.cpp`,
  `src/model/Person.cpp`, `src/model/Visit.cpp`,
  `tests/checks/test_project_checks.py`, `tests/dsa/test_priority_queue.cpp`,
  `tests/dsa/test_vector.cpp`, `tests/model/test_person.cpp`,
  `tests/model/test_visit.cpp`, `tools/check_project_sources.py`.
- Chưa theo dõi: `docs/ROADMAP.md`, `docs/architecture/README.md`,
  `docs/architecture/target.json`, `docs/development-workflow.md`,
  `docs/superpowers/plans/2026-11-03-retro-improvements.md`,
  `tests/checks/test_architecture_checks.py`, `tools/check_architecture.py`,
  `tools/generate_uml.py`.

Các thay đổi cũ trong CLAUDE.md và README.md vẫn chưa commit theo phạm vi yêu cầu; chỉ phần bổ sung mới thuộc lượt này được tách để commit nếu kiểm tra an toàn. Các file gốc `HashTable.h`, `Linkedlist.h`,
`Staff.cpp`, `staff.h` là phần của thành viên khác: bảo toàn, chỉ đọc khi cần.
Không tự hoàn tác thay đổi, commit hoặc push nếu không được yêu cầu mới.


## Bổ sung 04/11/2026: tự động lưu Patient/Visit

Theo chỉ dẫn mới nhất, cập nhật trực tiếp bộ Patient/Visit người dùng chọn,
có backup. Không có nút lưu riêng: tự ghi đồng bộ sau thao tác, giữ RAM/tạm
chặn chỉnh sửa/tự thử lại nếu ghi lỗi; không đóng khi còn chưa ghi được.
SessionFileManager dùng rawbackup, snapshot, pending journal và committed
marker để hoàn tất cặp file khi bị dừng giữa hai lần ghi. Cấu hình chỉ nhớ
đường dẫn; lần sau tự nạp lại bộ. Khóa một tiến trình bằng QLockFile.
Xem [hướng dẫn](../repository/visit-files.md) và
[thiết kế](../superpowers/specs/2026-11-04-patient-visit-persistence-design.md).
Người dùng đã yêu cầu commit/push lên work; kiểm tra Git để xác định commit thực tế.
Qt5 Debug21/21 và ASan/UBSan21/21
với detect_leaks=0; SIGKILL/failure tests đạt. Qt6.11.2 mới kiểm tra biên dịch,
chưa runtime Qt6/Windows native hoặc mất điện vật lý. Chưa tự sinh mã.

Ngày 04/11/2026, người dùng yêu cầu đưa cải tiến tự sinh mã Patient/Visit vào
[mục đề xuất tương lai](../repository/README.md#đề-xuất-tự-sinh-mã-patient-và-visit-trong-tương-lai).
Chỉ ghi nhận đề xuất, **chưa triển khai và không tự triển khai nếu chưa được
yêu cầu/duyệt riêng**. Hướng đề xuất là Registry cấp hai dãy BN/LK, tiếp tục
từ dữ liệu đã lưu; quy tắc không tái sử dụng mã khi phục hồi bản cũ còn cần chốt.

## Hợp nhất work vào dev — 05/11/2026

Theo yêu cầu người dùng, ghép work `4389bee` vào dev đã cập nhật từ GitHub
`9c6f1a1`; kiểm tra Git để lấy merge commit thực tế. Giải quyết xung đột bằng
cách giữ cả nguồn/test nhân sự-lịch của dev và nguồn/test Patient-Triage-file
của work. Giữ các file Staff/DocFile/docs member-b cũ ở trạng thái đã xóa.
Demo dùng code lõi bệnh nhân thật, có nút mở AdminPanel trên desktop và menu
hẹp; phần nhân sự/lịch giữ cơ chế lưu riêng của phần B.

DoctorParticipation trên dev có thời điểm/vai trò/người được thay. Snapshot
Visit được ghép để giữ đầy đủ metadata; CSV Visit ghi v2 và đọc v1/v2. Dữ liệu
v1 thiếu thời điểm/vai trò giữ epoch0 và giá trị vai trò tương thích, không
coi đó là bằng chứng lịch sử thực. Chưa làm chế độ tra cứu bác sĩ tương lai.
Bản chương trình cũ chỉ đọc v1 sẽ không đọc được file v2 vừa ghi từ dev mới.

Kiểm tra lượt merge: Qt5 Debug28/28, ASan/UBSan28/28 với detect_leaks=0,
Release core24/24. Qt6.11.2 kiểm tra biên dịch nguồn GUI/test đạt, chưa chạy
runtime Qt6/Windows hoặc LeakSanitizer. Reviewer độc lập không thấy lỗi chặn;
đã sửa số lượng test và mô tả lịch trực cũ trong README. Có sửa signal
currentIndexChanged ở AdminPanel để chọn overload int trên Qt5/Qt6.
