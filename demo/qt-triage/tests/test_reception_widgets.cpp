#include "ui/DemoWindow.h"
#include "repository/SessionFileManager.h"
#include "model/Patient.h"
#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTemporaryDir>
#include <QTimer>
#include <cassert>
#include <ctime>

template<class T> T* Tim(QWidget* H, const char* Ma) {
    auto* W=H->findChild<T*>(Ma); assert(W); return W;
}
QWidget* Hop() { auto* H=QApplication::activeModalWidget(); assert(H); return H; }
void Nhap(QWidget* H, const char* Ma, const QString& Chu) { Tim<QLineEdit>(H,Ma)->setText(Chu); }
void Bam(QWidget* H, const char* Ma) { Tim<QPushButton>(H,Ma)->click(); }
void Chon(QWidget* H, const QString& Ma) {
    Nhap(H,"timHoSo",Ma);
    auto* DS=Tim<QListWidget>(H,"chonHoSo");
    for (int I=0; I<DS->count(); ++I)
        if (DS->item(I)->data(Qt::UserRole).toString()==Ma) { DS->setCurrentRow(I); return; }
    assert(false);
}
void Chup(QWidget* H, const char* Bien) {
    QApplication::processEvents(); QApplication::sendPostedEvents(nullptr,QEvent::LayoutRequest);
    QApplication::processEvents();
    const auto Tep=qEnvironmentVariable(Bien);
    if (!Tep.isEmpty()) assert(H->grab().save(Tep));
}

int main(int argc,char** argv) {
    QApplication App(argc,argv);
    QTemporaryDir Tmp; assert(Tmp.isValid()); qputenv("XDG_DATA_HOME",Tmp.path().toUtf8());
    PatientRegistry R;
    Patient P("A","Nguyễn An",Gender::Nam); P.DatTuoiUocLuong(30); P.CapNhatThongTinDiUng("Penicillin");
    const auto* Cu=R.ThemBenhNhan(P);
    R.ThemBenhNhan("B","Trần Bình",Gender::Nu,Date{1,1,2000},"999");
    const auto* VoDanh=R.ThemBenhNhan("C");
    TriageService T(R); VisitSearchService S(R);
    T.TiepNhan("V-OLD","A",std::time(nullptr)-3600,"Khám cũ"); T.Huy("V-OLD");
    const auto TepPatient=(Tmp.path()+"/patient.csv").toStdString();
    const auto TepVisit=(Tmp.path()+"/visit.csv").toStdString();
    DemoWindow W(R,T,S,TepPatient,TepVisit); W.show(); App.processEvents();

    QTimer::singleShot(0,[&] {
        auto* H=Hop(); auto* CheDo=Tim<QComboBox>(H,"cheDo");
        assert(!Tim<QListWidget>(H,"chonHoSo")->isVisible());
        assert(!Tim<QLineEdit>(H,"timHoSo")->isVisible());
        assert(!Tim<QLineEdit>(H,"HoTen")->isReadOnly());
        assert(Tim<QLineEdit>(H,"HoTen")->font().pixelSize()==18);
        Chup(H,"PBL2_RECEPTION_NEW");
        CheDo->setCurrentIndex(1);
        assert(Tim<QListWidget>(H,"chonHoSo")->isVisible());
        assert(!Tim<QPushButton>(H,"luu")->isEnabled());
        Chon(H,"A");
        assert(Tim<QLineEdit>(H,"MaBenhNhan")->text()=="A");
        assert(Tim<QLineEdit>(H,"MaBenhNhan")->isReadOnly());
        assert(Tim<QLineEdit>(H,"HoTen")->text()=="Nguyễn An" && Tim<QLineEdit>(H,"HoTen")->isReadOnly());
        assert(Tim<QLineEdit>(H,"DiUng")->text()=="Penicillin" && Tim<QLineEdit>(H,"DiUng")->isReadOnly());
        assert(Tim<QLineEdit>(H,"Tuoi")->text()=="30" && Tim<QLineEdit>(H,"Tuoi")->isReadOnly());
        assert(!Tim<QComboBox>(H,"GioiTinh")->isEnabled());
        for (const auto* Ma : {"CanCuoc","NgaySinh","TienSu"})
            assert(Tim<QLineEdit>(H,Ma)->isEnabled() && !Tim<QLineEdit>(H,Ma)->isReadOnly());
        Chup(H,"PBL2_RECEPTION_EXISTING");
        // Đổi chế độ/người không được mang dữ liệu hồ sơ trước sang hồ sơ mới.
        Nhap(H,"MaLanKham","LK001"); CheDo->setCurrentIndex(0);
        assert(!Tim<QListWidget>(H,"chonHoSo")->isVisible());
        assert(Tim<QLineEdit>(H,"MaBenhNhan")->text().isEmpty());
        assert(Tim<QLineEdit>(H,"HoTen")->text().isEmpty() && !Tim<QLineEdit>(H,"HoTen")->isReadOnly());
        assert(Tim<QLineEdit>(H,"MaLanKham")->text()=="LK001");
        CheDo->setCurrentIndex(1); Chon(H,"B");
        assert(Tim<QLineEdit>(H,"HoTen")->text()=="Trần Bình");
        assert(Tim<QLineEdit>(H,"DiUng")->text().isEmpty() && !Tim<QLineEdit>(H,"DiUng")->isReadOnly());
        Chon(H,"A"); Nhap(H,"CanCuoc","001"); Nhap(H,"TienSu","Hen");
        Bam(H,"dongHop");
    });
    W.MoTiepNhan();
    assert(Cu->LaySoCanCuoc().empty() && Cu->LayTienSuBenh().empty());

    QTimer::singleShot(0,[&] {
        auto* H=Hop(); Tim<QComboBox>(H,"cheDo")->setCurrentIndex(1); Chon(H,"A");
        Nhap(H,"MaLanKham","LK001"); Nhap(H,"TrieuChung","Khám lại");
        Nhap(H,"NgaySinh","31/02/2000"); Bam(H,"luu");
        assert(!Tim<QLabel>(H,"loiTong")->text().isEmpty() && !Cu->CoNgaySinh());
        Nhap(H,"NgaySinh",""); Nhap(H,"CanCuoc","999"); Nhap(H,"TienSu","Hen"); Bam(H,"luu");
        assert(R.TimLanKham("LK001")==nullptr && Cu->LayTienSuBenh().empty());
        Nhap(H,"CanCuoc","001"); Bam(H,"luu");
    });
    W.MoTiepNhan();
    assert(R.SoBenhNhan()==3 && R.TimBenhNhan("A")==Cu);
    assert(Cu->LaySoCanCuoc()=="001" && Cu->LayTienSuBenh()=="Hen");
    assert(Cu->LayHoTen()=="Nguyễn An" && Cu->LayThongTinDiUng()=="Penicillin" && Cu->LayTuoiUocLuong()==30);
    assert(R.TimLanKham("LK001")->LayMaBenhNhan()=="A" && R.TimLanKham("V-OLD")->LayTrangThai()==VisitStatus::DaHuy);
    TrangThaiHangCho Hang; auto* Doc=SessionFileManager().DocBoFile(TepPatient,TepVisit,Hang);
    assert(Doc->TimBenhNhan("A")->LaySoCanCuoc()=="001" && Doc->TimBenhNhan("A")->LayTienSuBenh()=="Hen"); delete Doc;

    QTimer::singleShot(0,[&] {
        auto* H=Hop(); Tim<QComboBox>(H,"cheDo")->setCurrentIndex(1); Chon(H,"A");
        assert(Tim<QLineEdit>(H,"CanCuoc")->isReadOnly() && Tim<QLineEdit>(H,"TienSu")->isReadOnly());
        Nhap(H,"MaLanKham","V-REJECT"); Nhap(H,"TrieuChung","Khám"); Nhap(H,"NgaySinh","01/01/2000");
        Bam(H,"luu");
        assert(!Cu->CoNgaySinh() && !R.TimLanKham("V-REJECT"));
        Chon(H,"C");
        assert(Tim<QComboBox>(H,"GioiTinh")->isEnabled());
        assert(Tim<QLineEdit>(H,"HoTen")->text().isEmpty() && !Tim<QLineEdit>(H,"HoTen")->isReadOnly());
        Nhap(H,"HoTen","Ngọc"); Nhap(H,"Tuoi","41"); Tim<QComboBox>(H,"GioiTinh")->setCurrentIndex(1);
        Nhap(H,"MaLanKham","LK002");
        H->resize(496,752); Chup(H,"PBL2_RECEPTION_NARROW");
        assert(Tim<QScrollArea>(H,"dialogScroll")->horizontalScrollBar()->maximum()==0);
        Bam(H,"luu");
    });
    W.MoTiepNhan();
    assert(R.TimBenhNhan("C")==VoDanh && VoDanh->LayHoTen()=="Ngọc");
    assert(VoDanh->LayTuoiUocLuong()==41 && VoDanh->LayGioiTinh()==Gender::Nu && !VoDanh->CoNgaySinh());
    assert(R.TimLanKham("LK002")->LayMaBenhNhan()=="C");
    T.Huy("LK001");
    QTimer::singleShot(0,[&] {
        auto* H=Hop(); Tim<QComboBox>(H,"cheDo")->setCurrentIndex(1); Chon(H,"A");
        assert(Tim<QLineEdit>(H,"MaBenhNhan")->text()=="A");
        assert(Tim<QLineEdit>(H,"MaLanKham")->isReadOnly());
        Nhap(H,"TrieuChung","Khám lần tiếp theo"); Bam(H,"luu");
    });
    W.MoTiepNhan();
    assert(R.SoBenhNhan()==3 && R.TimBenhNhan("A")==Cu);
    assert(R.TimLanKham("LK003")->LayMaBenhNhan()=="A");
    auto* TaiNap=SessionFileManager().DocBoFile(TepPatient,TepVisit,Hang);
    assert(TaiNap->TimLanKham("LK003")->LayMaBenhNhan()=="A");
    assert(TaiNap->TaoMaLanKham()=="LK004"); delete TaiNap;
}
