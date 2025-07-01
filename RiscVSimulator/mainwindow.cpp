#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDir>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    memory = new Memory();
    regFile = new RegisterFile();
    cpu = new CPU(memory, regFile);
    const std::string binaryPath = "assembletest.bin";
    if (!memory->loadFromFile(binaryPath, 0x0)) {
        std::cerr << "Halting simulation due to file loading error." << std::endl;
    }
    qDebug() << "Current working directory:" << QDir::currentPath();

    memory->dump(0, 32);


}

MainWindow::~MainWindow()
{
    delete cpu;
    delete memory;
    delete regFile;
    delete ui;
}

// In MainWindow.cpp

void MainWindow::on_pushButton_clicked()
{


        cpu->printState();
        cpu->clockTick();


    //qDebug() << "Final PC:" << Qt::hex << cpu->getPC();
}
