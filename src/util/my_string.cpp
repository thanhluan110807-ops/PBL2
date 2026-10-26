#include "util/my_string.h"
#include <cstddef>

int my_string_length(const char* s){
    int length = 0;
    while (*s != '\0'){
        length++;
    }
    return length;
}

void my_string_copy( char* destination, const char* source){
    int i =0;
    while (source[i] != '\0'){
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

void my_string_append( char* destination, const char* source){
    int i=0;
    while (destination[i] != '\0'){
        i++;
    }
    int j = 0;
    while (source[j] != '\0'){
        destination[i] = source[j];
        i++;
        j++;
    }
    destination[i] = '\0';
}


int my_string_compare(const char *left, const char *right) {
    int i = 0;
    while (left[i] != '\0' && right[i] != '\0') {
        if (left[i] != right[i]) {
            return left[i] - right[i];
        }
        ++i;
    }
    return left[i] - right[i];
}



my_string::my_string()
    : data(new char[1]), length(0)
    {
        data[0] = '\0';
    }

my_string::my_string(const char* text)
    : data(nullptr), length(0){
        if (text == nullptr){
            data = new char[1];
            data[0] = '\0';
            return;
        }
        length = my_string_length(text);
        data = new char[length + 1];
        my_string_copy(data, text);
    }
    
my_string::my_string(const my_string& other)
    : data(nullptr), length(other.length)
{
    data = new char[length + 1];
    my_string_copy(data, other.data);
}

my_string::~my_string(){
    delete[] data;
}

my_string::my_string(my_string&& other) noexcept{
    data = other.data;
    
}

const my_string& my_string::operator=(const my_string& other){
    if (this == &other) return *this;
    delete[] data; 
    length = other.length;
    data = new char[length + 1];
    my_string_copy(data, other.data);
    return *this;

}

my_string& my_string::operator=(const char* text){
    if (data != nullptr) delete[] data;
    if (text == nullptr){
        data = new char[1];
        data[0] = '\0';
        length = 0;
        return *this;
    }
    length = my_string_length(text);
    data = new char[length + 1];
    my_string_copy(data, text);
    return *this;
}

int my_string::size() const noexcept{
    return length;
}


bool my_string::empty(){
    return length == 0;

}
const char* my_string::c_str() const noexcept{
    return data;
}
