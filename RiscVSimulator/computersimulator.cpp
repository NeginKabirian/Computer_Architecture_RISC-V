#include "computersimulator.h"
#include "DecodedInstruction.h"
#include "cpu.h"
#include <QFormLayout>
#include <QGraphicsColorizeEffect>
#include <QHeaderView>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QThread>

#define numOfReg 32
#define tableWidth 400
#define tableHeight 520
#define bgColor "#2b2b2b"
#define hlColor

ComputerSimulator::ComputerSimulator() {

    normalColor = new QColor(QString(bgColor));
    highlightColor = new QColor(QString(hlColor));
    initRegisterFile();
    initSpecRegisters();
    initPulse();
    initControlState();
}

void ComputerSimulator::updateRegisterFile(int index, QString content) {

    if (index < numOfReg) {
        highlightRegister(index);
        registerFile
            ->item(registerFileHighlightedRow, registerFileHighlightedCol + 1)
            ->setText(content);
        registerFile->resizeColumnsToContents();
    } else
        qDebug() << "invalid register number!";
}

void ComputerSimulator::updateSpecRegs(QString name, QString content) {
    if (name == "PC") {
        pcLineEdit->setText(content);
    } else if (name == "IR") {
        irLineEdit->setText(content);
    } else if (name == "DR") {
        drLineEdit->setText(content);
    } else if (name == "AR") {
        arLineEdit->setText(content);
    } else if (name == "A") {
        aLineEdit->setText(content);
    } else if (name == "B") {
        bLineEdit->setText(content);
    } else if (name == "IMM") {
        immLineEdit->setText(content);
    }
}

void ComputerSimulator::updateState(int cycle, CPUStage stage) {
    QString stageText;

    switch (stage) {
    case CPUStage::Fetch1: {
        stageText = "FETCH1";
        break;
    }
    case CPUStage::Fetch2: {
        stageText = "FETCH2";
        break;
    }
    case CPUStage::Decode: {
        stageText = "DECODE";
        break;
    }
    case CPUStage::Exec: {
        stageText = "EXEC";
        break;
    }
    case CPUStage::HALT: {
        stageText = "HALT";
        break;
    }
    case CPUStage::Mem: {
        stageText = "MEM";
        break;
    }
    case CPUStage::WriteBack: {
        stageText = "WriteBack";
        break;
    }

    default:
        break;
    }
    this->stage->setText(stageText);
    cycleStep->setText(QString::number(cycle));
}

void ComputerSimulator::updateCurrInstruction(DecodedInstruction *curr)
{
    QString result = "opcode: ";

    // Convert opcode enum to string
    static const char* inst_names[] = {
        "add", "sub", "xor", "or", "and", "sll", "srl", "sra", "slt", "sltu",
        "addi", "ori", "andi", "slli", "srli", "lb", "lh", "lw", "lbu", "lhu",
        "sb", "sh", "sw", "beq", "bne", "blt", "bge", "bltu", "bgeu",
        "jal", "jalr", "lui", "auipc",
        "mul", "mulh", "mulhsu", "mulhu", "div", "divu", "rem", "remu",
        "invalid"
    };

    if (curr->opcode >= 0 && curr->opcode <= invalid) {
        result += inst_names[static_cast<int>(curr->opcode)];
    } else {
        result += "unknown";
    }

    if (curr->has_rd) {
        result += QString(" rd:0x%1").arg(curr->rd, 2, 16, QLatin1Char('0'));
    }

    if (curr->has_rs1) {
        result += QString(" rs1:0x%1").arg(curr->rs1, 2, 16, QLatin1Char('0'));
    }

    if (curr->has_rs2) {
        result += QString(" rs2:0x%1").arg(curr->rs2, 2, 16, QLatin1Char('0'));
    }

    if (curr->has_imm) {
        result += QString(" imm:%1").arg(curr->immediate);
    }

    currInstruction->setText(result); // assuming currInstruction is a QLineEdit*
}


void ComputerSimulator::initRegisterFile() {
    registerFileHighlightedRow = -1;
    registerFile = new QTableWidget(numOfReg / 2, 4, this); // 16 rows, 4 columns

    // Set table name/title
    registerFile->setObjectName("REGISTER_FILE");

    // Set table headers
    QStringList headers;
    headers << "Register" << "Value" << "Register" << "Value";
    registerFile->setHorizontalHeaderLabels(headers);

    // Style the table - dark gray background with blue border
    registerFile->setStyleSheet(
        "QTableWidget {"
        "    background-color:" +
        QString(bgColor) +
        ";"                              // Dark gray background
                             "    border: 2px solid #0066cc;" // Blue border
                             "    gridline-color: #404040;"   // Darker grid lines
                             "    color: white;"              // White text
                             "}"
                             "QTableWidget::item {"
                             "    background-color:" +
        QString(bgColor) +
        ";"
                             "    border: 1px solid #404040;"
                             "    padding: 5px;"
                             "}"
                             "QTableWidget::item:selected {"
                             "    background-color: #0066cc;" // Blue when selected
                             "}"
                             "QHeaderView::section {"
                             "    background-color: #1a1a1a;" // Darker header
                             "    color: white;"
                             "    border: 1px solid #0066cc;"
                             "    padding: 5px;"
                             "    font-weight: bold;"
                             "}");

    // Populate registers 0-15 in columns 0-1 (rows 0-15)
    for (int i = 0; i < 16; ++i) {
        // First column: Register name (R0 to R15)
        QTableWidgetItem *nameItem = new QTableWidgetItem("R" + QString::number(i));
        nameItem->setFlags(nameItem->flags() &
                           ~Qt::ItemIsEditable); // Make read-only
        registerFile->setItem(i, 0, nameItem);

        // Second column: Register value
        QTableWidgetItem *valueItem = new QTableWidgetItem("0x00000000");
        valueItem->setFlags(nameItem->flags() &
                            ~Qt::ItemIsEditable); // Make read-only
        registerFile->setItem(i, 1, valueItem);
    }

    // Populate registers 16-31 in columns 2-3 (rows 0-15)
    for (int i = 16; i < 32; ++i) {
        int row = i - 16; // Map register 16-31 to rows 0-15

        // Third column: Register name (R16 to R31)
        QTableWidgetItem *nameItem = new QTableWidgetItem("R" + QString::number(i));
        nameItem->setFlags(nameItem->flags() &
                           ~Qt::ItemIsEditable); // Make read-only
        registerFile->setItem(row, 2, nameItem);

        // Fourth column: Register value
        QTableWidgetItem *valueItem = new QTableWidgetItem("0x00000000");
        valueItem->setFlags(nameItem->flags() &
                            ~Qt::ItemIsEditable); // Make read-only
        registerFile->setItem(row, 3, valueItem);
    }

    int parentWidth = this->width();
    // int parentHeight = this->height();

    registerFile->setGeometry(parentWidth - tableWidth -
                                  20, // 20px margin from right edge
                              20,     // 20px margin from top
                              tableWidth, tableHeight);

    //  Resize columns to fit content
    registerFile->resizeColumnsToContents();

    //  Make the table non-resizable by user
    registerFile->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    registerFile->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    registerFile->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    registerFile->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    registerFile->show();
}

void ComputerSimulator::initSpecRegisters() {
    // Create container widget
    specialRegsWidget = new QWidget(this);

    // Create form layout
    QFormLayout *formLayout = new QFormLayout(specialRegsWidget);
    formLayout->setVerticalSpacing(10);
    // Create line edits for each register
    // stageLineEdit = new QLineEdit();
    // cycleStepLineEdit = new QLineEdit();
    pcLineEdit = new QLineEdit();
    irLineEdit = new QLineEdit();
    drLineEdit = new QLineEdit();
    arLineEdit = new QLineEdit();
    aLineEdit = new QLineEdit();
    bLineEdit = new QLineEdit();
    immLineEdit = new QLineEdit();

    // Make them read-only
    // stageLineEdit->setReadOnly(true);
    // cycleStepLineEdit->setReadOnly(true);
    pcLineEdit->setReadOnly(true);
    irLineEdit->setReadOnly(true);
    drLineEdit->setReadOnly(true);
    arLineEdit->setReadOnly(true);
    aLineEdit->setReadOnly(true);
    bLineEdit->setReadOnly(true);
    immLineEdit->setReadOnly(true);

    pcLineEdit->setText("0x00000000");
    irLineEdit->setText("0x00000000");
    drLineEdit->setText("0x00000000");
    arLineEdit->setText("0x0000");
    aLineEdit->setText("0x00000000");
    bLineEdit->setText("0x00000000");
    immLineEdit->setText("0x00000000");

    // Add widgets to form layout with labels
    // formLayout->addRow("CPU Stage:", stageLineEdit);
    // formLayout->addRow("Cycle Step:", cycleStepLineEdit);
    formLayout->addRow("PC (32-bit):", pcLineEdit);
    formLayout->addRow("IR (32-bit):", irLineEdit);
    formLayout->addRow("DR (32-bit):", drLineEdit);
    formLayout->addRow("AR (16-bit):", arLineEdit);
    formLayout->addRow("A (32-bit):", aLineEdit);
    formLayout->addRow("B (32-bit):", bLineEdit);
    formLayout->addRow("Imm (32-bit):", immLineEdit);

    // Style the special registers widget
    specialRegsWidget->setStyleSheet("QWidget {"
                                     "    background-color:" +
                                     QString(bgColor) +
                                     ";"
                                                          "    border: 2px solid #32ff00;"
                                                          "    border-radius: 5px;"
                                                          "    padding: 10px;"
                                                          "}"
                                                          "QLabel {"
                                                          "    color: white;"
                                                          "    font-weight: bold;"
                                                          "    font-size: 12px;"
                                                          "    min-height: 25px;"
                                                          "    max-height: 25px;"
                                                          "    padding: 1px;"
                                                          "}"
                                                          "QLineEdit {"
                                                          "    background-color: #404040;"
                                                          "    color: white;"
                                                          "    border: 1px solid #666666;"
                                                          "    padding: 3px;"
                                                          "    font-family: 'Courier New', monospace;"
                                                          "    font-size: 15px;"
                                                          "}"
                                                          "QLineEdit:focus {"
                                                          "    border: 2px solid #0066cc;"
                                                          "}");

    // Position the widget
    specialRegsWidget->setGeometry(635, 180, 250, 360);
}

void ComputerSimulator::initPulse() {
    pulse = new QPushButton(this);
    pulse->setFlat(true);
    pulse->setStyleSheet("QPushButton {"
                         "    background-color:" +
                         QString(bgColor) +
                         ";"
                                              " border: 5px solid #ff8b00;"
                                              " color: #ff8b00;"
                                              " border-radius: 5px;"
                                              "}");
    pulse->setGeometry(900, 450, 70, 50);
    pulse->setText("PULSE");
    pulse->setFont(QFont("Berlin Sans FB Demi", 15, QFont::Bold, false));
}

void ComputerSimulator::initControlState() {
    // Create container widget
    controlRegsWidget = new QWidget(this);

    // Create form layout
    QFormLayout *formLayout = new QFormLayout(controlRegsWidget);
    formLayout->setVerticalSpacing(10);

    // Create line edits
    stage = new QLineEdit();
    cycleStep = new QLineEdit();
    currInstruction = new QLineEdit();
    currInstruction->setMinimumWidth(400);
    currInstruction->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Set read-only and default text
    stage->setReadOnly(true);
    cycleStep->setReadOnly(true);
    currInstruction->setReadOnly(true);

    stage->setText("Fetch");
    cycleStep->setText("0");
    currInstruction->setText("NOP");

    // Add widgets to layout with labels
    formLayout->addRow("Stage:", stage);
    formLayout->addRow("Cycle Step:", cycleStep);
    formLayout->addRow("Current Instruction:", currInstruction);

    // Style the control registers widget
    controlRegsWidget->setStyleSheet("QWidget {"
                                     "    background-color:" +
                                     QString(bgColor) +
                                     ";"
                                                          "    border: 2px solid #ffcc00;"
                                                          "    border-radius: 5px;"
                                                          "    padding: 10px;"
                                                          "}"
                                                          "QLabel {"
                                                          "    color: white;"
                                                          "    font-weight: bold;"
                                                          "    font-size: 12px;"
                                                          "    min-height: 25px;"
                                                          "    max-height: 25px;"
                                                          "    padding: 1px;"
                                                          "}"
                                                          "QLineEdit {"
                                                          "    background-color: #404040;"
                                                          "    color: white;"
                                                          "    border: 1px solid #666666;"
                                                          "    padding: 3px;"
                                                          "    font-family: 'Courier New', monospace;"
                                                          "    font-size: 15px;"
                                                          " font-weight: bold;"
                                                          "}"
                                                          "QLineEdit:focus {"
                                                          "    border: 2px solid #ffcc00;"
                                                          "}");

    // Position the widget
    controlRegsWidget->setGeometry(635, 20, 550, 150);

}

void ComputerSimulator::highlightRegister(int index) {
    // Clear previous highlight
    clearHighlight();

    // Set new highlight

    if (index < 16)
        registerFileHighlightedCol = 0;
    else {
        registerFileHighlightedCol = 2;
    index /= 2;
    }
    registerFileHighlightedRow = index;
    for (int col = registerFileHighlightedCol;
         col < registerFileHighlightedCol + 2; ++col) {
        QTableWidgetItem *item = registerFile->item(index, col);
        if (item) {
            item->setData(Qt::BackgroundRole, QColor(Qt::yellow));

            item->setFont(
                QFont(item->font().family(), item->font().pointSize(), QFont::Bold));
            item->setForeground(QBrush(QColor(Qt::green))); // font color
        }
    }
}

void ComputerSimulator::clearHighlight() {
    if (registerFileHighlightedRow >= 0) {
        for (int col = registerFileHighlightedCol;
             col < registerFileHighlightedCol + 2; ++col) {
            QTableWidgetItem *item =
                registerFile->item(registerFileHighlightedRow, col);
            if (item) {
                item->setForeground(QBrush(QColor(Qt::white)));
                QFont font = item->font();
                font.setBold(false);
                item->setFont(font);
            }
        }
        registerFileHighlightedRow = -1;
    }
}

void ComputerSimulator::initMemory(QString memoryText) {
    // Create a container widget for the memory display
    memoryWidget = new QWidget(this);

    // Set geometry and style
    memoryWidget->setGeometry(1200, 0, 400, 300);  // Adjust position/size
    memoryWidget->setStyleSheet("QWidget {"
                                "    background-color: #222222;"
                                "    border: 2px solid #aa00ff;"
                                "    border-radius: 5px;"
                                "    padding: 10px;"
                                "}");

    // Create a QTextEdit to display memory
    memoryTextEdit = new QTextEdit(memoryWidget);
    memoryTextEdit->setReadOnly(true);
    memoryTextEdit->setFont(QFont("Courier New", 11));
    memoryTextEdit->setStyleSheet("QTextEdit {"
                                  "    background-color: #333333;"
                                  "    color: white;"
                                  "    border: none;"
                                  "    padding: 5px;"
                                  "}");

    // Add lines from memoryText
    QStringList lines = memoryText.split('\n');
    QTextCursor cursor = memoryTextEdit->textCursor();
    for (const QString& line : lines) {
        cursor.insertText(line + "\n");
    }

    // Resize text edit inside the container
    memoryTextEdit->setGeometry(0, 0, 400, 300);  // Fit the parent exactly
}


/*void ComputerSimulator::updateCurrentInstruction(const DecodedInstruction&
instruction)
{
    QString coloredInstruction;
    QString opcodeStr = getOpcodeString(instruction.opcode); // You'll need to
implement this helper

    // Start with opcode (always present)
    coloredInstruction = QString("<span style='color:
#ff6b35;'>%1</span>").arg(opcodeStr);

    // Add destination register if present (rd != 0 or if instruction uses rd)
    if (instruction.rd != -1 ) {
        coloredInstruction += QString(" <span style='color:
#00ff88;'>x%1</span>").arg(instruction.rd);

        // Add comma if there are more operands
        if (instruction.rs1 != 0 || instruction.immediate != 0 ||
instruction.rs2 != 0 || instructionUsesRs1(instruction.opcode) ||
instructionUsesImmediate(instruction.opcode) ||
            instructionUsesRs2(instruction.opcode)) {
            coloredInstruction += ",";
        }
    }

    // Add first source register if present
    if (instruction.rs1 != 0 || ) {
        coloredInstruction += QString(" <span style='color:
#4dabf7;'>x%1</span>").arg(instruction.rs1);

        // Add comma if there are more operands
        if (instruction.rs2 != 0 || instruction.immediate != 0 ||
            instructionUsesRs2(instruction.opcode) ||
instructionUsesImmediate(instruction.opcode)) { coloredInstruction += ",";
        }
    }

    // Add second source register if present (for R-type instructions)
    if (instruction.rs2 != 0 || instructionUsesRs2(instruction.opcode)) {
        coloredInstruction += QString(" <span style='color:
#ff073a;'>x%1</span>").arg(instruction.rs2);
    }
    // Add immediate value if present (for I-type instructions)
    else if (instruction.immediate != 0 ||
instructionUsesImmediate(instruction.opcode)) { coloredInstruction += QString("
<span style='color: #ffd700;'>#%1</span>").arg(instruction.immediate);
    }

    currInstruction->setText(coloredInstruction);
}
*/
/*
QString ComputerSimulator::getOpcodeString(inst opcode)
{
    switch(opcode) {
    case add: return "ADD";
    case sub: return "SUB";
    case addi: return "ADDI";
    case lw: return "LW";
    case sw: return "SW";
    // Add more opcodes as needed
    default: return "INVALID";
    }
}



*/
