#pragma once
#include <string>
#include "repository/PatientRegistry.h"
#include "service/TriageService.h"
class QWidget;
namespace DemoUi {

bool DocBoFileGanNhat(const std::string &ThuMucCauHinh, std::string &TepPatient, std::string &TepVisit);
void NhoBoFileDaChon(const std::string &ThuMucCauHinh, const std::string &TepPatient, const std::string &TepVisit);

PatientRegistry *KhoiDongTuDong(const std::string &TepPatient, const std::string &TepVisit, TrangThaiHangCho &HangCho);

PatientRegistry *ChonDuLieu(QWidget *Cha = nullptr, TrangThaiHangCho *HangCho = nullptr,
                             std::string *TepPatient = nullptr, std::string *TepVisit = nullptr);
}
