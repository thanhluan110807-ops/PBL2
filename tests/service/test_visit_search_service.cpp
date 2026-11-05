#include <cassert>
#include <ctime>
#include <string>

#include "dsa/Vector.h"
#include "model/Date.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "repository/PatientRegistry.h"
#include "service/VisitSearchService.h"
#include "util/My_stdexcept.h"

int main() {
    PatientRegistry SoBo;
    SoBo.ThemBenhNhan("BN1", "Nguyen Van An");
    SoBo.ThemBenhNhan("BN2", "Nguyen Van An"); // Trùng tên nhưng khác mã Patient.
    SoBo.ThemBenhNhan("BN3", "Binh");
    SoBo.ThemBenhNhan("BN4", "Mai"); // Chưa có lần khám.

    const std::time_t Moc = 1767225600; // 01/01/2026 00:00 UTC.
    SoBo.ThemLanKham("LK1", "BN1", Moc, "Dau bung");
    SoBo.ThemLanKham("LK2", "BN2", Moc + 60, "Sot");
    SoBo.ThemLanKham("LK3", "BN3", Moc + 86400, "Ho");

    const PatientRegistry& ChiDoc = SoBo;
    const VisitSearchService TraCuu(ChiDoc);
    const Vector<KetQuaTraCuuLanKham> TatCa = TraCuu.TimTheoBenhNhan();
    assert(TatCa.KichThuoc() == 3);
    assert(TatCa[0].LanKham->LayMaLanKham() == "LK3");
    assert(TatCa[1].LanKham->LayMaLanKham() == "LK2");
    assert(TatCa[2].LanKham->LayMaLanKham() == "LK1");

    const Vector<KetQuaTraCuuLanKham> TheoTen = TraCuu.TimTheoBenhNhan("AN");
    assert(TheoTen.KichThuoc() == 2);
    assert(TheoTen[0].BenhNhan->LayMaDinhDanh() == "BN2");
    assert(TheoTen[1].BenhNhan->LayMaDinhDanh() == "BN1");

    const Date Ngay{1, 1, 2026};
    const Vector<KetQuaTraCuuLanKham> KetHop = TraCuu.TimTheoBenhNhan("An", &Ngay, "LK2");
    assert(KetHop.KichThuoc() == 1);
    assert(KetHop[0].LanKham->LayMaLanKham() == "LK2");
    assert(KetHop[0].BenhNhan->LayMaDinhDanh() == "BN2");
    assert(TraCuu.TimTheoBenhNhan("An", &Ngay, "LK3").Rong());
    const Date NgayKhac{2, 1, 2026};
    assert(TraCuu.TimTheoBenhNhan("An", &NgayKhac, "LK2").Rong());
    assert(TraCuu.TimTheoBenhNhan("Mai").Rong());

    const Date NgaySai{29, 2, 2026};
    bool DaBaoLoi = false;
    try {
        TraCuu.TimTheoBenhNhan("", &NgaySai);
    } catch (const My_stdexcept& Loi) {
        DaBaoLoi = Loi.LayLoaiLoi() == My_stdexcept::LoaiLoi::DoiSoKhongHopLe;
    }
    assert(DaBaoLoi);
}
