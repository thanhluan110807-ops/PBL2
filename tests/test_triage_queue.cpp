// test_triage_queue.cpp
#include <iostream>
#include <stdexcept>
#include "model/Person.h"
#include "model/Patient.h"
#include "util/PriorityQueue.h"

// Hàm bổ trợ in tiêu đề từng kịch bản test cho sạch đẹp
void printHeader(const std::string& title) {
    std::cout << "\n==================================================\n";
    std::cout << " " << title << "\n";
    std::cout << "==================================================\n";
}

int main() {
    try {
        // -----------------------------------------------------------------
        // TEST CASE 1: ĐIỀU PHỐI THEO THANG PHÂN LOẠI CẤP CỨU (ESI)
        // -----------------------------------------------------------------
        printHeader("TEST 1: PHÂN LOẠI ƯU TIÊN ESI (CẤP 1 -> CẤP 4)");
        
        // Khởi tạo hàng đợi cấp cứu với dung lượng ban đầu là 5
        PriorityQueue<Patient> triageQueue(5);

        // Tạo 4 bệnh nhân giả lập nhập viện
        Patient p1("BN01", "Nguyen Van A", 45, "Nam");
        p1.updateVitalSigns(150, 80, 25, 88.0, 7); // SpO2 < 90% & GCS <= 8 -> CẤP 1 (ĐỎ)

        Patient p2("BN02", "Tran Thi B", 30, "Nu");
        p2.updateVitalSigns(90, 120, 18, 98.0, 15); // Sinh hiệu bình thường -> CẤP 4 (XANH)

        Patient p3("BN03", "Le Van C", 60, "Nam");
        p3.updateVitalSigns(115, 170, 20, 95.0, 15); // Huyết áp cao (170) -> CẤP 3 (VÀNG)

        Patient p4("BN04", "Pham Thi D", 22, "Nu");
        p4.updateVitalSigns(100, 85, 22, 92.0, 12); // SpO2 < 94% & BP < 90 -> CẤP 2 (CAM)

        // Nạp bệnh nhân vào hàng đợi (Thứ tự nạp lộn xộn)
        std::cout << "[>] Đang nạp bệnh nhân vào hàng đợi...\n";
        triageQueue.push(p2); // Cấp 4
        triageQueue.push(p1); // Cấp 1
        triageQueue.push(p4); // Cấp 2
        triageQueue.push(p3); // Cấp 3

        std::cout << "=> Tổng số bệnh nhân đang chờ: " << triageQueue.getSize() << "\n\n";

        // Lấy từng bệnh nhân ra khám theo thứ tự ưu tiên của Heap
        std::cout << "[+] THỨ TỰ BÁC SĨ GỌI KHÁM (Mong đợi: Cấp 1 -> Cấp 2 -> Cấp 3 -> Cấp 4):\n";
        int stt = 1;
        while (!triageQueue.isEmpty()) {
            Patient current = triageQueue.pop();
            std::cout << "\nLượt " << stt++ << ":\n";
            current.displayInfo();
        }

        // -----------------------------------------------------------------
        // TEST CASE 2: TIÊU CHÍ PHỤ - CÙNG CẤP CỨU, AI ĐỜI LÂU HƠN KHÁM TRƯỚC
        // -----------------------------------------------------------------
        printHeader("TEST 2: SO SÁNH THỜI GIAN CHỜ KHI CÙNG MỨC CẤP CỨU");

        Patient pA("BN05", "Hoang Van E", 50, "Nam");
        pA.updateVitalSigns(100, 85, 20, 92.0, 12); // CẤP 2 (CAM)
        pA.setWaitingTime(15);                      // Đã chờ 15 phút

        Patient pB("BN06", "Vu Thi F", 28, "Nu");
        pB.updateVitalSigns(100, 85, 20, 92.0, 12); // CẤP 2 (CAM)
        pB.setWaitingTime(45);                      // Đã chờ 45 phút (Đợi lâu hơn)

        triageQueue.push(pA);
        triageQueue.push(pB);

        std::cout << "[+] Bệnh nhân ra khám trước (Mong đợi: Vu Thi F - Chờ 45 phút):\n";
        Patient firstOut = triageQueue.pop();
        firstOut.displayInfo();

        // Dọn dẹp hàng đợi
        triageQueue.pop(); 

        // -----------------------------------------------------------------
        // TEST CASE 3: TỰ ĐỘNG CO GIÃN BỘ NHỚ (RESIZE)
        // -----------------------------------------------------------------
        printHeader("TEST 3: TỰ ĐỘNG MỞ RỘNG MẢNG KHI ĐẦY");
        
        PriorityQueue<Patient> smallQueue(2); // Sức chứa ban đầu chỉ có 2
        std::cout << "[>] Tạo hàng đợi nhỏ (capacity = 2)...\n";
        
        // Thêm 5 bệnh nhân liên tiếp để kích hoạt hàm resize()
        for (int i = 1; i <= 5; ++i) {
            Patient p("TEMP_" + std::to_string(i), "Benh Nhan " + std::to_string(i), 20 + i, "Nam");
            smallQueue.push(p);
            std::cout << "-> Đã thêm Bệnh nhân " << i << " | Kích thước hiện tại: " << smallQueue.getSize() << "\n";
        }
        std::cout << "=> Mở rộng mảng tự động thành công, không bị lỗi tràn bộ nhớ!\n";

        // -----------------------------------------------------------------
        // TEST CASE 4: BẮT LỖI NGOẠI LỆ (EXCEPTION HANDLING)
        // -----------------------------------------------------------------
        printHeader("TEST 4: BẮT LỖI KHI HÀNG ĐỢI RỖNG");
        
        PriorityQueue<Patient> emptyQueue;
        std::cout << "[>] Thử pop() trên hàng đợi rỗng...\n";
        
        // Dòng này sẽ kích hoạt throw std::underflow_error
        emptyQueue.pop(); 

    } 
    catch (const std::underflow_error& e) {
        std::cerr << " [BẮT LỖI THÀNH CÔNG]: " << e.what() << "\n";
    }
    catch (const std::invalid_argument& e) {
        std::cerr << " [LỖI THAM SỐ]: " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << " [LỖI KHÔNG XÁC ĐỊNH]: " << e.what() << "\n";
    }

    printHeader("HOÀN THÀNH TẤT CẢ CÁC BÀI KIỂM THỬ!");
    return 0;
}