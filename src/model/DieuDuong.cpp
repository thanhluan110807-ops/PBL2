// Phần B — Chức năng chính: Quản lý điều dưỡng và thời gian trực sàng lọc.
#include "model/DieuDuong.h"
#include "util/My_stdexcept.h"
#include <cmath>
#include <utility>
namespace {
void YeuCau(bool Dung,const char* Loi) {
    if (!Dung) throw My_stdexcept(My_stdexcept::LoaiLoi::DoiSoKhongHopLe,Loi);
}
}
DieuDuong::DieuDuong(const std::string& Ma, const std::string& Ten, CapBac Bac,
                     int KinhNghiem, double DiemNen, const CaTrucMongMuon& MongMuon)
    : NhanVienYTe(Ma, Ten, Bac, KinhNghiem, DiemNen, MongMuon) { DatCapBac(Bac); }
void DieuDuong::BatDauTrucSangLoc() {
    YeuCau(!TheoDongHo,"Dang theo dong ho; khong tron voi che do cong gio");
    if (!DangTrucSangLoc) { DangTrucSangLoc=true; GioSangLocLienTuc=0; }
}
void DieuDuong::KetThucTrucSangLoc() {
    DangTrucSangLoc=false; GioSangLocLienTuc=0; TheoDongHo=false; BatDauSangLoc=MocDongHo=0;
}
void DieuDuong::CongDonGioSangLoc(double SoGio) {
    YeuCau(!TheoDongHo,"Khong cong gio thu cong khi dang theo dong ho");
    YeuCau(std::isfinite(SoGio) && SoGio>=0 && std::isfinite(GioSangLocLienTuc+SoGio),"Gio sang loc khong hop le");
    if (DangTrucSangLoc) GioSangLocLienTuc+=SoGio;
}
bool DieuDuong::CanXoayTuaSangLoc() const { return DangTrucSangLoc && GioSangLocLienTuc>=4; }
void DieuDuong::BatDauTrucSangLoc(std::time_t ThoiDiem) {
    YeuCau(ThoiDiem>=0,"Thoi diem khong hop le");
    if (DangTrucSangLoc) {
        YeuCau(TheoDongHo,"Dang cong gio thu cong; ket thuc dot cu truoc");
        CapNhatThoiGianSangLoc(ThoiDiem); return; // Không reset mốc ban đầu.
    }
    DangTrucSangLoc=true; TheoDongHo=true; GioSangLocLienTuc=0;
    BatDauSangLoc=MocDongHo=ThoiDiem;
}
double DieuDuong::TinhThoiGianSangLocLienTuc(std::time_t ThoiDiem) const {
    if (!DangTrucSangLoc) return 0;
    if (!TheoDongHo) return GioSangLocLienTuc;
    YeuCau(ThoiDiem>=MocDongHo,"Thoi diem khong duoc lui so voi moc da ghi nhan");
    return GioSangLocLienTuc+std::difftime(ThoiDiem,MocDongHo)/3600.0;
}
void DieuDuong::CapNhatThoiGianSangLoc(std::time_t ThoiDiem) {
    YeuCau(DangTrucSangLoc && TheoDongHo,"Chua bat dau sang loc theo dong ho");
    const double GioMoi=TinhThoiGianSangLocLienTuc(ThoiDiem);
    YeuCau(std::isfinite(GioMoi),"Thoi gian vuot gioi han");
    GioSangLocLienTuc=GioMoi; MocDongHo=ThoiDiem;
}
bool DieuDuong::CanXoayTuaSangLoc(std::time_t ThoiDiem) const {
    return DangTrucSangLoc && TinhThoiGianSangLocLienTuc(ThoiDiem)>=4;
}
void DieuDuong::KetThucTrucSangLoc(std::time_t ThoiDiem) {
    if (DangTrucSangLoc && TheoDongHo) CapNhatThoiGianSangLoc(ThoiDiem);
    KetThucTrucSangLoc();
}
void DieuDuong::KhoiPhucSangLoc(bool Dang,double SoGio,bool Theo,std::time_t BatDau,std::time_t MocCuoi) {
    YeuCau(std::isfinite(SoGio) && SoGio>=0,"Gio sang loc khong hop le");
    YeuCau(Dang || (SoGio==0 && !Theo),"Trang thai sang loc khong nhat quan");
    if (Theo) {
        YeuCau(Dang && BatDau>=0 && MocCuoi>=BatDau,"Moc sang loc khong hop le");
        YeuCau(std::fabs(SoGio-std::difftime(MocCuoi,BatDau)/3600.0)<0.000001,"So gio khong khop moc dong ho");
    } else YeuCau(BatDau==0 && MocCuoi==0,"Che do thu cong khong co moc dong ho");
    DangTrucSangLoc=Dang; GioSangLocLienTuc=SoGio; TheoDongHo=Theo;
    BatDauSangLoc=BatDau; MocDongHo=MocCuoi;
}
void DieuDuong::HoanDoiDuLieu(NhanVienYTe& Khac) {
    auto* DD=dynamic_cast<DieuDuong*>(&Khac);
    YeuCau(DD!=nullptr,"Khong the doi dieu duong thanh bac si");
    HoanDoiThongTinChung(*DD);
    std::swap(DangTrucSangLoc,DD->DangTrucSangLoc); std::swap(GioSangLocLienTuc,DD->GioSangLocLienTuc);
    std::swap(TheoDongHo,DD->TheoDongHo); std::swap(BatDauSangLoc,DD->BatDauSangLoc); std::swap(MocDongHo,DD->MocDongHo);
}
