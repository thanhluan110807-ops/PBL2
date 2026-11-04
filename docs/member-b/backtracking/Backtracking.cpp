#include "Backtracking.h"

bool XepCa(int CaHienTai, const bool BacSiLamTruong[2],
           const bool CaCanTruong[2], int PhanCong[2]) {
    if (CaHienTai == 2) return true; // Đã xếp xong ca 0 và ca 1.

    for (int BacSi = 0; BacSi < 2; ++BacSi) {
        bool DaDuocXep = false;
        for (int CaCu = 0; CaCu < CaHienTai; ++CaCu)
            if (PhanCong[CaCu] == BacSi) DaDuocXep = true;
        if (DaDuocXep) continue; // Hai ca trùng giờ, một người không làm cả hai.

        if (CaCanTruong[CaHienTai] && !BacSiLamTruong[BacSi])
            continue; // Cắt nhánh: bác sĩ không đủ điều kiện làm trưởng ca.

        PhanCong[CaHienTai] = BacSi; // Bước 1: chọn thử.
        if (XepCa(CaHienTai + 1, BacSiLamTruong, CaCanTruong, PhanCong))
            return true; // Bước 2: phần còn lại xếp được thì giữ kết quả.
        PhanCong[CaHienTai] = -1; // Bước 3: bế tắc, bỏ lựa chọn để thử người khác.
    }
    return false; // Đã thử hết bác sĩ mà ca này vẫn không xếp được.
}
