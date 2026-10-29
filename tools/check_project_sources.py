#!/usr/bin/env python3
"""Kiểm tra C++ do dự án quản lý; không mở rộng include sang thư viện khác."""

import argparse
import os
from pathlib import Path
import re
import sys

GocMaNguon = ("include", "src", "tests")
NhomLoi = ("model", "dsa", "service", "repository", "util")
DuoiHeader = {".h", ".hh", ".hpp", ".hxx"}
DuoiMaNguon = DuoiHeader | {".c", ".cc", ".cpp", ".cxx", ".inl", ".ipp", ".tpp"}
TenBoQua = {"third_party", "third-party", "thirdparty", "vendor", "external",
            "build", "_deps", "cmakefiles", ".git"}

# Nhận biết literal trước khi tìm ký hiệu Qt để không báo lỗi trên nội dung văn bản.
MauToken = re.compile(
    r'(?P<ChuThich>//[^\n]*|/\*.*?\*/)'
    r'|(?P<Raw>(?:u8|u|U|L)?R"(?P<DauRaw>[^\s()\\]{0,16})\(.*?\)(?P=DauRaw)")'
    r'|(?P<Chuoi>(?:u8|u|U|L)?"(?:\\[\s\S]|[^"\\\n])*")'
    r"|(?P<KyTu>(?:u8|u|U|L)?'(?:\\[\s\S]|[^'\\\n])*')",
    re.DOTALL,
)
MauInclude = re.compile(
    r'^[ \t\ufeff]*#[ \t]*include\s*(?:<(?P<Goc>[^>\n]+)>|"(?P<Nhay>[^"\n]+)")',
    re.MULTILINE,
)
MauQt = re.compile(r'\b(?:Q[A-Z][A-Za-z0-9_]*|Q_[A-Z0-9_]+|QT_[A-Z0-9_]+|Qt(?:[A-Z][A-Za-z0-9_]*)?)\b|\b(?:signals|slots)\s*:|\bemit\b')


def LaThuMucBoQua(Ten):
    Ten = Ten.lower()
    return Ten in TenBoQua or Ten.startswith(("build-", "cmake-build-"))


def BaoLoiDocThuMuc(Loi):
    raise Loi


def TepDuAn(Goc):
    """Chỉ đi vào include/src/tests; bỏ qua symlink và thư mục không thuộc phạm vi."""
    KetQua = []
    for TenGoc in GocMaNguon:
        ThuMuc = Goc / TenGoc
        if ThuMuc.is_symlink() or not ThuMuc.exists():
            continue
        for DuongDan, CacThuMuc, CacTep in os.walk(
                ThuMuc, followlinks=False, onerror=BaoLoiDocThuMuc):
            CacThuMuc[:] = sorted(
                Ten for Ten in CacThuMuc
                if not LaThuMucBoQua(Ten) and not (Path(DuongDan) / Ten).is_symlink())
            for Ten in sorted(CacTep):
                Tep = Path(DuongDan) / Ten
                if not Tep.is_symlink() and Tep.suffix.lower() in DuoiMaNguon:
                    KetQua.append(Tep.relative_to(Goc))
    return sorted(KetQua)


def LaMaLoi(Tep):
    return len(Tep.parts) > 2 and Tep.parts[0] in GocMaNguon and Tep.parts[1] in NhomLoi


def DanhSachHeaderLoi(Goc):
    return [Tep for Tep in TepDuAn(Goc)
            if Tep.parts[0] == "include" and LaMaLoi(Tep) and Tep.suffix.lower() in DuoiHeader]


def GhepDong(NoiDung):
    """Xử lý nối dòng C++ trước khi xét comment; giữ vị trí gốc để báo đúng số dòng."""
    CacPhan = []
    ViTriGoc = []
    ViTri = 0
    while ViTri < len(NoiDung):
        if NoiDung.startswith("\\\r\n", ViTri):
            ViTri += 3
        elif NoiDung.startswith("\\\n", ViTri):
            ViTri += 2
        else:
            CacPhan.append(NoiDung[ViTri])
            ViTriGoc.append(ViTri)
            ViTri += 1
    return "".join(CacPhan), ViTriGoc


def CheVanBan(VanBan):
    return "".join("\n" if KyTu == "\n" else " " for KyTu in VanBan)


def LaIncludeQt(Ten):
    # Gồm kiểu QtCore/..., QObject và header Qt dạng cũ như qobject.h.
    for ThanhPhan in Ten.replace("\\", "/").split("/"):
        if re.fullmatch(r'Qt[A-Za-z0-9_]*|qt[56]?|Q[A-Z][A-Za-z0-9_]*(?:\.h)?|q[a-z0-9_]+\.h', ThanhPhan):
            return True
    return False


def KiemTraTep(Tep, NoiDung):
    MaDaGhep, ViTriGoc = GhepDong(NoiDung)
    KhongChuThich = MauToken.sub(
        lambda Token: CheVanBan(Token.group()) if Token.group("ChuThich") is not None else Token.group(),
        MaDaGhep,
    )
    MaThuan = MauToken.sub(lambda Token: CheVanBan(Token.group()), MaDaGhep)
    Loi = set()

    def ThemLoi(ViTri, ThongBao):
        SoDong = NoiDung.count("\n", 0, ViTriGoc[ViTri]) + 1
        Loi.add((Tep.as_posix(), SoDong, ThongBao))

    for Include in MauInclude.finditer(KhongChuThich):
        # Dòng giống #include bên trong raw string chỉ là dữ liệu, không phải chỉ thị.
        ViTriDauThang = KhongChuThich.index("#", Include.start(), Include.end())
        if MaThuan[ViTriDauThang] != "#":
            continue
        Ten = Include.group("Goc") or Include.group("Nhay")
        if Ten == "stdexcept":
            ThemLoi(Include.start(), "Include truc tiep stdexcept; dung util/My_stdexcept.h")
        if LaMaLoi(Tep) and LaIncludeQt(Ten):
            ThemLoi(Include.start(), "Include Qt trong phan loi: " + Ten)
        # Header đã kiểm tra riêng, không xét lại tên header như một token C++.
        MaThuan = MaThuan[:Include.start()] + CheVanBan(MaThuan[Include.start():Include.end()]) + MaThuan[Include.end():]

    if LaMaLoi(Tep):
        for Token in MauQt.finditer(MaThuan):
            ThemLoi(Token.start(), "Ky hieu Qt trong phan loi: " + Token.group())
    return sorted(Loi)


def KiemTraDuAn(Goc):
    CacTep = TepDuAn(Goc)
    Loi = []
    for Tep in CacTep:
        Loi.extend(KiemTraTep(Tep, (Goc / Tep).read_text(encoding="utf-8")))
    return CacTep, Loi


def Main():
    ThamSo = argparse.ArgumentParser(description=__doc__)
    ThamSo.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    ThamSo.add_argument("--list-core-headers", action="store_true")
    TuyChon = ThamSo.parse_args()
    Goc = TuyChon.root.resolve()
    try:
        if TuyChon.list_core_headers:
            CacHeader = DanhSachHeaderLoi(Goc)
            if not CacHeader:
                raise ValueError("Khong tim thay header loi trong pham vi du an")
            for Tep in CacHeader:
                print(Tep.as_posix())
            return 0
        CacTep, Loi = KiemTraDuAn(Goc)
        if not CacTep:
            raise ValueError("Khong tim thay ma C/C++ cua du an de kiem tra")
        for Tep, SoDong, ThongBao in Loi:
            print(f"{Tep}:{SoDong}: {ThongBao}", file=sys.stderr)
        if Loi:
            return 1
        print(f"PASS: source policy ({len(CacTep)} project files)")
        return 0
    except (OSError, UnicodeError, ValueError) as Loi:
        print(f"Khong the kiem tra day du: {Loi}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(Main())
