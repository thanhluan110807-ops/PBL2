// Phần B — Chức năng chính: Lưu và kiểm tra nguyện vọng ca trực.
#include "model/CaTrucMongMuon.h"
bool CaTrucMongMuon::HopLe() const {
    return GioBatDau >= 0 && GioBatDau < 24 && GioKetThuc >= 0 && GioKetThuc < 24 && GioBatDau != GioKetThuc;
}
