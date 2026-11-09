#include "repository/PatientRegistry.h"

#include <cassert>
#include <cstdlib>
#include <new>
#include <string>

#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"

namespace {
// Chỉ binary test này thay new/delete; mã nghiệp vụ không có điểm tiêm lỗi giả.
int SoLanCapPhatConLai = -1;
int SoKhoiDangSong = 0;

void* CapPhatThu(std::size_t SoByte) {
    if (SoLanCapPhatConLai == 0) throw std::bad_alloc();
    if (SoLanCapPhatConLai > 0) --SoLanCapPhatConLai;
    void* DiaChi = std::malloc(SoByte == 0 ? 1 : SoByte);
    if (DiaChi == nullptr) throw std::bad_alloc();
    ++SoKhoiDangSong;
    return DiaChi;
}
void GiaiPhongThu(void* DiaChi) noexcept {
    if (DiaChi != nullptr) { --SoKhoiDangSong; std::free(DiaChi); }
}
} // namespace

void* operator new(std::size_t SoByte) { return CapPhatThu(SoByte); }
void* operator new[](std::size_t SoByte) { return CapPhatThu(SoByte); }
void operator delete(void* DiaChi) noexcept { GiaiPhongThu(DiaChi); }
void operator delete[](void* DiaChi) noexcept { GiaiPhongThu(DiaChi); }
void operator delete(void* DiaChi, std::size_t) noexcept { GiaiPhongThu(DiaChi); }
void operator delete[](void* DiaChi, std::size_t) noexcept { GiaiPhongThu(DiaChi); }

void KiemTraLoiThem(bool LaThemVisit, int SoHoSoCu) {
    bool DaThanhCong = false;
    int SoLanDaLoi = 0;
    // Chạy lại từ đầu, làm hỏng lần cấp phát thứ 0, 1, ... đến khi thêm thành công.
    // 12 hồ sơ làm lần thêm thứ 13 phải rehash bảng có dung lượng ban đầu 16.
    for (int Buoc = 0; Buoc < 200 && !DaThanhCong; ++Buoc) {
        const int SoKhoiBanDau = SoKhoiDangSong;
        {
            PatientRegistry SoBo;
            const Patient* Cu = nullptr;
            Visit* KhamCu = nullptr;
            for (int I = 0; I < SoHoSoCu; ++I) {
                const std::string Ma = "HoSoCu_" + std::to_string(I);
                const Patient* BenhNhan = SoBo.ThemBenhNhan(Ma);
                if (I == 0) Cu = BenhNhan;
                if (LaThemVisit) {
                    Visit* LanKham = SoBo.ThemLanKham(Ma, Ma, 1, "Kiem tra");
                    if (I == 0) KhamCu = LanKham;
                }
            }
            SoBo.ThemBenhNhan("NguoiMoi");
            const std::string MaMoi(80, 'M');
            const std::string TenMoi(120, 'T');
            const std::string CanCuocMoi = "001234567890";
            const int SoBenhNhanCu = SoBo.SoBenhNhan(), SoLanKhamCu = SoBo.SoLanKham();
            SoLanCapPhatConLai = Buoc;
            try {
                if (LaThemVisit) SoBo.ThemLanKham(MaMoi, "NguoiMoi", 2, TenMoi);
                else SoBo.ThemBenhNhan(MaMoi, TenMoi, Gender::Nam, CanCuocMoi);
                DaThanhCong = true;
            } catch (const std::bad_alloc&) { ++SoLanDaLoi; }
            SoLanCapPhatConLai = -1;
            assert(SoBo.SoBenhNhan() == SoBenhNhanCu + (DaThanhCong && !LaThemVisit ? 1 : 0));
            assert(SoBo.SoLanKham() == SoLanKhamCu + (DaThanhCong && LaThemVisit ? 1 : 0));
            if (Cu != nullptr) assert(SoBo.TimBenhNhan("HoSoCu_0") == Cu);
            if (KhamCu != nullptr) assert(SoBo.TimLanKham("HoSoCu_0") == KhamCu);
            if (LaThemVisit) assert((SoBo.TimLanKham(MaMoi) != nullptr) == DaThanhCong);
            else {
                assert((SoBo.TimBenhNhan(MaMoi) != nullptr) == DaThanhCong);
                assert((SoBo.TimBenhNhanTheoCanCuoc(CanCuocMoi) != nullptr) == DaThanhCong);
            }
        }
        // So sánh sau khi Registry hủy: dung lượng bảng có thể tăng trước khi lỗi.
        assert(SoKhoiDangSong == SoKhoiBanDau);
    }
    assert(DaThanhCong && SoLanDaLoi > 0);
}

void KiemTraLoiBoSungDanhTinh() {
    bool DaThanhCong = false;
    int SoLanDaLoi = 0;
    for (int Buoc = 0; Buoc < 50 && !DaThanhCong; ++Buoc) {
        const int SoKhoiBanDau = SoKhoiDangSong;
        {
            PatientRegistry SoBo;
            const Patient* Cu = SoBo.ThemBenhNhan("BN", "Ten cu", Gender::Nam,
                                                 Date{1,1,2000}, "001");
            const std::string CanCuocMoi(80, '2'), TenMoi(160, 'T');
            SoLanCapPhatConLai = Buoc;
            try {
                SoBo.BoSungDanhTinh("BN", CanCuocMoi, TenMoi, Gender::Nu, Date{2,2,2001});
                DaThanhCong = true;
            } catch (const std::bad_alloc&) { ++SoLanDaLoi; }
            SoLanCapPhatConLai = -1;
            assert(SoBo.TimBenhNhan("BN") == Cu && SoBo.SoBenhNhan() == 1);
            if (!DaThanhCong) {
                assert(Cu->LayHoTen() == "Ten cu" && Cu->LaySoCanCuoc() == "001");
                assert(Cu->LayGioiTinh() == Gender::Nam && Cu->LayNgaySinh().Nam == 2000);
                assert(SoBo.TimBenhNhanTheoCanCuoc("001") == Cu);
                assert(SoBo.TimBenhNhanTheoCanCuoc(CanCuocMoi) == nullptr);
            } else {
                assert(Cu->LayHoTen() == TenMoi && Cu->LaySoCanCuoc() == CanCuocMoi);
                assert(SoBo.TimBenhNhanTheoCanCuoc("001") == nullptr);
            }
        }
        assert(SoKhoiDangSong == SoKhoiBanDau);
    }
    assert(DaThanhCong && SoLanDaLoi > 0);
}

void KiemTraLoiBoSungThongTinConThieu() {
    bool ThanhCong=false;
    int SoLoi=0;
    for (int Buoc=0; Buoc<60 && !ThanhCong; ++Buoc) {
        const int SoKhoiCu=SoKhoiDangSong;
        {
            PatientRegistry R; const auto* Cu=R.ThemBenhNhan("BN");
            const std::string Ten(120,'T'), Cccd(80,'1'), DiUng(140,'D'), TienSu(160,'S');
            Patient Nguon("BN",Ten,Gender::Nu,Date{1,1,2000},Cccd);
            Nguon.CapNhatThongTinDiUng(DiUng); Nguon.CapNhatTienSuBenh(TienSu);
            SoLanCapPhatConLai=Buoc;
            try { R.BoSungThongTinConThieu("BN",Nguon); ThanhCong=true; }
            catch (const std::bad_alloc&) { ++SoLoi; }
            SoLanCapPhatConLai=-1;
            assert(R.TimBenhNhan("BN")==Cu);
            if (ThanhCong) {
                assert(Cu->LayHoTen()==Ten && Cu->LaySoCanCuoc()==Cccd);
                assert(Cu->LayThongTinDiUng()==DiUng && Cu->LayTienSuBenh()==TienSu);
            } else {
                assert(Cu->LayHoTen().empty() && Cu->LaySoCanCuoc().empty() && !Cu->CoNgaySinh());
                assert(Cu->LayThongTinDiUng().empty() && Cu->LayTienSuBenh().empty());
            }
        }
        assert(SoKhoiDangSong==SoKhoiCu);
    }
    assert(ThanhCong && SoLoi>0);
}

int main() {
    KiemTraLoiBoSungThongTinConThieu();
    KiemTraLoiThem(false, 0);
    KiemTraLoiThem(false, 11); // Cộng NguoiMoi = 12 Patient trước lần thêm.
    KiemTraLoiThem(true, 0);
    KiemTraLoiThem(true, 12);
    KiemTraLoiBoSungDanhTinh();
}
