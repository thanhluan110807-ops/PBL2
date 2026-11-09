# Bàn giao PBL2 — cập nhật 09/11/2026

Đây là bản bàn giao chính; [bản trước](../handoffs/2026-11-03-chuyen-chat.md) là ảnh chụp lịch sử và có thông tin Git cũ.

Đọc [AGENTS.md](../../AGENTS.md), [CLAUDE.md](../../CLAUDE.md), tài liệu theo
tác vụ và kiểm tra Git trước khi làm việc. Đây là ảnh chụp từ phiên hiện tại,
không thay thế việc đọc code, các chỉ dẫn mới hoặc kiểm chứng mới. Nội dung của
file chưa commit sẽ không xuất hiện trong một checkout khác chỉ từ commit này.

## Hiện trạng khi chuyển chat — 09/11/2026

Phần này cập nhật và ưu tiên hơn các ảnh chụp cũ bên dưới. Đặc biệt, mục cũ
“Chưa triển khai trong lõi: ... TriageService”, thứ tự bắt buộc làm Calculator
trước TriageService, danh sách thay đổi cục bộ và đường dẫn retro không mô tả
checkout hiện tại. Không coi kết quả test lịch sử là kết quả chạy mới.

### Người dùng đang làm gì

Người dùng đã duyệt và yêu cầu triển khai các cải tiến demo bên dưới trên
nhánh `dev`, dùng skill UI/UX Pro Max đã cài trong repository. Sau các lượt
chỉnh giao diện, người dùng yêu cầu cập nhật toàn bộ thay đổi demo, commit và
push lên `dev`. Không suy yêu cầu đó thành được phép triển khai các đề xuất
tương lai. Người dùng muốn xem code `.h`/`.cpp` liên quan khi nhờ sửa class.

### Quyết định nghiệp vụ và phạm vi

- Đồ án C++17 sinh viên năm 2: phân loại cấp cứu, quản lí khám và lịch trực;
  Qt Widgets cho GUI, DSA tự viết, lưu file. Ưu tiên dễ đọc, dễ giải thích.
- `Patient` là hồ sơ một người, `Visit` là một lần đến khám, liên kết bằng mã;
  khám lại sau cận lâm sàng thuộc cùng Visit. Một Patient tối đa một Visit mở.
  [Thiết kế Registry](../superpowers/specs/2026-11-01-patient-registry-design.md).
- Có hai hàng ưu tiên lần đầu và quay lại, mức 1 nghiêm trọng nhất đến mức 5.
  Trong cùng hàng, cùng mức ưu tiên người vào trước. So hai đầu hàng, mức
  nghiêm trọng hơn được chọn; nếu bằng nhau thì luân phiên giữa hai hàng,
  lần hòa đầu ưu tiên quay lại. Các lượt không hòa không đổi lượt ưu tiên hòa.
  Đánh giá lại khi đang chờ giữ nguyên hàng và thời điểm vào hàng; mức/vị trí
  có thể đổi. Xem [TriageService](../../src/service/TriageService.cpp).
- Tự lưu Patient/Visit vào bộ file đã chọn sau thao tác, có dự phòng và phục
  hồi khi ghi gián đoạn; không có nút lưu dữ liệu Patient/Visit riêng. Không
  khẳng định bảo toàn tuyệt đối khi mất điện. Nhân sự/lịch có cơ chế lưu riêng.
  Chi tiết: [file phiên](../repository/visit-files.md), [phần B](../PHAN-B.md).
- Code phần đọc/ghi Patient/Visit theo yêu cầu trước đây không thêm `<memory>`
  hay class trừu tượng; không suy rộng hạn chế này thành cấm tuyệt đối trên
  toàn phần B, nơi code đã dùng `unique_ptr`. Quy tắc chung: [CLAUDE.md](../../CLAUDE.md).

### Đã triển khai trong checkout; giới hạn

- Model Patient/Visit, `PatientRegistry`, `TriageService`, DSA, quản lí file và
  demo Qt nối lõi; có khôi phục hai hàng và trạng thái phiên. Người dùng vẫn
  nhập mã Patient/Visit thủ công. Xem [demo](../../demo/qt-triage/README.md).
- `VisitSearchService::TimTheoBenhNhan` hiện kết hợp AND tên, ngày tiếp nhận
  UTC+7 và mã Visit; kết quả là Visit, không tự gồm Patient chưa có Visit.
  Đừng nhầm bộ lọc kết hợp hiện có với nâng cấp tra cứu chưa duyệt. Xem
  [thiết kế](../superpowers/specs/2026-11-04-visit-search-patient-design.md).
- Sau merge có code nhân sự, lịch trực, mệt mỏi, file và AdminPanel; xem
  [phần B](../PHAN-B.md). Sự tồn tại của module không chứng minh mọi tích
  hợp hoặc hành vi trong UML đã hoàn thành; kiểm tra theo yêu cầu cụ thể.
- **Nhân sự và lịch trực** hiện trong vùng nội dung chính qua `QStackedWidget`;
  `AdminPanel` vẫn giữ tên class, được nhúng dưới dạng widget và không mở
  hộp thoại riêng. Ba tab nhân sự/ca/lịch được giữ. Tự nạp
  `nhan-su-lich.pbl2` trong thư mục dữ liệu ứng dụng; một nút xanh
  **Lưu thay đổi** ghi trực tiếp. Ghi lỗi giữ thay đổi để thử lại; nguồn có
  sẵn lỗi đọc sẽ chặn lưu đè. Không còn các nút chọn/mở/xuất file trong GUI.
- Hồ sơ bệnh nhân mở ở chế độ xem, mỗi dòng có nhãn/giá trị, thông tin chưa
  biết để trống. **Bổ sung / cập nhật thông tin** mới hiện ô nhập danh tính;
  có lưu/hủy và quay lại phần xem. Chi tiết lần khám cũng xếp thông tin từ
  trên xuống. Chữ nội dung 18px, tiêu đề 24px; đánh giá sinh hiệu có dòng
  hướng dẫn 14px ngay dưới từng ô, đúng giới hạn nhập liệu trong `VitalSigns`.
- Tiếp nhận chỉ hiện tìm kiếm/danh sách khi chọn hồ sơ cũ. Chọn bệnh nhân tự
  điền dữ liệu: ô xám chỉ đọc là thông tin đã có, ô trắng cho bổ sung mục còn
  thiếu. `PatientRegistry::BoSungThongTinConThieu` kiểm tra CCCD, từ chối khi
  còn lượt khám đang mở, giữ mã/địa chỉ Patient và dữ liệu đã có. Có thể bổ
  sung tuổi ước lượng, dị ứng, tiền sử tại bước tiếp nhận lại.
- Đã bỏ ô tìm bệnh nhân trên thanh đầu và khối ghi chú/đường dẫn dài ở cuối
  trang. Ô tìm trong tab Lượt khám hỗ trợ tên/mã/CCCD không dấu; bộ lọc ngày
  và mã Visit tiếp tục đi qua `VisitSearchService`. Trạng thái lưu hồ sơ được
  tóm tắt trên thanh đầu. Luồng Patient/Visit vẫn tự lưu theo cơ chế đã có.
- Tra cứu bảng băm theo mã trung bình O(1), trường hợp xấu O(n); tìm tên/CCCD
  hiện duyệt dữ liệu, không nói mọi tìm kiếm là O(1). `Visit` có lịch sử đánh
  giá, bác sĩ tham gia và cận lâm sàng, không khẳng định có đầy đủ ghi chú khám.
- Visit CSV ghi v2, đọc v1/v2; thông tin vắng trong v1 không phải lịch sử thực.
  V2 có thể không tương thích chương trình cũ chỉ đọc v1.

### Đề xuất, chưa triển khai hoặc chưa chốt

- **Đăng nhập và phân quyền theo vai trò:** người dùng muốn ghi nhận để làm
  sau, chưa yêu cầu triển khai. Ý tưởng là có màn hình chung, liên kết người
  dùng với mã nhân viên và chỉ cho phép chức năng phù hợp:
  - Điều dưỡng tiếp nhận, tra cứu hồ sơ, nhập sinh hiệu và đánh giá phân loại.
  - Bác sĩ xem bệnh nhân được phân công và tra cứu Patient/Visit mình đã tham
    gia khám; phạm vi xem các Visit do bác sĩ khác khám còn phải thống nhất.
  - Người quản lý phân lịch bác sĩ/điều dưỡng, xác nhận và điều phối thay ca.
    Một người có thể có nhiều vai trò; quyền quản lý cần được cấp rõ ràng,
    không tự suy ra từ cấp bậc chuyên môn.
  - Có thể xem lịch cá nhân và đăng ký ca mong muốn. Khi đã xác định người
    dùng, dự kiến lấy mã nhân viên từ phiên thay vì nhập thủ công khi thao tác.
    Phải kiểm tra quyền khi thực hiện thao tác, không chỉ ẩn nút giao diện.
  - Chưa chốt chọn vai trò/mã để **mô phỏng** hay xác thực bằng mã và mật khẩu,
    cách cấp/quản lý tài khoản, lưu mật khẩu an toàn, các quyền chi tiết và
    phạm vi dữ liệu. Chọn vai trò/mã không chứng minh danh tính. Nếu bổ sung
    thư viện xác thực, cần thống nhất theo quy định dự án; không tự thiết kế
    thuật toán bảo mật. Tra cứu theo bác sĩ là đề xuất, chưa mặc định đã có.
  Không coi đây là phân quyền đã triển khai hoặc được duyệt thực hiện; chỉ làm
  khi người dùng yêu cầu riêng và sau khi chốt các điểm còn mở.
- Tính áp lực hàng đợi: người dùng đã chọn tạm hoãn. Không tự triển khai hoặc
  giả định TriageService hiện cần nó. Công thức, ngưỡng và tích hợp cần chốt
  khi được yêu cầu; [lịch sử kế hoạch cũ](../handoffs/2026-11-03-chuyen-chat.md).
- Tự sinh mã Patient/Visit: [chỉ là đề xuất tương lai](../repository/README.md#đề-xuất-tự-sinh-mã-patient-và-visit-trong-tương-lai),
  chỉ triển khai khi người dùng yêu cầu/duyệt riêng.
- Các [nâng cấp tra cứu](../repository/README.md#đề-xuất-nâng-cấp-tra-cứu-trong-tương-lai),
  như tìm theo bác sĩ và các hướng thiết kế mở rộng, chưa được duyệt triển khai.
  Người dùng yêu cầu tuyệt đối không tự triển khai các đề xuất tương lai.
- Không mặc định có cảnh báo quá hạn/tái đánh giá tự động chỉ từ thiết kế;
  kiểm tra code cụ thể trước khi khẳng định. Có thể chủ động đánh giá lại Visit
  đang chờ theo thao tác hiện có.

### Retro, tài liệu và giới hạn thông tin

Retro từng ghi nhận bỏ sót service/phụ thuộc và nhầm UML mục tiêu với hiện
trạng. Cách áp dụng còn phù hợp: đọc toàn cảnh, đối chiếu quyết định mới, code,
build, phụ thuộc và trình bày rõ phần đã có/đề xuất trước khi tư vấn; quy tắc
trong [AGENTS.md](../../AGENTS.md) và [CLAUDE.md](../../CLAUDE.md). README,
`CMakeLists.txt`, workflow và `tools/check_project_sources.py` hiện có cơ chế
kiểm tra/build riêng; không suy ra mọi cải tiến retro cũ đã được áp dụng.

Các đường dẫn từng nêu `docs/ROADMAP.md`, `docs/architecture/README.md`,
`docs/architecture/target.json`, `docs/development-workflow.md`, kế hoạch
`2026-11-03-retro-improvements.md` và công cụ kiến trúc không có trong checkout
đã kiểm tra. Bản ghi đầy đủ retro và bản bàn giao đầu vào trước 02/11 cũng chưa
tìm thấy; không tự suy diễn. Người dùng từng gửi ảnh UML tổng quan trong chat:
đó là kiến trúc mục tiêu ngày 03/11, không phải báo cáo tiến độ; ảnh/file ngoài
repo có thể không hiện diện trong chat mới, cần người dùng cung cấp lại nếu cần.
[Bản lịch sử](../handoffs/2026-11-03-chuyen-chat.md) có các ảnh chụp cũ.

### Git và kiểm chứng — ảnh chụp trước commit cải tiến demo

Ngày 09/11, checkout `/workspace/PBL2` ở `dev`, nền trước lần cập nhật này là
`b3f338a` (giao diện phân lịch đầu tiên). Đã fetch `origin/dev` và xác nhận
hai nhánh trùng nhau trước khi commit các cải tiến tiếp theo. Remote origin:
`https://github.com/thanhluan110807-ops/PBL2.git`. Xem Git thực tế để lấy commit
mới sau push; không suy trạng thái mới từ SHA nền này. Các thay đổi hiện tại
gồm GUI, API bổ sung thông tin còn thiếu, kiểm thử và tài liệu liên quan.
Giữ nguyên các thư mục build/kit Qt cục bộ và file bị ignore; không đưa chúng
hoặc ảnh chụp tạm vào commit. Các danh sách thay đổi bên dưới là lịch sử.

Kiểm chứng trên Linux, Qt5 Debug với `QT_QPA_PLATFORM=offscreen`: build đầy đủ
và CTest **30/30** đạt; `git diff --check` đạt. Bộ test mới
`test_patient_detail_widgets` kiểm tra xem/sửa/hủy hồ sơ và bố cục rộng/hẹp;
`test_reception_widgets` kiểm tra tự điền, khóa ô, bổ sung phần thiếu, lỗi và
lưu/đọc lại. Có kiểm tra lỗi cấp phát khi bổ sung để giữ nguyên hồ sơ cũ.
Đây là kiểm chứng cục bộ; chưa chạy lại Qt6/Windows native hay sanitizer cho
các thay đổi mới, không coi kết quả CI/sanitizer lịch sử là kết quả hiện tại.

---

## Lịch sử các ảnh chụp trước — đọc cùng phần cập nhật ở trên

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
