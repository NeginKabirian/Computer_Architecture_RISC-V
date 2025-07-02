#ifndef COMPUTERSIMULATOR_H
#define COMPUTERSIMULATOR_H

#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include "DecodedInstruction.h"
#include "cpuStage.h"


class ComputerSimulator : public QWidget {
    Q_OBJECT
public:
    ComputerSimulator();
    QPushButton *pulse;
    void initMemory(QString );
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


    int registerFileHighlightedRow;
    int registerFileHighlightedCol;
    QColor *normalColor;
    QColor *highlightColor; // Light yellow...

    QWidget *specialRegsWidget;
   QWidget *  controlRegsWidget;
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
    QWidget* memoryWidget;
    QTextEdit* memoryTextEdit;
};

#endif // COMPUTERSIMULATOR_H
