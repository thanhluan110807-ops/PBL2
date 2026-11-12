#!/usr/bin/env python3
"""Chạy demo WSL/Linux với bản sao dữ liệu mẫu, tách khỏi dữ liệu đang dùng."""

import argparse
import csv
import os
from pathlib import Path
import shutil
import sys
import tempfile


def main():
    BoDoc = argparse.ArgumentParser(description=__doc__)
    BoDoc.add_argument("chuong_trinh", nargs="?", help="Đường dẫn pbl2_qt_demo đã build")
    ThamSo = BoDoc.parse_args()
    if not sys.platform.startswith("linux"):
        BoDoc.error("Hãy chạy lệnh này trong terminal WSL/Linux với bản build Linux.")

    ThuMucScript = Path(__file__).resolve().parent
    ThuMucDuAn = ThuMucScript.parent.parent
    if ThamSo.chuong_trinh:
        ChuongTrinh = Path(ThamSo.chuong_trinh).resolve()
    else:
        CacBanBuild = [ThuMucDuAn / Ten / "demo/qt-triage/pbl2_qt_demo"
                      for Ten in ("build-qt-demo", "build-demo", "build")]
        ChuongTrinh = next((Tep for Tep in CacBanBuild if Tep.is_file()), CacBanBuild[0])
    if ChuongTrinh.suffix.lower() == ".exe":
        BoDoc.error("Cần bản build Linux pbl2_qt_demo, không dùng .exe Windows trong lệnh này.")
    if not ChuongTrinh.is_file() or not os.access(ChuongTrinh, os.X_OK):
        BoDoc.error("Chưa thấy chương trình chạy được. Hãy build Qt demo hoặc truyền đường dẫn binary.")

    ThuMucMau = ThuMucScript / "data/demo-day-du"
    CacTep = ("patient.csv", "visit.csv", "nhan-su-lich.pbl2")
    for Ten in CacTep:
        if not (ThuMucMau / Ten).is_file():
            BoDoc.error("Thiếu file mẫu: " + str(ThuMucMau / Ten))

    # Mỗi lần mở có một bản sao mới; không đổi cấu hình, file thật hoặc mẫu gốc.
    ThuMucPhien = Path(tempfile.mkdtemp(prefix="pbl2-demo-"))
    CauHinh = ThuMucPhien / "config/PBL2/AnTam"
    DuLieu = ThuMucPhien / "data/PBL2/AnTam"
    CauHinh.mkdir(parents=True)
    DuLieu.mkdir(parents=True)
    for Ten in CacTep:
        shutil.copyfile(ThuMucMau / Ten, DuLieu / Ten)
    # Dùng đúng cấu hình hiện có của chương trình để tự nạp, không mở hộp chọn file.
    with (CauHinh / "bo-file.csv").open("w", encoding="utf-8", newline="") as Tep:
        csv.writer(Tep).writerow(["PBL2_FILES", "1", str(DuLieu / "patient.csv"), str(DuLieu / "visit.csv")])

    MoiTruong = os.environ.copy()
    MoiTruong["XDG_CONFIG_HOME"] = str(ThuMucPhien / "config")
    MoiTruong["XDG_DATA_HOME"] = str(ThuMucPhien / "data")
    print("Đang mở bộ demo hư cấu: 16 bệnh nhân, 17 lượt khám, 20 nhân viên.", flush=True)
    print("Bản sao phiên demo: " + str(ThuMucPhien), flush=True)
    # Giữ bản sao sau khi đóng để còn dữ liệu đã thử; /tmp có thể được hệ thống dọn.
    os.execve(str(ChuongTrinh), [str(ChuongTrinh)], MoiTruong)


if __name__ == "__main__":
    try:
        main()
    except OSError as Loi:
        print("Không mở được demo: " + str(Loi), file=sys.stderr)
        sys.exit(1)
