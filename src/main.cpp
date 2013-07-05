#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(w.canRun()) {
        w.show();

        return a.exec();
    }
    return EXIT_FAILURE;
}
