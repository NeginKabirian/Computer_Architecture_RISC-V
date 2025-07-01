#ifndef COMPUTERSIMULATOR_H
#define COMPUTERSIMULATOR_H

#include <QPushButton>
#include <QTableWidget>

class ComputerSimulator : public QWidget {
    Q_OBJECT
public:
    ComputerSimulator();
    QPushButton *pulse;

public slots:
    void updateRegisterFile(int index, QString content);
    void updateSpecRegs(QString name, QString content);

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
};

#endif // COMPUTERSIMULATOR_H
