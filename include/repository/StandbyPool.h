
#pragma once
#include "dsa/LinkedList.h"
#include <string>


class StandbyPool {
    LinkedList<std::string> CacMa;
public:
    bool Co(const std::string& Ma) const { return CacMa.Timkiem(Ma) != nullptr; }
    bool Them(const std::string& Ma) {
        if (Co(Ma)) return false;
        CacMa.Themcuoi(Ma); return true;
    }
    bool Xoa(const std::string& Ma) { return CacMa.Xoagiatri(Ma); }
    template<class F> void Duyet(F Ham) const { for (const auto& Ma : CacMa) Ham(Ma); }
};
