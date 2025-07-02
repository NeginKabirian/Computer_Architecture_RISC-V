#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    memory = new Memory();
    regFile = new RegisterFile();
    cpu = new CPU(memory, regFile);
    simulator = new ComputerSimulator();
    const std::string binaryPath = "assembletest.bin";
    if (!memory->loadFromFile(binaryPath, 0x1000)) {
        std::cerr << "Halting simulation due to file loading error." << std::endl;
    }
    qDebug() << "Current working directory:" << QDir::currentPath();
    simulator->initMemory(memory->dump(0, 32));
    cpu->setSimulator(simulator);
    connect(regFile, &RegisterFile::registerChanged, simulator,
            &ComputerSimulator::updateRegisterFile);
    connect(simulator->runAutoButton,&QPushButton::clicked,this,&MainWindow::on_runAutoButton_clicked);
    connect(simulator->pushButton,&QPushButton::clicked,this,&MainWindow::on_pushButton_clicked);
    connect(simulator->stopButton,&QPushButton::clicked,this,&MainWindow::on_stopButton_clicked);

    simulationTimer = new QTimer(this);

    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::simulationStep);




}

MainWindow::~MainWindow() {
    delete cpu;
    delete memory;
    delete regFile;
}

void MainWindow::on_pushButton_clicked() {

    if (simulationTimer->isActive()) {
        on_stopButton_clicked();
    }

    simulationStep();
}

void MainWindow::on_runAutoButton_clicked() {
    int interval = simulator->speedSpinBox->value();
    simulationTimer->setInterval(interval);

    simulationTimer->start();

    simulator->runAutoButton->setEnabled(false);
    simulator->pushButton->setEnabled(false);
    simulator->stopButton->setEnabled(true);
}

void MainWindow::on_stopButton_clicked() {
    simulationTimer->stop();

    simulator->runAutoButton->setEnabled(true);
    simulator->pushButton->setEnabled(true);
    simulator->stopButton->setEnabled(false);
}
void MainWindow::simulationStep() {

    if (cpu->isHalted()) {
        on_stopButton_clicked();
        qDebug() << "CPU Halted. Simulation stopped.";
        return;
    }
    cpu->clockTick();
    updateUI();
}

void MainWindow::updateUI() { cpu->printState(); }
