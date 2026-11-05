#include <QString>
#include <QWidget>
#include "ui/DemoStartup.h"
#include "repository/PatientRegistry.h"
#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTemporaryFile>
#include <QTimer>
#include <cassert>

QWidget *HopKhoiDong() { auto *W = QApplication::activeModalWidget(); assert(W); return W; }
void BamKhoiDong(const char *Ma) {
    auto *N = HopKhoiDong()->findChild<QPushButton*>(Ma); assert(N); N->click();
}
int main(int SoThamSo, char **ThamSo) {
    QApplication U(SoThamSo, ThamSo);
    QTimer::singleShot(0, [] { BamKhoiDong("batDauRong"); });
    PatientRegistry *R = DemoUi::ChonDuLieu(); assert(R && R->SoBenhNhan() == 0); delete R;
    QTemporaryFile Tep; assert(Tep.open());
    Tep.write("MaBenhNhan,HoTen,GioiTinh,NgaySinh,SoCanCuoc,NhomMau,ThongTinDiUng,TienSuBenh,TuoiUocLuong\nBN1,An,Nam,,,O,,,30\n");
    Tep.flush();
    QTimer::singleShot(0, [&] {
        auto *E = HopKhoiDong()->findChild<QLineEdit*>("duongDanCsv"); assert(E);
        E->setText(Tep.fileName() + ".missing"); BamKhoiDong("docCsv");
        assert(!HopKhoiDong()->findChild<QLabel*>("loiCsv")->text().isEmpty());
        E->setText(Tep.fileName());
        QTimer::singleShot(0, [] { static_cast<QFileDialog*>(HopKhoiDong())->reject(); });
        BamKhoiDong("chonCsv");
        assert(E->text() == Tep.fileName());
        BamKhoiDong("docCsv");
    });
    R = DemoUi::ChonDuLieu(); assert(R && R->SoBenhNhan() == 1 && R->SoLanKham() == 0); delete R;
    QTimer::singleShot(0, [] { static_cast<QDialog*>(HopKhoiDong())->reject(); });
    assert(DemoUi::ChonDuLieu() == nullptr);
}
