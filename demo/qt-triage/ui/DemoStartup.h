#pragma once
#include "repository/PatientRegistry.h"
class QWidget;
namespace DemoUi {
// Trả Registry do bên gọi sở hữu; nullptr nếu đóng hộp thoại khởi động.
PatientRegistry *ChonDuLieu(QWidget *Cha = nullptr);
}
