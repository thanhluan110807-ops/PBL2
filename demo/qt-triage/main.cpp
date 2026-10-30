#include "ui/DemoWindow.h"
#include <QApplication>
#include <QTimer>

int main(int SoThamSo, char *ThamSo[]) {
    QApplication UngDung(SoThamSo, ThamSo);
    QApplication::setStyle("Fusion");
    // Service lớn được sở hữu bằng new/delete rõ ràng; không phụ thuộc Qt.
    auto *DichVu = new Demo::DemoTriageService;
    DichVu->NapMau();
    int MaThoat = 0;
    {
        DemoWindow CuaSo(*DichVu);
        CuaSo.show();
        const QStringList CacThamSo = UngDung.arguments();
        const int Anh = CacThamSo.indexOf("--screenshot");
        if (Anh >= 0 && Anh + 1 < CacThamSo.size())
            QTimer::singleShot(500, &CuaSo,
                               [&] { UngDung.exit(CuaSo.grab().save(CacThamSo[Anh + 1]) ? 0 : 2); });
        MaThoat = UngDung.exec();
    }
    delete DichVu;
    return MaThoat;
}
