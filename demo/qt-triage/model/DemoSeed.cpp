#include "model/DemoTriageService.h"

namespace Demo {
// Chỉ dữ liệu mẫu được dựng trực tiếp trong service; widget không sửa trạng thái.
void DemoTriageService::NapMau(bool KichBanBaMot) {
    XoaDuLieu();
    const char *Ten[] = {"Nguyễn Minh Khôi", "Trần Thảo Vy",  "",
                         "Lê Hoàng Nam",     "Phạm Ngọc Mai", "Võ Đức Huy",
                         "Đặng Thu Hà",      "Bùi Gia Bảo",   "Hoàng Thanh Trúc",
                         "Đỗ Anh Tuấn",      "Ngô Hải Yến",   "Phan Quang Minh",
                         "Dương Bảo Anh",    "Lý Minh Châu"};
    const TrangThai Loai[] = {TrangThai::LanDau,     TrangThai::LanDau,   TrangThai::LanDau,
                              TrangThai::QuayLai,    TrangThai::LanDau,   TrangThai::QuayLai,
                              TrangThai::LanDau,     TrangThai::QuayLai,  TrangThai::SangLoc,
                              TrangThai::SangLoc,    TrangThai::DangKham, TrangThai::DangKham,
                              TrangThai::CanLamSang, TrangThai::KetThuc};
    const int CacMuc[] = {2, 3, 1, 2, 4, 3, 5, 4, 0, 0, 3, 4, 3, 5};
    const int Cho[] = {14, 22, 2, 8, 18, 37, 45, 63, 4, 2, 6, 5, 17, 80};
    const char *TrieuChung[] = {"Đau ngực, khó thở",        "Đau bụng thượng vị",
                                "Được đưa đến sau tai nạn", "Đọc kết quả xét nghiệm máu",
                                "Đau cổ tay sau té ngã",    "Đọc kết quả chụp X-quang",
                                "Đau họng, mệt mỏi",        "Đọc kết quả siêu âm",
                                "Chóng mặt, buồn nôn",      "Đau đầu kéo dài",
                                "Sốt và đau bụng",          "Đau thắt lưng",
                                "Đau bụng, chờ xét nghiệm", "Vết thương phần mềm"};
    for (int ViTri = 0; ViTri < (KichBanBaMot ? 7 : 14); ++ViTri) {
        const TrangThai Dich =
            KichBanBaMot ? (ViTri < 5 ? TrangThai::QuayLai : TrangThai::LanDau) : Loai[ViTri];
        const int Muc = KichBanBaMot ? 3 : CacMuc[ViTri];
        HoSo Benh;
        Benh.HoTen = Ten[ViTri];
        Benh.ChuaRo = ViTri == 2;
        Benh.CoNgaySinh = ViTri != 2 && ViTri != 7;
        Benh.NgaySinh = {12, 4, 1985 + ViTri};
        Benh.TuoiUocLuong = ViTri == 7 ? 30 : -1;
        Benh.GioiTinh = ViTri % 2 ? "Nữ" : "Nam";
        if (ViTri == 0) {
            Benh.DiUng = "Penicillin (theo lời khai)";
            Benh.TienSu = "Tăng huyết áp";
        }
        BayGio = 570 - Cho[ViTri] - 15;
        const int So = TiepNhan(Benh, TrieuChung[ViTri]).ChiSo;
        LuotKham &Luot = CacLuot[So];
        if (Dich == TrangThai::SangLoc)
            continue;
        BayGio = 570 - Cho[ViTri];
        const SinhHieu SoDo{82, 122, 18, 15, Muc == 1 ? 88.0 : Muc == 2 ? 94.0 : Muc == 4 ? 96.0 : 98.0};
        XacNhan(So, SoDo, Muc);
        Luot.LichSu[0].DuLieuMau = true;
        if (Dich != TrangThai::LanDau) {
            Luot.TinhTrang = TrangThai::DangKham;
            Luot.VaoHangLuc = -1;
            Luot.SoThuTu = 0;
            GhiMoc(Luot, "Được gọi · đang khám");
        }
        if (Dich == TrangThai::QuayLai || Dich == TrangThai::CanLamSang) {
            ChuyenCanLamSang(So, "Xét nghiệm / chẩn đoán hình ảnh mẫu");
            if (Dich == TrangThai::QuayLai)
                QuayLai(So, "Kết quả mẫu đã sẵn sàng, chờ bác sĩ đánh giá.");
        }
        if (Dich == TrangThai::KetThuc)
            DongLuot(So);
    }
    BayGio = 570;
    for (int &Dem : BoDem)
        Dem = 0;
}
} // namespace Demo
