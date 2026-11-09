#include "ui/DemoWindow.h"
#include "repository/SessionFileManager.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTemporaryDir>
#include <QTimer>
#include <cassert>
#include <ctime>

template<class T> T* Tim(QWidget* Cha, const char* Ma) {
    auto* W=Cha->findChild<T*>(Ma); assert(W); return W;
}
QWidget* Hop() { auto* W=QApplication::activeModalWidget(); assert(W); return W; }
void Bam(QWidget* H, const char* Ma) { Tim<QPushButton>(H,Ma)->click(); }
void Nhap(QWidget* H, const char* Ma, const QString& GiaTri) { Tim<QLineEdit>(H,Ma)->setText(GiaTri); }
void Chup(QWidget* H, const char* Bien) {
    QApplication::processEvents();
    const auto Tep=qEnvironmentVariable(Bien);
    if (!Tep.isEmpty()) assert(H->grab().save(Tep));
}

int main(int argc, char** argv) {
    QApplication App(argc,argv);
    QTemporaryDir Tmp; assert(Tmp.isValid());
    qputenv("XDG_DATA_HOME",Tmp.path().toUtf8());
    PatientRegistry R;
    Patient P("P1","Nguyễn An",Gender::Nam,Date{1,1,1996});
    P.CapNhatNhomMau(BloodGroup::O);
    P.CapNhatThongTinDiUng("Penicillin");
    P.CapNhatTienSuBenh("Hen phế quản");
    const auto* Cu=R.ThemBenhNhan(P);
    R.ThemBenhNhan("P2");
    R.ThemBenhNhan("P3","Người khác",Gender::Nu,Date{1,1,2000},"999");
    TriageService T(R); VisitSearchService S(R);
    T.TiepNhan("V1","P1",std::time(nullptr),"Đau đầu và chóng mặt");
    const auto TepPatient=(Tmp.path()+"/patient.csv").toStdString();
    const auto TepVisit=(Tmp.path()+"/visit.csv").toStdString();
    DemoWindow W(R,T,S,TepPatient,TepVisit); W.show(); App.processEvents();

    QTimer::singleShot(0,[&] {
        auto* H=Hop();
        assert(!Tim<QLineEdit>(H,"HoTen")->isVisible());
        assert(!Tim<QPushButton>(H,"luu")->isVisible());
        assert(Tim<QPushButton>(H,"suaThongTin")->text()=="Bổ sung / cập nhật thông tin");
        auto* Ten=Tim<QLabel>(H,"hoSoHoTen");
        auto* Gioi=Tim<QLabel>(H,"hoSoGioiTinh");
        auto* Mau=Tim<QLabel>(H,"hoSoNhomMau");
        assert(Ten->text()=="Họ tên: Nguyễn An");
        assert(Tim<QLabel>(H,"hoSoCanCuoc")->text().trimmed()=="CCCD:");
        assert(Gioi->text()=="Giới tính: Nam" && Mau->text()=="Nhóm máu: O");
        assert(Tim<QLabel>(H,"hoSoDiUng")->text()=="Dị ứng: Penicillin");
        assert(Ten->font().pixelSize()==18);
        assert(Mau->y()>Gioi->y() && Mau->y()-Gioi->y()<55);
        auto* Cuon=Tim<QScrollArea>(H,"dialogScroll");
        assert(Ten->mapTo(Cuon->widget(),QPoint()).y()<40);
        Chup(H,"PBL2_PROFILE_VIEW");
        H->resize(496,720); App.processEvents();
        assert(Cuon->horizontalScrollBar()->maximum()==0);
        Chup(H,"PBL2_PROFILE_NARROW");

        // Chỉ bấm sửa mới hiện ô nhập; hủy phải giữ nguyên hồ sơ.
        Bam(H,"suaThongTin");
        assert(Tim<QLineEdit>(H,"HoTen")->isVisible() && !Ten->isVisible());
        Chup(H,"PBL2_PROFILE_EDIT");
        Nhap(H,"HoTen","Chưa lưu"); Bam(H,"dongHop");
        assert(Cu->LayHoTen()=="Nguyễn An" && Ten->isVisible());
        Bam(H,"suaThongTin");
        assert(Tim<QLineEdit>(H,"HoTen")->text()=="Nguyễn An");
        Nhap(H,"HoTen","Nguyễn An cập nhật"); Nhap(H,"NgaySinh","31/02/1996");
        Bam(H,"luu");
        assert(!Tim<QLabel>(H,"loiTong")->text().isEmpty());
        assert(Cu->LayHoTen()=="Nguyễn An");
        Nhap(H,"NgaySinh","02/03/1996"); Nhap(H,"CanCuoc","999"); Bam(H,"luu");
        assert(Cu->LayHoTen()=="Nguyễn An" && Cu->LaySoCanCuoc().empty());
        Nhap(H,"CanCuoc","001"); Tim<QComboBox>(H,"GioiTinh")->setCurrentIndex(1);
        Bam(H,"luu");
        assert(Ten->isVisible() && Ten->text()=="Họ tên: Nguyễn An cập nhật");
        assert(!Tim<QLineEdit>(H,"HoTen")->isVisible());
        assert(Tim<QLabel>(H,"hoSoCanCuoc")->text()=="CCCD: 001");
        assert(Tim<QLabel>(H,"hoSoGioiTinh")->text()=="Giới tính: Nữ");
        Bam(H,"dongHop");
    });
    W.MoHoSo("P1");
    assert(R.TimBenhNhan("P1")==Cu && R.TimLanKham("V1")->LayMaBenhNhan()=="P1");
    assert(Cu->LayNhomMau()==BloodGroup::O && Cu->LayThongTinDiUng()=="Penicillin");
    TrangThaiHangCho Hang;
    auto* Doc=SessionFileManager().DocBoFile(TepPatient,TepVisit,Hang);
    assert(Doc->TimBenhNhan("P1")->LayHoTen()=="Nguyễn An cập nhật"); delete Doc;

    QTimer::singleShot(0,[] {
        auto* H=Hop();
        for (const auto* Ma : {"hoSoHoTen","hoSoCanCuoc","hoSoNgaySinh","hoSoGioiTinh","hoSoNhomMau"}) {
            const auto Chuoi=Tim<QLabel>(H,Ma)->text();
            assert(Chuoi.mid(Chuoi.indexOf(':')+1).trimmed().isEmpty());
        }
        Bam(H,"dongHop");
    });
    W.MoHoSo("P2");

    QTimer::singleShot(0,[] {
        auto* H=Hop();
        assert(Tim<QLabel>(H,"chiTietHoTen")->text()=="Họ tên: Nguyễn An cập nhật");
        auto* TrieuChung=Tim<QLabel>(H,"chiTietTrieuChung");
        auto* TiepNhan=Tim<QLabel>(H,"chiTietTiepNhan");
        assert(TrieuChung->font().pixelSize()==18);
        assert(TiepNhan->y()>TrieuChung->y() && TiepNhan->y()-TrieuChung->y()<55);
        assert(H->findChild<QPushButton*>("danhGia") && !H->findChild<QPushButton*>("chuyenCls"));
        Chup(H,"PBL2_VISIT_VIEW"); Bam(H,"dongHop");
    });
    W.MoChiTiet("V1");
    T.DanhGia("V1",VitalSigns(80,120,18,98,15),std::time(nullptr),"DD1");
    T.GoiTiep("BS1");
    T.ChuyenCanLamSang("V1","Xét nghiệm máu","BS1");
    assert(W.DanhDauThayDoi());
    QTimer::singleShot(0,[] {
        auto* H=Hop();
        assert(Tim<QLabel>(H,"chiTietChiDinh")->text()=="Chỉ định: Xét nghiệm máu");
        assert(Tim<QLabel>(H,"danhGiaNhipTim")->text().contains("80"));
        assert(H->findChildren<QPushButton*>("quayLai").size()==1);
        assert(!H->findChild<QPushButton*>("danhGia"));
        H->resize(496,720); QApplication::processEvents();
        assert(Tim<QScrollArea>(H,"dialogScroll")->horizontalScrollBar()->maximum()==0);
        Chup(H,"PBL2_VISIT_NARROW"); Bam(H,"dongHop");
    });
    W.MoChiTiet("V1");
}
