#include "model/DoctorParticipation.h"
#include "util/My_stdexcept.h"
DoctorParticipation::DoctorParticipation(const std::string& M, std::time_t T, DoctorRole V, const std::string& Thay)
    : MaBacSi(M), MaBacSiDuocThay(Thay), ThoiDiemThamGia(T), VaiTro(V) {
    if (M.empty() || T < 0 || (V != DoctorRole::KhamChinh && V != DoctorRole::HoTro && V != DoctorRole::ThayThe) ||
        (V == DoctorRole::ThayThe ? Thay.empty() || Thay == M : !Thay.empty()))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ban ghi bac si tham gia khong hop le");
}
