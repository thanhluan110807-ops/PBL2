# Skill Retro

Skill `retro` dùng để nhìn lại một phiên làm việc với coding agent và đề xuất cách cải thiện môi trường cho những lần làm việc sau. Skill đọc phiên được chỉ định; nếu không chỉ định, mặc định xem xét phiên hiện tại.

Các nội dung được xem xét gồm khả năng tìm đúng tệp, kiểm tra tự động và CI, quy chuẩn lập trình, hướng dẫn trong AGENTS.md/CLAUDE.md, hiệu quả sử dụng công cụ và khả năng tiếp cận thông tin. Kết quả là các đề xuất cải thiện được trình bày theo mức độ nghiêm trọng.

Skill được thiết kế để người dùng chủ động gọi khi cần tổng kết một phiên. Nội dung nằm tại `.agents/skills/retro/SKILL.md`. Khi thực thi, skill yêu cầu tham khảo skill `writing-for-agents`; skill tham khảo đó đã được cài tại `.agents/skills/writing-for-agents/`.
