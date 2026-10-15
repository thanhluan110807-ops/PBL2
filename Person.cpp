// Person.cpp
#include "Person.h"
#include <iostream>

// Constructor sử dụng Member Initializer List tối ưu hiệu năng [cite: 49]
Person::Person(const std::string& id, const std::string& name, int age, const std::string& gender)
    : id(id), fullName(name), age(age), gender(gender) {}

// Destructor ảo
Person::~Person() {}

// =========================================================
// PHẦN KHỞI TẠO CÁC PHƯƠNG THỨC GETTER [cite: 49]
// =========================================================

const std::string& Person::getId() const noexcept{ 
    return id; 
}

const std::string& Person::getFullName() const noexcept{ 
    return fullName; 
}

int Person::getAge() const noexcept{ 
    return age; 
}

const std::string& Person::getGender() const noexcept { 
    return gender; 
}

// =========================================================
// PHẦN KHỞI TẠO CÁC PHƯƠNG THỨC SETTER (CÓ VALIDATE DỮ LIỆU) [cite: 74, 102]
// =========================================================

void Person::setId(const std::string& newId) {
    if (!newId.empty()) { // Ngăn chặn gán ID rỗng phá hỏng logic hệ thống [cite: 74, 102]
        id = newId;
    }
}

void Person::setFullName(const std::string& name) {
    if (!name.empty()) {
        fullName = name;
    }
}

void Person::setAge(int newAge) {
    if (newAge >= 0) { // Đảm bảo tuổi không bao giờ âm (Lỗi logic lâm sàng) [cite: 74, 102]
        age = newAge;
    }
}

void Person::setGender(const std::string& newGender) {
    if (!newGender.empty()) {
        gender = newGender;
    }
}

// =========================================================
// PHƯƠNG THỨC HIỂN THỊ THÔNG TIN [cite: 49]
// =========================================================

void Person::displayInfo() const {
    std::cout << "ID: " << id 
              << " | Ten: " << fullName 
              << " | Tuoi: " << age 
              << " | Gioi tinh: " << gender << "\n";
}
