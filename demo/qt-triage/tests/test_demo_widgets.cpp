#include "ui/DemoWindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTableWidget>
#include <QTimer>
#include <cassert>
#include <iostream>

template <class T> T *Tim(QWidget *Cha, const char *Ma) {
    auto *K = Cha->findChild<T *>(Ma);
    assert(K);
    return K;
}
QString ThuMucAnh;
void Chup(QWidget *W, const char *Ten) {
    if (!ThuMucAnh.isEmpty()) {
        QDir().mkpath(ThuMucAnh);
        assert(W->grab().save(ThuMucAnh + "/" + Ten));
    }
}
QWidget *Hop() {
    auto *H = QApplication::activeModalWidget();
    assert(H);
    return H;
}
void Bam(QWidget *Cha, const char *Ma) {
    Tim<QPushButton>(Cha, Ma)->click();
}
void Nhap(QWidget *Cha, const char *Ma, const QString &Chu) {
    Tim<QLineEdit>(Cha, Ma)->setText(Chu);
}
void NhapSoDo(QWidget *H, int Muc = 1) {
    Nhap(H, "Mach", "80");
    Nhap(H, "HuyetAp", "120");
    Nhap(H, "NhipTho", "18");
    Nhap(H, "SpO2", "98");
    Nhap(H, "GCS", "15");
    Tim<QComboBox>(H, "Muc")->setCurrentIndex(Muc);
}
void DongChiTietSau() {
    QTimer::singleShot(0, [] { Bam(Hop(), "dongChiTiet"); });
}

int main(int SoThamSo, char **ThamSo) {
    QApplication U(SoThamSo, ThamSo);
    ThuMucAnh = qEnvironmentVariable("PBL2_DEMO_CAPTURE_DIR");
    QApplication::setStyle("Fusion");
    auto *D = new Demo::DemoTriageService;
    D->NapMau();
    {
        DemoWindow W(*D);
        W.show();
        U.processEvents();
        assert(Tim<QTableWidget>(&W, "hangCho")->rowCount() == 8);
        assert(Tim<QLabel>(&W, "kpi0")->text() == "02");
        // Bộ lọc không làm thay đổi lượt thực sự được gọi.
        const int Dau = D->LayHangCho().CacLuot[0];
        Tim<QComboBox>(&W, "locMuc")->setCurrentIndex(5);
        Nhap(&W, "timHang", "không có");
        Bam(&W, "goiTiep");
        assert(D->LayLuot(Dau).TinhTrang == Demo::TrangThai::DangKham);
        Nhap(&W, "timHang", "");
        Tim<QComboBox>(&W, "locMuc")->setCurrentIndex(0);
        // Tiếp nhận thật qua các trường native; kiểm tra lỗi không mất dữ liệu.
        QTimer::singleShot(0, [&] {
            auto *H = Hop();
            Nhap(H, "HoTen", "Nguyễn Thử Nghiệm");
            Nhap(H, "NgaySinh", "10/10/1995");
            Bam(H, "luu");
            assert(Tim<QLineEdit>(H, "HoTen")->text() == "Nguyễn Thử Nghiệm");
            assert(!Tim<QLabel>(H, "errTrieuChung")->text().isEmpty());
            Tim<QPlainTextEdit>(H, "TrieuChung")->setPlainText("Đau đầu mẫu");
            DongChiTietSau();
            Bam(H, "luu");
        });
        Bam(&W, "tiepNhan");
        assert(D->LaySoLuot() == 15);
        const int Moi = 14;
        // Không điền sẵn sinh hiệu; đề xuất và xác nhận tách nhau.
        QTimer::singleShot(0, [&] {
            auto *H = Hop();
            assert(Tim<QLineEdit>(H, "Mach")->text().isEmpty());
            Bam(H, "luu");
            assert(!Tim<QLabel>(H, "errMach")->text().isEmpty());
            NhapSoDo(H);
            Bam(H, "xemDeXuat");
            assert(Tim<QLabel>(H, "deXuat")->text().contains("Mức 5"));
            Bam(H, "luu");
        });
        W.MoDanhGia(Moi);
        assert(D->LayLuot(Moi).Muc == 1);
        assert(D->LayHangCho().CacLuot[0] == Moi);
        Bam(&W, "goiTiep");
        assert(D->LayLuot(Moi).TinhTrang == Demo::TrangThai::DangKham);
        // Hai vòng cận lâm sàng, cùng mã lượt.
        for (int Lan = 0; Lan < 2; ++Lan) {
            QTimer::singleShot(0, [] {
                Tim<QPlainTextEdit>(Hop(), "NoiDung")->setPlainText("Xét nghiệm mẫu");
                Bam(Hop(), "luu");
            });
            W.MoCanLamSang(Moi, false);
            Bam(&W, "tienGio");
            QTimer::singleShot(0, [] {
                Tim<QPlainTextEdit>(Hop(), "NoiDung")->setPlainText("Kết quả mẫu");
                Bam(Hop(), "luu");
            });
            W.MoCanLamSang(Moi, true);
            assert(D->LayLuot(Moi).TinhTrang == Demo::TrangThai::QuayLai);
            Bam(&W, "goiTiep");
            assert(D->LayLuot(Moi).TinhTrang == Demo::TrangThai::DangKham);
        }
        QTimer::singleShot(0, [] {
            auto *M = qobject_cast<QMessageBox *>(Hop());
            assert(M);
            for (auto *B : M->buttons())
                if (B->text() == "Xác nhận kết thúc") {
                    B->click();
                    break;
                }
        });
        W.XacNhanDong(Moi, false);
        assert(D->LayLuot(Moi).TinhTrang == Demo::TrangThai::KetThuc);
        // Bổ sung danh tính, mã ổn định.
        const auto Ma = D->LayHoSo(2).Ma;
        QTimer::singleShot(0, [] {
            auto *H = Hop();
            Tim<QCheckBox>(H, "ChuaRo")->setChecked(false);
            Nhap(H, "HoTen", "Nguyễn Đã Xác Minh");
            Nhap(H, "Tuoi", "32");
            Bam(H, "luu");
        });
        W.MoHoSo(2);
        assert(D->LayHoSo(2).Ma == Ma && D->LayLuot(2).BenhNhan == 2);
        // Đánh giá lại vẫn giữ mốc vào hàng, lịch sử tăng.
        const auto Vao = D->LayLuot(6).VaoHangLuc;
        QTimer::singleShot(0, [] {
            NhapSoDo(Hop(), 1);
            Bam(Hop(), "luu");
        });
        W.MoDanhGia(6);
        assert(D->LayLuot(6).SoDanhGia == 2 && D->LayLuot(6).VaoHangLuc == Vao);
        assert(D->LayHangCho().CacLuot[0] == 6);
        // Đóng hộp thoại bằng Esc qua sự kiện bàn phím native.
        QTimer::singleShot(0, [] {
            QKeyEvent E(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
            QApplication::sendEvent(Hop(), &E);
        });
        W.MoDanhGia(6);
        assert(!QApplication::activeModalWidget());
        // Kịch bản 3:1 nạp bằng nút, xác nhận QMessageBox.
        QTimer::singleShot(0, [] {
            auto *M = qobject_cast<QMessageBox *>(Hop());
            assert(M);
            M->button(QMessageBox::Yes)->click();
        });
        Bam(&W, "baMot");
        const auto Hang = D->LayHangCho();
        for (int I = 0; I < 7; ++I) {
            Bam(&W, "goiTiep");
            assert(D->LayLuot(Hang.CacLuot[I]).TinhTrang == Demo::TrangThai::DangKham);
        }
        assert(!Tim<QPushButton>(&W, "goiTiep")->isEnabled());
        // Khung hẹp: đổi sang thẻ, vẫn thấy mức/thời gian/thao tác.
        D->NapMau();
        W.LamMoi();
        W.resize(600, 900);
        U.processEvents();
        assert(!Tim<QTableWidget>(&W, "hangCho")->isVisible());
        assert(Tim<QPushButton>(&W, "theChiTiet0")->isVisible());
        assert(Tim<QScrollArea>(&W, "mainScroll")->horizontalScrollBar()->maximum() == 0);
        Chup(&W, "narrow.png");
        W.resize(1440, 980);
        U.processEvents();
        Chup(&W, "desktop.png");
        QTimer::singleShot(0, [] {
            Chup(Hop(), "visit-detail.png");
            Bam(Hop(), "dongChiTiet");
        });
        W.MoChiTiet(0);
    }
    delete D;
    std::cout << "PASS: native widgets intake/assessment/queue/lab/identity/history/3:1/Esc/responsive\n";
}
