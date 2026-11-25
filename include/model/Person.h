#pragma once

#include <string>
#include "model/Date.h"
#include "model/Gender.h"



class Person {
private:
    const std::string MaDinhDanh;
    std::string HoTen;
    Gender GioiTinh;
    Date NgaySinh;
    bool DaBietNgaySinh;

    static bool LaKhoangTrang(char KyTu);
    static void KiemTraMaDinhDanh(const std::string& MaDinhDanh);
    static void KiemTraThongTinCaNhan(const std::string& HoTen, Gender GioiTinh);
    static int TinhTuoiTheoNgay(const Date& NgaySinh, const Date& NgayXet);
    static void KiemTraNgaySinh(const Date& NgaySinh);

protected:

    void HoanDoiThongTin(Person& Khac) noexcept;
public:

    explicit Person(const std::string& MaDinhDanh,
                    const std::string& HoTen = "",
                    Gender GioiTinh = Gender::ChuaXacDinh);

    Person(const std::string& MaDinhDanh, const std::string& HoTen,
           Gender GioiTinh, const Date& NgaySinh);


    virtual ~Person() = default;

    const std::string& LayMaDinhDanh() const;
    const std::string& LayHoTen() const;
    Gender LayGioiTinh() const;
    bool CoNgaySinh() const;


    Date LayNgaySinh() const;
    int TinhTuoi(const Date& NgayXet) const;


    void CapNhatThongTinCaNhan(const std::string& HoTen, Gender GioiTinh);
    void CapNhatThongTinCaNhan(const std::string& HoTen, Gender GioiTinh,
                              const Date& NgaySinh);


    void XoaNgaySinh();
};
