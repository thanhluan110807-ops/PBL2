// Patient.h
#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"

class Patient : public Person {
private:
    // 1. Các chỉ số sinh hiệu (Vital Signs) phục vụ phân loại cấp cứu
    int heartRate;          // Nhịp tim (BPM)
    int systolicBP;         // Huyết áp tâm thu (mmHg)
    int respiratoryRate;    // Nhịp thở (lần/phút)
    double spO2;            // Nồng độ Oxy trong máu (%)
    int gcsScore;           // Thang điểm hôn mê Glasgow (3 -> 15)

    // 2. Thuộc tính phân luồng và quản lý trạng thái bệnh nhân
    int triageLevel;        // Mức độ cấp cứu: 1 (Nguy kịch) -> 5 (Nhẹ)
    bool isAnonymous;       // true nếu là bệnh nhân tai nạn hôn mê chưa rõ danh tính
    bool isReentrant;       // true nếu là bệnh nhân quay lại phòng cấp cứu đọc kết quả khám

    // 3. Thời gian (giả lập phút) để phục vụ cảnh báo Tái phân loại động (Dynamic Re-triage)
    int waitingTime;        // Số phút bệnh nhân đã phải ngồi chờ ở sảnh

public:
    // Constructor kế thừa từ lớp cha Person
    Patient(std::string id = "", std::string name = "", int age = 0, std::string gender = "")
        : Person(id, name, age, gender), 
          heartRate(0), systolicBP(0), respiratoryRate(0), spO2(100.0), gcsScore(15),
          triageLevel(5), isAnonymous(false), isReentrant(false), waitingTime(0) {}

    // Các hàm Getters & Setters
    void setAnonymous(bool status) { isAnonymous = status; }
    bool getAnonymous() const { return isAnonymous; }

    void setReentrant(bool status) { isReentrant = status; }
    bool getReentrant() const { return isReentrant; }

    int getTriageLevel() const { return triageLevel; }
    
    void setWaitingTime(int minutes) { waitingTime = minutes; }
    int getWaitingTime() const { return waitingTime; }

    // Hàm cập nhật sinh hiệu đồng thời tự động kích hoạt bộ phân loại ESI
    void updateVitalSigns(int hr, int bp, int rr, double spo2, int gcs) {
        this->heartRate = hr;
        this->systolicBP = bp;
        this->respiratoryRate = rr;
        this->spO2 = spo2;
        this->gcsScore = gcs;
        this->evaluateTriageLevel(); // Tự động chạy thuật toán phân lớp ưu tiên
    }

    // Thuật toán Phân loại Cấp cứu Sơ bộ (ESI - Emergency Severity Index) 5 cấp độ
    void evaluateTriageLevel() {
        // Cấp 1: Đỏ (Nguy kịch tính mạng - Đòi hỏi xử trí tức thì)
        if (spO2 < 90.0 || heartRate < 40 || heartRate > 140 || gcsScore <= 8) {
            triageLevel = 1;
        }
        // Cấp 2: Cam (Tình huống khẩn cấp - Đe dọa chức năng sống)
        else if (spO2 < 94.0 || systolicBP < 90 || gcsScore <= 13) {
            triageLevel = 2;
        }
        // Cấp 3: Vàng (Ổn định nhưng cần nhiều nguồn lực xét nghiệm)
        else if (systolicBP > 160 || heartRate > 110) {
            triageLevel = 3;
        }
        // Cấp 4 & 5: Xanh lá (Nhẹ, ít hoặc không cần nguồn lực khẩn cấp)
        else {
            triageLevel = 4;
        }
    }

    // ⚠️ QUAN TRỌNG: Quá tải toán tử so sánh '<' để dùng cho Max-Heap (Priority Queue) sau này
    // Chú ý logic: Cấp 1 (Nguy kịch) có độ ưu tiên cao hơn Cấp 2.
    // Nên Patient A < Patient B nghĩa là mức độ của A ít khẩn cấp hơn B (số triageLevel của A lớn hơn B)
    bool operator<(const Patient& other) const {
        // Nếu cùng cấp độ ưu tiên, bệnh nhân nào đợi lâu hơn (waitingTime lớn hơn) sẽ được ưu tiên trước
        if (this->triageLevel == other.triageLevel) {
            return this->waitingTime < other.waitingTime;
        }
        return this->triageLevel > other.triageLevel; 
    }

    // Ghi đè phương thức in thông tin của lớp cha
    void displayInfo() const override {
        std::cout << "========================================\n";
        if (isAnonymous) {
            std::cout << "[HỒ SƠ KHẨN CẤP VÔ DANH] ";
        }
        Person::displayInfo();
        std::cout << "-> Sinh hiệu: HR: " << heartRate << " | BP: " << systolicBP 
                  << " | RR: " << respiratoryRate << " | SpO2: " << spO2 << "% | GCS: " << gcsScore << "\n"
                  << "-> Trạng thái: Thang phân loại: Cấp " << triageLevel 
                  << " | Chờ: " << waitingTime << " phút"
                  << " | Quay lại khám: " << (isReentrant ? "Có" : "Không") << "\n";
    }
};

#endif