#include "model/Person.h"

// Ngoại lệ giúp GUI nhận biết dữ liệu sai mà model không tự in thông báo.
#include "util/My_stdexcept.h"

Person::Person(const std::string& MaDinhDanh, const std::string& HoTen,
               Gender GioiTinh)
    : MaDinhDanh(MaDinhDanh), HoTen(HoTen), GioiTinh(GioiTinh),
      NgaySinh{0, 0, 0}, DaBietNgaySinh(false) {
    KiemTraMaDinhDanh(MaDinhDanh);
    KiemTraThongTinCaNhan(HoTen, GioiTinh);
}

Person::Person(const std::string& MaDinhDanh, const std::string& HoTen,
               Gender GioiTinh, const Date& NgaySinh)
    : Person(MaDinhDanh, HoTen, GioiTinh) {
    KiemTraNgaySinh(NgaySinh);
    this->NgaySinh = NgaySinh;
    DaBietNgaySinh = true;
}

bool Person::LaKhoangTrang(char KyTu) {
    return KyTu == ' ' || KyTu == '\t' || KyTu == '\n' ||
           KyTu == '\r' || KyTu == '\f' || KyTu == '\v';
}

void Person::KiemTraMaDinhDanh(const std::string& MaDinhDanh) {
    if (MaDinhDanh.empty()) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ma dinh danh khong duoc rong");
    }
    for (char KyTu : MaDinhDanh) {
        if (LaKhoangTrang(KyTu)) {
            throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ma dinh danh khong duoc chua khoang trang");
        }
    }
}

void Person::KiemTraThongTinCaNhan(const std::string& HoTen, Gender GioiTinh) {
    // Tên rỗng nghĩa là chưa biết; tên chỉ gồm khoảng trắng là lỗi nhập.
    bool ChiCoKhoangTrang = !HoTen.empty();
    for (char KyTu : HoTen) {
        if (!LaKhoangTrang(KyTu)) {
            ChiCoKhoangTrang = false;
            break;
        }
    }
    if (ChiCoKhoangTrang) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ho ten khong duoc chi chua khoang trang");
    }
    switch (GioiTinh) {
        case Gender::Nam:
        case Gender::Nu:
        case Gender::Khac:
        case Gender::ChuaXacDinh:
            return;
    }
    throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Gioi tinh khong hop le");
}

int Person::TinhTuoiTheoNgay(const Date& NgaySinh, const Date& NgayXet) {
    int Tuoi = NgayXet.Nam - NgaySinh.Nam;
    // Chưa tới sinh nhật trong năm đang xét thì chưa tăng tuổi.
    if (NgayXet.Thang < NgaySinh.Thang ||
        (NgayXet.Thang == NgaySinh.Thang && NgayXet.Ngay < NgaySinh.Ngay)) {
        --Tuoi;
    }
    return Tuoi;
}

void Person::KiemTraNgaySinh(const Date& NgaySinh) {
    if (!NgaySinh.HopLe()) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay sinh khong hop le");
    }
    const Date NgayHienTai = Date::HomNay();
    if (NgayHienTai.Truoc(NgaySinh)) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay sinh khong duoc nam trong tuong lai");
    }
    // Giới hạn dữ liệu mô phỏng đã chọn cho đồ án, không phải ngưỡng lâm sàng.
    if (TinhTuoiTheoNgay(NgaySinh, NgayHienTai) > 150) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Tuoi nhap vao khong duoc vuot qua 150");
    }
}

const std::string& Person::LayMaDinhDanh() const { return MaDinhDanh; }
const std::string& Person::LayHoTen() const { return HoTen; }
Gender Person::LayGioiTinh() const { return GioiTinh; }
bool Person::CoNgaySinh() const { return DaBietNgaySinh; }

Date Person::LayNgaySinh() const {
    if (!DaBietNgaySinh) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, "Chua biet ngay sinh");
    }
    // Trả bản sao để bên gọi không sửa trực tiếp ngày sinh bên trong đối tượng.
    return NgaySinh;
}

int Person::TinhTuoi(const Date& NgayXet) const {
    if (!NgayXet.HopLe()) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay tinh tuoi khong hop le");
    }
    const Date NgaySinhDaBiet = LayNgaySinh();
    if (NgayXet.Truoc(NgaySinhDaBiet)) {
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Ngay tinh tuoi khong duoc truoc ngay sinh");
    }
    return TinhTuoiTheoNgay(NgaySinhDaBiet, NgayXet);
}

void Person::CapNhatThongTinCaNhan(const std::string& HoTen, Gender GioiTinh) {
    KiemTraThongTinCaNhan(HoTen, GioiTinh);
    this->HoTen = HoTen;
    this->GioiTinh = GioiTinh;
}

void Person::CapNhatThongTinCaNhan(const std::string& HoTen, Gender GioiTinh,
                                  const Date& NgaySinh) {
    // Kiểm tra tất cả trước khi cập nhật để dữ liệu sai không đổi một phần hồ sơ.
    KiemTraThongTinCaNhan(HoTen, GioiTinh);
    KiemTraNgaySinh(NgaySinh);
    this->HoTen = HoTen;
    this->GioiTinh = GioiTinh;
    this->NgaySinh = NgaySinh;
    DaBietNgaySinh = true;
}

void Person::XoaNgaySinh() {
    DaBietNgaySinh = false;
    NgaySinh = {0, 0, 0}; // Giá trị nội bộ; không trả ra khi chưa có ngày sinh.
}
