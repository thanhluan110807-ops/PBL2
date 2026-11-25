#ifndef HashTable_h
#define HashTable_h

#include <string>
#include "dsa/LinkedList.h"
#include "util/My_stdexcept.h"



















template <typename K>
struct Bamhash;

template <>
struct Bamhash<std::string> {




    unsigned long long operator()(const std::string& s) const {
        unsigned long long h = 5381;
        for (unsigned char c : s) h = h * 33 + c;
        h ^= h >> 33;
        h *= 0xff51afd7ed558ccdULL;
        h ^= h >> 33;
        return h;
    }
};

template <>
struct Bamhash<int> {

    unsigned long long operator()(int k) const {
        return static_cast<unsigned long long>(static_cast<unsigned int>(k)) * 2654435761u;
    }
};

template <typename K, typename V, typename Hash = Bamhash<K> >
class HashTable {
public:

    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

private:
    LinkedList<Entry>* buckets;
    int capacity;
    int count;
    Hash hasher;


    static double Nguongtai() { return 0.75; }

    int Chisovoi(const K& key, int cap) const {
        return static_cast<int>(hasher(key) % static_cast<unsigned long long>(cap));
    }
    int Chiso(const K& key) const { return Chisovoi(key, capacity); }




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

    explicit HashTable(int dungluongBandau = 16)
        : buckets(nullptr), capacity(dungluongBandau > 0 ? dungluongBandau : 16),
          count(0), hasher() {
        buckets = new LinkedList<Entry>[capacity];
    }


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


    HashTable& operator=(const HashTable& other) {
        if (this == &other) return *this;
        HashTable tmp(other);
        Hoandoi(tmp);
        return *this;
    }


    HashTable(HashTable&& other) noexcept
        : buckets(other.buckets), capacity(other.capacity), count(other.count),
          hasher(other.hasher) {
        other.buckets = nullptr;
        other.capacity = 0;
        other.count = 0;
    }


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


    ~HashTable() { delete[] buckets; }

    void Hoandoi(HashTable& other) noexcept {
        LinkedList<Entry>* b = buckets;  buckets = other.buckets;  other.buckets = b;
        int c = capacity;                capacity = other.capacity;  other.capacity = c;
        int n = count;                   count = other.count;        other.count = n;
        Hash h = hasher;                 hasher = other.hasher;      other.hasher = h;
    }




    bool Them(const K& key, const V& value) {
        if (buckets == nullptr) {
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
        Entry PhanTuMoi(key, value);
        if (static_cast<double>(count + 1) / capacity > Nguongtai()) {
            Rehash(capacity * 2);
            idx = Chiso(PhanTuMoi.key);
        }
        buckets[idx].Themcuoi(PhanTuMoi);
        count++;
        return true;
    }


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


    int Kichthuoc() const { return count; }
    bool Rong() const { return count == 0; }
    int Dungluong() const { return capacity; }


    double Hesotai() const {
        return capacity > 0 ? static_cast<double>(count) / capacity : 0.0;
    }


    double Tilexungdot() const {
        if (count == 0) return 0.0;
        int bucketCoNguoi = 0;
        for (int i = 0; i < capacity; i++)
            if (!buckets[i].Rong()) bucketCoNguoi++;
        return static_cast<double>(count - bucketCoNguoi) / count;
    }


    int Chuoidainhat() const {
        int maxLen = 0;
        for (int i = 0; i < capacity; i++)
            if (buckets[i].Kichthuoc() > maxLen) maxLen = buckets[i].Kichthuoc();
        return maxLen;
    }
};




#endif