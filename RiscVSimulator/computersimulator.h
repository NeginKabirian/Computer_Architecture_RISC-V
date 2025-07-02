#ifndef COMPUTERSIMULATOR_H
#define COMPUTERSIMULATOR_H

#include "DecodedInstruction.h"
#include "memory.h"
#include "cpuStage.h"
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>

class ComputerSimulator : public QWidget {
    Q_OBJECT
public:
    ComputerSimulator();
    QPushButton *runAutoButton;
    QPushButton *pushButton;
    QPushButton *stopButton;
    QSpinBox *speedSpinBox;
    void initMemory(Memory* memory);
    void updateMemoryDump();

public slots:
    void updateRegisterFile(int index, QString content);
    void updateSpecRegs(QString name, QString content);
    void updateState(int cycle, CPUStage stage);
    void updateCurrInstruction(DecodedInstruction *curr);

protected:
           // void resizeEvent(QResizeEvent* event) override;

private:
    QTableWidget *registerFile;
    void initRegisterFile();
    void initSpecRegisters();
    void initPulse();
    void initControlState();
    void highlightRegister(int index);
    void clearHighlight();
    Memory* memoryRef = nullptr;


    int registerFileHighlightedRow;
    int registerFileHighlightedCol;
    QColor *normalColor;
    QColor *highlightColor; // Light yellow...

    QWidget *specialRegsWidget;
    QWidget *controlRegsWidget;
    QWidget *pulseWidget;
    QLineEdit *pcLineEdit;
    QLineEdit *irLineEdit;
    QLineEdit *drLineEdit;
    QLineEdit *arLineEdit;
    QLineEdit *aLineEdit;
    QLineEdit *bLineEdit;
    QLineEdit *immLineEdit;
    QLineEdit *stage;
    QLineEdit *cycleStep;
    QLineEdit *currInstruction;
    QWidget *memoryWidget;
    QTextEdit *memoryTextEdit;
};

#endif // COMPUTERSIMULATOR_H
