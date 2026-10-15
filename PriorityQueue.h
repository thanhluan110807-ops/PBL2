// PriorityQueue.h
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdexcept>
#include <iostream>

/**
 * @class PriorityQueue
 * @brief Cấu trúc dữ liệu Hàng đợi ưu tiên tự xây dựng bằng thuật toán Max-Heap.
 * @tparam T Kiểu dữ liệu của phần tử trong hàng đợi (yêu cầu nạp chồng toán tử '<') [cite: 49].
 */
template <typename T>
class PriorityQueue {
private:
    T* heap;          // Mảng động lưu trữ các phần tử của Heap
    int capacity;     // Sức chứa tối đa hiện tại của mảng động
    int size;         // Số lượng phần tử thực tế đang có trong hàng đợi

    // Các phương thức bổ trợ nội bộ (Helper Methods) - Giấu kín trong private
    void resize();
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    // Constructor khởi tạo hàng đợi [cite: 49]
    explicit PriorityQueue(int initCapacity = 10);//explicit — Từ khóa chống ép kiểu ngầm định nguy hiểm

    // Destructor giải phóng vùng nhớ động tránh rò rỉ bộ nhớ [cite: 49]
    ~PriorityQueue();

    // =========================================================
    // NGUYÊN TẮC AN TOÀN BỘ NHỚ (RULE OF THREE) [cite: 49]
    // Vô hiệu hóa tính năng sao chép mặc định để tránh lỗi phân rã vùng nhớ (Double Free)
    // =========================================================
    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    // Các phương thức tương tác công khai (Public Interface) [cite: 49]
    bool isEmpty() const;
    int getSize() const;
    void push(const T& element);
    T pop();
    T peek() const;
};

// ============================================================================
//   PHẦN ĐỊNH NGHĨA CHI TIẾT (IMPLEMENTATION) - ĐẶT Ở PHÍA NGOÀI CLASS [cite: 49]
// ============================================================================

/**
 * @brief Constructor khởi tạo mảng động lưu trữ [cite: 49].
 */
template <typename T>
PriorityQueue<T>::PriorityQueue(int initCapacity)
    : heap(nullptr), capacity(initCapacity), size(0) {
    if (initCapacity <= 0) {
        throw std::invalid_argument("Suc chua ban dau cua Hang doi phai lon hon 0!"); // [cite: 74, 102]
    }
    heap = new T[capacity];
}

/**
 * @brief Destructor dọn dẹp vùng nhớ động khi đối tượng ra khỏi phạm vi hoạt động [cite: 49].
 */
template <typename T>
PriorityQueue<T>::~PriorityQueue() {
    delete[] heap;
    heap = nullptr; // Chống con trỏ lơ lửng (Dangling Pointer) nguy hiểm
}

/**
 * @brief Hàm tự động co giãn gấp đôi kích thước mảng khi hàng đợi bị đầy.
 */
template <typename T>
void PriorityQueue<T>::resize() {
    capacity *= 2;
    T* newHeap = new T[capacity];
    
    // Copy dữ liệu cũ sang mảng mới [cite: 49]
    for (int i = 0; i < size; ++i) {
        newHeap[i] = heap[i];
    }
    
    delete[] heap; // Giải phóng mảng cũ an toàn [cite: 49]
    heap = newHeap; // Trỏ sang vùng nhớ mới
}

/**
 * @brief Vun đống lên (Heapify Up) để duy trì cấu trúc Max-Heap khi thêm phần tử mới.
 */
template <typename T>
void PriorityQueue<T>::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        
        // GIẢI THÍCH CHO THÀNH VIÊN B: 
        // Toán tử '<' ở đây chính là operator< được định nghĩa trong class Patient!
        // Nếu nút cha "ít ưu tiên hơn" nút con, ta đổi chỗ để đẩy bệnh nhân nguy kịch lên đầu.
        if (heap[parent] < heap[index]) {
            T temp = heap[parent];
            heap[parent] = heap[index];
            heap[index] = temp;
            
            index = parent; // Tiếp tục lần ngược lên nút cha
        } else {
            break; // Đã đạt trạng thái cân bằng Heap
        }
    }
}

/**
 * @brief Vun đống xuống (Heapify Down) để duy trì cấu trúc Max-Heap khi lấy phần tử ở đỉnh ra.
 */
template <typename T>
void PriorityQueue<T>::heapifyDown(int index) {
    while (2 * index + 1 < size) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int largest = index;

        // So sánh với nút con bên trái
        if (heap[largest] < heap[leftChild]) {
            largest = leftChild;
        }
        // So sánh với nút con bên phải (nếu nút phải tồn tại trong phạm vi size)
        if (rightChild < size && heap[largest] < heap[rightChild]) {
            largest = rightChild;
        }

        // Nếu nút con có độ ưu tiên cao hơn nút cha, thực hiện đổi chỗ
        if (largest != index) {
            T temp = heap[index];
            heap[index] = heap[largest];
            heap[largest] = temp;
            
            index = largest; // Tiếp tục đi xuống các nhánh dưới
        } else {
            break; // Cấu trúc Heap đã ổn định
        }
    }
}

/**
 * @brief Kiểm tra xem hàng đợi có trống hay không.
 */
template <typename T>
bool PriorityQueue<T>::isEmpty() const {
    return size == 0;
}

/**
 * @brief Lấy số lượng phần tử hiện tại trong hàng đợi.
 */
template <typename T>
int PriorityQueue<T>::getSize() const {
    return size;
}

/**
 * @brief Thêm một phần tử mới vào hàng đợi.
 * @param element Phần tử cần thêm (ví dụ: đối tượng Patient) [cite: 49].
 */
template <typename T>
void PriorityQueue<T>::push(const T& element) {
    if (size == capacity) {
        resize(); // Tự động mở rộng bộ nhớ khi đầy
    }
    heap[size] = element; // Gán vào vị trí trống cuối cùng của Heap
    size++;
    heapifyUp(size - 1);  // Vun đống lên để tái thiết lập trật tự ưu tiên
}

/**
 * @brief Lấy phần tử có độ ưu tiên cao nhất ra khỏi hàng đợi và xóa nó.
 * @return T Phần tử có độ ưu tiên cao nhất được lấy ra.
 */
template <typename T>
T PriorityQueue<T>::pop() {
    if (isEmpty()) {
        throw std::underflow_error("Loi: Hang doi cap cuu hien dang trong rỗng!"); // [cite: 74, 102]
    }
    T rootValue = heap[0];       // Lưu lại phần tử ở đỉnh Heap (độ ưu tiên cao nhất)
    heap[0] = heap[size - 1]; // Đưa phần tử ở đáy Heap lên đỉnh
    size--;
    
    if (size > 0) {
        heapifyDown(0); // Vun đống xuống từ đỉnh để thiết lập lại trật tự Heap
    }
    return rootValue;
}

/**
 * @brief Xem trước phần tử đầu tiên mà không xóa nó khỏi hàng đợi.
 */
template <typename T>
T PriorityQueue<T>::peek() const {
    if (isEmpty()) {
        throw std::underflow_error("Loi: Hang doi cap cuu hien dang trong rỗng!"); // [cite: 74, 102]
    }
    return heap[0];
}

#endif // PRIORITY_QUEUE_H