#include "trayapp.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QIcon appIcon(":/tlaloc.png"); 
    a.setWindowIcon(appIcon);
    TrayApp app;

    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == "--server") {
            QMetaObject::invokeMethod(&app, "toggleServerMode", Qt::QueuedConnection);
        }
    }

    return a.exec();
}