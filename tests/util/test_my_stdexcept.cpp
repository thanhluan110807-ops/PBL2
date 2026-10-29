#include "util/My_stdexcept.h"
#include "dsa/LinkedList.h"
#include "dsa/HashTable.h"
#include <cassert>
#include <iostream>

bool ChuoiBangNhau(const char* Trai, const char* Phai) {
    int ViTri = 0;
    while (Trai[ViTri] == Phai[ViTri]) {
        if (Trai[ViTri] == '\0') return true;
        ++ViTri;
    }
    return false;
}

template <typename HamKiemTra>
void PhaiBaoNgoaiPhamVi(HamKiemTra Ham) {
    bool DaBat = false;
    try {
        Ham();
    } catch (const My_stdexcept& Loi) {
        DaBat = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::NgoaiPhamVi;
        assert(Loi.LayThongBao()[0] != '\0');
    }
    assert(DaBat);
}

int main() {
    using LoaiLoi = My_stdexcept::LoaiLoi;
    char NoiDung[] = "Du lieu sai";
    My_stdexcept Loi(LoaiLoi::DoiSoKhongHopLe, NoiDung);
    NoiDung[0] = 'X';
    assert(ChuoiBangNhau(Loi.LayThongBao(), "Du lieu sai"));
    assert(Loi.LayLoaiLoi() == LoaiLoi::DoiSoKhongHopLe);

    // Sao chép đối tượng không làm hai thông báo dùng chung vùng nhớ.
    My_stdexcept BanSao = Loi;
    Loi = My_stdexcept(LoaiLoi::LoiThucThi, "Thong bao moi");
    assert(ChuoiBangNhau(BanSao.LayThongBao(), "Du lieu sai"));
    assert(Loi.LayLoaiLoi() == LoaiLoi::LoiThucThi);
    My_stdexcept KhongCoChuoi(LoaiLoi::LoiThucThi, nullptr);
    assert(ChuoiBangNhau(KhongCoChuoi.LayThongBao(), "Loi khong xac dinh"));
    My_stdexcept ChuoiRong(LoaiLoi::TrangThaiKhongHopLe, "");
    assert(ChuoiRong.LayThongBao()[0] == '\0');

    char ChuoiDai[400];
    for (int ViTri = 0; ViTri < 399; ++ViTri) ChuoiDai[ViTri] = 'A';
    ChuoiDai[399] = '\0';
    My_stdexcept LoiDai(LoaiLoi::LoiThucThi, ChuoiDai);
    for (int ViTri = 0; ViTri < 255; ++ViTri) assert(LoiDai.LayThongBao()[ViTri] == 'A');
    assert(LoiDai.LayThongBao()[255] == '\0');

    // Thông báo vẫn còn sau khi nguồn cục bộ đã hết thời gian sống.
    bool DaBat = false;
    try {
        char Tam[] = "Loi tu ham";
        throw My_stdexcept(LoaiLoi::TrangThaiKhongHopLe, Tam);
    } catch (const My_stdexcept& BatDuoc) {
        DaBat = true;
        assert(BatDuoc.LayLoaiLoi() == LoaiLoi::TrangThaiKhongHopLe);
        assert(ChuoiBangNhau(BatDuoc.LayThongBao(), "Loi tu ham"));
    }
    assert(DaBat);

    LinkedList<int> DanhSach;
    const LinkedList<int>& DanhSachHang = DanhSach;
    PhaiBaoNgoaiPhamVi([&] { DanhSach.Dau(); });
    PhaiBaoNgoaiPhamVi([&] { DanhSach.Cuoi(); });
    PhaiBaoNgoaiPhamVi([&] { DanhSach.Lay(-1); });
    PhaiBaoNgoaiPhamVi([&] { DanhSachHang.Dau(); });
    PhaiBaoNgoaiPhamVi([&] { DanhSachHang.Cuoi(); });
    PhaiBaoNgoaiPhamVi([&] { DanhSachHang.Lay(0); });
    DanhSach.Themcuoi(42);
    assert(DanhSach.Dau() == 42 && DanhSachHang.Lay(0) == 42);
    PhaiBaoNgoaiPhamVi([&] { DanhSach.Lay(1); });

    HashTable<int, int> Bang;
    const HashTable<int, int>& BangHang = Bang;
    PhaiBaoNgoaiPhamVi([&] { Bang.Lay(1); });
    PhaiBaoNgoaiPhamVi([&] { BangHang.Lay(1); });
    assert(Bang.Them(1, 99) && BangHang.Lay(1) == 99);
    assert(Bang.Xoa(1));
    PhaiBaoNgoaiPhamVi([&] { Bang.Lay(1); });
    std::cout << "PASS: My_stdexcept - owned message, copy, bounds, catch, DSA integration\n";
}
