#ifndef PATIENT_FILE_MANAGER_H
#define PATIENT_FILE_MANAGER_H

#include "model/Patient.h"
#include "util/Vector.h" // Sử dụng Custom Vector tự tạo
#include <string>

class PatientFileManager {
public:
    // Ghi danh sách bệnh nhân từ Vector ra file CSV
    static bool savePatientsToFile(const std::string& filePath, const Vector<Patient>& patients);

    // Đọc danh sách bệnh nhân từ file CSV nạp trực tiếp vào Vector (Tự động mở rộng)
    static int loadPatientsFromFile(const std::string& filePath, Vector<Patient>& patientsList);

    // Ghi bổ sung một bệnh nhân vào cuối file (Append mode)
    static bool appendPatientToFile(const std::string& filePath, const Patient& patient);

private:
    static void parseCSVLine(const std::string& line, 
                             std::string& id, std::string& fullName, int& age, std::string& gender, 
                             bool& isAnonymous, bool& isReentrant, 
                             int& heartRate, int& systolicBP, int& respiratoryRate, 
                             double& spO2, int& gcsScore, int& waitingTime);
};

#endif // PATIENT_FILE_MANAGER_H