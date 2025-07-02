#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "cpu.h"
#include <QTimer>
#include<QDir>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateUI();
private slots:
    void on_pushButton_clicked();
    void on_runAutoButton_clicked();
    void on_stopButton_clicked();
    void simulationStep();

private:
    Ui::MainWindow *ui;
    CPU* cpu;
    Memory* memory;
    RegisterFile* regFile;
    QTimer *simulationTimer;

    ComputerSimulator *simulator;
};
#endif // MAINWINDOW_H

