#pragma once

// Bài học nhỏ có đúng 2 bác sĩ và 2 ca trùng giờ.
// PhanCong[I] là chỉ số bác sĩ của ca I; -1 nghĩa là chưa xếp.
bool XepCa(int CaHienTai, const bool BacSiLamTruong[2],
           const bool CaCanTruong[2], int PhanCong[2]);
