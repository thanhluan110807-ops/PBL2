#ifndef LinkedList_h
#define LinkedList_h


#include "util/My_stdexcept.h"
#include <utility>















template <typename T>
class LinkedList {
private:

    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

public:

    class Iterator {
    private:
        Node* cur;
    public:
        explicit Iterator(Node* n = nullptr) : cur(n) {}
        T& operator*() const { return cur->data; }
        T* operator->() const { return &cur->data; }
        Iterator& operator++() { cur = cur->next; return *this; }
        Iterator operator++(int) { Iterator t(*this); cur = cur->next; return t; }
        bool operator==(const Iterator& o) const { return cur == o.cur; }
        bool operator!=(const Iterator& o) const { return cur != o.cur; }
    };

    class ConstIterator {
    private:
        const Node* cur;
    public:
        explicit ConstIterator(const Node* n = nullptr) : cur(n) {}
        const T& operator*() const { return cur->data; }
        const T* operator->() const { return &cur->data; }
        ConstIterator& operator++() { cur = cur->next; return *this; }
        ConstIterator operator++(int) { ConstIterator t(*this); cur = cur->next; return t; }
        bool operator==(const ConstIterator& o) const { return cur == o.cur; }
        bool operator!=(const ConstIterator& o) const { return cur != o.cur; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }


    LinkedList() : head(nullptr), tail(nullptr), size(0) {}


    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), size(0) {
        try {
            for (Node* p = other.head; p != nullptr; p = p->next)
                Themcuoi(p->data);
        } catch (...) {
            Xoahet();
            throw;
        }
    }



    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;
        LinkedList tmp(other);
        Hoandoi(tmp);
        return *this;
    }


    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), size(other.size) {
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }


    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this == &other) return *this;
        Xoahet();
        head = other.head;
        tail = other.tail;
        size = other.size;
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
        return *this;
    }


    ~LinkedList() { Xoahet(); }


    void Themdau(const T& value) {
        Node* n = new Node(value);
        n->next = head;
        head = n;
        if (tail == nullptr) tail = n;
        size++;
    }

    void Themcuoi(const T& value) {
        Node* n = new Node(value);
        if (tail == nullptr) {
            head = tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
        size++;
    }


    bool Xoadau() {
        if (head == nullptr) return false;
        Node* t = head;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete t;
        size--;
        return true;
    }


    template <typename Pred>
    bool Xoatheo(Pred pred) {
        Node* prev = nullptr;
        for (Node* cur = head; cur != nullptr; prev = cur, cur = cur->next) {
            if (pred(cur->data)) {
                if (prev == nullptr) head = cur->next;
                else prev->next = cur->next;
                if (cur == tail) tail = prev;
                delete cur;
                size--;
                return true;
            }
        }
        return false;
    }


    bool Xoagiatri(const T& value) {
        return Xoatheo([&value](const T& x) { return x == value; });
    }

    void Xoahet() {
        Node* cur = head;
        while (cur != nullptr) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        size = 0;
    }



    T* Timkiem(const T& value) {
        return Timtheo([&value](const T& x) { return x == value; });
    }
    const T* Timkiem(const T& value) const {
        return Timtheo([&value](const T& x) { return x == value; });
    }


    template <typename Pred>
    T* Timtheo(Pred pred) {
        for (Node* cur = head; cur != nullptr; cur = cur->next)
            if (pred(cur->data)) return &cur->data;
        return nullptr;
    }
    template <typename Pred>
    const T* Timtheo(Pred pred) const {
        for (const Node* cur = head; cur != nullptr; cur = cur->next)
            if (pred(cur->data)) return &cur->data;
        return nullptr;
    }


    T& Dau() {
        if (head == nullptr) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "LinkedList::Dau - danh sach rong");
        return head->data;
    }
    const T& Dau() const {
        if (head == nullptr) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "LinkedList::Dau - danh sach rong");
        return head->data;
    }
    T& Cuoi() {
        if (tail == nullptr) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "LinkedList::Cuoi - danh sach rong");
        return tail->data;
    }
    const T& Cuoi() const {
        if (tail == nullptr) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "LinkedList::Cuoi - danh sach rong");
        return tail->data;
    }


    T& Lay(int index) {
        if (index < 0 || index >= size) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "LinkedList::Lay - chi so ngoai pham vi");
        Node* cur = head;
        while (index-- > 0) cur = cur->next;
        return cur->data;
    }
    const T& Lay(int index) const {
        if (index < 0 || index >= size) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "LinkedList::Lay - chi so ngoai pham vi");
        const Node* cur = head;
        while (index-- > 0) cur = cur->next;
        return cur->data;
    }


    int Kichthuoc() const { return size; }
    bool Rong() const { return size == 0; }


    void Hoandoi(LinkedList& other) noexcept {
        Node* h = head;  head = other.head;  other.head = h;
        Node* t = tail;  tail = other.tail;  other.tail = t;
        int s = size;    size = other.size;  other.size = s;
    }
};





#endif