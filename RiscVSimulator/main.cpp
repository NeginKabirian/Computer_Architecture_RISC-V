#include "computersimulator.h"
#include "mainwindow.h"
#include <QApplication>
#include <QThread>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
   // ComputerSimulator ui;

    //ui.show();

    // TEST
   // ui.updateRegisterFile(4, "REYHANEH");
    return a.exec();
}
