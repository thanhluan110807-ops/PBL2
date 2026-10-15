#include "PatientFileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "vector.h"

// 1. GHI DỮ LIỆU TỪ VECTOR RA FILE
bool PatientFileManager::savePatientsToFile(const std::string& filePath, const Vector<Patient>& patients) {
    std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
    
    if (!outFile.is_open()) {
        std::cerr << "❌ [LỖI FILE]: Không thể mở file " << filePath << " để ghi dữ liệu!\n";
        return false;
    }

    outFile << "id,fullName,age,gender,isAnonymous,isReentrant,heartRate,systolicBP,respiratoryRate,spO2,gcsScore,waitingTime\n";

    for (int i = 0; i < patients.size(); ++i) {
        outFile << patients[i].getId() << ","
                << patients[i].getFullName() << ","
                << patients[i].getAge() << ","
                << patients[i].getGender() << ","
                << (patients[i].isAnonymousPatient() ? 1 : 0) << ","
                << (patients[i].isReentrantPatient() ? 1 : 0) << ","
                << patients[i].getHeartRate() << ","
                << patients[i].getSystolicBP() << ","
                << patients[i].getRespiratoryRate() << ","
                << patients[i].getSpO2() << ","
                << patients[i].getGcsScore() << ","
                << patients[i].getWaitingTime() << "\n";
    }

    outFile.close();
    std::cout << "✅ [HỆ THỐNG]: Đã lưu thành công " << patients.size() << " hồ sơ vào '" << filePath << "'.\n";
    return true;
}

// 2. NẠP DỮ LIỆU TỪ FILE VÀO VECTOR (KHÔNG LO TRÀN BỘ ĐỆM)
int PatientFileManager::loadPatientsFromFile(const std::string& filePath, Vector<Patient>& patientsList) {
    std::ifstream inFile(filePath);
    
    if (!inFile.is_open()) {
        std::cerr << "⚠️ [LỖI FILE]: Không tìm thấy file dữ liệu '" << filePath << "'.\n";
        return 0;
    }

    patientsList.clear(); // Dọn dẹp vector trước khi nạp
    std::string line;
    int count = 0;

    // Bỏ qua dòng Header
    if (std::getline(inFile, line)) {}

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        try {
            std::string id, fullName, gender;
            int age, heartRate, systolicBP, respiratoryRate, gcsScore, waitingTime;
            double spO2;
            bool isAnonymous, isReentrant;

            parseCSVLine(line, id, fullName, age, gender, isAnonymous, isReentrant, 
                         heartRate, systolicBP, respiratoryRate, spO2, gcsScore, waitingTime);

            Patient p = isAnonymous ? Patient(id) : Patient(id, fullName, age, gender);
            p.updateVitalSigns(heartRate, systolicBP, respiratoryRate, spO2, gcsScore);
            p.setWaitingTime(waitingTime);
            p.setReentrant(isReentrant);

            // 🚀 TỰ ĐỘNG THÊM VÀ TỰ ĐỘNG TĂNG DUNG LƯỢNG MẢNG
            patientsList.push_back(p); 
            count++;
        } 
        catch (const std::exception& e) {
            std::cerr << "⚠️ [LỖI DÒNG " << (count + 2) << "]: " << e.what() << " -> Đã bỏ qua.\n";
        }
    }

    inFile.close();
    std::cout << "✅ [HỆ THỐNG]: Đã nạp thành công " << count << " hồ sơ bệnh nhân từ file CSV!\n";
    return count;
}