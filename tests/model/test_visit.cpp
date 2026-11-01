#include "model/Visit.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <limits>

template <typename F> void PhaiLoi(F Ham) {
    bool Loi = false;
    try { Ham(); } catch (const My_stdexcept&) { Loi = true; }
    assert(Loi);
}
int main() {
    // Kiểm tra ranh giới nhóm tuổi và thứ tự xét của luật cũ.
    assert(TriageAssessment(VitalSigns(89, 80, 40, 98, 15), 0, true, 1, "DD").LayMuc() == 2);
    assert(TriageAssessment(VitalSigns(90, 80, 40, 98, 15), 0, true, 1, "DD").LayMuc() == 5);
    assert(TriageAssessment(VitalSigns(121, 100, 25, 98, 15), 1, true, 1, "DD").LayMuc() == 3);
    assert(TriageAssessment(VitalSigns(101, 100, 20, 98, 15), 6, true, 1, "DD").LayMuc() == 3);
    assert(TriageAssessment(VitalSigns(75, 120, 35, 98, 15), 13, true, 1, "DD").LayMuc() == 1);
    assert(TriageAssessment(VitalSigns(91, 120, 16, 98, 15), 65, true, 1, "DD").LayMuc() == 3);
    assert(TriageAssessment(VitalSigns(75, 120, 16, 96.9, 15), 40, true, 1, "DD").LayMuc() == 4);
    assert(TriageAssessment(VitalSigns(75, 120, 16, 90, 15), 40, true, 1, "DD").LayMuc() == 2);
    assert(TriageAssessment(VitalSigns(75, 120, 16, 89.9, 15), 40, true, 1, "DD").LayMuc() == 1);
    Patient BenhNhan("BN1");
    Visit LanKham("LK1", "BN1", 1767225600, "Dau");
    VitalSigns BinhThuong(75,120,16,98,15);
    PhaiLoi([&]{ LanKham.LayDanhGiaGanNhat(); });
    // Dựng lần khám khác để không để thao tác thử ảnh hưởng luồng chính.
    Visit LanKhamThu("LK2", "BN1", 1767225600, "Dau");
    PhaiLoi([&]{ LanKhamThu.BatDauKham("BS0"); });
    PhaiLoi([&]{ LanKhamThu.DanhGia(BenhNhan,BinhThuong,1767225601,"DD1"); });
    assert(LanKhamThu.SoDanhGia() == 0);
    LanKhamThu.HuyLanKham();
    PhaiLoi([&]{ LanKhamThu.BatDauKham("BS0"); });
    PhaiLoi([&]{ LanKham.DanhGia(BenhNhan, BinhThuong, 1767225601, "DD1"); });
    assert(LanKham.SoDanhGia() == 0);
    BenhNhan.DatTuoiUocLuong(40);
    LanKham.DanhGia(BenhNhan, BinhThuong, 1767225601, "DD1");
    assert(LanKham.LayDanhGiaGanNhat().LayMuc() == 5);
    assert(LanKham.LayDanhGiaGanNhat().LaTuoiUocLuong());
    assert(LanKham.LayDanhGiaGanNhat().LayTuoi() == 40);
    LanKham.DanhGia(BenhNhan, VitalSigns(151,120,16,94,15), 1767225602, "DD1");
    assert(LanKham.LayDanhGiaGanNhat().LayMuc() == 2); // Luật chung mức 2 xét trước luật tuổi mức 1.
    BenhNhan.CapNhatThongTinCaNhan("A", Gender::Nam, Date{1,1,2000});
    LanKham.DanhGia(BenhNhan, BinhThuong, 1767225603, "DD1");
    assert(!LanKham.LayDanhGiaGanNhat().LaTuoiUocLuong());
    assert(LanKham.LayDanhGiaGanNhat().LayTuoi() == 26);
    assert(LanKham.LayDanhGiaTai(0).LaTuoiUocLuong());
    PhaiLoi([&]{ VitalSigns Loi(1,1,1,std::numeric_limits<double>::quiet_NaN(),3); });
    Patient Khac("BN2");
    PhaiLoi([&]{ LanKham.DanhGia(Khac,BinhThuong,1767225604,"DD1"); });
    LanKham.PhanCongBacSi("BS1");
    LanKham.PhanCongBacSi("BS3");
    assert(LanKham.SoBacSiDuocPhanCong() == 2);
    assert(LanKham.SoLanBacSiThamGiaKham() == 0);
    PhaiLoi([&]{ LanKham.GhiNhanBacSiThamGiaKham("BS1"); });
    assert(LanKham.SoLanBacSiThamGiaKham() == 0);
    PhaiLoi([&]{ LanKham.BatDauKham(""); });
    assert(LanKham.LayTrangThai() == VisitStatus::ChoKhamLanDau);
    assert(LanKham.SoLanBacSiThamGiaKham() == 0);
    LanKham.BatDauKham("BS1");
    assert(LanKham.LayTrangThai() == VisitStatus::DangKham);
    assert(LanKham.LayBacSiThamGiaKhamTai(0) == "BS1");
    PhaiLoi([&]{ LanKham.ChuyenDiCanLamSang("Xet nghiem", "BS3"); });
    LanKham.GhiNhanBacSiThamGiaKham("BS3"); // Đã khám nhưng không ra chỉ định.
    assert(LanKham.SoLanBacSiThamGiaKham() == 2);
    PhaiLoi([&]{ LanKham.ChuyenDiCanLamSang("", "BS1"); });
    assert(LanKham.SoLanCanLamSang() == 0);
    LanKham.ChuyenDiCanLamSang("Xet nghiem", "BS1");
    assert(LanKham.SoLanCanLamSang() == 1);
    assert(LanKham.LayCanLamSangTai(0).LayChiDinh() == "Xet nghiem");
    assert(LanKham.LayCanLamSangTai(0).LayMaBacSiChiDinh() == "BS1");
    assert(!LanKham.LayCanLamSangTai(0).DaCoKetQua());
    PhaiLoi([&]{ LanKham.DanhDauQuayLai(); });
    PhaiLoi([&]{ LanKham.GhiNhanKetQua(""); });
    assert(!LanKham.LayCanLamSangTai(0).DaCoKetQua());
    LanKham.GhiNhanKetQua("Binh thuong");
    assert(LanKham.LayCanLamSangTai(0).DaCoKetQua());
    assert(LanKham.LayCanLamSangTai(0).LayKetQua() == "Binh thuong");
    PhaiLoi([&]{ LanKham.GhiNhanKetQua("Ghi de"); });
    LanKham.DanhDauQuayLai();
    LanKham.BatDauKham("BS2"); // Bác sĩ khám thay, chưa phân công.
    assert(LanKham.SoBacSiDuocPhanCong() == 2);
    assert(LanKham.SoLanBacSiThamGiaKham() == 3);
    assert(LanKham.LayBacSiThamGiaKhamTai(0) == "BS1");
    assert(LanKham.LayBacSiThamGiaKhamTai(1) == "BS3");
    assert(LanKham.LayBacSiThamGiaKhamTai(2) == "BS2");
    LanKham.ChuyenDiCanLamSang("Chup X-quang", "BS2");
    assert(LanKham.SoLanCanLamSang() == 2);
    assert(LanKham.LayCanLamSangTai(1).LayChiDinh() == "Chup X-quang");
    assert(LanKham.LayCanLamSangTai(0).LayMaBacSiChiDinh() == "BS1");
    assert(LanKham.LayCanLamSangTai(1).LayMaBacSiChiDinh() == "BS2");
    assert(!LanKham.LayCanLamSangTai(1).DaCoKetQua());
    assert(LanKham.LayCanLamSangTai(0).LayKetQua() == "Binh thuong");
    LanKham.GhiNhanKetQua("Khong thay ton thuong");
    assert(LanKham.LayCanLamSangTai(1).LayKetQua() == "Khong thay ton thuong");
    assert(LanKham.LayCanLamSangTai(0).LayKetQua() == "Binh thuong");
    assert(LanKham.LayKetQuaCanLamSang() == "Khong thay ton thuong");
    LanKham.DanhDauQuayLai();
    LanKham.BatDauKham("BS1");
    LanKham.KetThucLanKham();
    assert(LanKham.LayCanLamSangTai(0).LayKetQua() == "Binh thuong");
    assert(LanKham.LayCanLamSangTai(1).LayKetQua() == "Khong thay ton thuong");
    assert(LanKham.SoLanBacSiThamGiaKham() == 4);
    PhaiLoi([&]{ LanKham.DanhDauQuayLai(); });
    PhaiLoi([&]{ LanKham.DanhGia(BenhNhan,BinhThuong,1767225605,"DD1"); });

    // 16:59:59 UTC là 23:59:59 ngày 31/12; 17:00 UTC là 00:00 ngày 01/01 ở Việt Nam.
    Patient SinhNhat("BN3", "Sinh nhat", Gender::Nam, Date{1,1,2000});
    Visit QuaNgay("LK3", "BN3", 1767200399, "Kiem tra");
    QuaNgay.DanhGia(SinhNhat, BinhThuong, 1767200399, "DD1");
    assert(QuaNgay.LayDanhGiaGanNhat().LayTuoi() == 25);
    QuaNgay.DanhGia(SinhNhat, BinhThuong, 1767200400, "DD1");
    assert(QuaNgay.LayDanhGiaGanNhat().LayTuoi() == 26);
    assert(QuaNgay.LayDanhGiaGanNhat().LayThoiDiemDanhGia() == 1767200400);
    assert(QuaNgay.LayDanhGiaTai(0).LayTuoi() == 25);

    // Qua nửa đêm ngày 28/02 ở năm nhuận phải chuyển sang sinh nhật 29/02.
    Patient SinhNhatNamNhuan("BN4", "Sinh nhat nam nhuan", Gender::Nu, Date{29,2,2000});
    Visit QuaNgayNhuan("LK4", "BN4", 1709139599, "Kiem tra");
    QuaNgayNhuan.DanhGia(SinhNhatNamNhuan, BinhThuong, 1709139599, "DD1");
    assert(QuaNgayNhuan.LayDanhGiaGanNhat().LayTuoi() == 23);
    QuaNgayNhuan.DanhGia(SinhNhatNamNhuan, BinhThuong, 1709139600, "DD1");
    assert(QuaNgayNhuan.LayDanhGiaGanNhat().LayTuoi() == 24);
}
