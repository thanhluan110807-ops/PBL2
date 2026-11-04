#pragma once
#include "model/NhanVienYTe.h"
#include "dsa/HashTable.h"
#include "dsa/Vector.h"

class BacSi;

// Registry sở hữu nhân viên; HashTable chỉ giữ con trỏ để tra cứu nhanh theo mã.
// Cấm copy để tránh hai Registry cùng delete một nhân viên.
class StaffRegistry {
    HashTable<std::string, NhanVienYTe*> ChiMuc;
    Vector<NhanVienYTe*> DanhSach;
public:
    StaffRegistry() = default;
    ~StaffRegistry();
    StaffRegistry(const StaffRegistry&) = delete;
    StaffRegistry& operator=(const StaffRegistry&) = delete;
    void Them(const NhanVienYTe& NhanVien);
    void ThemNhanVien(const NhanVienYTe& NhanVien) { Them(NhanVien); }
    const NhanVienYTe* TimNhanVien(const std::string& Ma) const { return Tim(Ma); }
    void CapNhat(const NhanVienYTe& NhanVien);
    bool Xoa(const std::string& Ma);
    const NhanVienYTe* Tim(const std::string& Ma) const;
    Vector<const BacSi*> TimBacSiTheoTen(const std::string& Ten) const;
    int KichThuoc() const { return DanhSach.KichThuoc(); }
    const NhanVienYTe& Lay(int I) const { return *DanhSach[I]; }
};
