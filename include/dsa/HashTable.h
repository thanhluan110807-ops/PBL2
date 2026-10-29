#ifndef HashTable_h
#define HashTable_h

#include <cstddef>     // std::size_t
#include <string>
#include "dsa/LinkedList.h"
#include "util/My_stdexcept.h"

/* ============================================================
 *  HASHTABLE.H - Bảng băm tự viết (Custom DSA)
 *  Thành viên B - Giai đoạn 2
 *
 *  - Xử lý xung đột bằng CHAINING: mỗi ô (bucket) là một LinkedList
 *  - Hàm băm djb2 cho chuỗi (mã bệnh nhân / mã nhân viên)
 *  - Tự REHASH (gấp đôi dung lượng) khi hệ số tải > 0.75
 *  - Rule of Five + Deep Copy cho mảng bucket cấp phát động
 *  - Tra cứu / thêm / xóa trung bình O(1)
 *
 *  Dùng cho Late Data Binding: lưu V = Patient* (con trỏ).
 *  Bảng chỉ giữ ĐỊA CHỈ, không delete đối tượng được trỏ tới.
 * ============================================================ */

// ------------------------------------------------------------
//  Hàm băm: Bamhash<K>. Mặc định có cho std::string và int.
//  Muốn dùng kiểu khóa khác thì viết thêm một specialization.
// ------------------------------------------------------------
template <typename K>
struct Bamhash;

template <>
struct Bamhash<std::string> {
    // djb2: h = h * 33 + c, bắt đầu từ 5381.
    // Sau đó thêm bước "trộn bit" (avalanche) để các khóa chỉ khác nhau ở ký tự cuối
    // (vd EMG_..._001, EMG_..._002) vẫn rải đều khắp các bucket.
    std::size_t operator()(const std::string& s) const {
        unsigned long long h = 5381;
        for (unsigned char c : s) h = h * 33 + c;
        h ^= h >> 33;
        h *= 0xff51afd7ed558ccdULL;
        h ^= h >> 33;
        return static_cast<std::size_t>(h);
    }
};

template <>
struct Bamhash<int> {
    // Băm nhân (Knuth) giúp các số liên tiếp không dồn vào các ô sát nhau
    std::size_t operator()(int k) const {
        return static_cast<std::size_t>(static_cast<unsigned int>(k)) * 2654435761u;
    }
};

template <typename K, typename V, typename Hash = Bamhash<K> >
class HashTable {
public:
    // Một cặp (khóa, giá trị) lưu trong bucket
    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

private:
    LinkedList<Entry>* buckets;   // mảng các danh sách liên kết
    int capacity;                 // số bucket
    int count;                    // số phần tử đang lưu
    Hash hasher;

    // Ngưỡng hệ số tải: vượt mức này thì rehash
    static double Nguongtai() { return 0.75; }

    int Chisovoi(const K& key, int cap) const {
        return static_cast<int>(hasher(key) % static_cast<std::size_t>(cap));
    }
    int Chiso(const K& key) const { return Chisovoi(key, capacity); }

    // Dựng lại bảng với dung lượng mới.
    // An toàn ngoại lệ: dựng bảng MỚI xong hết rồi mới thay thế bảng cũ.
    // Nếu lỗi giữa chừng thì bảng cũ vẫn nguyên vẹn.
    void Rehash(int dungluongmoi) {
        LinkedList<Entry>* mang = new LinkedList<Entry>[dungluongmoi];
        try {
            for (int i = 0; i < capacity; i++)
                for (const Entry& e : buckets[i])
                    mang[Chisovoi(e.key, dungluongmoi)].Themcuoi(e);
        } catch (...) {
            delete[] mang;
            throw;
        }
        delete[] buckets;
        buckets = mang;
        capacity = dungluongmoi;
    }

public:
    // ---------------- Rule of Five ----------------
    explicit HashTable(int dungluongBandau = 16)
        : buckets(nullptr), capacity(dungluongBandau > 0 ? dungluongBandau : 16),
          count(0), hasher() {
        buckets = new LinkedList<Entry>[capacity];
    }

    // Copy constructor - DEEP COPY: sao chép từng bucket (và từng nút bên trong)
    HashTable(const HashTable& other)
        : buckets(nullptr), capacity(other.capacity), count(other.count), hasher(other.hasher) {
        if (other.buckets != nullptr) {
            buckets = new LinkedList<Entry>[capacity];
            try {
                for (int i = 0; i < capacity; i++) buckets[i] = other.buckets[i];
            } catch (...) {
                delete[] buckets;
                throw;
            }
        } else {
            capacity = 0;
            count = 0;
        }
    }

    // Copy assignment - copy-and-swap (an toàn ngoại lệ)
    HashTable& operator=(const HashTable& other) {
        if (this == &other) return *this;
        HashTable tmp(other);
        Hoandoi(tmp);
        return *this;
    }

    // Move constructor - cướp mảng bucket, không sao chép gì
    HashTable(HashTable&& other) noexcept
        : buckets(other.buckets), capacity(other.capacity), count(other.count),
          hasher(other.hasher) {
        other.buckets = nullptr;
        other.capacity = 0;
        other.count = 0;
    }

    // Move assignment
    HashTable& operator=(HashTable&& other) noexcept {
        if (this == &other) return *this;
        delete[] buckets;
        buckets = other.buckets;
        capacity = other.capacity;
        count = other.count;
        hasher = other.hasher;
        other.buckets = nullptr;
        other.capacity = 0;
        other.count = 0;
        return *this;
    }

    // Destructor - delete[] mảng bucket (mỗi LinkedList tự delete các nút của nó)
    ~HashTable() { delete[] buckets; }

    void Hoandoi(HashTable& other) noexcept {
        LinkedList<Entry>* b = buckets;  buckets = other.buckets;  other.buckets = b;
        int c = capacity;                capacity = other.capacity;  other.capacity = c;
        int n = count;                   count = other.count;        other.count = n;
        Hash h = hasher;                 hasher = other.hasher;      other.hasher = h;
    }

    // ---------------- Thao tác chính ----------------
    // Thêm mới hoặc cập nhật giá trị nếu khóa đã có.
    // Trả về true nếu là khóa MỚI, false nếu chỉ cập nhật.
    bool Them(const K& key, const V& value) {
        if (buckets == nullptr) {               // đối tượng vừa bị move đi -> dựng lại bảng rỗng
            buckets = new LinkedList<Entry>[16];
            capacity = 16;
            count = 0;
        }
        int idx = Chiso(key);
        Entry* e = buckets[idx].Timtheo([&key](const Entry& x) { return x.key == key; });
        if (e != nullptr) {
            e->value = value;
            return false;
        }
        if (static_cast<double>(count + 1) / capacity > Nguongtai()) {
            Rehash(capacity * 2);
            idx = Chiso(key);                   // dung lượng đổi -> phải tính lại chỉ số
        }
        buckets[idx].Themcuoi(Entry(key, value));
        count++;
        return true;
    }

    // Tìm theo khóa: trả về CON TRỎ tới giá trị (nullptr nếu không có). Trung bình O(1)
    V* Timkiem(const K& key) {
        if (buckets == nullptr) return nullptr;
        Entry* e = buckets[Chiso(key)].Timtheo([&key](const Entry& x) { return x.key == key; });
        return e != nullptr ? &e->value : nullptr;
    }
    const V* Timkiem(const K& key) const {
        if (buckets == nullptr) return nullptr;
        const Entry* e = buckets[Chiso(key)].Timtheo([&key](const Entry& x) { return x.key == key; });
        return e != nullptr ? &e->value : nullptr;
    }

    bool Cokhoa(const K& key) const { return Timkiem(key) != nullptr; }

    // Lấy giá trị theo khóa; ném ngoại lệ nếu không có
    V& Lay(const K& key) {
        V* p = Timkiem(key);
        if (p == nullptr) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "HashTable::Lay - khong tim thay khoa");
        return *p;
    }
    const V& Lay(const K& key) const {
        const V* p = Timkiem(key);
        if (p == nullptr) throw My_stdexcept(My_stdexcept::LoaiLoi::NgoaiPhamVi, "HashTable::Lay - khong tim thay khoa");
        return *p;
    }

    bool Xoa(const K& key) {
        if (buckets == nullptr) return false;
        bool daxoa = buckets[Chiso(key)].Xoatheo([&key](const Entry& x) { return x.key == key; });
        if (daxoa) count--;
        return daxoa;
    }

    void Xoahet() {
        for (int i = 0; i < capacity; i++) buckets[i].Xoahet();
        count = 0;
    }

    // Duyệt toàn bộ cặp (khóa, giá trị): fn(const K&, V&)
    template <typename Fn>
    void Duyet(Fn fn) {
        for (int i = 0; i < capacity; i++)
            for (Entry& e : buckets[i]) fn(e.key, e.value);
    }
    template <typename Fn>
    void Duyet(Fn fn) const {
        for (int i = 0; i < capacity; i++)
            for (const Entry& e : buckets[i]) fn(e.key, e.value);
    }

    // ---------------- Thông tin & thống kê (phục vụ báo cáo/benchmark) ----------------
    int Kichthuoc() const { return count; }
    bool Rong() const { return count == 0; }
    int Dungluong() const { return capacity; }

    // Hệ số tải = số phần tử / số bucket
    double Hesotai() const {
        return capacity > 0 ? static_cast<double>(count) / capacity : 0.0;
    }

    // Tỉ lệ xung đột = số phần tử phải "chen" vào bucket đã có người / tổng phần tử
    double Tilexungdot() const {
        if (count == 0) return 0.0;
        int bucketCoNguoi = 0;
        for (int i = 0; i < capacity; i++)
            if (!buckets[i].Rong()) bucketCoNguoi++;
        return static_cast<double>(count - bucketCoNguoi) / count;
    }

    // Độ dài chuỗi dài nhất (trường hợp xấu nhất của một lần tra cứu)
    int Chuoidainhat() const {
        int maxLen = 0;
        for (int i = 0; i < capacity; i++)
            if (buckets[i].Kichthuoc() > maxLen) maxLen = buckets[i].Kichthuoc();
        return maxLen;
    }
};

#endif // HashTable_h