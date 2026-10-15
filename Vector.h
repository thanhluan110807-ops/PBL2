#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>

template <typename T>
class Vector {
private:
    T* data;           // Con trỏ quản lý mảng động bên trong
    int capacity;      // Sức chứa hiện tại của mảng
    int count;         // Số lượng phần tử thực tế

    // Hàm tự động tăng gấp đôi sức chứa khi mảng bị đầy
    void reallocate(int newCapacity) {
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; ++i) {
            newData[i] = data[i]; // Copy dữ liệu cũ sang mảng mới
        }
        delete[] data; // Giải phóng mảng cũ
        data = newData;
        capacity = newCapacity;
    }

public:
    // Constructor
    explicit Vector(int initCapacity = 10) : capacity(initCapacity), count(0) {
        if (capacity <= 0) capacity = 10;
        data = new T[capacity];
    }

    // Destructor (RAII - Tự động giải phóng bộ nhớ)
    ~Vector() {
        delete[] data;
    }

    // Copy Constructor (Rule of Three/Five)
    Vector(const Vector& other) : capacity(other.capacity), count(other.count) {
        data = new T[capacity];
        for (int i = 0; i < count; ++i) {
            data[i] = other.data[i];
        }
    }

    // Copy Assignment Operator
    Vector& operator=(const Vector& other) {
       if (this == &other){
        return *this;
       }

       T* newData = new T[other.capacity];
       for (int i = 0; i < other.count; i++){
        newData[i] = other.data[i];
       }
       delete[] data;
       data = newData;
       capacity = other.capacity;
       count = other.count;
       return *this;
    }

    // Thêm phần tử vào cuối mảng (Tự động tăng dung lượng nếu đầy)
    void push_back(const T& value) {
        if (count >= capacity) {
            reallocate(capacity * 2);
        }
        data[count++] = value;
    }

    // Truy cập phần tử theo chỉ số (có kiểm tra biên)
    T& operator[](int index) {
        if (index < 0 || index >= count) {
            throw std::out_of_range("Vector index out of range!");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= count) {
            throw std::out_of_range("Vector index out of range!");
        }
        return data[index];
    }

    // Xóa toàn bộ dữ liệu
    void clear() {
        count = 0;
    }

    // Getters
    int size() const noexcept { return count; }
    int getCapacity() const noexcept { return capacity; }
    bool empty() const noexcept { return count == 0; }
};

#endif // VECTOR_H