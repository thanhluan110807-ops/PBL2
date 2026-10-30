#include "ui/DemoStyle.h"
#include <QPainter>
#include <QPainterPath>

namespace DemoUi {
QString Qss() {
    return QString::fromUtf8(R"(
QWidget { font-family: "Inter", "Segoe UI", "DejaVu Sans"; font-size: 14px; color: #28282B; }
QMainWindow, QDialog, QWidget#content, QWidget#dialogContent { background: #F6F5FA; }
QFrame#sidebar, QFrame#panel, QFrame#card { background:#FDFCFE; border:1px solid #ECEBF0; border-radius:17px; }
QFrame#sidebar { border-radius:0; border-right:1px solid #ECEBF0; }
QFrame#header { background:#F6F5FA; border-bottom:1px solid #ECEBF0; }
QFrame#subtle { background:#F5F4F8; border:0; border-radius:10px; }
QLabel { background:transparent; border:0; }
QLabel[role="title"] { font-size:27px; font-weight:650; }
QLabel[role="section"] { font-size:18px; font-weight:600; }
QLabel[role="muted"] { font-size:12px; color:#686571; }
QLabel[role="number"] { font-size:29px; font-weight:600; }
QLabel[role="brand"] { font-size:23px; font-weight:700; }
QLabel[role="error"] { font-size:12px; color:#AC2841; }
QLabel[role="status"] { color:#6545B5; background:#EEE9FD; border-radius:7px; padding:8px 12px; }
QLabel[role="notice"] { color:#614686; background:#F0EAFC; border-radius:9px; padding:12px; }
QPushButton { background:#FDFCFE; border:1px solid #DCD7E6; border-radius:9px; padding:9px 13px; min-height:24px; }
QPushButton:hover { background:#F1EDFB; }
QPushButton:pressed { background:#E5DCFA; }
QPushButton:focus, QLineEdit:focus, QComboBox:focus, QPlainTextEdit:focus { border:2px solid #7050F2; }
QPushButton[primary="true"] { color:white; background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #7856ED,stop:1 #6A47EB); border:1px solid #7050F2; font-weight:600; }
QPushButton[primary="true"]:hover { background:#5E3AD4; }
QPushButton:disabled { color:#8A8396; background:#F0EEF3; border-color:#ECEBF0; }
QPushButton[nav="true"] { text-align:left; border:0; background:transparent; color:#686571; padding:12px; }
QPushButton[nav="true"]:checked { color:#593AB8; background:#EEE9FA; font-weight:600; }
QPushButton[tab="true"] { background:#F5F4F8; border:0; font-size:13px; }
QPushButton[tab="true"]:checked { background:#EEE9FD; color:#5E3AD4; }
QPushButton[danger="true"] { color:#AC2841; border-color:#EBCBD3; background:#FFF3F6; }
QPushButton[link="true"] { color:#6240C8; background:#F8F5FF; border-color:#E7E0F7; font-size:13px; }
QLineEdit, QComboBox, QPlainTextEdit { background:white; border:1px solid #D6D1DE; border-radius:8px; padding:9px 10px; min-height:22px; selection-background-color:#DDD2FA; }
QLineEdit[invalid="true"], QPlainTextEdit[invalid="true"] { border:1px solid #B62D49; background:#FFF8F9; }
QComboBox::drop-down { border:0; width:22px; }
QTableWidget { background:#FDFCFE; border:0; gridline-color:#ECEBF0; selection-background-color:#EEE9FA; selection-color:#28282B; }
QTableWidget::item { padding:9px; border-bottom:1px solid #F0EDF5; }
QHeaderView::section { background:#F7F5FA; color:#686571; font-size:12px; font-weight:600; padding:12px 7px; border:0; border-bottom:1px solid #ECEBF0; }
QScrollArea { background:transparent; border:0; }
QScrollBar:vertical { background:#F2EFF7; width:9px; border-radius:4px; }
QScrollBar::handle:vertical { background:#CFC5E4; border-radius:4px; min-height:30px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
QCheckBox { spacing:9px; padding:6px 0; }
QCheckBox::indicator { width:18px; height:18px; }
QGroupBox { border:1px solid #ECEBF0; border-radius:12px; margin-top:14px; padding:17px; background:#FDFCFE; }
QGroupBox::title { subcontrol-origin:margin; left:14px; padding:0 4px; font-weight:600; }
QStatusBar { background:#F0EBFA; color:#5E467E; font-size:13px; }
)");
}
QLabel *Nhan(const QString &Chu, const char *VaiTro, QWidget *Cha) {
    auto *NhanMoi = new QLabel(Chu, Cha);
    NhanMoi->setTextFormat(Qt::PlainText);
    NhanMoi->setProperty("role", VaiTro);
    return NhanMoi;
}
QPushButton *Nut(const QString &Chu, const char *Ma, bool Chinh) {
    auto *NutMoi = new QPushButton(Chu);
    NutMoi->setObjectName(Ma);
    NutMoi->setProperty("primary", Chinh);
    NutMoi->setCursor(Qt::PointingHandCursor);
    NutMoi->setMinimumHeight(42);
    return NutMoi;
}
QLabel *HuyHieu(int Muc) {
    const char *Nen[] = {"#F1EDF8", "#FCE8ED", "#FFF0E3", "#FFF6DB", "#E8F5ED", "#EAF1FE"};
    const char *Chu[] = {"#655B76", "#AC2841", "#A34C0B", "#8B630D", "#27704A", "#3167AD"};
    auto *N = Nhan(Muc ? QString("●  Mức %1").arg(Muc) : "Chưa phân loại");
    N->setAlignment(Qt::AlignCenter);
    N->setStyleSheet(
        QString("color:%1;background:%2;border-radius:7px;padding:6px 9px;font-size:12px;font-weight:600;")
            .arg(Chu[Muc], Nen[Muc]));
    N->setMaximumHeight(30);
    return N;
}
QString Gio(long long Phut) {
    return QString("%1:%2").arg((Phut / 60) % 24, 2, 10, QChar('0')).arg(Phut % 60, 2, 10, QChar('0'));
}
QString Chu(const std::string &VanBan) {
    return QString::fromUtf8(VanBan.c_str());
}
QString ChuanHoa(const QString &VanBan) {
    QString KetQua;
    const QString DaTach = VanBan.toLower().normalized(QString::NormalizationForm_D);
    for (QChar KyTu : DaTach)
        if (KyTu.category() != QChar::Mark_NonSpacing)
            KetQua += KyTu == QChar(0x0111) ? QChar('d') : KyTu;
    return KetQua;
}
QIcon BieuTuong(const QString &Ten, const QColor &Mau) {
    QPixmap Anh(48, 48);
    Anh.fill(Qt::transparent);
    QPainter Ve(&Anh);
    Ve.setRenderHint(QPainter::Antialiasing);
    Ve.scale(2, 2);
    Ve.setPen(QPen(Mau, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (Ten == "plus") {
        Ve.drawLine(12, 5, 12, 19);
        Ve.drawLine(5, 12, 19, 12);
    } else if (Ten == "grid") {
        for (int X : {4, 14})
            for (int Y : {4, 14})
                Ve.drawRoundedRect(X, Y, 6, 6, 1, 1);
    } else if (Ten == "clock") {
        Ve.drawEllipse(3, 3, 18, 18);
        Ve.drawLine(12, 7, 12, 12);
        Ve.drawLine(12, 12, 16, 14);
    } else if (Ten == "users") {
        Ve.drawEllipse(8, 3, 8, 8);
        Ve.drawArc(4, 12, 16, 14, 0, 180 * 16);
    } else if (Ten == "pulse") {
        QPainterPath Duong;
        Duong.moveTo(2, 12);
        Duong.lineTo(7, 12);
        Duong.lineTo(10, 5);
        Duong.lineTo(14, 19);
        Duong.lineTo(17, 12);
        Duong.lineTo(22, 12);
        Ve.drawPath(Duong);
    } else if (Ten == "return") {
        Ve.drawLine(9, 3, 4, 8);
        Ve.drawLine(4, 8, 9, 13);
        Ve.drawLine(4, 8, 15, 8);
        Ve.drawArc(10, 8, 10, 10, -90 * 16, 180 * 16);
        Ve.drawLine(15, 18, 8, 18);
    } else if (Ten == "alert") {
        QPainterPath D;
        D.moveTo(12, 3);
        D.lineTo(22, 21);
        D.lineTo(2, 21);
        D.closeSubpath();
        Ve.drawPath(D);
        Ve.drawLine(12, 9, 12, 14);
        Ve.drawPoint(12, 18);
    } else if (Ten == "search") {
        Ve.drawEllipse(3, 3, 13, 13);
        Ve.drawLine(15, 15, 21, 21);
    } else if (Ten == "arrow") {
        Ve.drawLine(4, 12, 20, 12);
        Ve.drawLine(14, 6, 20, 12);
        Ve.drawLine(14, 18, 20, 12);
    } else {
        for (int Y : {6, 12, 18}) {
            Ve.drawLine(8, Y, 21, Y);
            Ve.drawPoint(3, Y);
        }
    }
    return QIcon(Anh);
}
} // namespace DemoUi
