#include "repository/PatientRegistry.h"

#include <cassert>
#include <ctime>
#include <string>
#include <type_traits>
#include <utility>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "util/My_stdexcept.h"

// Kiểm tra giao diện ngay lúc biên dịch: không phát con trỏ sửa CCCD ngoài Registry.
static_assert(std::is_same<decltype(std::declval<PatientRegistry&>().TimBenhNhan("")),
                           const Patient*>::value, "Patient muon phai chi doc");
static_assert(std::is_same<decltype(std::declval<const PatientRegistry&>().TimLanKham("")),
                           const Visit*>::value, "Visit cua Registry const phai chi doc");
static_assert(!std::is_copy_constructible<PatientRegistry>::value, "Khong copy chu so huu");
static_assert(!std::is_move_constructible<PatientRegistry>::value, "Khong move Registry");
static_assert(!std::is_copy_assignable<PatientRegistry>::value, "Khong gan chu so huu");
static_assert(!std::is_move_assignable<PatientRegistry>::value, "Khong gan move Registry");
static_assert(!std::is_move_constructible<Visit>::value, "Khong rut lich su khoi Visit muon");
static_assert(std::is_copy_constructible<Visit>::value, "Van cho phep sao chep lich su de xem");

template <typename HamThu>
void PhaiLoi(HamThu Ham, My_stdexcept::LoaiLoi Loai) {
    bool DaLoi = false;
    try { Ham(); }
    catch (const My_stdexcept& Loi) { DaLoi = true; assert(Loi.LayLoaiLoi() == Loai); }
    assert(DaLoi);
}

void KiemTraHoSoVaDanhTinh() {
    PatientRegistry SoBo;
    assert(SoBo.SoBenhNhan() == 0 && SoBo.SoLanKham() == 0);
    assert(SoBo.TimBenhNhan("thieu") == nullptr);
    assert(SoBo.TimLanKham("thieu") == nullptr);
    assert(SoBo.TimBenhNhanTheoCanCuoc("") == nullptr);
    assert(SoBo.TimBenhNhanTheoMaLanKham("thieu") == nullptr);
    const Patient* VoDanh = SoBo.ThemBenhNhan("BN1");
    SoBo.ThemBenhNhan("BN2");
    assert(SoBo.SoBenhNhan() == 2 && VoDanh->LaBenhNhanVoDanh());
    assert(SoBo.TimBenhNhanTheoCanCuoc("") == nullptr);
    assert(SoBo.LayLichSuLanKham("BN1").Rong());
    PhaiLoi([&]{ SoBo.ThemBenhNhan("BN1", "Ghi de"); }, My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    PhaiLoi([&]{ SoBo.ThemBenhNhan(""); }, My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    PhaiLoi([&]{ SoBo.ThemBenhNhan("BN sai"); }, My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    PhaiLoi([&]{ SoBo.ThemBenhNhan("BN_SAI", "A", Gender::Nam, Date{31,2,2000}); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    assert(SoBo.SoBenhNhan() == 2 && SoBo.TimBenhNhan("BN1") == VoDanh);

    const Patient* Cu = SoBo.ThemBenhNhan("BN3", "Nguyen Van A", Gender::Nam,
                                         Date{1,1,2000}, "001234567890");
    assert(SoBo.TimBenhNhanTheoCanCuoc("001234567890") == Cu);
    // CCCD trùng, dù cùng hay khác thông tin, đều yêu cầu kiểm tra và dùng mã cũ.
    PhaiLoi([&]{ SoBo.ThemBenhNhan("BN_MOI", "Nguyen Van A", Gender::Nam,
                                  Date{1,1,2000}, "001234567890"); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    PhaiLoi([&]{ SoBo.ThemBenhNhan("BN_MOI", "Nguoi khac", Gender::Nu, "001234567890"); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    assert(SoBo.TimBenhNhan("BN_MOI") == nullptr && SoBo.SoBenhNhan() == 3);
    assert(Cu->LayHoTen() == "Nguyen Van A" && Cu->LayGioiTinh() == Gender::Nam);

    Patient HoSo("BN4");
    HoSo.DatTuoiUocLuong(40);
    HoSo.CapNhatThongTinDiUng("Penicillin");
    HoSo.CapNhatTienSuBenh("Hen");
    const Patient* SaoChep = SoBo.ThemBenhNhan(HoSo);
    assert(SaoChep != &HoSo && SaoChep->LayTuoiUocLuong() == 40);
    HoSo.CapNhatTienSuBenh("Doi ban goc");
    assert(SaoChep->LayTienSuBenh() == "Hen");
    SoBo.BoSungDanhTinh("BN4", "009999999999", "Le B", Gender::Nu, Date{2,3,1990});
    assert(SoBo.TimBenhNhan("BN4") == SaoChep && !SaoChep->LaBenhNhanVoDanh());
    assert(SaoChep->LayThongTinDiUng() == "Penicillin" && SaoChep->LayTienSuBenh() == "Hen");
    assert(SoBo.TimBenhNhanTheoCanCuoc("009999999999") == SaoChep);
    PhaiLoi([&]{ SoBo.BoSungDanhTinh("BN4", "001234567890", "Doi", Gender::Nam, Date{1,1,2001}); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    PhaiLoi([&]{ SoBo.BoSungDanhTinh("BN4", "008888888888", "Doi", Gender::Nam, Date{31,2,2001}); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    assert(SaoChep->LayHoTen() == "Le B" && SaoChep->LaySoCanCuoc() == "009999999999");
    assert(SoBo.TimBenhNhanTheoCanCuoc("008888888888") == nullptr);
    // Giữ cùng CCCD trên chính hồ sơ đó là hợp lệ; đổi/xóa CCCD không để chỉ mục cũ.
    SoBo.BoSungDanhTinh("BN4", "009999999999", "Le B", Gender::Nu, Date{2,3,1990});
    SoBo.BoSungDanhTinh("BN4", "008888888888", "Le B", Gender::Nu, Date{2,3,1990});
    assert(SoBo.TimBenhNhanTheoCanCuoc("009999999999") == nullptr);
    SoBo.BoSungDanhTinh("BN4", "", "Le B", Gender::Nu, Date{2,3,1990});
    assert(SoBo.TimBenhNhanTheoCanCuoc("008888888888") == nullptr);
    PhaiLoi([&]{ SoBo.BoSungDanhTinh("thieu", "", "B", Gender::Nu, Date{1,1,2000}); },
            My_stdexcept::LoaiLoi::NgoaiPhamVi);
}

void KiemTraTimTen() {
    PatientRegistry SoBo;
    const Patient* A = SoBo.ThemBenhNhan("A", u8"Nguyễn ÁNH ĐƯỜNG");
    const Patient* B = SoBo.ThemBenhNhan("B", u8"Nguyễn ÁNH ĐƯỜNG");
    SoBo.ThemBenhNhan("C", "Tran Van An");
    SoBo.ThemBenhNhan("D");
    Vector<const Patient*> KetQua = SoBo.TimBenhNhanTheoTen(u8"nguyễn ánh");
    assert(KetQua.KichThuoc() == 2);
    assert((KetQua[0] == A && KetQua[1] == B) || (KetQua[0] == B && KetQua[1] == A));
    assert(SoBo.TimBenhNhanTheoTen(u8"đường").KichThuoc() == 2);
    assert(SoBo.TimBenhNhanTheoTen("vAn aN").KichThuoc() == 1);
    assert(SoBo.TimBenhNhanTheoTen("khong co").Rong());
    assert(SoBo.TimBenhNhanTheoTen("").Rong());
    assert(SoBo.TimBenhNhanTheoTen("nguyen").Rong()); // Không tự bỏ dấu.
    SoBo.BoSungDanhTinh("C", "", u8"Trần ĐỖ", Gender::Nam, Date{1,1,2000});
    assert(SoBo.TimBenhNhanTheoTen("vAn aN").Rong());
    assert(SoBo.TimBenhNhanTheoTen(u8"đỗ").KichThuoc() == 1);

    // Bao phủ tất cả chữ hoa/thường tiếng Việt dựng sẵn, không dựa vào locale máy.
    const std::string ChuHoa = u8"ÀÁÂÃÈÉÊÌÍÒÓÔÕÙÚÝĂĐĨŨƠƯẠẢẤẦẨẪẬẮẰẲẴẶẸẺẼẾỀỂỄỆỈỊỌỎỐỒỔỖỘỚỜỞỠỢỤỦỨỪỬỮỰỲỴỶỸ";
    const std::string ChuThuong = u8"àáâãèéêìíòóôõùúýăđĩũơưạảấầẩẫậắằẳẵặẹẻẽếềểễệỉịọỏốồổỗộớờởỡợụủứừửữựỳỵỷỹ";
    SoBo.ThemBenhNhan("E", ChuHoa);
    assert(SoBo.TimBenhNhanTheoTen(ChuThuong).KichThuoc() == 1);
    SoBo.ThemBenhNhan("F", ChuThuong);
    assert(SoBo.TimBenhNhanTheoTen(ChuHoa).KichThuoc() == 2);
}

void KiemTraLanKhamVaTaiSuDung() {
    PatientRegistry SoBo;
    const Patient* BenhNhan = SoBo.ThemBenhNhan("BN1", "A", Gender::Nam, Date{1,1,2000}, "001");
    SoBo.ThemBenhNhan("BN2");
    PhaiLoi([&]{ SoBo.ThemLanKham("LK", "thieu", 1767225600, "Dau"); },
            My_stdexcept::LoaiLoi::NgoaiPhamVi);
    PhaiLoi([&]{ SoBo.LayLichSuLanKham("thieu"); }, My_stdexcept::LoaiLoi::NgoaiPhamVi);
    const PatientRegistry& ChiDoc = SoBo;
    PhaiLoi([&]{ ChiDoc.LayLichSuLanKham("thieu"); }, My_stdexcept::LoaiLoi::NgoaiPhamVi);
    PhaiLoi([&]{ SoBo.ThemLanKham("", "BN1", 1767225600, "Dau"); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    assert(SoBo.SoLanKham() == 0);
    Visit* LanDau = SoBo.ThemLanKham("LK1", "BN1", 1767225600, "Dau");
    assert(SoBo.TimBenhNhanTheoMaLanKham("LK1") == BenhNhan);
    auto ChanLanMoi = [&] {
        PhaiLoi([&]{ SoBo.ThemLanKham("LK_MOI", "BN1", 1767225603, "Kiem tra"); },
                My_stdexcept::LoaiLoi::TrangThaiKhongHopLe);
        assert(SoBo.TimLanKham("LK_MOI") == nullptr);
    };
    ChanLanMoi(); // ChoKhamLanDau.
    SoBo.ThemLanKham("LK2", "BN2", 1767225600, "Khac"); // Người khác vẫn được tiếp nhận.
    PhaiLoi([&]{ SoBo.ThemLanKham("LK1", "BN2", 1767225600, "Ghi de"); },
            My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    LanDau->DanhGia(*BenhNhan, VitalSigns(75,120,16,98,15), 1767225601, "DD1");
    LanDau->BatDauKham("BS1");
    ChanLanMoi(); // DangKham.
    LanDau->ChuyenDiCanLamSang("Xet nghiem", "BS1");
    ChanLanMoi(); // ChoCanLamSang.
    LanDau->GhiNhanKetQua("On");
    LanDau->DanhDauQuayLai();
    ChanLanMoi(); // ChoKhamQuayLai.
    LanDau->BatDauKham("BS1");
    LanDau->KetThucLanKham();
    // Sao chép để xem lịch sử không được lấy mất dữ liệu của hồ sơ Registry.
    Visit BanChup(*LanDau);
    assert(BanChup.SoDanhGia() == 1 && LanDau->SoDanhGia() == 1);
    assert(BanChup.SoLanCanLamSang() == 1 && LanDau->SoLanCanLamSang() == 1);
    assert(LanDau->LayTrieuChungChinh() == "Dau");
    // Bên gọi xác nhận đúng người rồi dùng mã Patient tìm theo CCCD, không đề xuất mã mới.
    const Patient* TimDuoc = SoBo.TimBenhNhanTheoCanCuoc("001");
    Visit* LanHai = SoBo.ThemLanKham("LK3", TimDuoc->LayMaDinhDanh(), 1767312000, "Tai kham");
    LanHai->HuyLanKham();
    Visit* LanBa = SoBo.ThemLanKham("BN1", "BN1", 1767398400, "Tai kham"); // Hai không gian mã.
    assert(SoBo.SoBenhNhan() == 2 && SoBo.SoLanKham() == 4);
    assert(SoBo.TimBenhNhan("BN1") == BenhNhan && SoBo.TimLanKham("LK1") == LanDau);
    assert(SoBo.LayLichSuLanKham("BN1").KichThuoc() == 3);
    Vector<const Visit*> LichSu = ChiDoc.LayLichSuLanKham("BN1");
    bool CoLanDau = false, CoLanHai = false, CoLanBa = false;
    for (int I = 0; I < LichSu.KichThuoc(); ++I) {
        assert(LichSu[I]->LayMaBenhNhan() == "BN1");
        CoLanDau = CoLanDau || LichSu[I] == LanDau;
        CoLanHai = CoLanHai || LichSu[I] == LanHai;
        CoLanBa = CoLanBa || LichSu[I] == LanBa;
    }
    assert(CoLanDau && CoLanHai && CoLanBa);
    assert(ChiDoc.TimLanKham("LK1") == LanDau);
}

void KiemTraRehashVaVongDoi() {
    for (int Vong = 0; Vong < 3; ++Vong) {
        PatientRegistry SoBo;
        const Patient* Dau = SoBo.ThemBenhNhan("Goc");
        Visit* KhamDau = SoBo.ThemLanKham("Goc", "Goc", 1, "Trieu chung");
        for (int I = 0; I < 100; ++I) {
            const std::string Ma = "BN" + std::to_string(I);
            SoBo.ThemBenhNhan(Ma, "Ten " + Ma, Gender::Nam, "CCCD" + Ma);
            SoBo.ThemLanKham(Ma, Ma, 1, "Kiem tra");
        }
        assert(SoBo.SoBenhNhan() == 101 && SoBo.SoLanKham() == 101);
        assert(SoBo.TimBenhNhan("Goc") == Dau && SoBo.TimLanKham("Goc") == KhamDau);
        assert(SoBo.LayLichSuLanKham("Goc")[0] == KhamDau);
        for (int I = 0; I < 100; ++I) {
            const std::string Ma = "BN" + std::to_string(I);
            assert(SoBo.TimBenhNhanTheoCanCuoc("CCCD" + Ma) == SoBo.TimBenhNhan(Ma));
            assert(SoBo.TimBenhNhanTheoMaLanKham(Ma) == SoBo.TimBenhNhan(Ma));
        }
    } // ASan/LSan kiểm tra thu hồi toàn bộ Patient, Visit và nút bảng băm.
}

void KiemTraTraCuuKetHop() {
    PatientRegistry SoBo;
    assert(SoBo.TraCuuLanKham().Rong());
    const Patient* An = SoBo.ThemBenhNhan("BN1", u8"Nguyễn Văn An");
    SoBo.ThemBenhNhan("BN2", u8"Nguyễn Văn An"); // Cùng tên, khác người.
    SoBo.ThemBenhNhan("BN3", "Binh");
    SoBo.ThemBenhNhan("BN4"); // Có lần khám nhưng chưa biết tên.
    SoBo.ThemBenhNhan("BN5", "An chua kham"); // Chưa khám không có dòng kết quả.
    // Mốc 00:00 UTC ngày 01/01/2026; Việt Nam sang ngày mới từ 17:00 UTC hôm trước.
    const std::time_t Moc = 1767225600;
    Visit* Cu = SoBo.ThemLanKham("LK1", "BN1", Moc - 25201, "Dau");
    Cu->HuyLanKham();
    const Visit* Moi = SoBo.ThemLanKham("LK2", "BN1", Moc - 25200, "Tai kham");
    SoBo.ThemLanKham("LK3", "BN2", Moc - 25200, "Kham");
    SoBo.ThemLanKham("LK4", "BN3", Moc + 3600, "Kham");
    SoBo.ThemLanKham("LK5", "BN4", Moc + 86400, "Kham");

    const PatientRegistry& ChiDoc = SoBo;
    Vector<KetQuaTraCuuLanKham> TatCa = ChiDoc.TraCuuLanKham();
    assert(TatCa.KichThuoc() == 5);
    const char* ThuTu[] = {"LK5", "LK4", "LK2", "LK3", "LK1"};
    for (int I = 0; I < 5; ++I) {
        assert(TatCa[I].LanKham->LayMaLanKham() == ThuTu[I]);
        assert(TatCa[I].BenhNhan->LayMaDinhDanh() == TatCa[I].LanKham->LayMaBenhNhan());
    }
    Vector<KetQuaTraCuuLanKham> TheoTen = SoBo.TraCuuLanKham(u8"NGUYỄN vĂn");
    assert(TheoTen.KichThuoc() == 3);
    assert(TheoTen[0].BenhNhan == An && TheoTen[0].LanKham == Moi);
    assert(TheoTen[2].LanKham == Cu); // Giữ cả lần khám đã hủy trong lịch sử.
    const Date Ngay{1, 1, 2026}, HomTruoc{31, 12, 2025}, HomSau{2, 1, 2026};
    assert(SoBo.TraCuuLanKham("", &Ngay).KichThuoc() == 3);
    assert(SoBo.TraCuuLanKham("An", &Ngay).KichThuoc() == 2);
    assert(SoBo.TraCuuLanKham("An", &HomTruoc)[0].LanKham == Cu);
    assert(SoBo.TraCuuLanKham("An", &HomSau).Rong());
    assert(SoBo.TraCuuLanKham("", &HomSau)[0].BenhNhan->LaBenhNhanVoDanh());
    assert(SoBo.TraCuuLanKham("An", &Ngay, "LK2")[0].LanKham == Moi);
    assert(SoBo.TraCuuLanKham("Binh", &Ngay, "LK2").Rong());
    assert(SoBo.TraCuuLanKham("An", &HomTruoc, "LK2").Rong());
    assert(SoBo.TraCuuLanKham("", nullptr, "LK2")[0].BenhNhan == An);
    assert(SoBo.TraCuuLanKham("", nullptr, "LK").Rong()); // Mã khớp chính xác.
    assert(SoBo.TraCuuLanKham("khong ton tai").Rong());
    const Date NgaySai{29, 2, 2026};
    PhaiLoi([&]{ SoBo.TraCuuLanKham("", &NgaySai); }, My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    // Cập nhật tên phải phản ánh ngay trong lần tra cứu tiếp theo.
    SoBo.BoSungDanhTinh("BN1", "", "Ten moi", Gender::Nam, Date{1,1,2000});
    assert(SoBo.TraCuuLanKham("An").KichThuoc() == 1);
    assert(SoBo.TraCuuLanKham("Ten moi").KichThuoc() == 2);
}

void KiemTraNgayLocChuyenThang() {
    const std::time_t CacMoc[] = {1709139600, 1709226000, 1769878800};
    const Date NgaySau[] = {{29,2,2024}, {1,3,2024}, {1,2,2026}};
    const Date NgayTruoc[] = {{28,2,2024}, {29,2,2024}, {31,1,2026}};
    for (int I = 0; I < 3; ++I) {
        PatientRegistry SoBo;
        SoBo.ThemBenhNhan("BN1");
        SoBo.ThemBenhNhan("BN2");
        // Hai lần tiếp nhận cách một giây nhưng thuộc hai ngày Việt Nam khác nhau.
        SoBo.ThemLanKham("TRUOC", "BN1", CacMoc[I] - 1, "Kham");
        SoBo.ThemLanKham("SAU", "BN2", CacMoc[I], "Kham");
        assert(SoBo.TraCuuLanKham("", &NgaySau[I]).KichThuoc() == 1);
        assert(SoBo.TraCuuLanKham("", &NgaySau[I])[0].LanKham->LayMaLanKham() == "SAU");
        assert(SoBo.TraCuuLanKham("", &NgayTruoc[I]).KichThuoc() == 1);
        assert(SoBo.TraCuuLanKham("", &NgayTruoc[I])[0].LanKham->LayMaLanKham() == "TRUOC");
    }
}

void KiemTraDanhSachBenhNhan() {
    PatientRegistry R;
    assert(R.LayDanhSachBenhNhan().KichThuoc() == 0);
    const Patient* A = R.ThemBenhNhan("A");
    auto DanhSach = R.LayDanhSachBenhNhan();
    assert(DanhSach.KichThuoc() == 1 && DanhSach[0] == A);
    R.ThemBenhNhan("B");
    assert(DanhSach.KichThuoc() == 1);
    assert(R.LayDanhSachBenhNhan().KichThuoc() == 2);
    assert(R.SoLanKham() == 0);
}

void KiemTraBoSungThongTinConThieu() {
    PatientRegistry R;
    Patient Goc("A","Tên đã biết",Gender::Nam); Goc.DatTuoiUocLuong(0);
    Goc.CapNhatNhomMau(BloodGroup::O); Goc.CapNhatThongTinDiUng("Penicillin");
    const auto* Cu=R.ThemBenhNhan(Goc);
    R.ThemBenhNhan("B","Người khác",Gender::Nu,"999");
    Patient Trung("A","Tên khác",Gender::Nu,Date{1,1,2000},"999"); Trung.CapNhatTienSuBenh("Hen");
    PhaiLoi([&] { R.BoSungThongTinConThieu("A",Trung); },My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    assert(Cu->LaySoCanCuoc().empty() && !Cu->CoNgaySinh() && Cu->LayTienSuBenh().empty());
    Patient Them("A","Tên khác",Gender::Nu,Date{1,1,2000},"001");
    Them.DatTuoiUocLuong(50); Them.CapNhatNhomMau(BloodGroup::AB);
    Them.CapNhatThongTinDiUng("Khác"); Them.CapNhatTienSuBenh("Hen");
    R.BoSungThongTinConThieu("A",Them);
    assert(R.TimBenhNhan("A")==Cu && R.TimBenhNhanTheoCanCuoc("001")==Cu);
    assert(Cu->LayHoTen()=="Tên đã biết" && Cu->LayGioiTinh()==Gender::Nam);
    assert(Cu->LayNgaySinh().Nam==2000 && Cu->LayTuoiUocLuong()==0);
    assert(Cu->LayNhomMau()==BloodGroup::O && Cu->LayThongTinDiUng()=="Penicillin");
    assert(Cu->LayTienSuBenh()=="Hen");
    // Nội dung đã có vẫn giữ nguyên, kể cả khi nguồn gửi lại khác đi.
    R.BoSungThongTinConThieu("A",Trung);
    assert(Cu->LaySoCanCuoc()=="001");
    PhaiLoi([&] { R.BoSungThongTinConThieu("A",Patient("SAI")); },My_stdexcept::LoaiLoi::DoiSoKhongHopLe);
    R.ThemLanKham("V","A",1,"Khám");
    PhaiLoi([&] { R.BoSungThongTinConThieu("A",Them); },My_stdexcept::LoaiLoi::TrangThaiKhongHopLe);
    const auto* VoDanh=R.ThemBenhNhan("C");
    Patient MotPhan("C","Ngọc",Gender::Nu); MotPhan.DatTuoiUocLuong(41);
    R.BoSungThongTinConThieu("C",MotPhan);
    assert(R.TimBenhNhan("C")==VoDanh && !VoDanh->LaBenhNhanVoDanh());
    assert(VoDanh->LayHoTen()=="Ngọc" && VoDanh->LayTuoiUocLuong()==41 && !VoDanh->CoNgaySinh());
}

int main() {
    {
        PatientRegistry TuDong;
        assert(TuDong.TaoMaBenhNhan() == "BN001");
        assert(TuDong.TaoMaLanKham() == "LK001");
        TuDong.ThemBenhNhan("BN009"); TuDong.ThemBenhNhan("MaCu");
        assert(TuDong.TaoMaBenhNhan() == "BN010");
        TuDong.ThemBenhNhan("BN999");
        assert(TuDong.TaoMaBenhNhan() == "BN1000");
        TuDong.ThemLanKham("LK099", "MaCu", 1, "Kham");
        assert(TuDong.TaoMaLanKham() == "LK100");
        assert(TuDong.TaoMaLanKham() == "LK100");
        TuDong.ThemBenhNhan("BN18446744073709551615");
        bool HetMa = false;
        try { TuDong.TaoMaBenhNhan(); } catch (const My_stdexcept&) { HetMa = true; }
        assert(HetMa);
    }

    KiemTraBoSungThongTinConThieu();
    KiemTraDanhSachBenhNhan();
    KiemTraHoSoVaDanhTinh();
    KiemTraTimTen();
    KiemTraLanKhamVaTaiSuDung();
    KiemTraRehashVaVongDoi();
    KiemTraTraCuuKetHop();
    KiemTraNgayLocChuyenThang();
}
