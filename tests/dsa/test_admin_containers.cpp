#include "dsa/LinkedList.h"
#include "dsa/HashTable.h"
#include "util/My_stdexcept.h"
#include <cassert>
#include <utility>
// Cố ý cho mọi khóa va chạm để kiểm tra chaining thật sự.
struct BamTrung { std::size_t operator()(int) const { return 0; } };
int main() {
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
}
