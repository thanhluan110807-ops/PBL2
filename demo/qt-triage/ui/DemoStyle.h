#pragma once
#include <string>
#include <ctime>
#include "model/Visit.h"
#include <QColor>
#include <QFrame>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QString>

namespace DemoUi {
QString Qss();
QString ThoiDiem(std::time_t Luc);
QString TenTrangThai(const Visit &LanKham);
QIcon BieuTuong(const QString &Ten, const QColor &Mau = QColor("#766a90"));
QLabel *Nhan(const QString &Chu, const char *VaiTro = "", QWidget *Cha = nullptr);
QPushButton *Nut(const QString &Chu, const char *Ma, bool Chinh = false);
QLabel *HuyHieu(int Muc);
QString Gio(long long Phut);
QString Chu(const std::string &VanBan);
QString ChuanHoa(const QString &VanBan);
} // namespace DemoUi
