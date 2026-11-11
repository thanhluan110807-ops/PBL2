#include "dsa/LinkedList.h"
#include "dsa/HashTable.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <utility>
#include <string>
// Cố ý cho mọi khóa va chạm để kiểm tra chaining thật sự.
struct BamTrung { std::size_t operator()(int) const { return 0; } };
struct GiaTriThu {
    static int DangSong;
    static int SoLanSaoChep;
    int GiaTri;
    explicit GiaTriThu(int So) : GiaTri(So) { ++DangSong; }
    GiaTriThu(const GiaTriThu& Khac) : GiaTri(Khac.GiaTri) {
        if (SoLanSaoChep == 0)
            throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi, "Loi sao chep thu");
        if (SoLanSaoChep > 0) --SoLanSaoChep;
        ++DangSong;
    }
    ~GiaTriThu() { --DangSong; }
};
int GiaTriThu::DangSong = 0;
int GiaTriThu::SoLanSaoChep = -1;

void KiemTraSaoChepLoi() {
    LinkedList<GiaTriThu> Nguon, Dich;
    Nguon.Themcuoi(GiaTriThu(1)); Nguon.Themcuoi(GiaTriThu(2));
    Dich.Themcuoi(GiaTriThu(9));
    const int SoBanDau = GiaTriThu::DangSong;
    GiaTriThu::SoLanSaoChep = 1;
    bool CoLoi = false;
    try { Dich = Nguon; } catch (const My_stdexcept&) { CoLoi = true; }
    GiaTriThu::SoLanSaoChep = -1;
    assert(CoLoi && GiaTriThu::DangSong == SoBanDau);
    assert(Dich.Kichthuoc() == 1 && Dich.Dau().GiaTri == 9);
    assert(Nguon.Kichthuoc() == 2 && Nguon.Cuoi().GiaTri == 2);
}

void KiemTraThemKhiRehash() {
    const std::string GiaTri = "GiaTriDaiDeKhongNamTrongSmallStringOptimization";
    HashTable<std::string, std::string> Bang(2);
    Bang.Them("A", GiaTri);
    assert(Bang.Them("B", Bang.Lay("A")));
    assert(Bang.Lay("B") == GiaTri && Bang.Lay("A") == GiaTri);
    HashTable<std::string, std::string> BangKhac(2);
    BangKhac.Them("A", GiaTri);
    assert(BangKhac.Them(BangKhac.Lay("A"), BangKhac.Lay("A")));
    assert(BangKhac.Lay(GiaTri) == GiaTri);
}

int main() {
    KiemTraThemKhiRehash();
    KiemTraSaoChepLoi();
    assert(GiaTriThu::DangSong == 0);
    LinkedList<int> L;
    assert(!L.Xoadau());
    L.Themcuoi(1); L.Themcuoi(2); L.Themcuoi(3);
    LinkedList<int> C=L; C.Lay(0)=9;
    assert(L.Dau()==1 && C.Dau()==9);
    assert(L.Xoagiatri(3)); L.Themcuoi(4); assert(L.Cuoi()==4);
    assert(L.Xoagiatri(1) && L.Dau()==2);
    LinkedList<int> M=std::move(L); assert(L.Rong() && M.Kichthuoc()==2);
    L.Themcuoi(5); assert(L.Cuoi()==5);
    HashTable<int,int,BamTrung> H(2);
    for (int I=0;I<100;++I) assert(H.Them(I,I*2));
    assert(H.Kichthuoc()==100 && H.Dungluong()>2);
    for (int I=0;I<100;++I) assert(H.Lay(I)==I*2);
    HashTable<int,int,BamTrung> K=H;
    K.Lay(3)=777; assert(H.Lay(3)==6);
    assert(K.Xoa(0) && K.Xoa(50) && K.Xoa(99) && !K.Xoa(999));
    assert(K.Lay(1)==2 && K.Lay(98)==196);
    HashTable<int,int,BamTrung> N=std::move(K);
    assert(N.Lay(3)==777); K.Them(9,42); assert(K.Lay(9)==42);
    int So=0; const auto& Const=N;
    Const.Duyet([&](const int&,const int&){++So;}); assert(So==97);
    H=H; assert(H.Kichthuoc()==100);
    LinkedList<int> Dich;
    Dich.Themcuoi(99); Dich=M;
    Dich.Dau()=7; assert(M.Dau()==2);
    Dich=std::move(M); assert(M.Rong() && Dich.Cuoi()==4);
    assert(Dich.Xoadau() && Dich.Xoadau() && Dich.Rong());
    Dich.Themdau(8); assert(Dich.Dau()==8 && Dich.Cuoi()==8);
    assert(Dich.Xoagiatri(8) && Dich.Rong());
    Dich.Themcuoi(10); assert(Dich.Dau()==10 && Dich.Cuoi()==10);
    HashTable<int,int,BamTrung> DichBang;
    DichBang=H; DichBang.Lay(1)=99; assert(H.Lay(1)==2);
    DichBang=std::move(N); assert(N.Rong() && DichBang.Lay(3)==777);
    N.Them(1,8); assert(N.Lay(1)==8);
    DichBang.Xoahet(); assert(DichBang.Rong());
    DichBang.Them(1,9); assert(!DichBang.Them(1,10));
    assert(DichBang.Kichthuoc()==1 && DichBang.Lay(1)==10);
}
