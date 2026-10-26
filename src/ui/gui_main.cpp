// gui_main.cpp - Điểm vào của ứng dụng Qt (src/main.cpp là bản console cũ nên giữ nguyên)
#include <QApplication>
#include <QFont>
#include "ui/MainWindow.h"
#include "ui/uistyle.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setFont(QFont("Segoe UI", 10));
    app.setStyleSheet(UiStyle::globalStyle());

    MainWindow window;
    window.show();
    return app.exec();
}
