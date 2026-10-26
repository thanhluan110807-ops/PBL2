// uistyle.h - Hằng số màu và QSS dùng chung cho toàn bộ giao diện (header-only)
#ifndef UISTYLE_H
#define UISTYLE_H

#include <QString>

namespace UiStyle {

const char* const COLOR_PRIMARY = "#4F46E5";
const char* const COLOR_CALL = "#0D9488";
const char* const COLOR_OK = "#10B981";
const char* const COLOR_WARN = "#F59E0B";
const char* const COLOR_DANGER = "#EF4444";

// Màu nền / chữ của badge theo mức ESI 1..5 (1: Đỏ, 2: Cam, 3: Vàng, 4: Xanh dương, 5: Xanh lá)
inline void levelColors(int level, QString& bg, QString& fg) {
    switch (level) {
        case 1: bg = "#FEE2E2"; fg = "#991B1B"; break;
        case 2: bg = "#FFEDD5"; fg = "#9A3412"; break;
        case 3: bg = "#FEF3C7"; fg = "#92400E"; break;
        case 4: bg = "#DBEAFE"; fg = "#1E40AF"; break;
        default: bg = "#DCFCE7"; fg = "#166534"; break;
    }
}

inline QString levelName(int level) {
    switch (level) {
        case 1: return "Cấp 1 - Nguy kịch (Đỏ)";
        case 2: return "Cấp 2 - Khẩn cấp (Cam)";
        case 3: return "Cấp 3 - Trung bình (Vàng)";
        case 4: return "Cấp 4 - Nhẹ (Xanh dương)";
        default: return "Cấp 5 - Không khẩn (Xanh lá)";
    }
}

inline QString badgeStyle(int level) {
    QString bg, fg;
    levelColors(level, bg, fg);
    return QString("background:%1; color:%2; border-radius:6px; padding:3px 10px; font-weight:600;")
        .arg(bg, fg);
}

inline QString pressureBarStyle(const QString& color) {
    return QString("QProgressBar { border:1px solid #E2E8F0; border-radius:4px; background:#F1F5F9;"
                   " text-align:center; min-height:20px; }"
                   "QProgressBar::chunk { background:%1; border-radius:4px; }").arg(color);
}

inline QString globalStyle() {
    return QString(R"(
        * { font-family: "Segoe UI", "Inter", "Roboto", sans-serif; font-size: 13px; color: #1E293B; }
        QMainWindow, QDialog { background: #F4F6F9; }
        QMenuBar, QToolBar { background: #FFFFFF; border: none; border-bottom: 1px solid #E2E8F0; spacing: 8px; padding: 4px; }
        QGroupBox { background: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 8px;
                    margin-top: 14px; padding: 14px; font-size: 14px; font-weight: 600; }
        QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QPlainTextEdit, QTextBrowser {
            background: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 6px; padding: 4px 8px; min-height: 26px; }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QPlainTextEdit:focus {
            border: 1px solid %1; }
        QPushButton { background: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 6px;
                      padding: 6px 14px; min-height: 22px; font-weight: 500; }
        QPushButton:disabled { color: #94A3B8; background: #F1F5F9; }
        QPushButton#btnAddPatient, QPushButton#btnSave { background: %1; color: #FFFFFF; border: none; }
        QPushButton#btnAddPatient:hover, QPushButton#btnSave:hover { background: #4338CA; }
        QPushButton#btnCallNext { background: %2; color: #FFFFFF; border: none; }
        QPushButton#btnCallNext:hover { background: #0F766E; }
        QPushButton#btnDeletePatient { background: #FEE2E2; color: #991B1B; border: 1px solid #FCA5A5; }
        QPushButton#btnDeletePatient:disabled { background: #F1F5F9; color: #94A3B8; border: 1px solid #E2E8F0; }
        QTableWidget { background: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 8px;
                       gridline-color: #F1F5F9; selection-background-color: #E0E7FF; selection-color: #1E293B; }
        QHeaderView::section { background: #F8FAFC; border: none; border-bottom: 1px solid #E2E8F0;
                               padding: 6px; font-weight: 600; color: #64748B; }
        QStatusBar { background: #FFFFFF; border-top: 1px solid #E2E8F0; font-size: 12px; }
        QLabel#lblQueueHeader { font-size: 15px; font-weight: 600; }
        QLabel#lblCalledName { font-size: 20px; font-weight: 700; }
        QLabel#lblValidationError { color: #DC2626; }
    )").arg(COLOR_PRIMARY, COLOR_CALL);
}

} // namespace UiStyle

#endif // UISTYLE_H
