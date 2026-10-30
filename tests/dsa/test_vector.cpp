#include "dsa/Vector.h"
#include <cassert>
#include <string>
#include <utility>

// Đếm đối tượng sống và chủ động ném lỗi để kiểm tra việc dọn mảng tạm.
struct GiaTriThu {
    static int DangSong;
    static int SoLanGanConLai;
    int GiaTri = 0;
    GiaTriThu() { ++DangSong; }
    GiaTriThu(const GiaTriThu& Khac) : GiaTri(Khac.GiaTri) { ++DangSong; }
    ~GiaTriThu() { --DangSong; }
    GiaTriThu& operator=(const GiaTriThu& Khac) {
        if (SoLanGanConLai == 0)
            throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi sao chep thu nghiem");
        if (SoLanGanConLai > 0) --SoLanGanConLai;
        GiaTri = Khac.GiaTri;
        return *this;
    }
};
int GiaTriThu::DangSong = 0;
int GiaTriThu::SoLanGanConLai = -1;

template <typename Ham>
void PhaiBaoLoi(Ham ThaoTac, My_stdexcept::LoaiLoi Loai) {
    bool DaBat = false;
    try { ThaoTac(); }
    catch (const My_stdexcept& Loi) { DaBat = true; assert(Loi.LayLoaiLoi() == Loai); }
    assert(DaBat);
}

int main() {
    const auto NgoaiPhamVi = My_stdexcept::LoaiLoi::NgoaiPhamVi;
    PhaiBaoLoi([] { Vector<int> Mang(0); }, My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    PhaiBaoLoi([] { Vector<int> Mang(-1); }, My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    Vector<int> GioiHan(5001);
    for (int I = 0; I < 10001; ++I) GioiHan.ThemCuoi(I);
    assert(GioiHan.KichThuoc() == 10001 && GioiHan[10000] == 10000);
    Vector<int> Mang(1);
    assert(Mang.Rong());
    PhaiBaoLoi([&] { Mang.XoaCuoi(); }, NgoaiPhamVi);
    PhaiBaoLoi([&] { Mang.Lay(0); }, NgoaiPhamVi);
    Mang.ThemCuoi(42);
    Mang.ThemCuoi(Mang[0]); // Tham chiếu vào chính mảng đang tăng dung lượng.
    assert(Mang[1] == 42);
    for (int I = 0; I < 100; ++I) Mang.ThemCuoi(I);
    assert(Mang.KichThuoc() == 102 && Mang[101] == 99);
    const Vector<int>& Hang = Mang;
    assert(Hang.Lay(0) == 42 && Hang[101] == 99);
    PhaiBaoLoi([&] { (void)Hang[-1]; }, NgoaiPhamVi);
    PhaiBaoLoi([&] { (void)Mang[102]; }, NgoaiPhamVi);
    Vector<int> BanSao(Mang);
    BanSao[0] = 7;
    assert(Mang[0] == 42);
    Mang = Mang;
    Vector<int> Dich;
    Dich = BanSao;
    assert(Dich[0] == 7);
    Vector<int> Chuyen(std::move(Dich)); // std::move chỉ dùng để gọi API move trong test.
    assert(Dich.Rong() && Chuyen[0] == 7);
    Dich.ThemCuoi(8);
    Dich = std::move(Chuyen);
    assert(Chuyen.Rong() && Dich[0] == 7);
    Vector<int>& BiDanh = Dich;
    Dich = std::move(BiDanh);
    assert(Dich[0] == 7);
    Chuyen.ThemCuoi(9);
    Chuyen.XoaCuoi();
    assert(Chuyen.Rong());
    Dich.XoaHet(); Dich.XoaHet();
    Dich.ThemCuoi(11);
    assert(Dich[0] == 11);
    Vector<int> Rong(Chuyen);
    assert(Rong.Rong());
    Vector<std::string> Chuoi(1);
    Chuoi.ThemCuoi("benh nhan"); Chuoi.ThemCuoi(Chuoi[0]);
    assert(Chuoi[1] == "benh nhan");
    Chuoi.XoaCuoi();
    Chuoi.ThuGon();
    assert(Chuoi.KichThuoc() == 1 && Chuoi.DungLuong() == 1 && Chuoi[0] == "benh nhan");
    std::string* DiaChi = &Chuoi[0];
    Chuoi.ThuGon(); // Đã gọn thì không cấp phát lại.
    assert(&Chuoi[0] == DiaChi);
    Chuoi.XoaCuoi(); Chuoi.ThuGon();
    assert(Chuoi.Rong() && Chuoi.DungLuong() == 0);
    Chuoi.ThemCuoi("dung lai");
    assert(Chuoi[0] == "dung lai");
    int GiaTri = 12;
    { Vector<int*> ConTro; ConTro.ThemCuoi(&GiaTri); ConTro.XoaHet(); }
    assert(GiaTri == 12);
    {
        GiaTriThu Mau; Mau.GiaTri = 17;
        Vector<GiaTriThu> Nguon(2); Nguon.ThemCuoi(Mau); Nguon.ThemCuoi(Mau);
        Vector<GiaTriThu> DichLoi(1); DichLoi.ThemCuoi(Mau);
        const int Truoc = GiaTriThu::DangSong;
        for (int Buoc = 0; Buoc < 3; ++Buoc) {
            GiaTriThu::SoLanGanConLai = Buoc;
            PhaiBaoLoi([&] { Nguon.ThemCuoi(Mau); }, My_stdexcept::LoaiLoi::LoiThucThi);
            assert(Nguon.KichThuoc() == 2 && Nguon[0].GiaTri == 17);
            assert(GiaTriThu::DangSong == Truoc);
        }
        GiaTriThu::SoLanGanConLai = 1;
        PhaiBaoLoi([&] { Vector<GiaTriThu> BanLoi(Nguon); }, My_stdexcept::LoaiLoi::LoiThucThi);
        assert(GiaTriThu::DangSong == Truoc);
        GiaTriThu::SoLanGanConLai = 0;
        PhaiBaoLoi([&] { DichLoi = Nguon; }, My_stdexcept::LoaiLoi::LoiThucThi);
        assert(DichLoi.KichThuoc() == 1 && DichLoi[0].GiaTri == 17);
        assert(GiaTriThu::DangSong == Truoc);
        GiaTriThu::SoLanGanConLai = -1;
        Nguon.XoaCuoi();
        GiaTriThu::SoLanGanConLai = 0;
        PhaiBaoLoi([&] { Nguon.ThemCuoi(Mau); }, My_stdexcept::LoaiLoi::LoiThucThi);
        assert(Nguon.KichThuoc() == 1 && Nguon[0].GiaTri == 17);
        PhaiBaoLoi([&] { Nguon.ThuGon(); }, My_stdexcept::LoaiLoi::LoiThucThi);
        assert(Nguon.KichThuoc() == 1 && Nguon.DungLuong() == 2 && Nguon[0].GiaTri == 17);
        assert(GiaTriThu::DangSong == Truoc);
        GiaTriThu::SoLanGanConLai = -1;
        Nguon.ThuGon();
        assert(Nguon.DungLuong() == 1 && GiaTriThu::DangSong == Truoc - 1);
        Nguon.XoaHet();
    }
    assert(GiaTriThu::DangSong == 0);
}
