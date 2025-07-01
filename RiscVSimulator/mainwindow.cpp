#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  //  ui->setupUi(this);
    memory = new Memory();
    regFile = new RegisterFile();
    cpu = new CPU(memory, regFile);
    auto simulator=new ComputerSimulator();
    cpu->setSimulator(simulator);
    connect(simulator->pulse,&QPushButton::clicked,this,&MainWindow::on_pushButton_clicked);
    connect(regFile,&RegisterFile::registerChanged,simulator,&ComputerSimulator::updateRegisterFile);
    const std::string binaryPath = "assembletest.bin";
    if (!memory->loadFromFile(binaryPath, 0x0)) {
        std::cerr << "Halting simulation due to file loading error." << std::endl;
    }
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
