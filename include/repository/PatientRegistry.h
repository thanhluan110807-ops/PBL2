#pragma once

#include <ctime>
#include <string>

#include "dsa/HashTable.h"
#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Gender.h"
#include "model/Patient.h"
#include "model/Visit.h"



struct KetQuaTraCuuLanKham {
    const Patient* BenhNhan = nullptr;
    const Visit* LanKham = nullptr;
};


class PatientRegistry {
private:
    HashTable<std::string, Patient*> DanhSachBenhNhan;
    HashTable<std::string, Visit*> DanhSachLanKham;

    Patient* LayBenhNhanBatBuoc(const std::string& MaBenhNhan) const;
    void KiemTraCanCuoc(const std::string& SoCanCuoc, const Patient* BoQua = nullptr) const;
    bool CoLanKhamDangMo(const std::string& MaBenhNhan) const;

public:

    std::string TaoMaBenhNhan() const;
    std::string TaoMaLanKham() const;
    PatientRegistry() = default;
    ~PatientRegistry();

    PatientRegistry(const PatientRegistry&) = delete;
    PatientRegistry& operator=(const PatientRegistry&) = delete;
    PatientRegistry(PatientRegistry&&) = delete;
    PatientRegistry& operator=(PatientRegistry&&) = delete;


    const Patient* ThemBenhNhan(const Patient& HoSo);
    const Patient* ThemBenhNhan(const std::string& MaBenhNhan, const std::string& HoTen = "",
                               Gender GioiTinh = Gender::ChuaXacDinh,
                               const std::string& SoCanCuoc = "");
    const Patient* ThemBenhNhan(const std::string& MaBenhNhan, const std::string& HoTen,
                               Gender GioiTinh, const Date& NgaySinh,
                               const std::string& SoCanCuoc = "");


    const Patient* TimBenhNhan(const std::string& MaBenhNhan) const;
    const Patient* TimBenhNhanTheoCanCuoc(const std::string& SoCanCuoc) const;
    const Patient* TimBenhNhanTheoMaLanKham(const std::string& MaLanKham) const;


    Vector<const Patient*> TimBenhNhanTheoTen(const std::string& TenCanTim) const;

    Vector<const Patient*> LayDanhSachBenhNhan() const;




    Vector<KetQuaTraCuuLanKham> TraCuuLanKham(
        const std::string& TenCanTim = "", const Date* NgayTiepNhan = nullptr,
        const std::string& MaLanKham = "") const;
    void BoSungDanhTinh(const std::string& MaBenhNhan, const std::string& SoCanCuoc,
                       const std::string& HoTen, Gender GioiTinh, const Date& NgaySinh);

    void BoSungThongTinConThieu(const std::string& MaBenhNhan, const Patient& ThongTin);


    Visit* ThemLanKham(const std::string& MaLanKham, const std::string& MaBenhNhan,
                      std::time_t ThoiDiemTiepNhan, const std::string& TrieuChungChinh);

    void KhoiPhucLanKham(const Vector<BanChupLanKham>& DanhSach);
    Visit* TimLanKham(const std::string& MaLanKham);
    const Visit* TimLanKham(const std::string& MaLanKham) const;


    Vector<Visit*> LayLichSuLanKham(const std::string& MaBenhNhan);
    Vector<const Visit*> LayLichSuLanKham(const std::string& MaBenhNhan) const;
    int SoBenhNhan() const;
    int SoLanKham() const;
};
