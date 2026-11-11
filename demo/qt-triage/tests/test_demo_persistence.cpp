#include "ui/DemoWindow.h"
#include "ui/DemoStartup.h"
#include "repository/SessionFileManager.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "service/VisitSearchService.h"
#include "model/Patient.h"
#include "model/Gender.h"
#include "model/Visit.h"
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QTemporaryDir>
#include <QMetaObject>
#include <QString>
#include <QWidget>
#include <filesystem>
#include <fstream>
#include <cassert>

QWidget* Hop() { auto* W=QApplication::activeModalWidget(); assert(W); return W; }
void Nhap(const char* Ma,const QString& Chuoi) { auto* E=Hop()->findChild<QLineEdit*>(Ma); assert(E); E->setText(Chuoi); }
int main(int So,char** Args) {
    QApplication A(So,Args); QTemporaryDir ThuMuc; assert(ThuMuc.isValid());
    const std::string Goc=ThuMuc.path().toStdString()+"/tu-dong";
    std::filesystem::create_directory(std::filesystem::u8path(Goc));
    const std::string TepPatient=Goc+"/patient.csv", TepVisit=Goc+"/visit.csv";
    std::string NhoP,NhoV;
    assert(!DemoUi::DocBoFileGanNhat(ThuMuc.path().toStdString(),NhoP,NhoV));
    DemoUi::NhoBoFileDaChon(ThuMuc.path().toStdString(),TepPatient,TepVisit);
    assert(DemoUi::DocBoFileGanNhat(ThuMuc.path().toStdString(),NhoP,NhoV));
    assert(NhoP==TepPatient && NhoV==TepVisit);
    PatientRegistry R; TriageService T(R); VisitSearchService S(R);
    DemoWindow W(R,T,S,TepPatient,TepVisit); W.show(); A.processEvents();
    assert(!W.findChild<QPushButton*>("luuDuLieu"));
    // Tiếp nhận thật qua GUI, không bấm bất kỳ nút lưu nào.
    QTimer::singleShot(0,[] {
        Nhap("HoTen","Nguyễn An"); Nhap("Tuoi","30");
        Nhap("TrieuChung","Khám");
        Hop()->findChild<QPushButton*>("luu")->click();
    });
    W.MoTiepNhan(); assert(!W.isWindowModified());
    TrangThaiHangCho Hang;
    PatientRegistry* Moi=SessionFileManager().DocBoFile(TepPatient,TepVisit,Hang);
    assert(Moi && Moi->SoBenhNhan()==1 && Moi->SoLanKham()==1);
    assert(Moi->TaoMaBenhNhan()=="BN002" && Moi->TaoMaLanKham()=="LK002"); delete Moi;
    QTimer::singleShot(0,[] {
        Nhap("Mach","80"); Nhap("HuyetAp","120"); Nhap("NhipTho","18");
        Nhap("SpO2","98"); Nhap("GCS","15"); Nhap("MaDieuDuong","DD1");
        Hop()->findChild<QPushButton*>("luu")->click();
    });
    W.MoDanhGia("LK001"); assert(!W.isWindowModified());
    // Mở lại không cần tự tìm bản lưu hoặc chọn file.
    Moi=DemoUi::KhoiDongTuDong(TepPatient,TepVisit,Hang);
    assert(Moi && Moi->TimLanKham("LK001")->SoDanhGia()==1);
    { TriageService Tai(*Moi); Tai.KhoiPhucHangCho(Hang); assert(Tai.SoDangCho()==1); }
    delete Moi;
    W.findChild<QLineEdit*>("maBacSi")->setText("BS1"); W.findChild<QPushButton*>("goiTiep")->click();
    Moi=SessionFileManager().DocBoFile(TepPatient,TepVisit,Hang);
    assert(Moi->TimLanKham("LK001")->LayTrangThai()==VisitStatus::DangKham); delete Moi;
    // Chặn I/O sau thao tác: không báo đã lưu, không nhận thêm thay đổi, không cho đóng mất dữ liệu.
    namespace Fs=std::filesystem;
    Fs::rename(Fs::u8path(Goc),Fs::u8path(Goc+"-giu"));
    { std::ofstream Chan(Fs::u8path(Goc)); Chan<<"chan ghi"; }
    R.ThemBenhNhan("P2"); W.DanhDauThayDoi(); assert(W.isWindowModified());
    assert(!W.close() && W.isVisible());
    W.MoTiepNhan(); assert(QApplication::activeModalWidget()==nullptr);
    Fs::remove(Fs::u8path(Goc)); Fs::rename(Fs::u8path(Goc+"-giu"),Fs::u8path(Goc));
    // Timer tự thử lại, không có thao tác lưu thủ công.
    auto* ThuLai=W.findChild<QTimer*>("thuLuuTuDong"); assert(ThuLai);
    assert(QMetaObject::invokeMethod(ThuLai,"timeout",Qt::DirectConnection));
    assert(!W.isWindowModified());
    Moi=SessionFileManager().DocBoFile(TepPatient,TepVisit,Hang); assert(Moi->TimBenhNhan("P2")); delete Moi;
    assert(W.close());
}
