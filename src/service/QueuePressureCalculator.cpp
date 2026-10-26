// QueuePressureCalculator.cpp
#include "service/QueuePressureCalculator.h"
#include <cmath>
#include <iostream>

QueuePressureReport::QueuePressureReport() noexcept
    : totalPatients(0), esiCounts{0, 0, 0, 0, 0, 0}, reentrantCount(0), anonymousCount(0),
      overdueRetriageCount(0), maxWaitingTime(0), averageWaitingTime(0.0),
      baseAcuityScore(0.0), waitingPenaltyScore(0.0), reentrantLoadBonus(0.0),
      totalPressureScore(0.0), normalizedPressure(0.0), level(PressureLevel::NORMAL),
      recommendedDoctors(1), recommendedNurses(1), needFlexibleBufferShift(false),
      needStandbyMobilization(false), estimatedFatigueDelta(0.0) {}

int QueuePressureCalculator::getRetriageThresholdMinutes(int triageLevel) noexcept {
    switch (triageLevel) {
        case 1: return 0;
        case 2: return 10;
        case 3: return 30;
        case 4: return 60;
        default: return 120;
    }
}

bool QueuePressureCalculator::isOverdueForRetriage(const Patient& patient) noexcept {
    return patient.getWaitingTime() > getRetriageThresholdMinutes(patient.getTriageLevel());
}

double QueuePressureCalculator::getBaseEsiWeight(int triageLevel) noexcept {
    switch (triageLevel) {
        case 1: return WEIGHT_ESI_1;
        case 2: return WEIGHT_ESI_2;
        case 3: return WEIGHT_ESI_3;
        case 4: return WEIGHT_ESI_4;
        default: return WEIGHT_ESI_5;
    }
}

// Áp lực cá nhân = trọng số ESI + 50% trọng số nếu chờ quá hạn + 3 điểm nếu là bệnh nhân quay lại
double QueuePressureCalculator::calculateSinglePatientPressure(const Patient& patient) noexcept {
    const double base = getBaseEsiWeight(patient.getTriageLevel());
    double score = base;
    if (isOverdueForRetriage(patient)) score += base * 0.5;
    if (patient.isReentrantPatient()) score += REENTRANT_BONUS_WEIGHT;
    return score;
}

void QueuePressureCalculator::accumulatePatientMetrics(const Patient& p, QueuePressureReport& report) noexcept {
    const int level = p.getTriageLevel();
    const double base = getBaseEsiWeight(level);
    report.totalPatients++;
    if (level >= 1 && level <= 5) report.esiCounts[level]++;
    report.baseAcuityScore += base;
    if (isOverdueForRetriage(p)) {
        report.overdueRetriageCount++;
        report.waitingPenaltyScore += base * 0.5;
    }
    if (p.isReentrantPatient()) {
        report.reentrantCount++;
        report.reentrantLoadBonus += REENTRANT_BONUS_WEIGHT;
    }
    if (p.isAnonymousPatient()) report.anonymousCount++;
    if (p.getWaitingTime() > report.maxWaitingTime) report.maxWaitingTime = p.getWaitingTime();
    report.averageWaitingTime += p.getWaitingTime(); // cộng dồn, chia cho tổng ở bước hoàn tất
}

void QueuePressureCalculator::finalizeSchedulingParameters(QueuePressureReport& report, int currentActiveDoctors) noexcept {
    if (report.totalPatients > 0) report.averageWaitingTime /= report.totalPatients;
    report.totalPressureScore = report.baseAcuityScore + report.waitingPenaltyScore + report.reentrantLoadBonus;

    report.normalizedPressure = report.totalPressureScore / MAX_BENCHMARK_PRESSURE * 100.0;
    if (report.normalizedPressure > 100.0) report.normalizedPressure = 100.0;

    // Ngưỡng: <25 bình thường, <50 cảnh báo nhẹ, <75 tải cao, >=75 hoặc từ 2 ca ESI 1 trở lên là quá tải
    if (report.normalizedPressure >= 75.0 || report.esiCounts[1] >= 2) report.level = PressureLevel::CRITICAL_OVERLOAD;
    else if (report.normalizedPressure >= 50.0) report.level = PressureLevel::HIGH_LOAD;
    else if (report.normalizedPressure >= 25.0) report.level = PressureLevel::ELEVATED;
    else report.level = PressureLevel::NORMAL;

    const int doctors = currentActiveDoctors > 0 ? currentActiveDoctors : 1;
    const int needDoctors = static_cast<int>(std::ceil(report.totalPressureScore / DOCTOR_CAPACITY_UNIT));
    const int needNurses = static_cast<int>(std::ceil(report.totalPressureScore / NURSE_CAPACITY_UNIT));
    report.recommendedDoctors = needDoctors > 1 ? needDoctors : 1;
    report.recommendedNurses = needNurses > 1 ? needNurses : 1;
    report.needFlexibleBufferShift = report.level >= PressureLevel::HIGH_LOAD;
    report.needStandbyMobilization = report.level == PressureLevel::CRITICAL_OVERLOAD;
    // Mệt mỏi cộng thêm mỗi bác sĩ = tải trên mỗi bác sĩ / công suất chuẩn (>1 nghĩa là quá sức)
    report.estimatedFatigueDelta = report.totalPressureScore / (doctors * DOCTOR_CAPACITY_UNIT);
}

QueuePressureReport QueuePressureCalculator::evaluateQueuePressure(const PriorityQueue<Patient>& waitingQueue,
                                                                   int currentActiveDoctors) {
    QueuePressureReport report;
    for (int i = 0; i < waitingQueue.getSize(); ++i) accumulatePatientMetrics(waitingQueue.at(i), report);
    finalizeSchedulingParameters(report, currentActiveDoctors);
    return report;
}

QueuePressureReport QueuePressureCalculator::evaluateDualQueuePressure(const PriorityQueue<Patient>& initialQueue,
                                                                       const PriorityQueue<Patient>& reentrantQueue,
                                                                       int currentActiveDoctors) {
    QueuePressureReport report;
    for (int i = 0; i < initialQueue.getSize(); ++i) accumulatePatientMetrics(initialQueue.at(i), report);
    for (int i = 0; i < reentrantQueue.getSize(); ++i) accumulatePatientMetrics(reentrantQueue.at(i), report);
    finalizeSchedulingParameters(report, currentActiveDoctors);
    return report;
}

Vector<Patient> QueuePressureCalculator::getOverdueRetriagePatients(const PriorityQueue<Patient>& waitingQueue) {
    Vector<Patient> overdue;
    for (int i = 0; i < waitingQueue.getSize(); ++i)
        if (isOverdueForRetriage(waitingQueue.at(i))) overdue.push_back(waitingQueue.at(i));
    return overdue;
}

std::string QueuePressureCalculator::pressureLevelToString(PressureLevel level) {
    switch (level) {
        case PressureLevel::NORMAL: return "🟢 BÌNH THƯỜNG";
        case PressureLevel::ELEVATED: return "🟡 CẢNH BÁO NHẸ";
        case PressureLevel::HIGH_LOAD: return "🟠 TẢI CAO";
        default: return "🔴 QUÁ TẢI NGUY KỊCH";
    }
}

void QueuePressureCalculator::displayPressureReport(const QueuePressureReport& r) {
    std::cout << "===== ÁP LỰC HÀNG ĐỢI =====\n"
              << "Tổng bệnh nhân: " << r.totalPatients << " | Quá hạn re-triage: " << r.overdueRetriageCount << "\n"
              << "Áp lực: " << r.normalizedPressure << "% - " << pressureLevelToString(r.level) << "\n"
              << "Khuyến nghị: " << r.recommendedDoctors << " bác sĩ, " << r.recommendedNurses << " điều dưỡng\n";
}
