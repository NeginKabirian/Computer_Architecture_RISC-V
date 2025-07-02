#include "mainwindow.h"
#include <QThread>
#include<QDir>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    memory = new Memory();
    regFile = new RegisterFile();
    cpu = new CPU(memory, regFile);
    auto simulator=new ComputerSimulator();
    simulator->initMemory(memory->dump(0,32));
    cpu->setSimulator(simulator);
    connect(simulator->pulse,&QPushButton::clicked,this,&MainWindow::on_pushButton_clicked);
    connect(regFile,&RegisterFile::registerChanged,simulator,&ComputerSimulator::updateRegisterFile);
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
}

// In MainWindow.cpp

void MainWindow::on_pushButton_clicked()
{


        cpu->printState();
        cpu->clockTick();


    //qDebug() << "Final PC:" << Qt::hex << cpu->getPC();
}
