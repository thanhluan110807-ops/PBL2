#pragma once

// Ngoại lệ tối giản dùng chung cho model và DSA, không phụ thuộc thư viện chuẩn.
// Khi bắt lỗi, dùng const My_stdexcept& để giữ nguyên thông tin và tránh sao chép.
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
    // Sao chép thông báo vào đối tượng để không giữ con trỏ tới chuỗi đã bị hủy.
    // Tối đa 255 byte; byte cuối luôn dành cho ký tự kết thúc chuỗi '\0'.
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
