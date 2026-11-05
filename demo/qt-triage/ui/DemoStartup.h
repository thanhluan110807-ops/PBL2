#pragma once
#include <string>
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
class QWidget;
namespace DemoUi {
// Chỉ nhớ đường dẫn bộ file; dữ liệu bệnh nhân vẫn nằm tại nơi người dùng chọn.
bool DocBoFileGanNhat(const std::string &ThuMucCauHinh, std::string &TepPatient, std::string &TepVisit);
void NhoBoFileDaChon(const std::string &ThuMucCauHinh, const std::string &TepPatient, const std::string &TepVisit);
// Mở bản đã công bố gần nhất; chỉ hiện hộp chọn CSV khi chưa có dữ liệu tự lưu.
PatientRegistry *KhoiDongTuDong(const std::string &TepPatient, const std::string &TepVisit, TrangThaiHangCho &HangCho);
// Trả Registry do bên gọi sở hữu; nullptr nếu đóng hộp thoại khởi động.
PatientRegistry *ChonDuLieu(QWidget *Cha = nullptr, TrangThaiHangCho *HangCho = nullptr,
                             std::string *TepPatient = nullptr, std::string *TepVisit = nullptr);
}
