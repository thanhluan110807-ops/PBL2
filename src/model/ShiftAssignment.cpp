#include "model/ShiftAssignment.h"
#include "util/My_stdexcept.h"
namespace {
void Loi() { throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe, "Du lieu hoac chuyen trang thai phan cong khong hop le"); }
}
ShiftAssignment::ShiftAssignment(const std::string& N, const std::string& C, AssignmentRole V)
    : ShiftAssignment("PC-" + std::to_string(C.size()) + ":" + C + N, N, C, V) {}
ShiftAssignment::ShiftAssignment(const std::string& M, const std::string& N, const std::string& C, AssignmentRole V)
    : MaPhanCong(M), MaNhanVien(N), MaCa(C), VaiTro(V) {
    if (M.empty() || N.empty() || C.empty() || (V != AssignmentRole::BacSi && V != AssignmentRole::TruongCa && V != AssignmentRole::DieuDuong)) Loi();
}
void ShiftAssignment::XacNhan() {
    if (TrangThai == AssignmentStatus::DaXacNhan) return;
    if (TrangThai != AssignmentStatus::DuKien || MaPhanCong.empty()) Loi();
    TrangThai = AssignmentStatus::DaXacNhan;
}
void ShiftAssignment::Huy(const std::string& LyDo) {
    // Ca đã bắt đầu cần quy trình bàn giao, không được giả vờ chưa làm.
    if (LyDo.find_first_not_of(" \t\r\n") == std::string::npos || TrangThai == AssignmentStatus::DangTruc || TrangThai == AssignmentStatus::DaHuy) Loi();
    LyDoHuy = LyDo;
    TrangThai = AssignmentStatus::DaHuy;
}
void ShiftAssignment::GhiNhanBatDau(std::time_t T) {
    if (TrangThai != AssignmentStatus::DaXacNhan || T < 0) Loi();
    ThoiDiemBatDau = T;
    TrangThai = AssignmentStatus::DangTruc;
}
