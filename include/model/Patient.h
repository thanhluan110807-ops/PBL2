#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
#include <iostream>
#include <string>

class Patient : public Person {
private:
    int heartRate;        // Nhịp tim (BPM)
    int systolicBP;       // Huyết áp tâm thu (mmHg)
    int respiratoryRate;  // Nhịp thở (lần/phút)
    double spO2;          // SpO2 (%) - Lưu dữ liệu thô (Raw Data) từ thiết bị
    int gcsScore;         // Thang điểm hôn mê Glasgow (3 - 15 điểm)

    int stt;                 // Số thứ tự bệnh nhân
    static int nextStt;      // Bộ đếm dùng chung cho các bệnh nhân

    int triageLevel;      // Mức ESI (1: Đỏ, 2: Cam, 3: Vàng, 4: Xanh dương, 5: Xanh lá)
    int waitingTime;      // Thời gian chờ (phút)
    bool isAnonymous;     // Bệnh nhân vô danh cấp cứu khẩn cấp
    bool isReentrant;     // Bệnh nhân quay lại khám

public:
    Patient();
    ~Patient() override;
          
     // 🟢 HAM TẠO 1: Dành cho Bệnh nhân THƯỜNG (Có đầy đủ thông tin)
    Patient(const std::string& id, 
            const std::string& fullName, 
            int age, 
            const std::string& gender);

    // 🔴 HÀM TẠO 2: Dành cho Bệnh nhân VÔ DANH CẤP CỨU (Chỉ cần truyền Mã tạm)
    explicit Patient(const std::string& tempId);
    
    // Cập nhật sinh hiệu an toàn (Atomic Validation)
    void updateVitalSigns(int hr, int bp, int rr, double spo2, int gcs);

    // Thuật toán đánh giá mức ESI 5 cấp độ
    void evaluateTriageLevel();

    // Hồ sơ khẩn cấp & Cập nhật danh tính sau
    void bindPatientData(const std::string& newId, const std::string& newName, int newAge, const std::string& newGender);

    // Getters / Setters tối ưu noexcept
    void setWaitingTime(int mins);
    int getStt() const noexcept;
    int getWaitingTime() const noexcept;
    int getTriageLevel() const noexcept;
    int getHeartRate() const noexcept;
    int getSystolicBP() const noexcept;
    int getRespiratoryRate() const noexcept;
    double getSpO2() const noexcept;
    int getGcsScore() const noexcept;
    bool isAnonymousPatient() const noexcept;
    bool isReentrantPatient() const noexcept;
    bool getAnonymous() const;
    void setAnonymous(bool status);
    bool getReentrant() const;
    void setReentrant(bool reentrant) noexcept;

    // Ghi đè phương thức hiển thị (OOP Inheritance)
    void displayInfo() const override;

    // Đa năng hóa toán tử so sánh ưu tiên cho Max-Heap (noexcept)
    bool operator<(const Patient& other) const noexcept;

    // Toán tử in dữ liệu
    friend std::ostream& operator<<(std::ostream& os, const Patient& p);
};

#endif // PATIENT_H