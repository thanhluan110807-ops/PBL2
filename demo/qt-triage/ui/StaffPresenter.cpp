// Phần B — Chức năng chính: Định dạng thông tin nhân viên để hiển thị trên Qt.
#include "ui/StaffPresenter.h"
#include "model/BacSi.h"
#include "model/DieuDuong.h"
QString StaffPresenter::MoTa(const NhanVienYTe& NV) {
    QString S = QString("%1 · %2\nCấp bậc: %3 · Kinh nghiệm: %4 năm\nGiờ đã ghi nhận: %5 · Điểm mệt mỏi: %6\nCa đêm tháng: %7 · Ca cuối tuần: %8")
        .arg(QString::fromStdString(NV.LayMaDinhDanh()),QString::fromStdString(NV.LayHoTen()),
             QString::fromStdString(CapBacThanhChuoi(NV.LayCapBac())))
        .arg(NV.LaySoNamKinhNghiem()).arg(NV.LayGioLamTuan()).arg(NV.LayDiemMetMoi())
        .arg(NV.LaySoCaDemThang()).arg(NV.LaySoCaCuoiTuanThang());
    if (NV.LayLoaiNhanVien()==StaffType::BacSi) {
        const auto& BS=static_cast<const BacSi&>(NV);
        S += QString("\nChuyên khoa: %1 · Nội trú: %2 · Làm trưởng ca: %3")
            .arg(QString::fromStdString(BS.LayChuyenKhoa()),BS.LaBacSiNoiTru()?"Có":"Không",BS.CoTheLamTruongCa()?"Có":"Không");
    } else {
        const auto& DD=static_cast<const DieuDuong&>(NV);
        S += QString("\nĐang sàng lọc: %1 · Liên tục: %2 giờ · Cần xoay tua: %3")
            .arg(DD.DangSangLoc()?"Có":"Không").arg(DD.LayGioSangLocLienTuc()).arg(DD.CanXoayTuaSangLoc()?"Có":"Không");
    }
    return S;
}
