#pragma once

#include <string>
#include "model/Date.h"
#include "model/Gender.h"

// Thông tin hành chính chung của bệnh nhân và nhân viên.
// Model chỉ quản lý dữ liệu; việc hiển thị thuộc giao diện Qt.
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

public:
    // Mã luôn bắt buộc, kể cả khi người bệnh chưa rõ danh tính.
    explicit Person(const std::string& MaDinhDanh,
                    const std::string& HoTen = "",
                    Gender GioiTinh = Gender::ChuaXacDinh);

    Person(const std::string& MaDinhDanh, const std::string& HoTen,
           Gender GioiTinh, const Date& NgaySinh);

    // Bảo đảm hủy đúng lớp con khi dùng qua con trỏ Person.
    virtual ~Person() = default;

    const std::string& LayMaDinhDanh() const;
    const std::string& LayHoTen() const;
    Gender LayGioiTinh() const;
    bool CoNgaySinh() const;

    // Báo My_stdexcept (TrangThaiKhongHopLe) nếu chưa biết ngày sinh; có thể kiểm tra CoNgaySinh trước.
    Date LayNgaySinh() const;
    int TinhTuoi(const Date& NgayXet) const;

    // Bản không nhận ngày sinh giữ nguyên thông tin ngày sinh đang có.
    void CapNhatThongTinCaNhan(const std::string& HoTen, Gender GioiTinh);
    void CapNhatThongTinCaNhan(const std::string& HoTen, Gender GioiTinh,
                              const Date& NgaySinh);

    // Dùng khi xác định lại rằng ngày sinh cũ chưa đủ tin cậy.
    void XoaNgaySinh();
};
