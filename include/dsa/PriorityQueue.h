#pragma once

#include "dsa/Vector.h"


template <typename T>
struct SoSanhMacDinh {
    bool operator()(const T& Trai, const T& Phai) const { return Trai < Phai; }
};





template <typename T, typename SoSanh = SoSanhMacDinh<T>>
class PriorityQueue {
private:
    Vector<T> DuLieu;
    SoSanh BoSoSanh;

    void KiemTraChiSo(int ChiSo) const {
        if (ChiSo < 0 || ChiSo >= DuLieu.KichThuoc())
            throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Chi so heap ngoai pham vi");
    }

    void DoiCho(int Trai, int Phai) {
        T Tam = DuLieu[Trai];
        DuLieu[Trai] = DuLieu[Phai];
        DuLieu[Phai] = Tam;
    }


    void VunLen(int ChiSo) {
        while (ChiSo > 0) {
            int Cha = (ChiSo - 1) / 2;
            if (!BoSoSanh(DuLieu[Cha], DuLieu[ChiSo])) break;
            DoiCho(Cha, ChiSo);
            ChiSo = Cha;
        } 
    }


    void VunXuong(int ChiSo) {
        int SoPhanTu = DuLieu.KichThuoc();
        while (SoPhanTu >= 2 && ChiSo <= (SoPhanTu - 2) / 2) {
            int ConTrai = 2 * ChiSo + 1;
            int ConPhai = ConTrai + 1;
            int ConUuTien = ConTrai;
            if (ConPhai < SoPhanTu && BoSoSanh(DuLieu[ConTrai], DuLieu[ConPhai]))
                ConUuTien = ConPhai;
            if (!BoSoSanh(DuLieu[ChiSo], DuLieu[ConUuTien])) break;
            DoiCho(ChiSo, ConUuTien);
            ChiSo = ConUuTien;
        }
    }

    void DieuChinh(int ChiSo) {
        if (ChiSo > 0 && BoSoSanh(DuLieu[(ChiSo - 1) / 2], DuLieu[ChiSo]))
            VunLen(ChiSo);
        else
            VunXuong(ChiSo);
    }

public:
    explicit PriorityQueue(int DungLuongBanDau = 10, const SoSanh& BoSoSanh = SoSanh())
        : DuLieu(DungLuongBanDau), BoSoSanh(BoSoSanh) {}

    bool Rong() const { return DuLieu.Rong(); }
    int KichThuoc() const { return DuLieu.KichThuoc(); }


    const T& LayTai(int ChiSo) const { KiemTraChiSo(ChiSo); return DuLieu[ChiSo]; }
    const T& LayDinh() const { return LayTai(0); }

    void Them(const T& GiaTri) {
        DuLieu.ThemCuoi(GiaTri);
        VunLen(DuLieu.KichThuoc() - 1);
    }


    void SuaTai(int ChiSo, const T& GiaTri) {
        KiemTraChiSo(ChiSo);
        DuLieu[ChiSo] = GiaTri;
        DieuChinh(ChiSo);
    }

    void XoaTai(int ChiSo) {
        KiemTraChiSo(ChiSo);
        int Cuoi = DuLieu.KichThuoc() - 1;
        if (ChiSo != Cuoi) DuLieu[ChiSo] = DuLieu[Cuoi];
        DuLieu.XoaCuoi();
        if (ChiSo < DuLieu.KichThuoc()) DieuChinh(ChiSo);
    }

    void XoaDinh() { XoaTai(0); }
    void XoaHet() { DuLieu.XoaHet(); }

    void HoanDoi(PriorityQueue& Khac) noexcept {
        DuLieu.HoanDoi(Khac.DuLieu);
        SoSanh Tam = BoSoSanh; BoSoSanh = Khac.BoSoSanh; Khac.BoSoSanh = Tam;
    }
};
