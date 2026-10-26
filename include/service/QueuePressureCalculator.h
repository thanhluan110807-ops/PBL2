// QueuePressureCalculator.h
#ifndef QUEUE_PRESSURE_CALCULATOR_H
#define QUEUE_PRESSURE_CALCULATOR_H

#include "model/Patient.h"
#include "util/PriorityQueue.h"
#include "util/Vector.h"
#include <string>

/**
 * @enum PressureLevel
 * @brief Phân cấp mức độ áp lực hiện tại tại Khoa Cấp cứu.
 * Làm cơ sở để thuật toán Backtracking (Thành viên B) quyết định cắt nhánh (Pruning)
 * hoặc kích hoạt ca trực linh hoạt vắt giờ (Flexible Shifts).
 */
enum class PressureLevel {
    NORMAL = 0,            // 🟢 Bình thường (Chỉ số áp lực < 25%)
    ELEVATED = 1,          // 🟡 Cảnh báo nhẹ (Chỉ số áp lực 25% - 49%)
    HIGH_LOAD = 2,         // 🟠 Tải cao (Chỉ số áp lực 50% - 74%) -> Khuyến nghị mở Ca đệm linh hoạt
    CRITICAL_OVERLOAD = 3  // 🔴 Quá tải nguy kịch (Chỉ số áp lực >= 75% hoặc dồn ca ESI 1) -> Huy động Standby Pool
};

/**
 * @struct QueuePressureReport
 * @brief Gói dữ liệu đầu ra (DTO) chứa toàn bộ tham số "Áp lực hàng đợi"
 * do Thành viên A tính toán từ PriorityQueue để truyền vào bộ xếp lịch của Thành viên B.
 */
struct QueuePressureReport {
    // 1. Thống kê số lượng bệnh nhân theo phân tầng lâm sàng
    int totalPatients;           // Tổng số bệnh nhân đang chờ khám
    int esiCounts[6];            // Số ca theo từng mức ESI (chỉ số 1..5 tương ứng ESI 1..5)
    int reentrantCount;          // Số bệnh nhân quay lại đọc kết quả CLS (Reentrant Patients)
    int anonymousCount;          // Số bệnh nhân cấp cứu vô danh (Anonymous Profiles)
    int overdueRetriageCount;    // Số ca chờ vượt ngưỡng bắt buộc phải Tái phân loại động (Re-Triage)

    // 2. Thống kê thời gian chờ (phút)
    int maxWaitingTime;          // Thời gian chờ lâu nhất trong hàng đợi (phút)
    double averageWaitingTime;   // Thời gian chờ trung bình (phút)

    // 3. Điểm số Áp lực hàng đợi (Queue Pressure Metrics)
    double baseAcuityScore;      // Điểm áp lực bệnh nặng cơ sở (dựa trên trọng số ESI 1-5)
    double waitingPenaltyScore;  // Điểm phạt áp lực do chờ lâu / quá hạn Re-Triage
    double reentrantLoadBonus;   // Điểm áp lực tăng thêm từ luồng bệnh nhân khám lại (Reentrant)
    double totalPressureScore;   // Tổng điểm áp lực thô (Raw Queue Pressure Score)
    double normalizedPressure;   // Chỉ số áp lực chuẩn hóa [0.0 - 100.0%]
    PressureLevel level;         // Mức cảnh báo áp lực khoa Cấp cứu

    // 4. Tham số đầu vào trực tiếp cho Thuật toán Backtracking & Chống Burnout (Thành viên B)
    int recommendedDoctors;      // Số bác sĩ tối thiểu cần xếp trong ca trực để giải phóng hàng đợi
    int recommendedNurses;       // Số điều dưỡng sàng lọc & lâm sàng cần thiết
    bool needFlexibleBufferShift;// Cờ báo hiệu cần mở Ca trực linh hoạt vắt giờ (ví dụ: 11:00 - 15:00)
    bool needStandbyMobilization;// Cờ báo hiệu cần gọi bác sĩ dự phòng từ Standby Pool ngay lập tức
    double estimatedFatigueDelta;// Trọng số mệt mỏi (MBI / Fatigue Score) cộng thêm cho mỗi bác sĩ trong ca

    QueuePressureReport() noexcept;
};

/**
 * @class QueuePressureCalculator
 * @brief Lớp dịch vụ tĩnh (Static Domain Service) của Thành viên A dùng để định lượng
 * "Áp lực hàng đợi" từ PriorityQueue<Patient> và cảnh báo Tái phân loại động (Dynamic Re-Triage).
 */
class QueuePressureCalculator {
private:
    // =========================================================
    // HẰNG SỐ Y KHOA & TRỌNG SỐ LÂM SÀNG (CLINICAL WEIGHTS)
    // =========================================================
    static constexpr double WEIGHT_ESI_1 = 30.0; // Cấp 1 (Đỏ - Tối khẩn): Cần hồi sức ngay lập tức
    static constexpr double WEIGHT_ESI_2 = 15.0; // Cấp 2 (Cam - Rất khẩn): Nguy cơ diễn tiến nặng cao
    static constexpr double WEIGHT_ESI_3 = 6.0;  // Cấp 3 (Vàng - Khẩn): Cần nhiều can thiệp cận lâm sàng
    static constexpr double WEIGHT_ESI_4 = 2.5;  // Cấp 4 (Xanh dương - Ít khẩn): Cần 1 can thiệp đơn giản
    static constexpr double WEIGHT_ESI_5 = 1.0;  // Cấp 5 (Xanh lá - Không khẩn): Khám ngoại trú thông thường

    // Trọng số phụ trội cho bệnh nhân quay lại đọc kết quả (Reentrant) để ưu tiên giải phóng phòng chờ
    static constexpr double REENTRANT_BONUS_WEIGHT = 3.0;

    // Công suất xử lý chuẩn của 1 bác sĩ cấp cứu (điểm áp lực / bác sĩ) trước khi rơi vào quá tải
    static constexpr double DOCTOR_CAPACITY_UNIT = 25.0;
    static constexpr double NURSE_CAPACITY_UNIT  = 18.0;

    // Điểm áp lực chuẩn hóa tối đa (tương ứng trạng thái bão hòa 100% công suất khoa Cấp cứu)
    static constexpr double MAX_BENCHMARK_PRESSURE = 120.0;

    // Phương thức nội bộ quét từng bệnh nhân và cộng dồn chỉ số vào báo cáo
    static void accumulatePatientMetrics(const Patient& p, QueuePressureReport& report) noexcept;

    // Phương thức nội bộ hoàn tất tính toán các tham số khuyến nghị cho Backtracking
    static void finalizeSchedulingParameters(QueuePressureReport& report, int currentActiveDoctors) noexcept;

public:
    QueuePressureCalculator() = delete; // Lớp tiện ích tĩnh, không khởi tạo đối tượng

    /**
     * @brief Lấy ngưỡng thời gian chờ tối đa (phút) trước khi bắt buộc Tái phân loại động (Dynamic Re-Triage).
     * Theo chuẩn đề tài: ESI 1 (0p), ESI 2 (10p), ESI 3 (30p), ESI 4 (60p), ESI 5 (120p).
     */
    static int getRetriageThresholdMinutes(int triageLevel) noexcept;

    /**
     * @brief Kiểm tra một bệnh nhân có đang chờ vượt chu kỳ bắt buộc cần đo lại sinh hiệu hay không.
     */
    static bool isOverdueForRetriage(const Patient& patient) noexcept;

    /**
     * @brief Lấy trọng số áp lực cơ bản tương ứng với từng cấp độ ESI (1 - 5).
     */
    static double getBaseEsiWeight(int triageLevel) noexcept;

    /**
     * @brief Tính điểm áp lực cá nhân của một bệnh nhân cụ thể (bao gồm ESI + Phạt chờ lâu + Reentrant).
     */
    static double calculateSinglePatientPressure(const Patient& patient) noexcept;

    /**
     * @brief [HÀM CỐT LÕI 1] Tính toán "Áp lực hàng đợi" từ 1 PriorityQueue<Patient> hiện tại.
     * Độ phức tạp: O(N) duyệt mảng Heap chỉ đọc, không làm thay đổi cấu trúc hàng đợi.
     * @param waitingQueue Hàng đợi ưu tiên chứa danh sách bệnh nhân đang chờ.
     * @param currentActiveDoctors Số bác sĩ đang trực hiện tại (mặc định = 2) để ước lượng độ tăng Fatigue.
     * @return QueuePressureReport Gói tham số đầu vào cho thuật toán xếp lịch của Thành viên B.
     */
    static QueuePressureReport evaluateQueuePressure(
        const PriorityQueue<Patient>& waitingQueue,
        int currentActiveDoctors = 2
    );

    /**
     * @brief [HÀM CỐT LÕI 2] Tính toán "Áp lực hàng đợi" cho mô hình Hàng đợi đa luồng (Multi-Queue):
     * Kết hợp cả Hàng đợi khám ban đầu (Initial Queue) và Hàng đợi tái nhập viện (Reentrant Queue).
     */
    static QueuePressureReport evaluateDualQueuePressure(
        const PriorityQueue<Patient>& initialQueue,
        const PriorityQueue<Patient>& reentrantQueue,
        int currentActiveDoctors = 2
    );

    /**
     * @brief Trích xuất danh sách các bệnh nhân đã chờ quá hạn cần đánh giá lại sinh hiệu (Dynamic Re-Triage).
     * Sử dụng cấu trúc Vector<Patient> tự định nghĩa (không dùng STL).
     */
    static Vector<Patient> getOverdueRetriagePatients(const PriorityQueue<Patient>& waitingQueue);

    /**
     * @brief Chuyển đổi mức áp lực (PressureLevel) sang chuỗi nhãn kèm mã màu trực quan.
     */
    static std::string pressureLevelToString(PressureLevel level);

    /**
     * @brief Hiển thị Bảng điều khiển Áp lực Hàng đợi & Tham số Lịch trực trên giao diện Console (CLI).
     */
    static void displayPressureReport(const QueuePressureReport& report);
};

#endif // QUEUE_PRESSURE_CALCULATOR_H