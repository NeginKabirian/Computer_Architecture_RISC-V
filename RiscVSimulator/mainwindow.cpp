#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    memory = new Memory();
    regFile = new RegisterFile();
    cpu = new CPU(memory, regFile);
}

MainWindow::~MainWindow()
{
    delete cpu;
    delete memory;
    delete regFile;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    cpu->clockTick();
}

