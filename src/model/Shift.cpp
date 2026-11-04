// Phần B — Chức năng chính: Lưu ca trực và kiểm tra thời gian, ca đêm, cuối tuần.
#include "model/Shift.h"
#include "util/My_stdexcept.h"
namespace {
constexpr std::time_t MocThuHai = 320400;
std::time_t DauTuan(std::time_t T) { return MocThuHai + ((T-MocThuHai)/604800)*604800; }
}
Shift::Shift(const std::string& Ma, int Dau, int Cuoi, int BS, int DD, bool Truong)
    : MaCa(Ma), BatDau(MocThuHai+static_cast<std::time_t>(Dau)*60),
      KetThuc(MocThuHai+static_cast<std::time_t>(Cuoi)*60), SoBacSi(BS), SoDieuDuong(DD), CanTruongCa(Truong) {}
int Shift::LayPhutBatDau() const { return static_cast<int>((BatDau-DauTuan(BatDau))/60); }
int Shift::LayPhutKetThuc() const { return static_cast<int>((KetThuc-DauTuan(BatDau))/60); }
bool Shift::HopLe() const {
    return !MaCa.empty() && BatDau>=MocThuHai && KetThuc<=4102444800LL && BatDau%60==0 && KetThuc%60==0 &&
           KetThuc>BatDau && KetThuc-BatDau<=86400 &&
           SoBacSi>=0 && SoBacSi<=10 && SoDieuDuong>=0 && SoDieuDuong<=10 &&
           SoBacSi+SoDieuDuong>0 && (!CanTruongCa || SoBacSi>0);
}
bool Shift::LaCaDem() const {
    const int Dau=LayPhutBatDau(), Cuoi=LayPhutKetThuc();
    for (int Ngay=0; Ngay<=7; ++Ngay) {
        if (Dau<Ngay*1440+360 && Cuoi>Ngay*1440) return true;
        if (Dau<(Ngay+1)*1440 && Cuoi>Ngay*1440+1320) return true;
    }
    return false;
}
bool Shift::LaCuoiTuan() const { return LayPhutBatDau()<7*1440 && LayPhutKetThuc()>5*1440; }

bool Shift::CoChongGio(const Shift& CaKhac) const {
    return LayBatDau()<CaKhac.LayKetThuc() && CaKhac.LayBatDau()<LayKetThuc();
}

Shift Shift::TuThoiDiem(const std::string& Ma, std::time_t Dau, std::time_t Cuoi, int BS, int DD, bool Truong) {
    // UTC+7 cố định: không phụ thuộc timezone máy chạy. Giới hạn đến năm 2100.
    if (Dau < 320400 || Cuoi > 4102444800LL || Cuoi <= Dau || Dau % 60 || Cuoi % 60 || Cuoi - Dau > 86400)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ca can moc phut hop le, dai toi da 24 gio");
    const std::time_t Moc = 320400 + ((Dau - 320400) / 604800) * 604800;
    Shift C(Ma, static_cast<int>((Dau-Moc)/60), static_cast<int>((Cuoi-Moc)/60), BS, DD, Truong);
    C.BatDau = Dau; C.KetThuc = Cuoi;
    if (!C.HopLe()) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ca truc khong hop le");
    return C;
}
