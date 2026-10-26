// main.cpp
#include "model/Patient.h"

int main() {
    // 1. Tạo bệnh nhân A bình thường nhưng đợi lâu (30 phút)
    Patient pA("BN01", "Nguyen Van A", 28, "Nam");
    pA.updateVitalSigns(80, 120, 16, 98.0, 15);
    pA.setWaitingTime(30);

    // 2. Tạo bệnh nhân B suy hô hấp khẩn cấp (SpO2 tụt sâu xuống 88%) vừa mới đến (0 phút)
    Patient pB("EMG_99", "Vo Danh", 45, "Nu");
    pB.setAnonymous(true);
    pB.updateVitalSigns(130, 85, 28, 88.0, 12);
    pB.setWaitingTime(0);

    std::cout << "--- THÔNG TIN LÂM SÀNG BAN ĐẦU ---\n";
    pA.displayInfo();
    pB.displayInfo();

    std::cout << "\n--- KIỂM TRA LOGIC SO SÁNH ĐỘ ƯU TIÊN ---\n";
    if (pA < pB) {
        std::cout << "-> KIỂM TRA ĐÚNG: Bệnh nhân Vô Danh (pB) có độ ưu tiên cấp cứu cao hơn Nguyễn Văn A (pA).\n";
    } else {
        std::cout << "-> KIỂM TRA SAI: Hãy xem lại toán tử so sánh operator< !\n";
    }

    return 0;
}