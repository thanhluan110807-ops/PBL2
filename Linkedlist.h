#ifndef LinkedList_h
#define LinkedList_h

#include <cstddef>     // nullptr
#include <stdexcept>   // std::out_of_range (chỉ là lớp ngoại lệ, không phải container STL)
#include <utility>     // std::move

/* ============================================================
 *  LINKEDLIST.H - Danh sách liên kết đơn tự viết (Custom DSA)
 *  Thành viên B - Giai đoạn 1
 *
 *  - Template: dùng được cho int, string, Nhanvienyte*, ...
 *  - Rule of Five + Deep Copy: sao chép TOÀN BỘ các nút
 *  - Copy assignment dùng copy-and-swap -> an toàn ngoại lệ
 *  - Là nền cho HashTable (Chaining) và Standby Pool (Stack)
 *
 *  LƯU Ý: nếu T là con trỏ (vd Nhanvienyte*), danh sách chỉ giữ
 *  ĐỊA CHỈ, KHÔNG sở hữu đối tượng được trỏ tới -> không tự delete
 *  chúng. Người tạo đối tượng chịu trách nhiệm delete.
 * ============================================================ */

template <typename T>
class LinkedList {
private:
    // Nút của danh sách (ẩn hoàn toàn với bên ngoài - Đóng gói)
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
    };

    Node* head;   // nút đầu
    Node* tail;   // nút cuối (để Themcuoi chạy O(1))
    int size;     // số phần tử

public:
    // ---------------- Iterator (cho phép duyệt và dùng range-for) ----------------
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

    // ---------------- Rule of Five ----------------
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    // Copy constructor - DEEP COPY: tạo nút mới cho từng phần tử
    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), size(0) {
        try {
            for (Node* p = other.head; p != nullptr; p = p->next)
                Themcuoi(p->data);
        } catch (...) {
            Xoahet();   // lỗi giữa chừng (vd hết RAM) -> dọn phần đã tạo, không rò rỉ
            throw;
        }
    }

    // Copy assignment - copy-and-swap:
    // tạo bản sao tạm TRƯỚC; nếu bản sao thất bại thì *this vẫn nguyên vẹn
    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;   // chống tự gán
        LinkedList tmp(other);
        Hoandoi(tmp);
        return *this;                       // tmp chứa dữ liệu cũ, tự hủy khi ra khỏi hàm
    }

    // Move constructor - "cướp" các nút của other, không sao chép gì cả
    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), size(other.size) {
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }

    // Move assignment
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this == &other) return *this;
        Xoahet();                           // giải phóng dữ liệu cũ của mình
        head = other.head;
        tail = other.tail;
        size = other.size;
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
        return *this;
    }

    // Destructor - delete TỪNG nút để không rò rỉ bộ nhớ
    ~LinkedList() { Xoahet(); }

    // ---------------- Thêm ----------------
    void Themdau(const T& value) {          // O(1)
        Node* n = new Node(value);          // nếu new ném ngoại lệ, danh sách chưa bị đổi
        n->next = head;
        head = n;
        if (tail == nullptr) tail = n;
        size++;
    }

    void Themcuoi(const T& value) {         // O(1) nhờ có con trỏ tail
        Node* n = new Node(value);
        if (tail == nullptr) {
            head = tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
        size++;
    }

    // ---------------- Xóa ----------------
    bool Xoadau() {                         // O(1) - thao tác Pop của Stack
        if (head == nullptr) return false;
        Node* t = head;
        head = head->next;
        if (head == nullptr) tail = nullptr;
        delete t;
        size--;
        return true;
    }

    // Xóa phần tử ĐẦU TIÊN thỏa điều kiện pred (pred(const T&) -> bool). O(n)
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

    // Xóa phần tử đầu tiên bằng value (cần operator== của T). O(n)
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

    // ---------------- Tìm kiếm ----------------
    // Trả về CON TRỎ tới phần tử trong danh sách (sửa trực tiếp được), hoặc nullptr nếu không có.
    T* Timkiem(const T& value) {
        return Timtheo([&value](const T& x) { return x == value; });
    }
    const T* Timkiem(const T& value) const {
        return Timtheo([&value](const T& x) { return x == value; });
    }

    // Tìm theo điều kiện - HashTable dùng hàm này để tìm theo KEY. O(n)
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

    // ---------------- Truy cập ----------------
    T& Dau() {
        if (head == nullptr) throw std::out_of_range("LinkedList::Dau - danh sach rong");
        return head->data;
    }
    const T& Dau() const {
        if (head == nullptr) throw std::out_of_range("LinkedList::Dau - danh sach rong");
        return head->data;
    }
    T& Cuoi() {
        if (tail == nullptr) throw std::out_of_range("LinkedList::Cuoi - danh sach rong");
        return tail->data;
    }
    const T& Cuoi() const {
        if (tail == nullptr) throw std::out_of_range("LinkedList::Cuoi - danh sach rong");
        return tail->data;
    }

    // Lấy phần tử thứ index (bắt đầu từ 0). O(n)
    T& Lay(int index) {
        if (index < 0 || index >= size) throw std::out_of_range("LinkedList::Lay - chi so ngoai pham vi");
        Node* cur = head;
        while (index-- > 0) cur = cur->next;
        return cur->data;
    }
    const T& Lay(int index) const {
        if (index < 0 || index >= size) throw std::out_of_range("LinkedList::Lay - chi so ngoai pham vi");
        const Node* cur = head;
        while (index-- > 0) cur = cur->next;
        return cur->data;
    }

    // ---------------- Thông tin ----------------
    int Kichthuoc() const { return size; }
    bool Rong() const { return size == 0; }

    // Hoán đổi nội dung 2 danh sách (chỉ đổi con trỏ, O(1), không ném ngoại lệ)
    void Hoandoi(LinkedList& other) noexcept {
        Node* h = head;  head = other.head;  other.head = h;
        Node* t = tail;  tail = other.tail;  other.tail = t;
        int s = size;    size = other.size;  other.size = s;
    }
};

#endif // LinkedList_h