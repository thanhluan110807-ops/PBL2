#include "model/Person.h"
#include <cassert>
#include <iostream>
#include "util/My_stdexcept.h"

// Hàm kiểm tra phải nhận đúng loại lỗi, không chấp nhận thành công giả.
template <typename HamKiemTra>
void PhaiBaoLoi(My_stdexcept::LoaiLoi LoaiMongDoi, HamKiemTra Ham) {
    bool DaBaoLoi = false;
    try {
        Ham();
    } catch (const My_stdexcept& Loi) {
        DaBaoLoi = Loi.LayLoaiLoi() == LoaiMongDoi;
    }
    assert(DaBaoLoi);
}

class PersonThu : public Person {
private:
    bool& DaHuy;
public:
    explicit PersonThu(bool& CoHuy) : Person("TEST"), DaHuy(CoHuy) {}
    ~PersonThu() override { DaHuy = true; }
};

int main() {
    Person VoDanh("EMG001");
    assert(VoDanh.LayMaDinhDanh() == "EMG001");
    assert(VoDanh.LayHoTen().empty());
    assert(VoDanh.LayGioiTinh() == Gender::ChuaXacDinh);
    assert(!VoDanh.CoNgaySinh());
    PhaiBaoLoi(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, [&] { VoDanh.LayNgaySinh(); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, [&] { VoDanh.TinhTuoi(Date{1, 1, 2026}); });

    const Person Nguoi("BN001", "Nguyen Van A", Gender::Nam, Date{10, 5, 2000});
    assert(Nguoi.CoNgaySinh());
    assert(Nguoi.TinhTuoi(Date{9, 5, 2026}) == 25);
    assert(Nguoi.TinhTuoi(Date{10, 5, 2026}) == 26);
    assert(Nguoi.TinhTuoi(Date{11, 5, 2026}) == 26);
    Date BanSaoNgaySinh = Nguoi.LayNgaySinh();
    BanSaoNgaySinh.Nam = 1900;
    assert(Nguoi.LayNgaySinh().Nam == 2000 && BanSaoNgaySinh.Nam == 1900);

    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi(""); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi("BN 001"); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi("BN\t001"); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi("P", " \t\n"); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi("P", "A", static_cast<Gender>(99)); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi("P", "A", Gender::Nam, Date{29, 2, 2023}); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [] { Person Loi("P", "A", Gender::Nam, Date{1, 1, 9999}); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [&] { Nguoi.TinhTuoi(Date{31, 2, 2026}); });
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [&] { Nguoi.TinhTuoi(Date{9, 5, 2000}); });

    const Date HomNay = Date::HomNay();
    Person SoSinh("NEWBORN", "", Gender::ChuaXacDinh, HomNay);
    assert(SoSinh.TinhTuoi(HomNay) == 0);
    Person GioiHan("AGE150", "A", Gender::Nu, Date{1, 1, HomNay.Nam - 150});
    assert(GioiHan.TinhTuoi(HomNay) == 150);
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [&] {
        Person Loi("OLD", "A", Gender::Nam, Date{1, 1, HomNay.Nam - 151});
    });

    VoDanh.CapNhatThongTinCaNhan("Tran Van B", Gender::Nam, Date{29, 2, 2000});
    assert(VoDanh.LayMaDinhDanh() == "EMG001" && VoDanh.CoNgaySinh());
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [&] {
        VoDanh.CapNhatThongTinCaNhan("Ten Moi", Gender::Nu, Date{31, 2, 2000});
    });
    assert(VoDanh.LayHoTen() == "Tran Van B" && VoDanh.LayGioiTinh() == Gender::Nam);
    assert(VoDanh.LayNgaySinh().Ngay == 29);
    PhaiBaoLoi(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, [&] {
        VoDanh.CapNhatThongTinCaNhan("", static_cast<Gender>(99));
    });
    assert(VoDanh.LayHoTen() == "Tran Van B");
    VoDanh.CapNhatThongTinCaNhan("Ten Da Sua", Gender::Khac);
    assert(VoDanh.LayNgaySinh().Nam == 2000);
    VoDanh.XoaNgaySinh();
    assert(!VoDanh.CoNgaySinh());
    PhaiBaoLoi(My_stdexcept::LoaiLoi::TrangThaiKhongHopLe, [&] { VoDanh.LayNgaySinh(); });

    // Cấp phát/xóa ở đây chỉ để kiểm tra destructor ảo qua con trỏ lớp cha.
    bool DaHuy = false;
    Person* ConTro = new PersonThu(DaHuy);
    delete ConTro;
    assert(DaHuy);
    std::cout << "PASS: Person - construction, validation, age, updates, identity, virtual destructor\n";
}
