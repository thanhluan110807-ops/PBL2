#pragma once
#include "model/NhanVienYTe.h"
#include <QString>

// Trình bày dữ liệu cho Qt; thay trách nhiệm Hienthithongtin trong model cũ.
class StaffPresenter {
public:
    static QString MoTa(const NhanVienYTe& NhanVien);
};
