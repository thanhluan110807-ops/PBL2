#include "service/TriageService.h"
#include <cassert>
#include <cstdlib>
#include <new>
#include <string>
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Visit.h"
#include "repository/PatientRegistry.h"

namespace {
int ConLai = -1;
int DangSong = 0;
void* CapPhat(std::size_t SoByte) {
    if (ConLai == 0) throw std::bad_alloc();
    if (ConLai > 0) --ConLai;
    void* KetQua = std::malloc(SoByte == 0 ? 1 : SoByte);
    if (!KetQua) throw std::bad_alloc();
    ++DangSong;
    return KetQua;
}
void GiaiPhong(void* DiaChi) noexcept {
    if (DiaChi) { --DangSong; std::free(DiaChi); }
}
}
void* operator new(std::size_t SoByte) { return CapPhat(SoByte); }
void* operator new[](std::size_t SoByte) { return CapPhat(SoByte); }
void operator delete(void* DiaChi) noexcept { GiaiPhong(DiaChi); }
void operator delete[](void* DiaChi) noexcept { GiaiPhong(DiaChi); }
void operator delete(void* DiaChi, std::size_t) noexcept { GiaiPhong(DiaChi); }
void operator delete[](void* DiaChi, std::size_t) noexcept { GiaiPhong(DiaChi); }

// Thử lỗi ở từng lần cấp phát trong đánh giá, gọi khám và ghi kết quả.
void KiemTra(int CongDoan) {
    bool ThanhCong = false;
    int SoLoi = 0;
    for (int Buoc = 0; Buoc < 80 && !ThanhCong; ++Buoc) {
        int BanDau = DangSong;
        {
            PatientRegistry Registry;
            TriageService Service(Registry);
            Registry.ThemBenhNhan("BN", "Ten", Gender::Nam, Date{1,1,2000});
            Service.TiepNhan("LK", "BN", 1767225600, "Dau");
            const std::string TenDai(100, 'X');
            if (CongDoan > 0)
                Service.DanhGia("LK", VitalSigns(75,120,16,98,15), 1767225601, "DD");
            if (CongDoan > 1) {
                Service.GoiTiep("BS");
                Service.ChuyenCanLamSang("LK", "XN", "BS");
            }
            ConLai = Buoc;
            try {
                if (CongDoan == 0)
                    Service.DanhGia("LK", VitalSigns(75,120,16,98,15), 1767225601, TenDai);
                else if (CongDoan == 1) Service.GoiTiep(TenDai);
                else Service.QuayLai("LK", TenDai, 1767225602);
                ThanhCong = true;
            } catch (const std::bad_alloc&) { ++SoLoi; }
            ConLai = -1;
            const Visit* LanKham = Registry.TimLanKham("LK");
            if (!ThanhCong) {
                assert(Service.SoDangCho() == (CongDoan == 1 ? 1 : 0));
                if (CongDoan == 0) assert(LanKham->SoDanhGia() == 0);
                if (CongDoan == 1) assert(LanKham->LayTrangThai() == VisitStatus::ChoKhamLanDau);
                if (CongDoan == 2) {
                    assert(LanKham->LayTrangThai() == VisitStatus::ChoCanLamSang);
                    assert(!LanKham->LayCanLamSangTai(0).DaCoKetQua());
                }
            }
        }
        assert(DangSong == BanDau);
    }
    assert(ThanhCong && SoLoi > 0);
}

int main() { KiemTra(0); KiemTra(1); KiemTra(2); }
