#pragma once



class My_stdexcept {
public:
    enum class LoaiLoi {
        DoiSoKhongHopLe,
        TrangThaiKhongHopLe,
        NgoaiPhamVi,
        LoiThucThi
    };

private:
    LoaiLoi MaLoaiLoi;
    char ThongBao[256];

public:


    My_stdexcept(LoaiLoi MaLoaiLoi, const char* NoiDung) noexcept
        : MaLoaiLoi(MaLoaiLoi) {
        if (NoiDung == nullptr) {
            NoiDung = "Loi khong xac dinh";
        }
        int ViTri = 0;
        while (ViTri < 255 && NoiDung[ViTri] != '\0') {
            ThongBao[ViTri] = NoiDung[ViTri];
            ++ViTri;
        }
        ThongBao[ViTri] = '\0';
    }

    LoaiLoi LayLoaiLoi() const noexcept { return MaLoaiLoi; }
    const char* LayThongBao() const noexcept { return ThongBao; }
};
