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
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::simulationStep);
    const std::string binaryPath = "assembletest.bin";
    if (!memory->loadFromFile(binaryPath, 0x1000)) {
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

/*void MainWindow::on_pushButton_clicked()
{


        cpu->printState();
        cpu->clockTick();


    //qDebug() << "Final PC:" << Qt::hex << cpu->getPC();
}*/
void MainWindow::on_pushButton_clicked()
{
    // اگر شبیه‌سازی خودکار در حال اجراست، آن را متوقف کن
    if (simulationTimer->isActive()) {
        on_stopButton_clicked();
    }

    // یک سیکل کلاک اجرا کن
    simulationStep();
}

// شروع اجرای خودکار
void MainWindow::on_runAutoButton_clicked()
{
    // گرفتن فاصله زمانی از SpinBox
    int interval = ui->speedSpinBox->value();
    simulationTimer->setInterval(interval); // تنظیم فاصله زمانی تایمر

    simulationTimer->start(); // شروع به کار تایمر


    ui->runAutoButton->setEnabled(false);
    ui->pushButton->setEnabled(false); // دکمه قدم به قدم هم غیرفعال شود
    ui->stopButton->setEnabled(true);
}

// توقف اجرای خودکار
void MainWindow::on_stopButton_clicked()
{
    simulationTimer->stop(); // متوقف کردن تایمر

    // مدیریت وضعیت دکمه‌ها
    ui->runAutoButton->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}
void MainWindow::simulationStep()
{

    if (cpu->isHalted()) {
        on_stopButton_clicked();
        qDebug() << "CPU Halted. Simulation stopped.";
        return;
    }
    cpu->clockTick();
    updateUI();
}


void MainWindow::updateUI()
{

    cpu->printState();
}
