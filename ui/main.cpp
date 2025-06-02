#include "trayapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon appIcon(":/tlaloc.png"); 
    a.setWindowIcon(appIcon);
    TrayApp app;  
    return a.exec();
}