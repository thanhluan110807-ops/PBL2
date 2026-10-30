#pragma once

#include <climits> // Giới hạn của int, để kiểm tra trước khi tăng dung lượng.
#include "util/My_stdexcept.h"

// Mảng động sở hữu các ô T; nếu T là con trỏ thì không sở hữu đối tượng đích.
// T cần dựng mặc định, sao chép và gán được; destructor không được ném lỗi.
template <typename T>
class Vector {
private:
    T* DuLieu;
    int SucChua;
    int SoPhanTu;

    void KiemTraChiSo(int ChiSo) const {
        if (ChiSo < 0 || ChiSo >= SoPhanTu)
            throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Chi so Vector ngoai pham vi");
    }

public:
    explicit Vector(int DungLuongBanDau = 10)
        : DuLieu(nullptr), SucChua(DungLuongBanDau), SoPhanTu(0) {
        if (DungLuongBanDau <= 0)
            throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Dung luong phai duong");
        DuLieu = new T[SucChua];
    }

    ~Vector() { delete[] DuLieu; }

    Vector(const Vector& Nguon)
        : DuLieu(nullptr), SucChua(Nguon.SucChua), SoPhanTu(Nguon.SoPhanTu) {
        if (SucChua == 0) return;
        DuLieu = new T[SucChua];
        try {
            for (int I = 0; I < SoPhanTu; ++I) DuLieu[I] = Nguon.DuLieu[I];
        } catch (...) {
            delete[] DuLieu;
            throw;
        }
    }

    Vector& operator=(const Vector& Nguon) {
        if (this != &Nguon) {
            Vector Tam(Nguon);
            HoanDoi(Tam);
        }
        return *this;
    }

    Vector(Vector&& Nguon) noexcept
        : DuLieu(Nguon.DuLieu), SucChua(Nguon.SucChua), SoPhanTu(Nguon.SoPhanTu) {
        Nguon.DuLieu = nullptr;
        Nguon.SucChua = Nguon.SoPhanTu = 0;
    }

    Vector& operator=(Vector&& Nguon) noexcept {
        if (this != &Nguon) {
            XoaHet();
            HoanDoi(Nguon);
        }
        return *this;
    }

    void HoanDoi(Vector& Khac) noexcept {
        T* Tam = DuLieu; DuLieu = Khac.DuLieu; Khac.DuLieu = Tam;
        int SoTam = SucChua; SucChua = Khac.SucChua; Khac.SucChua = SoTam;
        SoTam = SoPhanTu; SoPhanTu = Khac.SoPhanTu; Khac.SoPhanTu = SoTam;
    }

    // Chỉ công bố mảng mới sau khi chép xong, kể cả khi GiaTri là một ô trong mảng cũ.
    void ThemCuoi(const T& GiaTri) {
        if (SoPhanTu < SucChua) {
            DuLieu[SoPhanTu] = GiaTri;
            ++SoPhanTu;
            return;
        }
        if (SucChua > INT_MAX / 2)
            throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Vector vuot gioi han dung luong");
        const int SucChuaMoi = SucChua == 0 ? 10 : SucChua * 2;
        T* MangMoi = new T[SucChuaMoi];
        try {
            for (int I = 0; I < SoPhanTu; ++I) MangMoi[I] = DuLieu[I];
            MangMoi[SoPhanTu] = GiaTri;
        } catch (...) {
            delete[] MangMoi;
            throw;
        }
        delete[] DuLieu;
        DuLieu = MangMoi;
        SucChua = SucChuaMoi;
        ++SoPhanTu;
    }

    // Xóa logic O(1); ô bị loại còn sống đến khi ghi đè, tăng dung lượng hoặc XoaHet.
    void XoaCuoi() {
        KiemTraChiSo(SoPhanTu - 1);
        --SoPhanTu;
    }

    // Giải phóng toàn bộ các ô, kể cả những ô đã XoaCuoi; có thể thêm lại sau đó.
    void XoaHet() noexcept {
        delete[] DuLieu;
        DuLieu = nullptr;
        SucChua = SoPhanTu = 0;
    }

    // Thu sức chứa về đúng số phần tử; giữ nguyên dữ liệu nếu sao chép thất bại.
    void ThuGon() {
        if (SucChua == SoPhanTu) return;
        if (SoPhanTu == 0) {
            XoaHet();
            return;
        }
        T* MangMoi = new T[SoPhanTu];
        try {
            for (int I = 0; I < SoPhanTu; ++I) MangMoi[I] = DuLieu[I];
        } catch (...) {
            delete[] MangMoi;
            throw;
        }
        delete[] DuLieu;
        DuLieu = MangMoi;
        SucChua = SoPhanTu;
    }

    T& Lay(int ChiSo) { KiemTraChiSo(ChiSo); return DuLieu[ChiSo]; }
    const T& Lay(int ChiSo) const { KiemTraChiSo(ChiSo); return DuLieu[ChiSo]; }
    T& operator[](int ChiSo) { return Lay(ChiSo); }
    const T& operator[](int ChiSo) const { return Lay(ChiSo); }
    int KichThuoc() const noexcept { return SoPhanTu; }
    int DungLuong() const noexcept { return SucChua; }
    bool Rong() const noexcept { return SoPhanTu == 0; }
};
