#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include <string>

class Person {
protected:
    std::string id;
    std::string fullName;
    int age;
    std::string gender;

public:
    Person(std::string id = "", std::string name = "", int age = 0, std::string gender = "")
        : id(id), fullName(name), age(age), gender(gender) {}

    virtual ~Person() {}

    // Getters & Setters
    std::string getId() const { return id; }
    std::string getFullName() const { return fullName; }
    int getAge() const { return age; }
    std::string getGender() const { return gender; }

    virtual void displayInfo() const {
        std::cout << "ID: " << id << " | Ten: " << fullName 
                  << " | Tuoi: " << age << " | Gioi tinh: " << gender << "\n";
    }
};

#endif