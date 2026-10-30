#include "dsa/PriorityQueue.h"
#include <cassert>
#include <initializer_list>
#include <utility>

struct SoSanhNguoc {
    bool operator()(int Trai, int Phai) const { return Trai > Phai; }
};
struct SoSanhConTro {
    bool operator()(const int* Trai, const int* Phai) const { return *Trai < *Phai; }
};

template <typename Ham>
void PhaiNgoaiPhamVi(Ham ThaoTac) {
    bool DaBat = false;
    try { ThaoTac(); }
    catch (const My_stdexcept& Loi) {
        DaBat = true;
        assert(Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::NgoaiPhamVi);
    }
    assert(DaBat);
}

template <typename Hang>
int TimViTri(const Hang& Heap, int GiaTri) {
    for (int I = 0; I < Heap.KichThuoc(); ++I)
        if (Heap.LayTai(I) == GiaTri) return I;
    return -1;
}

int main() {
    PriorityQueue<int> Heap(1);
    assert(Heap.Rong());
    PhaiNgoaiPhamVi([&] { (void)Heap.LayDinh(); });
    PhaiNgoaiPhamVi([&] { Heap.XoaDinh(); });
    PhaiNgoaiPhamVi([&] { Heap.SuaTai(0, 1); });
    PhaiNgoaiPhamVi([&] { Heap.XoaTai(-1); });
    for (int I : {4, 2, 9, 9, 1, 7, 6}) Heap.Them(I);
    assert(Heap.KichThuoc() == 7 && Heap.LayDinh() == 9);
    PhaiNgoaiPhamVi([&] { (void)Heap.LayTai(Heap.KichThuoc()); });
    Heap.SuaTai(TimViTri(Heap, 1), 12);
    assert(Heap.LayDinh() == 12);
    Heap.SuaTai(0, 0);
    assert(Heap.LayDinh() == 9);
    Heap.XoaTai(TimViTri(Heap, 7));
    Heap.XoaTai(Heap.KichThuoc() - 1);
    int Truoc = Heap.LayDinh();
    while (!Heap.Rong()) {
        int HienTai = Heap.LayDinh();
        assert(HienTai <= Truoc);
        Truoc = HienTai;
        Heap.XoaDinh();
    }
    for (int I = 0; I < 100; ++I) Heap.Them(I);
    PriorityQueue<int> BanSao = Heap;
    Heap.XoaDinh();
    assert(BanSao.LayDinh() == 99 && Heap.LayDinh() == 98);
    PriorityQueue<int> Chuyen = std::move(BanSao);
    assert(BanSao.Rong() && Chuyen.LayDinh() == 99);
    BanSao.Them(3);
    assert(BanSao.LayDinh() == 3);
    Heap.XoaHet(); Heap.Them(8); assert(Heap.LayDinh() == 8);

    PriorityQueue<int, SoSanhNguoc> MinHeap;
    for (int I : {5, 1, 4, 2, 3}) MinHeap.Them(I);
    for (int I = 1; I <= 5; ++I) { assert(MinHeap.LayDinh() == I); MinHeap.XoaDinh(); }
    PriorityQueue<int> XoaGiua;
    for (int I : {10, 8, 9, 1, 2, 7, 6}) XoaGiua.Them(I);
    XoaGiua.XoaTai(TimViTri(XoaGiua, 8));
    for (int I : {10, 9, 7, 6, 2, 1}) { assert(XoaGiua.LayDinh() == I); XoaGiua.XoaDinh(); }
    assert(XoaGiua.Rong());
    PriorityQueue<int> ThamChieu;
    for (int I : {8, 6, 4}) ThamChieu.Them(I);
    ThamChieu.SuaTai(0, ThamChieu.LayTai(2));
    assert(ThamChieu.LayDinh() == 6);
    int A = 3, B = 9, C = 5;
    PriorityQueue<int*, SoSanhConTro> ConTro;
    ConTro.Them(&A); ConTro.Them(&B); ConTro.Them(&C);
    assert(*ConTro.LayDinh() == 9);
    ConTro.XoaHet(); assert(A == 3 && B == 9 && C == 5);
}
