#include <QWidget>
#include <QString>
#include <QObject>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <system_error>
#include "util/CsvFile.h"
#include "dsa/Vector.h"
#include "ui/DemoStartup.h"
#include "ui/DemoStyle.h"
#include "repository/SessionFileManager.h"
#include "service/TriageService.h"
#include <QFileInfo>
#include <QDir>
#include "repository/PatientRegistry.h"
#include "util/My_stdexcept.h"
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace DemoUi {
bool DocBoFileGanNhat(const std::string &ThuMuc, std::string &TepPatient, std::string &TepVisit) {
    const auto DuongDan=std::filesystem::u8path(ThuMuc)/"bo-file.csv";
    std::error_code Loi;
    const bool TonTai=std::filesystem::exists(DuongDan,Loi);
    if (Loi) throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Khong kiem tra duoc cau hinh bo file");
    if (!TonTai) return false;
    std::ifstream Vao(DuongDan,std::ios::binary);
    if (!Vao) throw My_stdexcept(My_stdexcept::LoaiLoi::LoiThucThi,"Khong mo duoc cau hinh bo file");
    CsvFile::BoQuaBom(Vao); int Dong=1,Dau=1; Vector<std::string> D,Du;
    if (!CsvFile::DocBanGhi(Vao,D,Dong,Dau) || D.KichThuoc()!=4 || D[0]!="PBL2_FILES" || D[1]!="1" ||
        D[2].empty() || D[3].empty() || CsvFile::DocBanGhi(Vao,Du,Dong,Dau))
        throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,"Cau hinh bo file bi hong; dung --chon-du-lieu de chon lai");
    TepPatient=D[2]; TepVisit=D[3]; return true;
}
void NhoBoFileDaChon(const std::string &ThuMuc, const std::string &TepPatient, const std::string &TepVisit) {
    Vector<std::string> D;
    D.ThemCuoi("PBL2_FILES"); D.ThemCuoi("1"); D.ThemCuoi(TepPatient); D.ThemCuoi(TepVisit);
    std::ostringstream Ra; CsvFile::GhiBanGhi(Ra,D);
    CsvFile::GhiFileAnToan((std::filesystem::u8path(ThuMuc)/"bo-file.csv").u8string(),Ra.str());
}
PatientRegistry *KhoiDongTuDong(const std::string &TepPatient, const std::string &TepVisit, TrangThaiHangCho &HangCho) {
    return SessionFileManager().DocBoFile(TepPatient,TepVisit,HangCho);
}
PatientRegistry *ChonDuLieu(QWidget *Cha, TrangThaiHangCho *HangCho,
                           std::string *TepPatient, std::string *TepVisit) {
    QDialog Hop(Cha);
    Hop.setWindowTitle("An Tâm · Chọn dữ liệu khởi động");
    Hop.setStyleSheet(Qss()); Hop.resize(620, 340);
    auto *Cot = new QVBoxLayout(&Hop);
    auto *MoTa = Nhan("Chọn Patient CSV và Visit CSV (nếu có) để tiếp tục công việc.\nCó thể bắt đầu rỗng. Chương trình tự lưu sau mỗi thay đổi.", "notice");
    MoTa->setWordWrap(true); Cot->addWidget(MoTa);
    auto *DuongDan = new QLineEdit;
    DuongDan->setObjectName("duongDanCsv"); DuongDan->setAccessibleName("Đường dẫn patients.csv");
    DuongDan->setPlaceholderText("Đường dẫn patients.csv"); Cot->addWidget(DuongDan);
    auto *Chon = Nut("Chọn file CSV…", "chonCsv"); Cot->addWidget(Chon);
    auto *Visit = new QLineEdit;
    Visit->setObjectName("duongDanVisit"); Visit->setAccessibleName("Đường dẫn Visit CSV");
    Visit->setPlaceholderText("Visit CSV (để trống: dùng/tạo visit.csv cạnh Patient)"); Cot->addWidget(Visit);
    auto *ChonVisit = Nut("Chọn file Visit…", "chonVisit"); Cot->addWidget(ChonVisit);
    QObject::connect(DuongDan, &QLineEdit::textChanged, &Hop, [&] {
        const QString GanBen = QFileInfo(DuongDan->text()).dir().filePath("visit.csv");
        Visit->setText(QFileInfo::exists(GanBen) ? GanBen : "");
    });
    QObject::connect(ChonVisit, &QPushButton::clicked, &Hop, [&] {
        const QString Tep = QFileDialog::getOpenFileName(&Hop,"Chọn Visit CSV","","CSV (*.csv)");
        if (!Tep.isEmpty()) Visit->setText(Tep);
    });
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
            TrangThaiHangCho Tam;
            const std::string P = QFileInfo(DuongDan->text().trimmed()).absoluteFilePath().toStdString();
            const std::string V = Visit->text().trimmed().isEmpty()
                ? QFileInfo(DuongDan->text()).absoluteDir().filePath("visit.csv").toStdString()
                : QFileInfo(Visit->text().trimmed()).absoluteFilePath().toStdString();
            PatientRegistry *Moi = SessionFileManager().DocBoFile(P,V,Tam);
            try { if (HangCho) *HangCho = Tam; if (TepPatient) *TepPatient=P; if (TepVisit) *TepVisit=V; }
            catch (...) { delete Moi; throw; }
            KetQua = Moi;
            Hop.accept();
        } catch (const My_stdexcept &E) { Loi->setText(QString::fromUtf8(E.LayThongBao())); }
    });
    QObject::connect(Rong, &QPushButton::clicked, &Hop, [&] {
        if (TepPatient && TepVisit) {
            const QString P = QFileDialog::getSaveFileName(&Hop,"Chọn nơi đặt bộ dữ liệu mới","patient.csv","CSV (*.csv)");
            if (P.isEmpty()) return;
            const QString V = QFileDialog::getSaveFileName(&Hop,"Chọn file Visit của bộ dữ liệu mới",
                QFileInfo(P).dir().filePath("visit.csv"),"CSV (*.csv)");
            if (V.isEmpty()) return;
            if (QFileInfo(P).absoluteFilePath()==QFileInfo(V).absoluteFilePath()) {
                Loi->setText("Hai file Patient và Visit phải khác nhau."); return;
            }
            *TepPatient=QFileInfo(P).absoluteFilePath().toStdString();
            *TepVisit=QFileInfo(V).absoluteFilePath().toStdString();
        }
        TrangThaiHangCho Tam; if (HangCho) *HangCho=Tam;
        KetQua = new PatientRegistry; Hop.accept();
    });
    Hop.exec(); return KetQua;
}
}
