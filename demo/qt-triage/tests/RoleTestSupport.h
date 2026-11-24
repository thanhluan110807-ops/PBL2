#pragma once
#include "model/BacSi.h"
#include "model/DieuDuong.h"
#include "model/NhanVienYTe.h"
#include "repository/AdminFileManager.h"
#include "repository/StaffRegistry.h"
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <cassert>

inline void TaoNhanSuMau(const QString& ThuMuc) {
    AdminData DuLieu;
    DuLieu.NhanSu->Them(DieuDuong("DD01", "Điều dưỡng Lan", CapBac::DieuDuongChinh, 5));
    DuLieu.NhanSu->Them(BacSi("BS1", "Bác sĩ mẫu", CapBac::BacSiChinh, 8, "Cấp cứu"));
    AdminFileManager::Luu((ThuMuc + "/nhan-su-lich.csv").toStdString(), DuLieu);
}

inline void HenChonVaiTro(const char* MaNut, const QString& MaNhanVien = {}) {
    QTimer::singleShot(0, [MaNut, MaNhanVien] {
        auto* Hop = QApplication::activeModalWidget();
        assert(Hop && Hop->objectName() == "chonVaiTroDialog");
        auto* VaiTro = Hop->findChild<QPushButton*>(MaNut);
        auto* Vao = Hop->findChild<QPushButton*>("xacNhanVaiTro");
        assert(VaiTro && Vao);
        VaiTro->click();
        if (!MaNhanVien.isEmpty()) {
            auto* NhanVien = Hop->findChild<QComboBox*>("chonNhanVienVaiTro");
            assert(NhanVien);
            const int ViTri = NhanVien->findData(MaNhanVien);
            assert(ViTri > 0);
            NhanVien->setCurrentIndex(ViTri);
        }
        assert(Vao->isEnabled());
        Vao->click();
    });
}
