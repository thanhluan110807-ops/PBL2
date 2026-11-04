#include "Backtracking.h"
#include <cassert>
#include <iostream>

int main() {
    // Bác sĩ 0 là bác sĩ chính; bác sĩ 1 là nội trú.
    const bool BacSiLamTruong[2] = {true, false};
    const bool CaCanTruong[2] = {false, true};
    int PhanCong[2] = {-1, -1};
    assert(XepCa(0, BacSiLamTruong, CaCanTruong, PhanCong));
    assert(PhanCong[0] == 1 && PhanCong[1] == 0); // Cần quay lui để tìm ra kết quả này.
    std::cout << "Ca A: bac si " << PhanCong[0] << "; Ca B: bac si " << PhanCong[1] << '\n';

    const bool KhongAiLamTruong[2] = {false, false};
    PhanCong[0] = PhanCong[1] = -1;
    assert(!XepCa(0, KhongAiLamTruong, CaCanTruong, PhanCong));
    assert(PhanCong[0] == -1 && PhanCong[1] == -1); // Thất bại vẫn hoàn tác sạch.

    const bool KhongCanTruong[2] = {false, false};
    assert(XepCa(0, KhongAiLamTruong, KhongCanTruong, PhanCong));
    assert(PhanCong[0] != PhanCong[1]);
    std::cout << "Da kiem tra ca co lich va ca khong co lich.\n";
}
