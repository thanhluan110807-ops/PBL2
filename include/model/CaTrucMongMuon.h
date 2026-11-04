// Phần B — Chức năng chính: Lưu và kiểm tra nguyện vọng ca trực.
#pragma once

// Nguyện vọng giờ trong ngày, khác với Shift là một ca có thời điểm cụ thể.
struct CaTrucMongMuon {
    int GioBatDau = 7;
    int GioKetThuc = 15;
    bool MongMuonCuoiTuan = false;
    bool HopLe() const;
};
