// Person.h
#ifndef PERSON_H
#define PERSON_H

#include <string>

/**
 * @class Person
 * @brief Lớp cơ sở đại diện cho một cá nhân trong hệ thống bệnh viện.
 */
class Person {
protected:
    std::string id;
    std::string fullName;
    int age;
    std::string gender;

public:
    // Sử dụng 'explicit' để tránh lỗi ép kiểu ngầm định nguy hiểm của trình biên dịch [cite: 17]
    // Truyền 'const std::string&' thay vì 'std::string' để tránh copy chuỗi vô ích trong bộ nhớ [cite: 17, 74]
    explicit Person(
        const std::string& id = "", 
        const std::string& name = "", 
        int age = 0, 
        const std::string& gender = ""
    );

    // Destructor ảo bắt buộc phải có để tránh rò rỉ bộ nhớ ở lớp con [cite: 49]
    virtual ~Person();

    const std::string& getId() const noexcept;
    const std::string& getFullName() const noexcept;
    int getAge() const noexcept;
    const std::string& getGender() const noexcept;

    // Setters - Bổ sung các phương thức cập nhật thông tin [cite: 49]
    void setId(const std::string& id);
    void setFullName(const std::string& name);
    void setAge(int age);
    void setGender(const std::string& gender);

    // Phương thức ảo hiển thị thông tin (sẽ được ghi đè ở lớp con) [cite: 49]
    virtual void displayInfo() const;
};

#endif // PERSON_H