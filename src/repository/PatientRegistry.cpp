#include "repository/PatientRegistry.h"

#include <ctime>
#include <string>

#include "dsa/HashTable.h"
#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "util/My_stdexcept.h"

namespace {
// Hai chuỗi có cùng thứ tự và cùng số byte cho từng cặp chữ tiếng Việt UTF-8.
// Tự đổi hoa/thường để kết quả không phụ thuộc locale và không cần Qt/ICU.
std::string ChuyenVeChuThuong(const std::string& VanBan) {
    const std::string ChuHoa = u8"ÀÁÂÃÈÉÊÌÍÒÓÔÕÙÚÝĂĐĨŨƠƯẠẢẤẦẨẪẬẮẰẲẴẶẸẺẼẾỀỂỄỆỈỊỌỎỐỒỔỖỘỚỜỞỠỢỤỦỨỪỬỮỰỲỴỶỸ";
    const std::string ChuThuong = u8"àáâãèéêìíòóôõùúýăđĩũơưạảấầẩẫậắằẳẵặẹẻẽếềểễệỉịọỏốồổỗộớờởỡợụủứừửữựỳỵỷỹ";
    std::string KetQua = VanBan;
    for (std::string::size_type I = 0; I < KetQua.size();) {
        const unsigned char KyTu = static_cast<unsigned char>(KetQua[I]);
        if (KyTu >= 'A' && KyTu <= 'Z') {
            KetQua[I++] = static_cast<char>(KyTu + ('a' - 'A'));
            continue;
        }
        // Các chữ tiếng Việt dựng sẵn chiếm 2 hoặc 3 byte; byte khác giữ nguyên.
        const std::string::size_type DoDai = (KyTu & 0xE0) == 0xC0 ? 2 : (KyTu & 0xF0) == 0xE0 ? 3 : 1;
        if (DoDai > 1 && I + DoDai <= KetQua.size()) {
            const std::string::size_type ViTri = ChuHoa.find(KetQua.substr(I, DoDai));
            if (ViTri != std::string::npos)
                KetQua.replace(I, DoDai, ChuThuong, ViTri, DoDai);
        }
        I += DoDai;
    }
    return KetQua;
}

// Đổi ngày UTC sang ngày Việt Nam bằng lịch, tránh cộng giây làm tràn time_t.
bool KhopNgayTiepNhan(std::time_t ThoiDiem, const Date& NgayCanTim) {
    const std::tm* Lich = std::gmtime(&ThoiDiem);
    if (Lich == nullptr)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,
                          "Khong doi duoc thoi diem tiep nhan sang ngay");
    // Date chỉ biểu diễn năm 1..9999; kiểm tra trước khi chuyển về int.
    const long long Nam = static_cast<long long>(Lich->tm_year) + 1900;
    if (Nam < 1 || Nam > 9999) return false;
    Date Ngay{Lich->tm_mday, Lich->tm_mon + 1, static_cast<int>(Nam)};
    if (Lich->tm_hour >= 17) {
        ++Ngay.Ngay;
        if (!Ngay.HopLe()) {
            Ngay.Ngay = 1;
            ++Ngay.Thang;
            if (Ngay.Thang > 12) {
                Ngay.Thang = 1;
                ++Ngay.Nam;
            }
        }
    }
    return Ngay.Ngay == NgayCanTim.Ngay && Ngay.Thang == NgayCanTim.Thang &&
           Ngay.Nam == NgayCanTim.Nam;
}

// Sắp xếp chèn: giữ dòng hiện tại rồi dịch các dòng đứng sai thứ tự sang phải.
// Mã là điều kiện phụ để kết quả ổn định dù thứ tự duyệt bảng băm thay đổi.
void SapXepLanKhamMoiNhat(Vector<KetQuaTraCuuLanKham>& DanhSach) {
    for (int I = 1; I < DanhSach.KichThuoc(); ++I) {
        const KetQuaTraCuuLanKham Dong = DanhSach[I];
        int J = I;
        while (J > 0) {
            const Visit* Truoc = DanhSach[J - 1].LanKham;
            const Visit* HienTai = Dong.LanKham;
            if (Truoc->LayThoiDiemTiepNhan() > HienTai->LayThoiDiemTiepNhan() ||
                (Truoc->LayThoiDiemTiepNhan() == HienTai->LayThoiDiemTiepNhan() &&
                 Truoc->LayMaLanKham() < HienTai->LayMaLanKham())) break;
            DanhSach[J] = DanhSach[J - 1];
            --J;
        }
        DanhSach[J] = Dong;
    }
}
} // namespace

PatientRegistry::~PatientRegistry() {
    // Bảng băm chỉ sở hữu nút chứa địa chỉ; Registry phải thu hồi các đối tượng.
    DanhSachLanKham.Duyet([](const std::string&, Visit* LanKham) { delete LanKham; });
    DanhSachBenhNhan.Duyet([](const std::string&, Patient* BenhNhan) { delete BenhNhan; });
}

Patient* PatientRegistry::LayBenhNhanBatBuoc(const std::string& MaBenhNhan) const {
    Patient* const* DiaChi = DanhSachBenhNhan.Timkiem(MaBenhNhan);
    if (DiaChi == nullptr)
        throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "Khong tim thay ma benh nhan");
    return *DiaChi;
}

void PatientRegistry::KiemTraCanCuoc(const std::string& SoCanCuoc, const Patient* BoQua) const {
    if (SoCanCuoc.empty()) return; // Nhiều người chưa có CCCD vẫn là những hồ sơ riêng.
    const Patient* DaCo = TimBenhNhanTheoCanCuoc(SoCanCuoc);
    if (DaCo != nullptr && DaCo != BoQua)
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,
                          "CCCD da co: can kiem tra danh tinh va dung dung ma benh nhan cu");
}

const Patient* PatientRegistry::ThemBenhNhan(const Patient& HoSo) {
    if (DanhSachBenhNhan.Cokhoa(HoSo.LayMaDinhDanh()))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ma benh nhan da ton tai");
    KiemTraCanCuoc(HoSo.LaySoCanCuoc());
    Patient* Moi = new Patient(HoSo);
    try {
        // Kiểm tra trùng trước vì HashTable::Them sẽ ghi đè nếu khóa đã có.
        DanhSachBenhNhan.Them(Moi->LayMaDinhDanh(), Moi);
    } catch (...) {
        delete Moi;
        throw;
    }
    return Moi;
}

const Patient* PatientRegistry::ThemBenhNhan(const std::string& MaBenhNhan,
                                            const std::string& HoTen, Gender GioiTinh,
                                            const std::string& SoCanCuoc) {
    return ThemBenhNhan(Patient(MaBenhNhan, HoTen, GioiTinh, SoCanCuoc));
}

const Patient* PatientRegistry::ThemBenhNhan(const std::string& MaBenhNhan,
                                            const std::string& HoTen, Gender GioiTinh,
                                            const Date& NgaySinh, const std::string& SoCanCuoc) {
    return ThemBenhNhan(Patient(MaBenhNhan, HoTen, GioiTinh, NgaySinh, SoCanCuoc));
}

const Patient* PatientRegistry::TimBenhNhan(const std::string& MaBenhNhan) const {
    Patient* const* DiaChi = DanhSachBenhNhan.Timkiem(MaBenhNhan);
    return DiaChi == nullptr ? nullptr : *DiaChi;
}

const Patient* PatientRegistry::TimBenhNhanTheoCanCuoc(const std::string& SoCanCuoc) const {
    if (SoCanCuoc.empty()) return nullptr;
    const Patient* KetQua = nullptr;
    DanhSachBenhNhan.Duyet([&](const std::string&, const Patient* BenhNhan) {
        if (BenhNhan->LaySoCanCuoc() == SoCanCuoc) KetQua = BenhNhan;
    });
    return KetQua;
}

const Patient* PatientRegistry::TimBenhNhanTheoMaLanKham(const std::string& MaLanKham) const {
    const Visit* LanKham = TimLanKham(MaLanKham);
    return LanKham == nullptr ? nullptr : TimBenhNhan(LanKham->LayMaBenhNhan());
}

Vector<const Patient*> PatientRegistry::TimBenhNhanTheoTen(const std::string& TenCanTim) const {
    Vector<const Patient*> KetQua;
    if (TenCanTim.empty()) return KetQua;
    const std::string TenThuong = ChuyenVeChuThuong(TenCanTim);
    DanhSachBenhNhan.Duyet([&](const std::string&, const Patient* BenhNhan) {
        if (ChuyenVeChuThuong(BenhNhan->LayHoTen()).find(TenThuong) != std::string::npos)
            KetQua.ThemCuoi(BenhNhan);
    });
    return KetQua;
}

Vector<KetQuaTraCuuLanKham> PatientRegistry::TraCuuLanKham(
    const std::string& TenCanTim, const Date* NgayTiepNhan, const std::string& MaLanKham) const {
    if (NgayTiepNhan != nullptr && !NgayTiepNhan->HopLe())
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay loc khong hop le");
    Vector<KetQuaTraCuuLanKham> KetQua;
    const std::string TenThuong = ChuyenVeChuThuong(TenCanTim);
    // Tách việc kiểm tra từng dòng khỏi thao tác duyệt bảng để dễ theo dõi.
    auto XetLanKham = [&](const std::string&, const Visit* LanKham) {
        if (NgayTiepNhan != nullptr &&
            !KhopNgayTiepNhan(LanKham->LayThoiDiemTiepNhan(), *NgayTiepNhan)) return;
        const Patient* BenhNhan = LayBenhNhanBatBuoc(LanKham->LayMaBenhNhan());
        if (!TenThuong.empty() &&
            ChuyenVeChuThuong(BenhNhan->LayHoTen()).find(TenThuong) == std::string::npos) return;
        KetQua.ThemCuoi({BenhNhan, LanKham});
    };
    // Có mã thì tra thẳng bảng băm, sau đó vẫn kiểm tra các bộ lọc còn lại.
    if (!MaLanKham.empty()) {
        const Visit* LanKham = TimLanKham(MaLanKham);
        if (LanKham != nullptr) XetLanKham(MaLanKham, LanKham);
    } else {
        DanhSachLanKham.Duyet(XetLanKham);
    }
    SapXepLanKhamMoiNhat(KetQua);
    return KetQua;
}

void PatientRegistry::BoSungDanhTinh(const std::string& MaBenhNhan, const std::string& SoCanCuoc,
                                    const std::string& HoTen, Gender GioiTinh, const Date& NgaySinh) {
    Patient* BenhNhan = LayBenhNhanBatBuoc(MaBenhNhan);
    KiemTraCanCuoc(SoCanCuoc, BenhNhan);
    // Sửa đúng đối tượng đang có: địa chỉ, mã nội bộ và dữ liệu y tế giữ nguyên.
    BenhNhan->BoSungDanhTinh(SoCanCuoc, HoTen, GioiTinh, NgaySinh);
}

bool PatientRegistry::CoLanKhamDangMo(const std::string& MaBenhNhan) const {
    bool CoLanMo = false;
    DanhSachLanKham.Duyet([&](const std::string&, const Visit* LanKham) {
        if (LanKham->LayMaBenhNhan() == MaBenhNhan &&
            LanKham->LayTrangThai() != VisitStatus::DaKetThuc &&
            LanKham->LayTrangThai() != VisitStatus::DaHuy) CoLanMo = true;
    });
    return CoLanMo;
}

Visit* PatientRegistry::ThemLanKham(const std::string& MaLanKham, const std::string& MaBenhNhan,
                                   std::time_t ThoiDiemTiepNhan, const std::string& TrieuChungChinh) {
    if (DanhSachLanKham.Cokhoa(MaLanKham))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ma lan kham da ton tai");
    LayBenhNhanBatBuoc(MaBenhNhan);
    if (CoLanKhamDangMo(MaBenhNhan))
        throw My_stdexcept(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, "Benh nhan con lan kham dang mo");
    Visit* Moi = new Visit(MaLanKham, MaBenhNhan, ThoiDiemTiepNhan, TrieuChungChinh);
    try {
        DanhSachLanKham.Them(MaLanKham, Moi);
    } catch (...) {
        delete Moi;
        throw;
    }
    return Moi;
}

Visit* PatientRegistry::TimLanKham(const std::string& MaLanKham) {
    Visit** DiaChi = DanhSachLanKham.Timkiem(MaLanKham);
    return DiaChi == nullptr ? nullptr : *DiaChi;
}

const Visit* PatientRegistry::TimLanKham(const std::string& MaLanKham) const {
    Visit* const* DiaChi = DanhSachLanKham.Timkiem(MaLanKham);
    return DiaChi == nullptr ? nullptr : *DiaChi;
}

Vector<Visit*> PatientRegistry::LayLichSuLanKham(const std::string& MaBenhNhan) {
    LayBenhNhanBatBuoc(MaBenhNhan);
    Vector<Visit*> KetQua;
    DanhSachLanKham.Duyet([&](const std::string&, Visit* LanKham) {
        if (LanKham->LayMaBenhNhan() == MaBenhNhan) KetQua.ThemCuoi(LanKham);
    });
    return KetQua;
}

Vector<const Visit*> PatientRegistry::LayLichSuLanKham(const std::string& MaBenhNhan) const {
    LayBenhNhanBatBuoc(MaBenhNhan);
    Vector<const Visit*> KetQua;
    DanhSachLanKham.Duyet([&](const std::string&, const Visit* LanKham) {
        if (LanKham->LayMaBenhNhan() == MaBenhNhan) KetQua.ThemCuoi(LanKham);
    });
    return KetQua;
}

int PatientRegistry::SoBenhNhan() const { return DanhSachBenhNhan.Kichthuoc(); }
int PatientRegistry::SoLanKham() const { return DanhSachLanKham.Kichthuoc(); }
