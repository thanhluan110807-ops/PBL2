#include <QString>
#include "ui/DemoWindow.h"
#include "ui/DemoStartup.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "service/VisitSearchService.h"
#include "util/My_stdexcept.h"
#include <exception>
#include <QApplication>
#include <QMessageBox>

int main(int SoThamSo, char *ThamSo[]) {
    QApplication UngDung(SoThamSo, ThamSo);
    QApplication::setStyle("Fusion");
    PatientRegistry *Registry = nullptr;
    int MaThoat = 0;
    try {
        Registry = DemoUi::ChonDuLieu();
        if (Registry == nullptr) return 0;
        // Cửa sổ và service hủy trước Registry, kể cả khi phát sinh ngoại lệ.
        TriageService Triage(*Registry);
        VisitSearchService TraCuu(*Registry);
        DemoWindow CuaSo(*Registry, Triage, TraCuu);
        CuaSo.show();
        MaThoat = UngDung.exec();
    } catch (const My_stdexcept &Loi) {
        QMessageBox::critical(nullptr, "Không thể chạy demo", QString::fromUtf8(Loi.LayThongBao()));
        MaThoat = 1;
    } catch (const std::exception &) {
        QMessageBox::critical(nullptr, "Không thể chạy demo", "Không đủ tài nguyên để chạy chương trình.");
        MaThoat = 1;
    }
    delete Registry;
    return MaThoat;
}
