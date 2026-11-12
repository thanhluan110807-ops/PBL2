#include "util/My_stdexcept.h"
#include <QPixmap>
#include <QString>
#include <QWidget>
#include "ui/DemoWindow.h"
#include "ui/AdminPanel.h"
#include <QByteArray>
#include <QtGlobal>
#include "ui/DemoStyle.h"
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
#include "service/VisitSearchService.h"
#include "model/Patient.h"
#include "model/Visit.h"
#include "model/Gender.h"
#include <QApplication>
#include <QTemporaryDir>
#include <QComboBox>
#include <QDateTime>
#include <QKeyEvent>
#include <QTimeZone>
#include <QListWidget>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QTimer>
#include <cassert>
#include <ctime>

template<class T> T* Tim(QWidget* Cha, const char* Ma) {
    auto* KetQua = Cha->findChild<T*>(Ma);
    assert(KetQua);
    return KetQua;
}
QWidget* Hop() { auto* H = QApplication::activeModalWidget(); assert(H); return H; }
void Nhap(QWidget* H, const char* Ma, const QString& Chu) { Tim<QLineEdit>(H, Ma)->setText(Chu); }
void Bam(QWidget* H, const char* Ma) { Tim<QPushButton>(H, Ma)->click(); }

int main(int SoThamSo, char** ThamSo) {
    QApplication UngDung(SoThamSo, ThamSo);
    // Kết quả UTC+7 không phụ thuộc múi giờ máy; kiểm tra cả mốc đổi ngày.
    assert(DemoUi::ThoiDiem(0) == "01/01/1970 07:00");
    assert(DemoUi::ThoiDiem(17 * 3600) == "02/01/1970 00:00");
    PatientRegistry R;
    TriageService T(R);
    VisitSearchService S(R);
    QTemporaryDir KhoDuLieu; assert(KhoDuLieu.isValid());
    qputenv("XDG_DATA_HOME",KhoDuLieu.path().toUtf8());
    DemoWindow W(R, T, S, (KhoDuLieu.path()+"/patient.csv").toStdString(), (KhoDuLieu.path()+"/visit.csv").toStdString());
    W.show(); UngDung.processEvents();
    assert(!W.findChild<QLineEdit*>("timChung"));
    // Nhân sự/lịch là trang trong cửa sổ chính, không mở hộp thoại riêng.
    Bam(&W,"openAdmin");
    auto* Trang=W.findChild<QStackedWidget*>("mainPages");
    auto* QuanTri=W.findChild<AdminPanel*>("adminPanel");
    assert(Trang && QuanTri && Trang->currentWidget()==QuanTri);
    assert(QApplication::activeModalWidget()==nullptr);
    assert(!QuanTri->findChild<QPushButton*>("adminClose")->isVisible());
    QKeyEvent Esc(QEvent::KeyPress,Qt::Key_Escape,Qt::NoModifier);
    UngDung.sendEvent(QuanTri,&Esc);
    assert(Trang->currentWidget()==QuanTri && QuanTri->isVisible());
    for (int I=0; I<5; ++I)
        assert(Tim<QPushButton>(&W,qPrintable(QString("nav%1").arg(I)))->isVisible());
    Bam(QuanTri,"adminSeed");
    assert(Tim<QTableWidget>(QuanTri,"staffTable")->rowCount()==4);
    if (!qEnvironmentVariable("PBL2_MAIN_ADMIN_CAPTURE").isEmpty()) {
        UngDung.processEvents();
        assert(W.grab().save(qEnvironmentVariable("PBL2_MAIN_ADMIN_CAPTURE")));
    }
    Bam(&W,"nav4");
    assert(Trang->currentWidget()!=QuanTri);
    Bam(&W,"openAdmin");
    assert(Tim<QTableWidget>(QuanTri,"staffTable")->rowCount()==4);
    QTimer::singleShot(0, [] {
        auto* H=qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
        assert(H && H->button(QMessageBox::No));
        H->button(QMessageBox::No)->click();
    });
    assert(!W.close() && W.isVisible());
    Bam(QuanTri,"adminSave");
    assert(QApplication::activeModalWidget()==nullptr);
    assert(!Tim<QPushButton>(QuanTri,"adminSave")->isEnabled());
    AdminPanel MoLaiNhanSu(nullptr,QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/nhan-su-lich.pbl2");
    assert(MoLaiNhanSu.LayDuLieu().NhanSu->KichThuoc()==4);
    Bam(&W,"nav0");
    assert(!Tim<QLabel>(&W,"tinhTrangLuu")->text().contains("Nơi lưu"));
    assert(!Tim<QLabel>(&W,"tinhTrangLuu")->text().contains(KhoDuLieu.path()));
    for (auto* N : W.findChildren<QLabel*>()) {
        assert(!N->text().contains("Mức 1 ưu tiên cao nhất"));
        assert(!N->text().contains("Thời điểm hiển thị theo UTC+7"));
    }
    assert(!W.findChild<QPushButton*>("baMot"));
    assert(!W.findChild<QPushButton*>("tienGio"));
    QTimer::singleShot(0, [&] {
        auto* H = Hop();
        assert(Tim<QLineEdit>(H,"MaBenhNhan")->isReadOnly()); Nhap(H, "HoTen", "Nguyễn An");
        Nhap(H, "Tuoi", "30"); assert(Tim<QLineEdit>(H,"MaLanKham")->isReadOnly());
        Nhap(H, "TrieuChung", "Đau đầu"); Bam(H, "luu");
    });
    W.MoTiepNhan();
    assert(R.SoBenhNhan() == 1 && R.SoLanKham() == 1);
    QTimer::singleShot(0, [&] {
        auto* H = Hop();
        assert(!H->findChild<QComboBox*>("Muc"));
        if (!qEnvironmentVariable("PBL2_VITAL_GUIDE_CAPTURE").isEmpty()) {
            UngDung.processEvents();
            assert(H->grab().save(qEnvironmentVariable("PBL2_VITAL_GUIDE_CAPTURE")));
        }
        const QSize KichThuocCu=H->size();
        H->resize(496,752); UngDung.processEvents();
        QApplication::sendPostedEvents(nullptr,QEvent::LayoutRequest);
        UngDung.processEvents();
        assert(Tim<QScrollArea>(H,"dialogScroll")->horizontalScrollBar()->maximum()==0);
        if (!qEnvironmentVariable("PBL2_VITAL_GUIDE_NARROW").isEmpty())
            assert(H->grab().save(qEnvironmentVariable("PBL2_VITAL_GUIDE_NARROW")));
        assert(!H->findChild<QLineEdit*>("MaDieuDuong"));
        for (const auto* Ma : {"Mach","HuyetAp","NhipTho","SpO2","GCS"}) {
            auto* O=Tim<QLineEdit>(H,Ma);
            auto* GoiY=Tim<QLabel>(H,qPrintable(QString(Ma)+"Hint"));
            assert(GoiY->y()>=O->y()+O->height());
        }
        H->resize(KichThuocCu); UngDung.processEvents();
        Nhap(H, "Mach", "sai"); Nhap(H, "HuyetAp", "120");
        Nhap(H, "NhipTho", "18"); Nhap(H, "SpO2", "98"); Nhap(H, "GCS", "15");
        Bam(H, "luu");
        assert(!Tim<QLabel>(H, "loiTong")->text().isEmpty());
        assert(R.TimLanKham("LK001")->SoDanhGia() == 0);
        Nhap(H, "Mach", "80"); Bam(H, "luu");
    });
    W.MoDanhGia("LK001");
    assert(T.SoDangCho() == 1);
    assert(R.TimLanKham("LK001")->LayDanhGiaGanNhat().LayMuc() == 5);
    assert(R.TimLanKham("LK001")->LayDanhGiaGanNhat().LayMaDieuDuong().empty());
    assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 1);
    Nhap(&W, "maBacSi", "BS1"); Bam(&W, "goiTiep");
    assert(R.TimLanKham("LK001")->LayTrangThai() == VisitStatus::DangKham);
    QTimer::singleShot(0, [&] {
        auto* H = Hop(); Nhap(H, "NoiDung", "Xét nghiệm");
        Nhap(H, "MaBacSi", "BS2"); Bam(H, "luu");
        assert(!Tim<QLabel>(H, "loiTong")->text().isEmpty());
        Nhap(H, "MaBacSi", "BS1"); Bam(H, "luu");
    });
    W.MoCanLamSang("LK001", false);
    assert(R.TimLanKham("LK001")->LayTrangThai() == VisitStatus::ChoCanLamSang);
    QTimer::singleShot(0, [&] { Nhap(Hop(), "NoiDung", "Bình thường"); Bam(Hop(), "luu"); });
    W.MoCanLamSang("LK001", true);
    assert(T.SoDangCho() == 1);
    Patient P("BN2", "Bình", Gender::Nam); P.DatTuoiUocLuong(30); R.ThemBenhNhan(P);
    T.TiepNhan("LK2", "BN2", std::time(nullptr), "Khám");
    T.DanhGia("LK2", VitalSigns(80,120,18,98,15), std::time(nullptr), "DD1");
    W.LamMoi();
    UngDung.processEvents();
    assert(W.grab().save("demo-core-workflow.png"));
    // Lọc không tiêu thụ lượt hòa: lần đầu vẫn nhường quay lại ở lần hòa đầu tiên.
    Bam(&W, "locLuong1"); Bam(&W, "goiTiep");
    assert(R.TimLanKham("LK001")->LayTrangThai() == VisitStatus::DangKham);
    assert(R.TimLanKham("LK2")->LayTrangThai() == VisitStatus::ChoKhamLanDau);
    Bam(&W, "nav3"); Nhap(&W, "timHang", "nguyễn");
    assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 1);
    Nhap(&W, "maLanKhamLoc", "LK2");
    assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 0);
    Nhap(&W, "maLanKhamLoc", "LK001");
    assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 1);
    // KPI phải mở cả nhóm chờ khám, bỏ bộ lọc chính xác LK1 của tra cứu trước.
    Nhap(&W, "ngayLoc", "01/01/2000"); Bam(&W, "xemKpi1");
    assert(Tim<QLineEdit>(&W, "timHang")->text().isEmpty());
    assert(Tim<QLineEdit>(&W, "maLanKhamLoc")->text().isEmpty());
    assert(Tim<QLineEdit>(&W, "ngayLoc")->text().isEmpty());
    assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 1);
    R.ThemBenhNhan("BN3", "Chưa khám"); Bam(&W, "nav4");
    assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 3);
    // Trùng mã Visit phải bị chặn trước khi tạo Patient mới.
    bool TrungMa = false;
    try { R.ThemLanKham("LK001", "BN001", std::time(nullptr), "Khám"); }
    catch (const My_stdexcept&) { TrungMa = true; }
    assert(TrungMa);
    QTimer::singleShot(0, [&] {
        auto *H = Hop(); Nhap(H, "MaBenhNhan", "BN_KHONG_TAO");
        Nhap(H, "MaLanKham", "LK001"); Nhap(H, "TrieuChung", ""); Bam(H, "luu");
        assert(!Tim<QLabel>(H, "loiTong")->text().isEmpty());
        assert(R.TimBenhNhan("BN_KHONG_TAO") == nullptr); Bam(H, "dongHop");
    });
    W.MoTiepNhan();
    // Chọn hồ sơ đã có và đang khám: không mở thêm Visit.
    QTimer::singleShot(0, [&] {
        auto *H = Hop(); Tim<QComboBox>(H, "cheDo")->setCurrentIndex(1);
        Nhap(H, "timHoSo", "BN001"); auto *L = Tim<QListWidget>(H, "chonHoSo");
        assert(L->count() == 1); L->setCurrentRow(0);
        Nhap(H, "MaLanKham", "LK_KHONG_TAO"); Nhap(H, "TrieuChung", "Khám"); Bam(H, "luu");
        assert(R.TimLanKham("LK_KHONG_TAO") == nullptr);
        assert(!Tim<QLabel>(H, "loiTong")->text().isEmpty()); Bam(H, "dongHop");
    });
    W.MoTiepNhan();
    // Đổi danh tính qua Registry giữ nguyên địa chỉ hồ sơ và lịch sử.
    const Patient *Cu = R.TimBenhNhan("BN001");
    QTimer::singleShot(0, [&] {
        auto *H = Hop(); Bam(H,"suaThongTin"); Nhap(H, "HoTen", "Nguyễn An mới");
        Nhap(H, "NgaySinh", "01/01/1996"); Nhap(H, "CanCuoc", "001"); Bam(H, "luu");
        Bam(H,"dongHop");
    });
    W.MoHoSo("BN001"); assert(R.TimBenhNhan("BN001") == Cu);
    assert(Cu->LayHoTen() == "Nguyễn An mới" && R.TimLanKham("LK001")->SoDanhGia() == 1);
    Bam(&W, "nav3"); Nhap(&W, "timHang", "an mới");
    const QString Ngay = QDateTime::fromSecsSinceEpoch(R.TimLanKham("LK001")->LayThoiDiemTiepNhan(), QTimeZone(0))
        .addSecs(7*3600).toString("dd/MM/yyyy");
    Nhap(&W, "ngayLoc", Ngay); assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 1);
    Nhap(&W, "ngayLoc", "31/02/2026"); assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 0);
    assert(Tim<QLabel>(&W, "soKetQua")->text().contains("không hợp lệ"));
    Nhap(&W, "ngayLoc", "01/01/2000"); assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 0);
    // Hộp chi tiết của Visit đang khám không cho đánh giá lại.
    QTimer::singleShot(0, [&] {
        assert(!Hop()->findChild<QPushButton*>("danhGia"));
        assert(Hop()->findChild<QPushButton*>("chuyenCls")); Bam(Hop(), "dongHop");
    });
    W.MoChiTiet("LK001");
    QTimer::singleShot(0, [] { Bam(Hop(), "xacNhanDong"); }); W.XacNhanDong("LK001", false);
    assert(R.TimLanKham("LK001")->LayTrangThai() == VisitStatus::DaKetThuc);
    QTimer::singleShot(0, [] { static_cast<QDialog*>(Hop())->reject(); }); W.XacNhanDong("LK2", true);
    assert(T.SoDangCho() == 1);
    QTimer::singleShot(0, [] { Bam(Hop(), "xacNhanDong"); }); W.XacNhanDong("LK2", true);
    assert(R.TimLanKham("LK2")->LayTrangThai() == VisitStatus::DaHuy && T.SoDangCho() == 0);
    Bam(&W, "nav4");
    W.resize(520, 800); UngDung.processEvents();
    assert(Tim<QScrollArea>(&W, "mainScroll")->horizontalScrollBar()->maximum() == 0);
    QTimer::singleShot(0, [] {
        for (auto* N : Hop()->findChildren<QPushButton*>("chonTrang"))
            if (N->text()=="Nhân sự và lịch trực") { N->click(); return; }
        assert(false);
    });
    Bam(&W,"menu"); UngDung.processEvents();
    assert(W.width()==520 && Trang->currentWidget()==QuanTri);
    auto* OMa=Tim<QLineEdit>(QuanTri,"staffId");
    auto* NapMau=Tim<QPushButton>(QuanTri,"adminSeed");
    if (!qEnvironmentVariable("PBL2_MAIN_ADMIN_NARROW").isEmpty())
        assert(W.grab().save(qEnvironmentVariable("PBL2_MAIN_ADMIN_NARROW")));
    assert(OMa->mapTo(&W,QPoint(OMa->width(),0)).x()<=W.width());
    assert(NapMau->mapTo(&W,QPoint(NapMau->width(),0)).x()<=W.width());
    Bam(&W,"nav4");
    assert(W.grab().save("demo-core-narrow.png"));
    W.resize(1440, 980); UngDung.processEvents();
    Bam(&W, "nav0"); assert(W.grab().save("demo-core-wide.png"));
}
