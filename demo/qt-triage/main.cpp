#include <QString>
#include "ui/DemoWindow.h"
#include "ui/DemoStartup.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "service/VisitSearchService.h"
#include "util/My_stdexcept.h"
#include <exception>
#include <QApplication>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QLockFile>
#include <QDir>
#include <QStringList>
#include <string>
#include <QMessageBox>

int main(int SoThamSo, char *ThamSo[]) {
    QApplication UngDung(SoThamSo, ThamSo);
    QCoreApplication::setOrganizationName("PBL2");
    QCoreApplication::setApplicationName("AnTam");
    QApplication::setStyle("Fusion");
    PatientRegistry *Registry = nullptr;
    int MaThoat = 0;
    try {
        // Khóa một tiến trình để hai cửa sổ không ghi đè bộ dữ liệu của nhau.
        const QString ThuMucCauHinh = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        if (ThuMucCauHinh.isEmpty() || !QDir().mkpath(ThuMucCauHinh))
            throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Khong tao duoc thu muc cau hinh");
        QLockFile Khoa(ThuMucCauHinh + "/an-tam.lock"); Khoa.setStaleLockTime(0);
        if (!Khoa.tryLock()) {
            QMessageBox::warning(nullptr,"Dữ liệu đang được mở","Hãy đóng cửa sổ An Tâm đang chạy trước khi mở thêm.");
            return 1;
        }
        std::string TepPatient,TepVisit;
        const bool ChonLai = UngDung.arguments().contains("--chon-du-lieu");
        const bool DungMau = UngDung.arguments().contains("--du-lieu-mau");
        const bool CoBoFile = !ChonLai && !DungMau &&
            DemoUi::DocBoFileGanNhat(ThuMucCauHinh.toStdString(),TepPatient,TepVisit);
        TrangThaiHangCho HangCho;
        if (DungMau || (!ChonLai && !CoBoFile)) {
            // Liên kết thẳng bộ CSV cố định. Lần sau đọc lại chính các file đã sửa.
            const QDir ThuMucMau(QString::fromUtf8(PBL2_SAMPLE_DATA_DIR));
            TepPatient = ThuMucMau.filePath("patient.csv").toStdString();
            TepVisit = ThuMucMau.filePath("visit.csv").toStdString();
            Registry = DemoUi::KhoiDongTuDong(TepPatient,TepVisit,HangCho);
        } else if (CoBoFile)
            Registry = DemoUi::KhoiDongTuDong(TepPatient,TepVisit,HangCho);
        else Registry = DemoUi::ChonDuLieu(nullptr, &HangCho, &TepPatient, &TepVisit);
        if (Registry == nullptr) return 0;
        // Cửa sổ và service hủy trước Registry, kể cả khi phát sinh ngoại lệ.
        TriageService Triage(*Registry);
        Triage.KhoiPhucHangCho(HangCho);
        VisitSearchService TraCuu(*Registry);
        DemoWindow CuaSo(*Registry, Triage, TraCuu, TepPatient, TepVisit);
        DemoUi::NhoBoFileDaChon(ThuMucCauHinh.toStdString(),TepPatient,TepVisit);
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
