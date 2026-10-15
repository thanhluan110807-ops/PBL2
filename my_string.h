#ifndef MY_STRING_H
#define MY_STRING_H


class my_string
{
    private:
        char* data;
        int length;

    public:
        my_string();
        my_string(const char* text);
        my_string(const my_string& other);
        my_string(my_string&& other) noexcept;
        ~my_string();

        const my_string& operator=(const my_string& other);
        my_string& operator=(const char* text);

        int size() const noexcept;
        bool empty();
        const char* c_str() const noexcept;

        

};

#endif