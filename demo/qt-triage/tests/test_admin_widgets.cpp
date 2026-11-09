#include "ui/AdminPanel.h"
#include "model/DieuDuong.h"
#include "model/BacSi.h"
#include "repository/StaffFileManager.h"
#include <QDateTime>
#include "service/SchedulerService.h"
#include <QApplication>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QTabWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <cassert>
int main(int argc, char** argv) {
    QApplication App(argc,argv);
    QTemporaryDir Tmp;
    assert(Tmp.isValid());
    qputenv("XDG_DATA_HOME",Tmp.path().toUtf8());
    const QString Tep = Tmp.path()+"/du-lieu/nhan-su-lich.pbl2";
    AdminPanel W(nullptr,Tep);
    W.show(); App.processEvents();
    auto* NutLuu=W.findChild<QPushButton*>("adminSave");
    assert(NutLuu && !NutLuu->isEnabled());
    for (const auto* Ma : {"adminLoad","staffImport","staffExport","scheduleImport","scheduleExport"})
        assert(!W.findChild<QPushButton*>(Ma));
    // Số liệu và hướng dẫn phải đọc được ngay, kể cả khi chưa có dữ liệu.
    assert(W.findChild<QLabel*>("adminStaffCount")->text()=="0");
    assert(W.findChild<QLabel*>("adminShiftCount")->text()=="0");
    assert(W.findChild<QLabel*>("adminAssignmentCount")->text()=="0");
    assert(W.findChild<QLabel*>("adminNextStep")->text().contains("nhân sự"));
    auto Click = [&](const char* Id) { auto* N = W.findChild<QPushButton*>(Id); assert(N); N->click(); App.processEvents(); };
    Click("adminSeed");
    assert(W.findChild<QLabel*>("adminStaffCount")->text()=="4");
    assert(W.findChild<QLabel*>("adminShiftCount")->text()=="2");
    assert(W.findChild<QTableWidget*>("staffTable")->rowCount()==4);
    assert(W.findChild<QTableWidget*>("shiftTable")->rowCount()==2);
    assert(W.findChild<QTableWidget*>("shiftTable")->item(0,5)->text()=="Có");
    Click("scheduleGenerate");
    assert(W.findChild<QLabel*>("adminAssignmentCount")->text()=="4");
    assert(W.findChild<QTableWidget*>("scheduleTable")->item(0,1)->text().contains("→"));
    W.findChild<QTableWidget*>("staffTable")->selectRow(0);
    assert(W.findChild<QLabel*>("adminStaffDetail")->text().contains("Nguyễn An"));
    W.findChild<QTableWidget*>("shiftTable")->selectRow(0);
    assert(W.findChild<QLabel*>("adminShiftDetail")->text().contains("T2-SANG"));
    W.findChild<QTableWidget*>("scheduleTable")->selectRow(0);
    assert(W.findChild<QLabel*>("adminAssignmentDetail")->text().contains("T2-SANG"));
    assert(W.findChild<QTableWidget*>("scheduleTable")->rowCount()==4);
    assert(SchedulerService::LichHopLe(*W.LayDuLieu().NhanSu,W.LayDuLieu().Lich));
    assert(NutLuu->isEnabled() && !QFile::exists(Tep));
    Click("adminSave");
    assert(!QApplication::activeModalWidget());
    assert(QFile::exists(Tep) && !NutLuu->isEnabled());
    AdminPanel Doc(nullptr,Tep);
    assert(Doc.findChild<QTableWidget*>("scheduleTable")->rowCount()==4);
    W.findChild<QLineEdit*>("staffId")->setText("BS01");
    W.findChild<QLineEdit*>("staffName")->setText("Trung ma");
    W.findChild<QLineEdit*>("staffSpecialty")->setText("Noi");
    Click("staffAdd");
    assert(W.LayDuLieu().NhanSu->KichThuoc()==4);
    assert(W.findChild<QLabel*>("adminMessage")->text().contains("Trung ma"));
    const QString Hong=Tmp.path()+"/hong.pbl2";
    { QFile F(Hong); assert(F.open(QIODevice::WriteOnly)); F.write("khong hop le"); }
    assert(!W.NapTep(Hong));
    assert(W.findChild<QTableWidget*>("scheduleTable")->rowCount()==4);
    // Kiểm tra thay người qua chính widget: thêm người dự phòng rồi chọn lại lịch.
    W.findChild<QLineEdit*>("staffId")->setText("BS03");
    W.findChild<QLineEdit*>("staffName")->setText("Du phong");
    W.findChild<QLineEdit*>("staffSpecialty")->setText(QString::fromUtf8("Cấp cứu"));
    W.findChild<QComboBox*>("staffRank")->setCurrentIndex(1);
    Click("staffAdd");
    assert(W.LayDuLieu().NhanSu->KichThuoc()==5);
    W.findChild<QTableWidget*>("scheduleTable")->selectRow(0);
    Click("replacementFind");
    auto* UngVien = W.findChild<QComboBox*>("replacementCandidates");
    const int ViTri = UngVien->findData("BS03");
    assert(ViTri>=0);
    UngVien->setCurrentIndex(ViTri);
    assert(UngVien->currentText().contains("Du phong"));
    Click("replacementApply");
    assert(W.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DaHuy);
    const int Cuoi=W.LayDuLieu().Lich.PhanCong.KichThuoc()-1;
    assert(W.LayDuLieu().Lich.PhanCong[Cuoi].MaNhanVien=="BS03");
    assert(W.LayDuLieu().Lich.PhanCong[Cuoi].LayTrangThai()==AssignmentStatus::DaXacNhan);
    assert(W.findChild<QLabel*>("adminAssignmentCount")->text()=="3");
    Click("adminSave");
    assert(!NutLuu->isEnabled());
    assert(Doc.NapTep(Tep));
    assert(Doc.LayDuLieu().Lich.PhanCong[Cuoi].MaNhanVien=="BS03");
    assert(Doc.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DaHuy);
    if (!qEnvironmentVariable("PBL2_ADMIN_CAPTURE").isEmpty()) {
        W.findChild<QTabWidget*>()->setCurrentIndex(2); App.processEvents();
        assert(W.grab().save(qEnvironmentVariable("PBL2_ADMIN_CAPTURE")));
    }
    W.resize(580,680); App.processEvents();
    assert(W.width()==580);
    auto* CacTab=W.findChild<QTabWidget*>();
    for (int I=0;I<CacTab->count();++I) {
        auto* Cuon=qobject_cast<QScrollArea*>(CacTab->widget(I));
        assert(Cuon && Cuon->verticalScrollBar()->maximum()>0);
    }
    assert(W.findChild<QTableWidget*>("scheduleTable")->horizontalScrollBar()->maximum()>0);
    if (!qEnvironmentVariable("PBL2_ADMIN_CAPTURE_NARROW").isEmpty()) {
        CacTab->setCurrentIndex(2); App.processEvents();
        assert(W.grab().save(qEnvironmentVariable("PBL2_ADMIN_CAPTURE_NARROW")));
    }
    W.close(); Doc.close();
    // Cập nhật tên trên GUI không được làm mất số liệu sàng lọc/giờ tuần.
    AdminData Mau;
    DieuDuong DD("D","Lan",CapBac::DieuDuongChinh,4);
    DD.BatDauTrucSangLoc(); DD.CongDonGioSangLoc(4); DD.CongDonGioLam(8);
    DD.CapNhatThongTinCaNhan("Lan",Gender::Nu,Date{1,1,2000});
    Mau.NhanSu->Them(DD);
    const QString TepSua=Tmp.path()+"/sua.pbl2";
    AdminFileManager::Luu(TepSua.toStdString(),Mau);
    AdminPanel Sua(nullptr,TepSua);
    Sua.findChild<QTableWidget*>("staffTable")->selectRow(0);
    Sua.findChild<QPushButton*>("staffEditSelect")->click();
    Sua.findChild<QLineEdit*>("staffName")->setText("Lan moi");
    Sua.findChild<QPushButton*>("staffUpdate")->click();
    const auto* DDSo=static_cast<const DieuDuong*>(Sua.LayDuLieu().NhanSu->Tim("D"));
    assert(DDSo && DDSo->LayHoTen()=="Lan moi" && DDSo->LayGioLamTuan()==8);
    assert(DDSo->DangSangLoc() && DDSo->CanXoayTuaSangLoc());
    assert(DDSo->LayNgaySinh().Nam==2000);
    assert(!Sua.findChild<QTableWidget*>("staffTable")->item(0,1)->toolTip().isEmpty());
    assert(Sua.LuuTep(TepSua)); Sua.close();
    // Kiểm tra luồng đồng hồ thật và tìm tên trên các nút mới.
    AdminPanel DongHo;
    DongHo.findChild<QPushButton*>("adminSeed")->click();
    auto* BangNV=DongHo.findChild<QTableWidget*>("staffTable");
    auto* TimBS=DongHo.findChild<QLineEdit*>("staffSearchDoctor");
    TimBS->setText(QString::fromUtf8("Nguyễn An"));
    assert(!BangNV->isRowHidden(0) && BangNV->isRowHidden(2));
    TimBS->clear();
    BangNV->selectRow(2);
    const auto* DiaChi=DongHo.LayDuLieu().NhanSu->Tim("DD01");
    DongHo.findChild<QPushButton*>("staffTriageStart")->click();
    assert(DongHo.LayDuLieu().NhanSu->Tim("DD01")==DiaChi);
    assert(static_cast<const DieuDuong*>(DiaChi)->DangTheoDongHo());
    BangNV->selectRow(2);
    DongHo.findChild<QPushButton*>("staffTriageRefresh")->click();
    DongHo.findChild<QPushButton*>("scheduleGenerate")->click();
    const QString TepDongHo=Tmp.path()+"/dong-ho.pbl2";
    assert(DongHo.LuuTep(TepDongHo));
    AdminPanel NapDongHo(nullptr,TepDongHo);
    assert(static_cast<const DieuDuong*>(NapDongHo.LayDuLieu().NhanSu->Tim("DD01"))->DangTheoDongHo());
    BangNV->selectRow(2);
    DongHo.findChild<QPushButton*>("staffTriageEnd")->click();
    assert(!static_cast<const DieuDuong*>(DiaChi)->DangSangLoc());
    assert(DongHo.LuuTep(TepDongHo)); DongHo.close(); NapDongHo.close();
    // GUI phải đi qua trạng thái xác nhận trước khi ghi nhận bắt đầu.
    AdminData CaDangDienRa;
    CaDangDienRa.NhanSu->Them(BacSi("B","An",CapBac::BacSiChinh,5,"Cap cuu"));
    const auto BayGio=QDateTime::currentSecsSinceEpoch();
    const auto Dau=BayGio-BayGio%60;
    CaDangDienRa.Lich.CacCa.ThemCuoi(Shift::TuThoiDiem("NOW",Dau,Dau+3600,1,0,true));
    CaDangDienRa.Lich.PhanCong.ThemCuoi(ShiftAssignment("P-NOW","B","NOW",AssignmentRole::TruongCa));
    const auto TepCa=Tmp.path()+"/ca-hien-tai.pbl2";
    AdminFileManager::Luu(TepCa.toStdString(),CaDangDienRa);
    AdminPanel Truc(nullptr,TepCa);
    auto* BangTruc=Truc.findChild<QTableWidget*>("scheduleTable"); BangTruc->selectRow(0);
    Truc.findChild<QPushButton*>("assignmentStart")->click();
    assert(Truc.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DuKien);
    Truc.findChild<QPushButton*>("assignmentConfirm")->click();
    assert(Truc.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DaXacNhan);
    BangTruc->selectRow(0); Truc.findChild<QPushButton*>("assignmentStart")->click();
    assert(Truc.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DangTruc);
    assert(Truc.LuuTep(TepCa)); Truc.close();
    AdminPanel MoLai(nullptr,TepCa);
    assert(MoLai.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DangTruc);
    MoLai.close();

    // Nạp nhân viên riêng phải cập nhật GUI, giữ lịch và từ chối file thiếu mã.
    const QString TepNV=Tmp.path()+"/nhan-vien.pbl2";
    assert(MoLai.LuuNhanSu(TepNV));
    const auto* DiaChiCu=MoLai.LayDuLieu().NhanSu->Tim("B");
    StaffRegistry Trong;
    StaffFileManager::LuuNhanVien(TepNV.toStdString(),Trong);
    assert(!MoLai.NapNhanSu(TepNV));
    assert(MoLai.LayDuLieu().NhanSu->Tim("B")==DiaChiCu);
    StaffRegistry Moi;
    Moi.Them(BacSi("B","Ten moi",CapBac::BacSiChinh,5,"Cap cuu"));
    StaffFileManager::LuuNhanVien(TepNV.toStdString(),Moi);
    assert(MoLai.NapNhanSu(TepNV));
    assert(MoLai.LayDuLieu().NhanSu->Tim("B")->LayHoTen()=="Ten moi");
    assert(MoLai.findChild<QTableWidget*>("staffTable")->item(0,1)->text()=="Ten moi");
    assert(MoLai.LayDuLieu().Lich.PhanCong[0].LayTrangThai()==AssignmentStatus::DangTruc);
    assert(!MoLai.findChild<QPushButton*>("staffImport"));
    assert(!MoLai.findChild<QPushButton*>("staffExport"));
    assert(MoLai.LuuTep(TepCa)); MoLai.close();

    // Lỗi ghi giữ thay đổi trong RAM để bấm lưu lại, không mở hộp chọn file.
    const QString TepLoiGhi=Tmp.path()+"/loi-ghi.pbl2";
    AdminPanel LoiGhi(nullptr,TepLoiGhi);
    LoiGhi.findChild<QPushButton*>("adminSeed")->click();
    assert(QDir().mkdir(TepLoiGhi));
    LoiGhi.findChild<QPushButton*>("adminSave")->click();
    assert(LoiGhi.findChild<QPushButton*>("adminSave")->isEnabled());
    assert(LoiGhi.findChild<QLabel*>("adminMessage")->text().startsWith("Lỗi:"));
    assert(LoiGhi.LayDuLieu().NhanSu->KichThuoc()==4);
    assert(QDir().rmdir(TepLoiGhi));
    LoiGhi.findChild<QPushButton*>("adminSave")->click();
    assert(!LoiGhi.findChild<QPushButton*>("adminSave")->isEnabled());
    AdminPanel MoSauLoi(nullptr,TepLoiGhi);
    assert(MoSauLoi.LayDuLieu().NhanSu->KichThuoc()==4);

    // Nguồn liên kết không đọc được phải giữ nguyên, không lưu đè dữ liệu rỗng.
    AdminPanel LoiDoc(nullptr,Hong);
    assert(!LoiDoc.findChild<QPushButton*>("adminSave")->isEnabled());
    LoiDoc.findChild<QPushButton*>("adminSave")->click();
    QFile FileHong(Hong); assert(FileHong.open(QIODevice::ReadOnly));
    assert(FileHong.readAll()=="khong hop le");

}
