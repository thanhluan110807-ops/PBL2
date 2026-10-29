#include "model/staff.h"
#include <iostream>
#include <sstream>
#include <utility>   // std::move

/* ============================================================
 *  STAFF.CPP - Cài đặt cho staff.h
 * ============================================================ */

// ------------------------------------------------------------
//  Capbac <-> string
// ------------------------------------------------------------
std::string Capbactostring(Capbac capbac) {
    switch (capbac) {
        case Capbac::Dieu_duong_tre:   return "Dieu_duong_tre";
        case Capbac::Dieu_duong_chinh: return "Dieu_duong_chinh";
        case Capbac::Bac_si_noi_tru:   return "Bac_si_noi_tru";
        case Capbac::Bac_si_chinh:     return "Bac_si_chinh";
        case Capbac::Truong_khoa:      return "Truong_khoa";
    }
    return "Khong_xac_dinh";
}

Capbac stringtoCapbac(const std::string& str) {
    if (str == "Dieu_duong_tre")   return Capbac::Dieu_duong_tre;
    if (str == "Dieu_duong_chinh") return Capbac::Dieu_duong_chinh;
    if (str == "Bac_si_noi_tru")   return Capbac::Bac_si_noi_tru;
    if (str == "Bac_si_chinh")     return Capbac::Bac_si_chinh;
    if (str == "Truong_khoa")      return Capbac::Truong_khoa;
    return Capbac::Dieu_duong_tre;   // giá trị mặc định an toàn
}

// ------------------------------------------------------------
//  Catrucmongmuon
// ------------------------------------------------------------
Catrucmongmuon::Catrucmongmuon(int Giobatdau_, int Gioketthuc_, bool Cuoituan)
    : Giobatdau(Giobatdau_), Gioketthuc(Gioketthuc_), Truccuoituan(Cuoituan) {}

// ============================================================
//  Nhanvienyte (lớp cơ sở)
// ============================================================
Nhanvienyte::Nhanvienyte(const std::string maNV,
                         const std::string Ten,
                         Capbac capbacInit,
                         int Namkinhnghiem,
                         const Catrucmongmuon& catruc)
    : Manhanvien(maNV), Hoten(Ten), capbac(capbacInit),
      Sonamkinhnghiem(Namkinhnghiem), catrucmongmuon(catruc),
      FatigueScore(0.0), WeeklyHours(0.0),
      Socatrucdemthang(0), Socatruccuoituanthang(0),
      Vuatrucdem(false) {}

// Copy constructor
Nhanvienyte::Nhanvienyte(const Nhanvienyte& other)
    : Manhanvien(other.Manhanvien), Hoten(other.Hoten), capbac(other.capbac),
      Sonamkinhnghiem(other.Sonamkinhnghiem), catrucmongmuon(other.catrucmongmuon),
      FatigueScore(other.FatigueScore), WeeklyHours(other.WeeklyHours),
      Socatrucdemthang(other.Socatrucdemthang),
      Socatruccuoituanthang(other.Socatruccuoituanthang),
      Vuatrucdem(other.Vuatrucdem) {}

// Copy assignment
Nhanvienyte& Nhanvienyte::operator=(const Nhanvienyte& other) {
    if (this == &other) return *this;   // chống tự gán
    Manhanvien = other.Manhanvien;
    Hoten = other.Hoten;
    capbac = other.capbac;
    Sonamkinhnghiem = other.Sonamkinhnghiem;
    catrucmongmuon = other.catrucmongmuon;
    FatigueScore = other.FatigueScore;
    WeeklyHours = other.WeeklyHours;
    Socatrucdemthang = other.Socatrucdemthang;
    Socatruccuoituanthang = other.Socatruccuoituanthang;
    Vuatrucdem = other.Vuatrucdem;
    return *this;
}

// Move constructor
Nhanvienyte::Nhanvienyte(Nhanvienyte&& other) noexcept
    : Manhanvien(std::move(other.Manhanvien)), Hoten(std::move(other.Hoten)),
      capbac(other.capbac), Sonamkinhnghiem(other.Sonamkinhnghiem),
      catrucmongmuon(other.catrucmongmuon), FatigueScore(other.FatigueScore),
      WeeklyHours(other.WeeklyHours), Socatrucdemthang(other.Socatrucdemthang),
      Socatruccuoituanthang(other.Socatruccuoituanthang),
      Vuatrucdem(other.Vuatrucdem) {}

// Move assignment
Nhanvienyte& Nhanvienyte::operator=(Nhanvienyte&& other) noexcept {
    if (this == &other) return *this;
    Manhanvien = std::move(other.Manhanvien);
    Hoten = std::move(other.Hoten);
    capbac = other.capbac;
    Sonamkinhnghiem = other.Sonamkinhnghiem;
    catrucmongmuon = other.catrucmongmuon;
    FatigueScore = other.FatigueScore;
    WeeklyHours = other.WeeklyHours;
    Socatrucdemthang = other.Socatrucdemthang;
    Socatruccuoituanthang = other.Socatruccuoituanthang;
    Vuatrucdem = other.Vuatrucdem;
    return *this;
}

// Destructor: lớp này không tự cấp phát bộ nhớ động.
// Vẫn để virtual để xóa qua con trỏ lớp cha (Nhanvienyte*) vẫn hủy đúng lớp con.
Nhanvienyte::~Nhanvienyte() {}

// --------------------- Getter / Setter ---------------------
std::string Nhanvienyte::Laymanhanvien() const { return Manhanvien; }
std::string Nhanvienyte::LayHoten() const { return Hoten; }
void Nhanvienyte::Dathoten(const std::string& Ten) { Hoten = Ten; }

Capbac Nhanvienyte::LayCapbac() const { return capbac; }
void Nhanvienyte::Datcapbac(Capbac capbacmoi) { capbac = capbacmoi; }

int Nhanvienyte::Laysonamkinhnghiem() const { return Sonamkinhnghiem; }
void Nhanvienyte::Datsonamkinhnghiem(int Sonam) {
    if (Sonam >= 0) Sonamkinhnghiem = Sonam;   // không cho số năm âm
}

Catrucmongmuon Nhanvienyte::Laycatrucmongmuon() const { return catrucmongmuon; }
void Nhanvienyte::DatCatrucmongmuon(const Catrucmongmuon& ca) { catrucmongmuon = ca; }

double Nhanvienyte::LayFatigueScore() const { return FatigueScore; }
double Nhanvienyte::LayWeeklyHours() const { return WeeklyHours; }
int Nhanvienyte::LaySocatrucdemthang() const { return Socatrucdemthang; }
int Nhanvienyte::LaySocatruccuoituanthang() const { return Socatruccuoituanthang; }
bool Nhanvienyte::Davuatrucdem() const { return Vuatrucdem; }

// --------------------- Nghiệp vụ chung ---------------------
void Nhanvienyte::CongDonGioLam(double Sogio) {
    if (Sogio > 0) WeeklyHours += Sogio;   // chặn giá trị âm
}

void Nhanvienyte::Ghinhancatruc(bool lacadem, bool lacuoituan) {
    if (lacadem) Socatrucdemthang++;
    if (lacuoituan) Socatruccuoituanthang++;
    Vuatrucdem = lacadem;   // ghi nhớ ca gần nhất có phải ca đêm không
}

void Nhanvienyte::Datlaituanmoi() {
    WeeklyHours = 0.0;
}

// Ràng buộc: cấm xếp ca ngày ngay sau ca đêm (nghỉ ít nhất 24h)
bool Nhanvienyte::Cannghinghoibatbuoc() const {
    return Vuatrucdem;
}

// Điều kiện cắt nhánh (pruning) cho Backtracking
bool Nhanvienyte::Vuotnguongmetmoi(double nguongtoida) const {
    return FatigueScore > nguongtoida;
}

// --------------------- Hàm ảo mặc định ---------------------
void Nhanvienyte::Hienthithongtin() const {
    std::cout << "[" << Layloainhanvien() << "] "
              << "Ma NV: " << Manhanvien
              << " | Ho ten: " << Hoten
              << " | Cap bac: " << Capbactostring(capbac)
              << " | KN: " << Sonamkinhnghiem << " nam"
              << " | Fatigue: " << FatigueScore
              << " | Gio/tuan: " << WeeklyHours
              << std::endl;
}

// Định dạng: MaNV;HoTen;CapBac;KinhNghiem;Fatigue;WeeklyHours;CaDem;CaCuoiTuan;VuaTrucDem
std::string Nhanvienyte::Xuatdongfile() const {
    std::ostringstream oss;
    oss << Manhanvien << ";" << Hoten << ";" << Capbactostring(capbac) << ";"
        << Sonamkinhnghiem << ";" << FatigueScore << ";" << WeeklyHours << ";"
        << Socatrucdemthang << ";" << Socatruccuoituanthang << ";"
        << (Vuatrucdem ? 1 : 0);
    return oss.str();
}

// ============================================================
//  Bacsi
// ============================================================
Bacsi::Bacsi(const std::string& maNV, const std::string& Ten,
             Capbac capbacInit, int Namkinhnghiem,
             const std::string& ChuyenKhoaInit, bool Noitru,
             const Catrucmongmuon& catruc)
    : Nhanvienyte(maNV, Ten, capbacInit, Namkinhnghiem, catruc),
      ChuyenKhoa(ChuyenKhoaInit), Labacsinoitru(Noitru) {}

Bacsi::Bacsi(const Bacsi& other)
    : Nhanvienyte(other), ChuyenKhoa(other.ChuyenKhoa),
      Labacsinoitru(other.Labacsinoitru) {}

Bacsi& Bacsi::operator=(const Bacsi& other) {
    if (this == &other) return *this;
    Nhanvienyte::operator=(other);   // gán phần lớp cha trước
    ChuyenKhoa = other.ChuyenKhoa;
    Labacsinoitru = other.Labacsinoitru;
    return *this;
}

Bacsi::Bacsi(Bacsi&& other) noexcept
    : Nhanvienyte(std::move(other)), ChuyenKhoa(std::move(other.ChuyenKhoa)),
      Labacsinoitru(other.Labacsinoitru) {}

Bacsi& Bacsi::operator=(Bacsi&& other) noexcept {
    if (this == &other) return *this;
    Nhanvienyte::operator=(std::move(other));
    ChuyenKhoa = std::move(other.ChuyenKhoa);
    Labacsinoitru = other.Labacsinoitru;
    return *this;
}

Bacsi::~Bacsi() {}

std::string Bacsi::Laychuyenkhoa() const { return ChuyenKhoa; }
void Bacsi::Datchuyenkhoa(const std::string& ck) { ChuyenKhoa = ck; }
bool Bacsi::Kiemtranoitru() const { return Labacsinoitru; }

// Chỉ Bác sĩ chính hoặc Trưởng khoa (không phải nội trú) mới làm Trưởng ca
bool Bacsi::Cothelamtruongca() const {
    return !Labacsinoitru &&
           (capbac == Capbac::Bac_si_chinh || capbac == Capbac::Truong_khoa);
}

// Công thức ví dụ: trọng số cao cho ca đêm và giờ làm; nội trú dễ quá tải hơn.
// Có thể chỉnh lại hệ số theo số liệu MBI thực tế của nhóm.
void Bacsi::CapnhatFatigueScore(double Hesoapluc) {
    double diem = Hesoapluc * (Socatrucdemthang * 1.5 + WeeklyHours * 0.12);
    if (Labacsinoitru) diem *= 1.2;
    FatigueScore += diem;
}

std::string Bacsi::Layloainhanvien() const { return "Bacsi"; }

void Bacsi::Hienthithongtin() const {
    Nhanvienyte::Hienthithongtin();   // in phần chung trước
    std::cout << "    -> Chuyen khoa: " << ChuyenKhoa
              << " | Noi tru: " << (Labacsinoitru ? "Co" : "Khong")
              << " | Du dieu kien Truong ca: " << (Cothelamtruongca() ? "Co" : "Khong")
              << std::endl;
}

std::string Bacsi::Xuatdongfile() const {
    std::ostringstream oss;
    oss << Nhanvienyte::Xuatdongfile() << ";" << ChuyenKhoa << ";"
        << (Labacsinoitru ? 1 : 0);
    return oss.str();
}

// ============================================================
//  Dieuduong
// ============================================================
Dieuduong::Dieuduong(const std::string& maNV, const std::string& Ten,
                     Capbac capbacInit, int Namkinhnghiem,
                     const Catrucmongmuon& catruc)
    : Nhanvienyte(maNV, Ten, capbacInit, Namkinhnghiem, catruc),
      Dangtrucsangloc(false), Sogiotrucsangloclientuc(0.0) {}

Dieuduong::Dieuduong(const Dieuduong& other)
    : Nhanvienyte(other), Dangtrucsangloc(other.Dangtrucsangloc),
      Sogiotrucsangloclientuc(other.Sogiotrucsangloclientuc) {}

Dieuduong& Dieuduong::operator=(const Dieuduong& other) {
    if (this == &other) return *this;
    Nhanvienyte::operator=(other);
    Dangtrucsangloc = other.Dangtrucsangloc;
    Sogiotrucsangloclientuc = other.Sogiotrucsangloclientuc;
    return *this;
}

Dieuduong::Dieuduong(Dieuduong&& other) noexcept
    : Nhanvienyte(std::move(other)), Dangtrucsangloc(other.Dangtrucsangloc),
      Sogiotrucsangloclientuc(other.Sogiotrucsangloclientuc) {}

Dieuduong& Dieuduong::operator=(Dieuduong&& other) noexcept {
    if (this == &other) return *this;
    Nhanvienyte::operator=(std::move(other));
    Dangtrucsangloc = other.Dangtrucsangloc;
    Sogiotrucsangloclientuc = other.Sogiotrucsangloclientuc;
    return *this;
}

Dieuduong::~Dieuduong() {}

bool Dieuduong::Kiemtradangtrucsangloc() const { return Dangtrucsangloc; }

void Dieuduong::Batdautrucsangloc() {
    Dangtrucsangloc = true;
    Sogiotrucsangloclientuc = 0.0;
}

void Dieuduong::Ketthuctrucsangloc() {
    Dangtrucsangloc = false;
    Sogiotrucsangloclientuc = 0.0;
}

// Chỉ cộng giờ khi đang trực sàng lọc
void Dieuduong::Congdongiotrucsanglong(double sogio) {
    if (Dangtrucsangloc && sogio > 0) Sogiotrucsangloclientuc += sogio;
}

double Dieuduong::Laysogiotrucsangloclentuc() const { return Sogiotrucsangloclientuc; }

// Xoay tua sàng lọc khi đã trực liên tục từ 4 giờ trở lên
bool Dieuduong::Canxoaytuasangloc() const {
    return Dangtrucsangloc && Sogiotrucsangloclientuc >= 4.0;
}

// Theo đề tài: Điều dưỡng không làm Trưởng ca
bool Dieuduong::Cothelamtruongca() const { return false; }

// Điều dưỡng trực sàng lọc lâu thì mệt mỏi nhận thức tăng nhanh hơn
void Dieuduong::CapnhatFatigueScore(double Hesoapluc) {
    double diem = Hesoapluc * (WeeklyHours * 0.15);
    if (Dangtrucsangloc) diem += Hesoapluc * Sogiotrucsangloclientuc * 0.8;
    FatigueScore += diem;
}

std::string Dieuduong::Layloainhanvien() const { return "Dieuduong"; }

void Dieuduong::Hienthithongtin() const {
    Nhanvienyte::Hienthithongtin();
    std::cout << "    -> Dang truc sang loc: " << (Dangtrucsangloc ? "Co" : "Khong")
              << " | Gio truc sang loc lien tuc: " << Sogiotrucsangloclientuc
              << " | Can xoay tua: " << (Canxoaytuasangloc() ? "Co" : "Khong")
              << std::endl;
}

std::string Dieuduong::Xuatdongfile() const {
    std::ostringstream oss;
    oss << Nhanvienyte::Xuatdongfile() << ";" << (Dangtrucsangloc ? 1 : 0) << ";"
        << Sogiotrucsangloclientuc;
    return oss.str();
}