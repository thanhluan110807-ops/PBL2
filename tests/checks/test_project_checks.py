"""Các tên test_* theo giao diện discovery bắt buộc của unittest."""
import importlib.util
from pathlib import Path
import tempfile
import subprocess
import sys
import unittest

GocDuAn = Path(__file__).resolve().parents[2]
DacTa = importlib.util.spec_from_file_location("project_checks", GocDuAn / "tools/check_project_sources.py")
KiemTra = importlib.util.module_from_spec(DacTa)
DacTa.loader.exec_module(KiemTra)


class ProjectChecksTest(unittest.TestCase):
    def test_direct_stdexcept(self):
        for NoiDung in ['#include <stdexcept>\n', '# include /* ghi chu */ "stdexcept"\n', '#include\n <stdexcept>\n']:
            with self.subTest(NoiDung=NoiDung):
                self.assertTrue(KiemTra.KiemTraTep(Path('include/model/Bad.h'), NoiDung))

    def test_line_splicing(self):
        NoiDung = '#inc\\\nlude <stdexcept>\n'
        Loi = KiemTra.KiemTraTep(Path('src/model/Bad.cpp'), NoiDung)
        self.assertEqual(len(Loi), 1)
        self.assertEqual(Loi[0][1], 1)

    def test_comments_and_literals_are_not_code(self):
        NoiDung = '''// #include <stdexcept> QObject
/* QString */
const char* VanBan = "QObject Qt::red #include <stdexcept>";
const char* Mau = R"tag(
#include <stdexcept>
Q_OBJECT
)tag";
char KyTu = 'Q';
'''
        self.assertEqual(KiemTra.KiemTraTep(Path('include/model/Good.h'), NoiDung), [])

    def test_comments_between_tokens(self):
        Loi = KiemTra.KiemTraTep(Path('include/model/Bad.h'), '#include /* x */ <QtCore/QObject>\n')
        self.assertEqual(len(Loi), 1)
        self.assertIn('Include Qt', Loi[0][2])

    def test_qt_headers_and_symbols(self):
        for NoiDung in ['#include <QString>', '#include <QtCore/QDate>', '#include <qobject.h>',
                        'class A : public QObject {};', 'QString HoTen;', 'Q_OBJECT',
                        'Qt::Alignment CanLe;', 'QT_VERSION', 'signals:', 'public slots:', 'emit DaXong();']:
            with self.subTest(NoiDung=NoiDung):
                self.assertTrue(KiemTra.KiemTraTep(Path('src/service/Bad.cpp'), NoiDung))

    def test_standard_queue_and_domain_class_are_allowed(self):
        self.assertEqual(KiemTra.KiemTraTep(Path('include/dsa/Queue.h'), '#include <queue>\nclass QueueEntry {};'), [])

    def test_qt_is_allowed_in_gui(self):
        self.assertEqual(KiemTra.KiemTraTep(Path('src/ui/Window.cpp'), '#include <QObject>\nQ_OBJECT'), [])

    def test_stdexcept_is_still_rejected_in_gui_and_tests(self):
        for Tep in ['src/ui/Window.cpp', 'tests/model/test_a.cpp']:
            with self.subTest(Tep=Tep):
                self.assertTrue(KiemTra.KiemTraTep(Path(Tep), '#include <stdexcept>'))

    def test_source_line_is_reported(self):
        Loi = KiemTra.KiemTraTep(Path('include/model/Bad.h'), '// ghi chu\n\nQString Ten;\n')
        self.assertEqual(Loi[0][1], 3)

    def test_scope_excludes_vendor_build_and_gui_headers(self):
        with tempfile.TemporaryDirectory() as ThuMuc:
            Goc = Path(ThuMuc)
            CacTep = {
                'include/model/Good.h': '#pragma once\n',
                'include/ui/Window.h': '#include <QObject>\n',
                'src/model/Good.cpp': '// hop le\n',
                'include/model/vendor/Bad.h': '#include <stdexcept>\n',
                'include/model/third_party/Bad.h': '#include <stdexcept>\n',
                'src/util/external/Bad.cpp': '#include <stdexcept>\n',
                'include/model/build/Bad.h': '#include <stdexcept>\n',
                'include/model/build-debug/Bad.h': '#include <stdexcept>\n',
                'include/model/cmake-build-debug/Bad.h': '#include <stdexcept>\n',
                'build/include/model/Bad.h': '#include <stdexcept>\n',
                'third_party/Bad.h': '#include <stdexcept>\n',
            }
            for Ten, NoiDung in CacTep.items():
                Tep = Goc / Ten
                Tep.parent.mkdir(parents=True, exist_ok=True)
                Tep.write_text(NoiDung)
            TepDaDoc, Loi = KiemTra.KiemTraDuAn(Goc)
            self.assertEqual(Loi, [])
            self.assertEqual(len(TepDaDoc), 3)
            self.assertEqual(KiemTra.DanhSachHeaderLoi(Goc), [Path('include/model/Good.h')])

    def test_utf8_bom_is_not_a_bypass(self):
        self.assertTrue(KiemTra.KiemTraTep(Path('include/model/Bad.h'), '\ufeff#include <stdexcept>\n'))

    def test_spliced_line_comment_is_ignored(self):
        self.assertEqual(KiemTra.KiemTraTep(Path('include/model/Good.h'), '// ghi chu \\\n#include <stdexcept>\n'), [])

    def test_cli_reports_failure_and_path(self):
        with tempfile.TemporaryDirectory() as ThuMuc:
            Tep = Path(ThuMuc) / 'include/model/Bad.h'
            Tep.parent.mkdir(parents=True)
            Tep.write_text('#include <stdexcept>\n')
            KetQua = subprocess.run(
                [sys.executable, str(GocDuAn / 'tools/check_project_sources.py'), '--root', ThuMuc],
                text=True, capture_output=True)
            self.assertEqual(KetQua.returncode, 1)
            self.assertIn('include/model/Bad.h:1:', KetQua.stderr)

    def test_cli_does_not_pass_unreadable_source(self):
        with tempfile.TemporaryDirectory() as ThuMuc:
            Tep = Path(ThuMuc) / 'include/model/Bad.h'
            Tep.parent.mkdir(parents=True)
            Tep.write_bytes(b'\xff')
            KetQua = subprocess.run(
                [sys.executable, str(GocDuAn / 'tools/check_project_sources.py'), '--root', ThuMuc],
                text=True, capture_output=True)
            self.assertEqual(KetQua.returncode, 2)


    def test_symlinks_are_not_followed(self):
        with tempfile.TemporaryDirectory() as ThuMuc:
            Goc = Path(ThuMuc)
            (Goc / 'include/model').mkdir(parents=True)
            (Goc / 'third_party').mkdir()
            (Goc / 'third_party/Bad.h').write_text('#include <stdexcept>\n')
            try:
                (Goc / 'include/model/Linked.h').symlink_to(Goc / 'third_party/Bad.h')
                (Goc / 'include/model/external_link').symlink_to(Goc / 'third_party', target_is_directory=True)
            except OSError:
                self.skipTest('He thong khong cho phep tao symlink de kiem tra')
            self.assertEqual(KiemTra.TepDuAn(Goc), [])


if __name__ == '__main__':
    unittest.main()
