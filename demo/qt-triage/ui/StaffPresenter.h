// Phần B — Chức năng chính: Định dạng thông tin nhân viên để hiển thị trên Qt.
#pragma once
#include "model/NhanVienYTe.h"
#include <QString>

// Trình bày dữ liệu cho Qt; thay trách nhiệm Hienthithongtin trong model cũ.
class StaffPresenter {
public:
    static QString MoTa(const NhanVienYTe& NhanVien);
};
