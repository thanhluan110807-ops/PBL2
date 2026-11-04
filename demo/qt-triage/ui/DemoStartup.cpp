#include <QWidget>
#include <QString>
#include <QObject>
#include <string>
#include "ui/DemoStartup.h"
#include "ui/DemoStyle.h"
#include "repository/PatientFileManager.h"
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace DemoUi {
PatientRegistry *ChonDuLieu(QWidget *Cha) {
    QDialog Hop(Cha);
    Hop.setWindowTitle("An Tâm · Chọn dữ liệu khởi động");
    Hop.setStyleSheet(Qss()); Hop.resize(620, 340);
    auto *Cot = new QVBoxLayout(&Hop);
    auto *MoTa = Nhan("Chọn patients.csv hoặc bắt đầu với danh sách rỗng.\nChỉ nạp hồ sơ bệnh nhân; chưa nạp lần khám và chưa lưu thay đổi khi thoát.", "notice");
    MoTa->setWordWrap(true); Cot->addWidget(MoTa);
    auto *DuongDan = new QLineEdit;
    DuongDan->setObjectName("duongDanCsv"); DuongDan->setAccessibleName("Đường dẫn patients.csv");
    DuongDan->setPlaceholderText("Đường dẫn patients.csv"); Cot->addWidget(DuongDan);
    auto *Chon = Nut("Chọn file CSV…", "chonCsv"); Cot->addWidget(Chon);
    auto *Loi = Nhan("", "error"); Loi->setObjectName("loiCsv"); Loi->setWordWrap(true); Cot->addWidget(Loi);
    auto *Doc = Nut("Đọc CSV & bắt đầu", "docCsv", true); Cot->addWidget(Doc);
    auto *Rong = Nut("Bắt đầu rỗng", "batDauRong"); Cot->addWidget(Rong);
    // Chỉ tạo service sau khi hộp thoại trả Registry thành công.
    PatientRegistry *KetQua = nullptr;
    QObject::connect(Chon, &QPushButton::clicked, &Hop, [&] {
        const QString Tep = QFileDialog::getOpenFileName(&Hop, "Chọn hồ sơ bệnh nhân", "", "CSV (*.csv)");
        if (!Tep.isEmpty()) DuongDan->setText(Tep);
    });
    QObject::connect(Doc, &QPushButton::clicked, &Hop, [&] {
        try {
            KetQua = PatientFileManager().DocBenhNhan(DuongDan->text().trimmed().toStdString());
            Hop.accept();
        } catch (const My_stdexcept &E) { Loi->setText(QString::fromUtf8(E.LayThongBao())); }
    });
    QObject::connect(Rong, &QPushButton::clicked, &Hop, [&] { KetQua = new PatientRegistry; Hop.accept(); });
    Hop.exec(); return KetQua;
}
}
