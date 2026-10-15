#include "Patient.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <cmath>     // 💡 BẮT BUỘC: Để sử dụng std::isnan

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
   // =========================================================================
    // 🔴 1. MÀNG LỌC TOÀN CỤC CẤP 1 (ESI LEVEL 1 - ĐỎ: TỐI KHẨN TOÀN DÂN) [cite: 259]
    // Hôn mê sâu (GCS <= 8) hoặc Thiếu Oxy máu nghiêm trọng (SpO2 < 90.0%) [cite: 193, 259].
    // =========================================================================
    if (this->gcsScore <= 8 || this->spO2 < 90.0) {
        this->triageLevel = 1; // MÃ ĐỎ [cite: 259]
        return;
    }

    // =========================================================================
    // 🟠 2. MÀNG LỌC TOÀN CỤC CẤP 2 (ESI LEVEL 2 - CAM: RẤT KHẨN / RỦI RO CAO) [cite: 260]
    // Tri giác giảm/Lơ mơ (GCS 9-12) hoặc Giảm Oxy máu nhẹ-vừa (SpO2 < 95.0%) [cite: 260, 261].
    // Áp dụng CÔNG BẰNG cho MỌI LỨA TUỔI (Trẻ em & Người lớn)!
    // =========================================================================
    if (this->gcsScore <= 12 || this->spO2 < 95.0) {
        this->triageLevel = 2; // MÃ CAM [cite: 260]
        return;
    }

    // 👶 NHÓM 1: TRẺ NHŨ NHI (Dưới 1 tuổi)
    if (this->age < 1) {
        // Cấp 1 (Nguy kịch nhi): HR quá chậm (<60) hoặc quá nhanh (>200), RR < 20 hoặc > 70, BP < 60
        if (this->heartRate < 60 || this->heartRate > 200 || 
            this->respiratoryRate < 20 || this->respiratoryRate > 70 || 
            this->systolicBP < 60) {
            this->triageLevel = 1;
            return;
        }
        // Cấp 2 (Rất khẩn): HR 60-90 hoặc 170-200, RR 20-29 hoặc 55-70, BP 60-70
        if (this->heartRate < 90 || this->heartRate > 170 || 
            this->respiratoryRate < 30 || this->respiratoryRate > 55 || 
            this->systolicBP < 70 || this->spO2 < 95.0) {
            this->triageLevel = 2;
            return;
        }
        // Cấp 3: Sinh hiệu biến đổi nhẹ
        if (this->heartRate > 160 || this->respiratoryRate > 50) {
            this->triageLevel = 3;
            return;
        }
    }
    // 🧒 NHÓM 2: TRẺ NHỎ / TIỀN HỌC ĐƯỜNG (1 đến 5 tuổi)
    else if (this->age <= 5) {
        int hypoBPThreshold = 70 + (2 * this->age); // Ngưỡng hạ HA nhi: 70 + 2*age

        // Cấp 1: HR < 60 hoặc > 180, RR < 12 hoặc > 50, BP dưới ngưỡng sốc
        if (this->heartRate < 60 || this->heartRate > 180 || 
            this->respiratoryRate < 12 || this->respiratoryRate > 50 || 
            this->systolicBP < hypoBPThreshold - 10) {
            this->triageLevel = 1;
            return;
        }
        // Cấp 2: HR bất thường, RR 35-50, BP sát ngưỡng hạ
        if (this->heartRate < 80 || this->heartRate > 140 || 
            this->respiratoryRate < 18 || this->respiratoryRate > 35 || 
            this->systolicBP < hypoBPThreshold || this->spO2 < 95.0) {
            this->triageLevel = 2;
            return;
        }
        // Cấp 3: Sinh hiệu biến đổi nhẹ
        if (this->heartRate > 120 || this->respiratoryRate > 30) {
            this->triageLevel = 3;
            return;
        }
    }
    // 👧 NHÓM 3: TRẺ HỌC ĐƯỜNG (6 đến 12 tuổi)
    else if (this->age <= 12) {
        // Cấp 1: HR < 50 hoặc > 160, RR < 10 hoặc > 40, BP < 75
        if (this->heartRate < 50 || this->heartRate > 160 || 
            this->respiratoryRate < 10 || this->respiratoryRate > 40 || 
            this->systolicBP < 75) {
            this->triageLevel = 1;
            return;
        }
        // Cấp 2: HR < 60 hoặc > 120, RR < 14 hoặc > 28, BP < 85
        if (this->heartRate < 60 || this->heartRate > 120 || 
            this->respiratoryRate < 14 || this->respiratoryRate > 28 || 
            this->systolicBP < 85 || this->spO2 < 95.0) {
            this->triageLevel = 2;
            return;
        }
        // Cấp 3: Sinh hiệu biến đổi nhẹ
        if (this->heartRate > 100 || this->respiratoryRate > 24) {
            this->triageLevel = 3;
            return;
        }
    }
    // 👨 NHÓM 4: VỊ THÀNH NIÊN & NGƯỜI LỚN (Trên 12 tuổi)
    else {
        // Cấp 1: Sốc nặng BP < 80, HR < 40 hoặc > 150, RR < 10 hoặc >= 35
        if (this->respiratoryRate < 10 || this->respiratoryRate >= 35 || 
            this->systolicBP < 80 || 
            this->heartRate < 40 || this->heartRate > 150) {
            this->triageLevel = 1;
            return;
        }
        // Cấp 2: BP < 90 hoặc > 180, HR < 50 hoặc > 120, RR < 12 hoặc >= 25, SpO2 < 95.0
        if (this->gcsScore <= 12 || this->spO2 < 95.0 || 
            this->respiratoryRate < 12 || this->respiratoryRate >= 25 || 
            this->systolicBP < 90 || this->systolicBP > 180 || 
            this->heartRate < 50 || this->heartRate > 120) {
            this->triageLevel = 2;
            return;
        }
        // Cấp 3: Sinh hiệu biến đổi vừa
        if (this->respiratoryRate > 20 || this->systolicBP > 140 || this->heartRate > 100) {
            this->triageLevel = 3;
            return;
        }
    }

    // 🔵 CẤP 4: Sinh hiệu biến đổi nhẹ ở ranh giới
    if (this->spO2 < 97.0) {
        this->triageLevel = 4;
        return;
    }

    // 🟢 CẤP 5: Sinh hiệu hoàn toàn lý tưởng theo lứa tuổi
    this->triageLevel = 5;
}