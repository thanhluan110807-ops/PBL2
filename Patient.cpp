// Patient.cpp
#include "Patient.h"
#include <iostream>
#include <cmath>    // Để dùng hàm std::round
#include <iomanip>  // Để dùng std::fixed và std::setprecision
#include <stdexcept> //Để sử dụng std::isnan

Patient() 
        : Person("UNallocated", "Khong xac dinh", 0, "Khong xac dinh"),
          heartRate(0), systolicBP(0), respiratoryRate(0), spO2(0.0), gcsScore(0),
          triageLevel(5), waitingTime(0), isAnonymous(false), isReentrant(false) {}

// 🟢 CÀI ĐẶT HÀM TẠO 1: Bệnh nhân THƯỜNG
Patient::Patient(const std::string& id, const std::string& fullName, int age, const std::string& gender)
    : Person(id, fullName, age, gender),
      heartRate(75), systolicBP(120), respiratoryRate(16), spO2(98.0), gcsScore(15),
      triageLevel(5), waitingTime(0), isAnonymous(false), isReentrant(false) {}

// 🔴 CÀI ĐẶT HÀM TẠO 2: Bệnh nhân VÔ DANH CẤP CỨU
Patient::Patient(const std::string& tempId)
    : Person(tempId, "Nguoi benh Vo danh", 0, "Khong xac dinh"), // Gọi Person khởi tạo thông tin tạm
      heartRate(75), systolicBP(120), respiratoryRate(16), spO2(98.0), gcsScore(15),
      triageLevel(5), waitingTime(0), isAnonymous(true), isReentrant(false) {}

Patient::~Patient() {}

// =========================================================
// PHẦN KHỞI TẠO GETTER & SETTER (CÓ CHỐNG DỮ LIỆU RÁC) [cite: 74, 102]
// =========================================================

bool Patient::getAnonymous() const { 
    return isAnonymous; 
}

void Patient::setAnonymous(bool status) { 
    isAnonymous = status; 
}

bool Patient::getReentrant() const { 
    return isReentrant; 
}

void Patient::setReentrant(bool status) { 
    isReentrant = status; 
}

int Patient::getTriageLevel() const { 
    return triageLevel; 
}

int Patient::getWaitingTime() const { 
    return waitingTime; 
}

void Patient::setWaitingTime(int minutes) {
    if (minutes >= 0) { // Thời gian chờ không bao giờ âm [cite: 74, 102]
        waitingTime = minutes;
    }
}

// =========================================================
// PHƯƠNG THỨC NGHIỆP VỤ LÂM SÀNG SÀNG LỌC BỆNH NHÂN
// =========================================================

// Cập nhật sinh hiệu an toàn (Mở rộng dải validation cho Nhi khoa)
void Patient::updateVitalSigns(int hr, int bp, int rr, double spo2, int gcs) {
    // 1. KIỂM TRA DẢI SINH LÝ HỢP LỆ (Chấp nhận ngưỡng nhi khoa & sơ sinh)
    if (hr < 0 || hr > 300) {
        throw std::invalid_argument("Lỗi sinh hiệu: Nhịp tim (HR) không hợp lệ [" + std::to_string(hr) + " BPM]!");
    }
    if (bp < 0 || bp > 300) {
        throw std::invalid_argument("Lỗi sinh hiệu: Huyết áp tâm thu (BP) không hợp lệ [" + std::to_string(bp) + " mmHg]!");
    }
    if (rr < 0 || rr > 120) { // Trẻ sơ sinh có thể thở đến 60-80 lần/phút
        throw std::invalid_argument("Lỗi sinh hiệu: Nhịp thở (RR) không hợp lệ [" + std::to_string(rr) + " lần/phút]!");
    }
    // 🔒 CHẶN TRIỆT ĐỂ GIÁ TRỊ NaN VÀ DẢI NGOÀI [0.0 - 100.0%]
    if (std::isnan(spo2) || spo2 < 0.0 || spo2 > 100.0) {
        throw std::invalid_argument("Lỗi sinh hiệu: Nồng độ SpO2 không xác định (NaN) hoặc vượt dải cho phép!");
    }
    if (gcs < 3 || gcs > 15) {
        throw std::invalid_argument("Lỗi sinh hiệu: Thang điểm GCS không hợp lệ [" + std::to_string(gcs) + " điểm]!");
    }

    // 2. LƯU GIÁ TRỊ THÔ
    this->heartRate = hr;
    this->systolicBP = bp;
    this->respiratoryRate = rr;
    this->spO2 = spo2;
    this->gcsScore = gcs;

    // 3. ĐÁNH GIÁ TRIAGE ĐỘNG THEO ĐỘ TUỔI (this->age)
    evaluateTriageLevel();
}

// Thuật toán Đánh giá ESI 5 Cấp độ thích ứng theo Độ tuổi (Pediatric & Adult Triage)
void Patient::evaluateTriageLevel() {
    // 0. BẢO VỆ AN TOÀN ĐỘ TUỔI (Age Guard)
    if (this->age < 0 || this->age > 150) {
        throw std::domain_error("Lỗi Triage: Tuổi bệnh nhân không hợp lệ [" + std::to_string(this->age) + "]!");
    }

    // =========================================================================
    // 🔴 1. MÀNG LỌC TOÀN CỤC CẤP 1 (ESI LEVEL 1 - ĐỎ: TỐI KHẨN TOÀN DÂN) [133, 259]
    // =========================================================================
    if (this->gcsScore <= 8 || this->spO2 < 90.0) {
        this->triageLevel = 1; // MÃ ĐỎ [133]
        return;
    }

    // =========================================================================
    // 🟠 2. MÀNG LỌC TOÀN CỤC CẤP 2 (ESI LEVEL 2 - CAM: RẤT KHẨN TOÀN DÂN) [133, 260]
    // =========================================================================
    if (this->gcsScore <= 12 || this->spO2 < 95.0) {
        this->triageLevel = 2; // MÃ CAM [133]
        return;
    }

    // =========================================================================
    // 🩺 3. PHÂN NHÁNH TRIAGE ĐA THẾ HỆ THEO LỨA TUỔI (PALS & GERIATRIC TRIAGE)
    // =========================================================================

    // 👶 NHÓM 1: TRẺ NHŨ NHI (Dưới 1 tuổi)
    if (this->age < 1) {
        if (this->heartRate < 60 || this->heartRate > 200 || 
            this->respiratoryRate < 20 || this->respiratoryRate > 70 || 
            this->systolicBP < 60) {
            this->triageLevel = 1;
            return;
        }
        if (this->heartRate < 90 || this->heartRate > 170 || 
            this->respiratoryRate < 30 || this->respiratoryRate > 55 || 
            this->systolicBP < 70) {
            this->triageLevel = 2;
            return;
        }
        if (this->heartRate > 160 || this->respiratoryRate > 50) {
            this->triageLevel = 3;
            return;
        }
    }
    // 🧒 NHÓM 2: TRẺ NHỎ / TIỀN HỌC ĐƯỜNG (1 đến 5 tuổi)
    else if (this->age <= 5) {
        int hypoBPThreshold = 70 + (2 * this->age);

        if (this->heartRate < 60 || this->heartRate > 180 || 
            this->respiratoryRate < 12 || this->respiratoryRate > 50 || 
            this->systolicBP < hypoBPThreshold - 10) {
            this->triageLevel = 1;
            return;
        }
        if (this->heartRate < 80 || this->heartRate > 140 || 
            this->respiratoryRate < 18 || this->respiratoryRate > 35 || 
            this->systolicBP < hypoBPThreshold) {
            this->triageLevel = 2;
            return;
        }
        if (this->heartRate > 120 || this->respiratoryRate > 30) {
            this->triageLevel = 3;
            return;
        }
    }
    // 👧 NHÓM 3: TRẺ HỌC ĐƯỜNG (6 đến 12 tuổi)
    else if (this->age <= 12) {
        if (this->heartRate < 50 || this->heartRate > 160 || 
            this->respiratoryRate < 10 || this->respiratoryRate > 40 || 
            this->systolicBP < 75) {
            this->triageLevel = 1;
            return;
        }
        if (this->heartRate < 60 || this->heartRate > 120 || 
            this->respiratoryRate < 14 || this->respiratoryRate > 28 || 
            this->systolicBP < 85) {
            this->triageLevel = 2;
            return;
        }
        if (this->heartRate > 100 || this->respiratoryRate > 24) {
            this->triageLevel = 3;
            return;
        }
    }
    // 👨 NHÓM 4A: NGƯỜI TRƯỞNG THÀNH (13 đến 64 tuổi)
    else if (this->age < 65) {
        if (this->respiratoryRate < 10 || this->respiratoryRate >= 35 || 
            this->systolicBP < 80 || 
            this->heartRate < 40 || this->heartRate > 150) {
            this->triageLevel = 1;
            return;
        }
        if (this->respiratoryRate < 12 || this->respiratoryRate >= 25 || 
            this->systolicBP < 90 || this->systolicBP > 180 || 
            this->heartRate < 50 || this->heartRate > 120) {
            this->triageLevel = 2;
            return;
        }
        if (this->respiratoryRate > 20 || this->systolicBP > 140 || this->heartRate > 100) {
            this->triageLevel = 3;
            return;
        }
    }
    // 👴 NHÓM 4B: NGƯỜI CAO TUỔI / LẠO KHOA (Từ 65 tuổi trở lên) [142, 174]
    else {
        // Cấp 1 (Lão khoa nguy kịch): Sốc BP < 90, HR < 45 hoặc > 130
        if (this->respiratoryRate < 10 || this->respiratoryRate >= 30 || 
            this->systolicBP < 90 || 
            this->heartRate < 45 || this->heartRate > 130) {
            this->triageLevel = 1;
            return;
        }
        // Cấp 2 (Rất khẩn Lão khoa): BP < 100 (hạ HA tương đối) hoặc > 170, HR < 55 hoặc > 110
        if (this->respiratoryRate < 12 || this->respiratoryRate >= 24 || 
            this->systolicBP < 100 || this->systolicBP > 170 || 
            this->heartRate < 55 || this->heartRate > 110) {
            this->triageLevel = 2;
            return;
        }
        // Cấp 3: Sinh hiệu biến đổi nhẹ
        if (this->respiratoryRate > 20 || this->systolicBP > 140 || this->heartRate > 90) {
            this->triageLevel = 3;
            return;
        }
    }

    // 🔵 CẤP 4 (ESI LEVEL 4): Sinh hiệu ranh giới nhẹ
    if (this->spO2 < 97.0) {
        this->triageLevel = 4;
        return;
    }

    // 🟢 CẤP 5 (ESI LEVEL 5): Sinh hiệu hoàn toàn lý tưởng
    this->triageLevel = 5;
}

// =========================================================
// QUÁ TẢI TOÁN TỬ & IN THÔNG TIN
// =========================================================

/**
 * @brief Quá tải toán tử '<' phục vụ thuật toán Hàng đợi ưu tiên (Heap).
 * GIẢI THÍCH CHO THÀNH VIÊN B:
 * - Hàng đợi ưu tiên (Heap) luôn đẩy phần tử "Lớn nhất" ra khám trước.
 * - Nhưng y khoa yêu cầu: Người mang số cấp cứu nhỏ nhất (Cấp 1 - Đỏ) phải khám trước Cấp 5.
 * - Vì vậy ta đảo ngược logic: A < B có nghĩa là "Độ ưu tiên của A thấp hơn B" [A ít khẩn cấp hơn B].
 */
bool Patient::operator<(const Patient& other) const noexcept {
    // TRƯỜNG HỢP 1: Hai bệnh nhân có cùng mức độ ưu tiên lâm sàng
    if (this->triageLevel == other.triageLevel) {
        // Ai ngồi đợi ít thời gian hơn (waitingTime nhỏ hơn) thì độ ưu tiên sẽ THẤP HƠN (nhỏ hơn)
        // Hệ thống sẽ ưu tiên người đợi lâu hơn khám trước!
        return this->waitingTime < other.waitingTime;
    }
    
    // TRƯỜNG HỢP 2: Khác mức độ ưu tiên lâm sàng
    // Số triageLevel lớn hơn (ví dụ Cấp 4 so với Cấp 1) nghĩa là độ ưu tiên THẤP HƠN (nhỏ hơn)
    return this->triageLevel > other.triageLevel; 
}

/**
 * @brief Hiển thị toàn bộ bệnh án.
 * Gọi lại hàm lớp cha để tận dụng in ID, Tên, Tuổi, Giới tính, tránh lặp code [cite: 49].
 */

 /**
 * @brief Cập nhật thông tin hành chính thực cho Bệnh nhân Vô danh (Late Data Binding)
 * @param newId Mã CCCD / Mã BHYT chính thức
 * @param newName Họ và tên thật
 * @param newAge Tuổi thật
 * @param newGender Giới tính thật
 */
void Patient::bindPatientData(const std::string& newId,
                             const std::string& newName,
                             int newAge,
                             const std::string& newGender) {
    if (newAge < 0 || newAge > 150) {
        throw std::invalid_argument("Lỗi dữ liệu: Tuổi bệnh nhân không hợp lệ [" + std::to_string(newAge) + "]!");
    }
    this->id = newId;
    this->fullName = newName;
    this->age = newAge;
    this->gender = newGender;
    this->isAnonymous = false;
}

void Patient::displayInfo() const {
    std::cout << "========================================\n";
    
    // 1. TÁI SỬ DỤNG HÀM CỦA LỚP CHA PERSON (In ID, Tên, Tuổi, Giới tính)
    Person::displayInfo(); 

    // Bổ sung nhãn nhận diện nếu đây là Hồ sơ vô danh cấp cứu khẩn cấp
    if (isAnonymous) {
        std::cout << " -> Thẻ trạng thái: [HỒ SƠ KHẨN CẤP VÔ DANH]\n";
    }

    // 2. BÁO ĐỘNG LÂM SÀNG NGUY KỊCH (Nhận diện Ngừng tim / Trụy mạch / Ngừng thở)
    if (heartRate == 0 || systolicBP == 0 || respiratoryRate == 0) {
        std::cout << " ⚠️ [CẢNH BÁO NGUY KỊCH]: BỆNH NHÂN CÓ DẤU HIỆU NGỪNG TUẦN HOÀN / HÔ HẤP!\n";
    }

    // 3. IN CHI TIẾT 5 CHỈ SỐ SINH HIỆU (SpO2 định dạng chuẩn 1 chữ số thập phân)
    std::cout << "-> Sinh hiệu: HR: " << heartRate << " BPM"
              << " | BP: " << systolicBP << " mmHg"
              << " | RR: " << respiratoryRate << " lần/phút"
              << " | SpO2: " << std::fixed << std::setprecision(1) << spO2 << "%"
              << " | GCS: " << gcsScore << " điểm\n";

    // 4. CHUYỂN ĐỔI MÃ MÀU VÀ NHÃN CẤP ĐỘ ESI (Chuẩn 5 cấp độ Y khoa)
    std::string colorStr;
    switch (triageLevel) {
        case 1: colorStr = "🔴 CẤP 1 [MÃ ĐỎ - TỐI KHẨN]"; break;
        case 2: colorStr = "🟠 CẤP 2 [MÃ CAM - RẤT KHẨN]"; break;
        case 3: colorStr = "🟡 CẤP 3 [MÃ VÀNG - KHẨN]"; break;
        case 4: colorStr = "🔵 CẤP 4 [MÃ XANH DƯƠNG - ÍT KHẨN]"; break;
        case 5: colorStr = "🟢 CẤP 5 [MÃ XANH LÁ - KHÔNG KHẨN]"; break;
        default: colorStr = "⚪ KHÔNG XÁC ĐỊNH"; break;
    }

    // 5. IN TRẠNG THÁI ĐIỀU PHỐI TRONG HÀNG ĐỢI
    std::cout << "-> Trạng thái: " << colorStr 
              << " | Thời gian chờ: " << waitingTime << " phút" 
              << " | Bệnh nhân khám lại: " << (isReentrant ? "Có" : "Không") << "\n";
}