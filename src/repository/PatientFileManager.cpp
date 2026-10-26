#include "repository/PatientFileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace {

void writePatientCsvRow(std::ostream& out, const Patient& p) {
    out << p.getId() << ","
        << p.getFullName() << ","
        << p.getAge() << ","
        << p.getGender() << ","
        << (p.isAnonymousPatient() ? 1 : 0) << ","
        << (p.isReentrantPatient() ? 1 : 0) << ","
        << p.getHeartRate() << ","
        << p.getSystolicBP() << ","
        << p.getRespiratoryRate() << ","
        << std::fixed << std::setprecision(1) << p.getSpO2() << ","
        << p.getGcsScore() << ","
        << p.getWaitingTime() << "\n";
}

bool looksLikeHeader(const std::string& line) {
    return line.find("id") != std::string::npos &&
           line.find("fullName") != std::string::npos;
}

} // namespace

bool PatientFileManager::savePatientsToFile(const std::string& filePath, const Vector<Patient>& patients) {
    std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);

    if (!outFile.is_open()) {
        std::cerr << "❌ [LỖI FILE]: Không thể mở file " << filePath << " để ghi dữ liệu!\n";
        return false;
    }

    outFile << "id,fullName,age,gender,isAnonymous,isReentrant,heartRate,systolicBP,respiratoryRate,spO2,gcsScore,waitingTime\n";

    for (int i = 0; i < patients.size(); ++i) {
        writePatientCsvRow(outFile, patients[i]);
    }

    if (!outFile.good()) {
        std::cerr << "❌ [LỖI FILE]: Ghi file '" << filePath << "' thất bại!\n";
        return false;
    }

    std::cout << "✅ [HỆ THỐNG]: Đã lưu thành công " << patients.size() << " hồ sơ vào '" << filePath << "'.\n";
    return true;
}

int PatientFileManager::loadPatientsFromFile(const std::string& filePath, Vector<Patient>& patientsList) {
    std::ifstream inFile(filePath);

    if (!inFile.is_open()) {
        std::cerr << "⚠️ [LỖI FILE]: Không tìm thấy file dữ liệu '" << filePath << "'.\n";
        return 0;
    }

    patientsList.clear();
    std::string line;
    int loaded = 0;
    int lineNumber = 0;

    auto tryLoadLine = [&](const std::string& csvLine) {
        if (csvLine.empty()) {
            return;
        }

        try {
            std::string id, fullName, gender;
            int age = 0, heartRate = 0, systolicBP = 0, respiratoryRate = 0, gcsScore = 0, waitingTime = 0;
            double spO2 = 0.0;
            bool isAnonymous = false, isReentrant = false;

            parseCSVLine(csvLine, id, fullName, age, gender, isAnonymous, isReentrant,
                         heartRate, systolicBP, respiratoryRate, spO2, gcsScore, waitingTime);

            Patient p = isAnonymous ? Patient(id) : Patient(id, fullName, age, gender);
            if (isAnonymous) {
                p.setFullName(fullName);
                p.setAge(age);
                p.setGender(gender);
            }
            p.updateVitalSigns(heartRate, systolicBP, respiratoryRate, spO2, gcsScore);
            p.setWaitingTime(waitingTime);
            p.setReentrant(isReentrant);

            patientsList.push_back(p);
            ++loaded;
        } catch (const std::exception& e) {
            std::cerr << "⚠️ [LỖI DÒNG " << lineNumber << "]: " << e.what() << " -> Đã bỏ qua.\n";
        }
    };

    if (std::getline(inFile, line)) {
        ++lineNumber;
        if (!looksLikeHeader(line)) {
            tryLoadLine(line);
        }
    }

    while (std::getline(inFile, line)) {
        ++lineNumber;
        tryLoadLine(line);
    }

    std::cout << "✅ [HỆ THỐNG]: Đã nạp thành công " << loaded << " hồ sơ bệnh nhân từ file CSV!\n";
    return loaded;
}

bool PatientFileManager::appendPatientToFile(const std::string& filePath, const Patient& patient) {
    bool needHeader = false;
    {
        std::ifstream check(filePath);
        needHeader = !check.good() || check.peek() == std::ifstream::traits_type::eof();
    }

    std::ofstream outFile(filePath, std::ios::out | std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "❌ [LỖI FILE]: Không thể mở file " << filePath << " để ghi bổ sung!\n";
        return false;
    }

    if (needHeader) {
        outFile << "id,fullName,age,gender,isAnonymous,isReentrant,heartRate,systolicBP,respiratoryRate,spO2,gcsScore,waitingTime\n";
    }
    writePatientCsvRow(outFile, patient);

    if (!outFile.good()) {
        std::cerr << "❌ [LỖI FILE]: Ghi bổ sung vào '" << filePath << "' thất bại!\n";
        return false;
    }
    return true;
}

void PatientFileManager::parseCSVLine(const std::string& line,
                                      std::string& id, std::string& fullName, int& age, std::string& gender,
                                      bool& isAnonymous, bool& isReentrant,
                                      int& heartRate, int& systolicBP, int& respiratoryRate,
                                      double& spO2, int& gcsScore, int& waitingTime) {
    std::stringstream ss(line);
    std::string fields[12];
    int n = 0;
    while (n < 12 && std::getline(ss, fields[n], ',')) {
        ++n;
    }

    if (n != 12) {
        throw std::invalid_argument("Dong CSV khong du 12 truong (hien co " + std::to_string(n) + ")");
    }

    id = fields[0];
    fullName = fields[1];
    age = std::stoi(fields[2]);
    gender = fields[3];
    isAnonymous = std::stoi(fields[4]) != 0;
    isReentrant = std::stoi(fields[5]) != 0;
    heartRate = std::stoi(fields[6]);
    systolicBP = std::stoi(fields[7]);
    respiratoryRate = std::stoi(fields[8]);
    spO2 = std::stod(fields[9]);
    gcsScore = std::stoi(fields[10]);
    waitingTime = std::stoi(fields[11]);
}
