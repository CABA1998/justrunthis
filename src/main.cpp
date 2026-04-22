#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("justrunthis");
    QApplication::setOrganizationName("justrunthis");

    MainWindow w;
    w.show();
    return app.exec();
}
