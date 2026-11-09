# UI/UX Pro Max trong PBL2

- Nguồn: https://github.com/nextlevelbuilder/ui-ux-pro-max-skill
- Cài ngày 09/11/2026 bằng CLI chính thức `ui-ux-pro-max-cli@2.15.0`.
- Lệnh: `npx ui-ux-pro-max-cli@2.15.0 init --ai codex --offline`.
- Giữ riêng gói `ui-ux-pro-max`; các skill khác do CLI cài kèm không thuộc bản cài này.
- Python 3 và thư viện chuẩn là đủ để chạy công cụ tìm kiếm; không thêm dependency cho ứng dụng C++.
- PBL2 dùng Qt Widgets/QSS. Áp dụng hướng dẫn màu sắc, bố cục, tương phản, bàn phím và phản hồi thao tác; không chuyển sang web hoặc thêm thư viện UI khác. Skill chưa có bộ quy tắc riêng cho Qt.
- Chạy từ gốc repository: `python3 .agents/skills/ui-ux-pro-max/scripts/search.py "healthcare scheduling dashboard" --design-system -p "An Tam"`.
- Các phiên Codex sau khám phá skill qua `.agents/skills/`; phiên hiện tại có thể đọc trực tiếp `SKILL.md`.
- Giấy phép MIT của nguồn được giữ tại `LICENSE`.

## Phạm vi kiểm tra bản cài

Hai mô-đun test phát triển `test_catalog_refresh.py` và `test_relevance_evaluator.py` cần các script cấp repository nguồn mà CLI không đóng gói (`refresh-google-fonts.py`, `refresh-icon-catalog.py`, `evaluate-relevance.py`). Vì vậy chạy discovery toàn bộ test đi kèm bản cài sẽ lỗi import ở hai mô-đun này. Đây không phải các công cụ cần để chạy `search.py`; giữ nguyên nguồn upstream và chỉ kiểm tra các mô-đun runtime có đủ phụ thuộc khi kiểm chứng bản cài trong PBL2.
