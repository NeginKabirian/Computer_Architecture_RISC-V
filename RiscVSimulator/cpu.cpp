#include "cpu.h"
#include <QDebug>
#include <QString>


CPU::CPU(Memory* mem, RegisterFile* rf) :  memory(mem),regFile(rf) {}

void CPU::reset() {
    PC = 0;
    AR = 0;
    IR = 0;
    stage = CPUStage::Fetch1;

    qDebug() << "CPU has been reset.";
}
void CPU::decode(uint32_t instruction) {
    uint8_t opcode = instruction & 0x7F;

    currentInstruction.opcode = inst::invalid;
    currentInstruction.rd = (instruction >> 7) & 0x1F;
    uint8_t funct3 = (instruction >> 12) & 0x07;
    currentInstruction.rs1 = (instruction >> 15) & 0x1F;
    currentInstruction.rs2 = (instruction >> 20) & 0x1F;
    uint8_t funct7 = (instruction >> 25) & 0x7F;
    currentInstruction.immediate = 0;

    switch (opcode) {
    // R-type (opcode = 0b0110011)
    case 0b0110011:
        switch (funct3) {
        case 0x0:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::add;
            else if (funct7 == 0x20)
                currentInstruction.opcode = inst::sub;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::mul;
            break;
        case 0x1:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::sll;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::mulh;
            break;
        case 0x2:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::slt;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::mulhsu;
            break;
        case 0x3:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::sltu;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::mulhu;
            break;
        case 0x4:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::_xor;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::_div;
            break;
        case 0x5:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::srl;
            else if (funct7 == 0x20)
                currentInstruction.opcode = inst::sra;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::divu;
            break;
        case 0x6:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::_or;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::rem;
            break;
        case 0x7:
            if (funct7 == 0x00)
                currentInstruction.opcode = inst::_and;
            else if (funct7 == 0x01)
                currentInstruction.opcode = inst::remu;
            break;
        }
        break;

        // I-type ALU immediate (opcode = 0b0010011)
    case 0b0010011: {
        uint32_t imm_12 = (instruction >> 20) & 0xFFF;
        int32_t imm = (imm_12 & 0x800) ? (imm_12 | 0xFFFFF000) : imm_12; // sign-extend 12 بیت
        uint8_t shamt = imm_12 & 0x1F;  // 5 بیت شیفت
        switch (funct3) {
        case 0x0:
            currentInstruction.opcode = inst::addi;
            currentInstruction.immediate = imm;
            currentInstruction.rs1 = (instruction >> 15) & 0x1F;
            currentInstruction.rd  = (instruction >> 7)  & 0x1F;
            break;
        case 0x1:
            if (funct7 == 0x00) {
                currentInstruction.opcode = inst::slli;
                currentInstruction.immediate = shamt;
            }
            break;
        case 0x5:
            if (funct7 == 0x00) {
                currentInstruction.opcode = inst::srli;
                currentInstruction.immediate = shamt;
            }
            break;
        case 0x6:
            currentInstruction.opcode = inst::ori;
            currentInstruction.immediate = imm;
            break;
        case 0x7:
            currentInstruction.opcode = inst::andi;
            currentInstruction.immediate = imm;
            break;
        }
        break;
    }

        // I-type Load instructions (opcode = 0b0000011)
    case 0b0000011: {
        int32_t imm = static_cast<int32_t>(instruction) >> 20;  // sign-extended immediate
        currentInstruction.immediate = imm;
        switch (funct3) {
        case 0x0: currentInstruction.opcode = inst::lb; break;
        case 0x1: currentInstruction.opcode = inst::lh; break;
        case 0x2: currentInstruction.opcode = inst::lw; break;
        case 0x4: currentInstruction.opcode = inst::lbu; break;
        case 0x5: currentInstruction.opcode = inst::lhu; break;
        default: currentInstruction.opcode = inst::invalid; break;
        }
        break;
    }

        // I-type jalr (opcode = 0b1100111)
    case 0b1100111: {
        int32_t imm = static_cast<int32_t>(instruction) >> 20;  // sign-extended immediate
        if (funct3 == 0x0)
            currentInstruction.opcode = inst::jalr;
        currentInstruction.immediate = imm;
        break;
    }

        // B-type Branch instructions (opcode = 0b1100011)
    case 0b1100011: {
        int32_t imm = 0;
        imm |= ((instruction >> 31) & 0x1) << 12;
        imm |= ((instruction >> 7) & 0x1) << 11;
        imm |= ((instruction >> 25) & 0x3F) << 5;
        imm |= ((instruction >> 8) & 0xF) << 1;
        // sign extend bit 12
        if (imm & (1 << 12))
            imm |= 0xFFFFE000;
        currentInstruction.immediate = imm;

        switch (funct3) {
        case 0b000: currentInstruction.opcode = inst::beq; break;
        case 0b001: currentInstruction.opcode = inst::bne; break;
        case 0b100: currentInstruction.opcode = inst::blt; break;
        case 0b101: currentInstruction.opcode = inst::bge; break;
        case 0b110: currentInstruction.opcode = inst::bltu; break;
        case 0b111: currentInstruction.opcode = inst::bgeu; break;
        default: currentInstruction.opcode = inst::invalid; break;
        }
        break;
    }

        // S-type Store instructions (opcode = 0b0100011)
    case 0b0100011: {
        int32_t imm = 0;
        imm |= ((instruction >> 25) & 0x7F) << 5;
        imm |= ((instruction >> 7) & 0x1F);
        // sign extend bit 11
        if (imm & (1 << 11))
            imm |= 0xFFFFF000;
        currentInstruction.immediate = imm;

        switch (funct3) {
        case 0b000: currentInstruction.opcode = inst::sb; break;
        case 0b001: currentInstruction.opcode = inst::sh; break;
        case 0b010: currentInstruction.opcode = inst::sw; break;
        default: currentInstruction.opcode = inst::invalid; break;
        }
        break;
    }

        // U-type LUI (opcode = 0b0110111)
    case 0b0110111:
        currentInstruction.immediate = instruction & 0xFFFFF000;
        currentInstruction.opcode = inst::lui;
        break;

        // U-type AUIPC (opcode = 0b0010111)
    case 0b0010111:
        currentInstruction.opcode = inst::auipc;
        currentInstruction.rd = (instruction >> 7) & 0x1F;
        currentInstruction.immediate = instruction >> 12;
        break;

        // J-type JAL (opcode = 0b1101111)
    case 0b1101111: {
        int32_t imm = 0;
        imm |= ((instruction >> 31) & 0x1) << 20;
        imm |= ((instruction >> 12) & 0xFF) << 12;
        imm |= ((instruction >> 20) & 0x1) << 11;
        imm |= ((instruction >> 21) & 0x3FF) << 1;
        // sign extend bit 20
        if (imm & (1 << 20))
            imm |= 0xFFF00000;
        currentInstruction.immediate = imm;
        currentInstruction.opcode = inst::jal;
        break;
    }

    default:
        currentInstruction.opcode = inst::invalid;
        break;
    }
}





void CPU::clockTick() {
    switch(stage) {
    case CPUStage::Fetch1:
        regFile->printRegisters();
        AR = PC;
        ui->updateSpecRegs("AR", QString("0x%1").arg(AR, 4, 16, QChar('0')).toUpper());
        stage = CPUStage::Fetch2;
        break;

    case CPUStage::Fetch2:
        IR = memory->read32(AR);
        PC += 4;
        ui->updateSpecRegs("PC", QString("0x%1").arg(PC, 8, 16, QChar('0')).toUpper());
        ui->updateSpecRegs("IR", QString("0x%1").arg(IR, 8, 16, QChar('0')).toUpper());


        stage = CPUStage::Decode;
        break;

    case CPUStage::Decode:
        decode(IR);
        cycleStep = 3;
        stage = CPUStage::Exec;
        break;

    case CPUStage::Exec:
        executeMicroStep();
        break;

    case CPUStage::Mem:
        //memoryAccess();
        break;

    case CPUStage::WriteBack:
        //writeBack();
        break;

    case CPUStage::HALT:
        qDebug()<<"HALT encountered. CPU stopped.";
        break;

    default:
        break;
    }
}







void CPU::printState() const {
    // --- چاپ وضعیت اصلی ---
    qDebug() <<"cycleStep : " << cycleStep;
    qDebug().noquote() << QString("Stage: %1 | PC: 0x%2 (%3) | AR: 0x%4 (%5) | IR: 0x%6")
                              .arg(static_cast<int>(stage))
                              .arg(PC, 8, 16, QChar('0')).arg(PC) // هگز و سپس دسیمال
                              .arg(AR, 8, 16, QChar('0')).arg(AR)
                              .arg(IR, 8, 16, QChar('0')); // برای IR معمولاً دسیمال معنی‌دار نیست

    // --- چاپ رجیسترهای داخلی ---
    qDebug().noquote() << QString("      -> Internal Regs | A: 0x%1 (%2) | B: 0x%3 (%4) | DR: 0x%5 (%6)")
                              .arg(A, 8, 16, QChar('0')).arg(A) // هگز و سپس دسیمال (بدون علامت)
                              .arg(B, 8, 16, QChar('0')).arg(B)
                              .arg(DR, 8, 16, QChar('0')).arg(DR);

    // چاپ Imm به صورت جداگانه برای نمایش مقدار با علامت
    qDebug().noquote() << QString("                      | Imm: 0x%1 (%2)")
                              .arg(static_cast<uint32_t>(Imm), 8, 16, QChar('0')) // هگز (کست به بدون علامت برای نمایش صحیح)
                              .arg(static_cast<int32_t>(Imm));                     // دسیمال (با علامت)


    // --- چاپ اطلاعات دیکود شده (بدون تغییر) ---
    if (stage == CPUStage::Exec) {
        if (IR != lastPrintedIR) {
            currentInstruction.print(); // تابع print خود immediate را با علامت چاپ می‌کند
            lastPrintedIR = IR;
        }
    } else {
        lastPrintedIR = 0xFFFFFFFF;
    }
}

void CPU::executeMicroStep() {
    switch(currentInstruction.opcode) {
        // ---------------------- ADD ----------------------
    case inst::add:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.add(A, B);

            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            std::cout << "After AND: x10 = 0x" << std::hex << regFile->read(10) << std::endl;
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SUB ----------------------
    case inst::sub:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
             

            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);\
                 

            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.sub(A, B);

            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- XOR ----------------------
    case inst::_xor:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
             

            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
              
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.xor_op(A, B);
             ui->updateSpecRegs("DR", QString("0x%1").arg(DR, 8, 16, QChar('0')).toUpper());
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- OR ----------------------
    case inst::_or:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.or_op(A, B);
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- AND ----------------------
    case inst::_and:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.and_op(A, B);
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SLL ----------------------
    case inst::sll:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.sll(A, B & 0x1F);
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SRL ----------------------
    case inst::srl:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.srl(A, B & 0x1F);
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SRA ----------------------
    case inst::sra:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.sra(A, B & 0x1F); // تابع sra باید توی ALU تعریف شده باشه
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SLT ----------------------
    case inst::slt:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.sub(A, B);
            cycleStep++;
        } else if (cycleStep == 6) {
            cycleStep++;
        } else if (cycleStep == 7) {
            DR = (static_cast<int32_t>(DR) < 0) ? 1 : 0;
            cycleStep++;
        } else if (cycleStep == 8) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SLTU ----------------------
    case inst::sltu:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = (A < B) ? 1 : 0;
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- ADDI ----------------------
    case inst::addi:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.add(A,Imm);
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- LH ----------------------
    case inst::lh:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.add(A,Imm);
            cycleStep++;
        } else if (cycleStep == 6) {
            AR = DR;
            cycleStep++;
        } else if (cycleStep == 7) {
            int16_t val = static_cast<int16_t>(memory->read16(AR));
            DR = Alu.passThrough(static_cast<uint32_t>(val));
            cycleStep++;
        } else if (cycleStep == 8) {
            int16_t val = static_cast<int16_t>(DR & 0xFFFF); // sign-extend
            DR = static_cast<int32_t>(val);
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- LW ----------------------
    case inst::lw:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.add(A,Imm);
            cycleStep++;
        } else if (cycleStep == 6) {
            AR = DR;
            cycleStep++;
        } else if (cycleStep == 7) {
            A = memory->read32(AR);
              
            DR = Alu.passThrough(A);
            cycleStep++;
        } else if (cycleStep == 8) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SH ----------------------
    case inst::sh:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 6) {
            DR = Alu.add(A,Imm);
            cycleStep++;
        } else if (cycleStep == 7) {
            AR = DR;

            cycleStep++;
        } else if (cycleStep == 8) {
            DR = B & 0xFFFF;
            cycleStep++;
        } else if (cycleStep == 9) {
            memory->write16(AR, static_cast<uint16_t>(DR));
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- SW ----------------------
    case inst::sw:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 6) {
            DR = Alu.add(A,Imm);
            cycleStep++;
        } else if (cycleStep == 7) {
            AR = DR;
            cycleStep++;
        } else if (cycleStep == 8) {
            DR = Alu.passThrough(B);
            cycleStep++;
        } else if (cycleStep == 9) {
            memory->write32(AR, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- BEQ ----------------------
    case inst::beq:
        if (cycleStep == 3) { // T3
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 4) { // T4
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 5) { // T5
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 6) { // T6
            DR = A + Imm;
            cycleStep++;
        } else if (cycleStep == 7) { // T7
            A = regFile->read(currentInstruction.rs1);
              

            cycleStep++;
        } else if (cycleStep == 8) { // T8
            if (A == B)
                PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- BNE ----------------------
    case inst::bne:
        if (cycleStep == 3) { // T3
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 4) { // T4
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 5) { // T5
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 6) { // T6
            DR = A + Imm;
            cycleStep++;
        } else if (cycleStep == 7) { // T7
            A = regFile->read(currentInstruction.rs1);
              

            cycleStep++;
        } else if (cycleStep == 8) { // T8
            if (A != B)
                PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- BLT ----------------------
    case inst::blt:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        }else if(cycleStep == 4){
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = A - B;
            cycleStep++;

        } else if (cycleStep == 6) {
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 7) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 8) {
            DR = A + Imm;
            cycleStep++;
        } else if (cycleStep == 9) {
            if (static_cast<int32_t>(A) < static_cast<int32_t>(B))
                PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- BGE ----------------------
    case inst::bge:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
  
            cycleStep++;
        }else if(cycleStep == 4){
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = A - B;
            cycleStep++;
        } else if (cycleStep == 6) {
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 7) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 8) {
            DR = A + Imm;
            cycleStep++;
        } else if (cycleStep == 9) {
            if (static_cast<int32_t>(A) >= static_cast<int32_t>(B))
                PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;




        // ---------------------- BLTU ----------------------
    case inst::bltu:
        if(cycleStep == 3){
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 4) { // T4
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 5) { // T5
            Imm = currentInstruction.immediate;  // فرض می‌کنیم قبلاً sign-extend شده
            cycleStep++;
        } else if (cycleStep == 6) { // T6
            DR = A + Imm;
            cycleStep++;
        } else if (cycleStep == 7) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 8) { // T8
            if (A < B)
                PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;

        // ---------------------- BGEU ----------------------
    case inst::bgeu:
        if(cycleStep == 3){
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 4) { // T4
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 5) { // T5
            Imm = currentInstruction.immediate;  // فرض می‌کنیم قبلاً sign-extend شده
            cycleStep++;
        } else if (cycleStep == 6) { // T6
            DR = Alu.add(A, Imm);
            cycleStep++;
        } else if (cycleStep == 7) {
            A = regFile->read(currentInstruction.rs1);
              
            cycleStep++;
        } else if (cycleStep == 8) { // T8
            if (A >= B)
                PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- LUI ----------------------
    case inst::lui:
        if (cycleStep == 3) { // UT3
            Imm = currentInstruction.immediate;
            DR = Alu.passThrough(Imm);
            cycleStep++;
        } else if (cycleStep == 4) { // UT4
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- AUIPC ----------------------
    case inst::auipc:
        if (cycleStep == 3) { // UT3
            A = PC - 4;
              
            cycleStep++;
        } else if (cycleStep == 4) { // UT4
            Imm = currentInstruction.immediate;
            DR = Alu.sll(Imm, 12);
            cycleStep++;
        } else if (cycleStep == 5) { // UT5
            B = DR;
            cycleStep++;
        } else if (cycleStep == 6) { // UT6
            DR = Alu.add(A, B);
            cycleStep++;
        } else if (cycleStep == 7) { // UT7
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;


        // ---------------------- JAL ----------------------
    case inst::jal:
        if (cycleStep == 3) {
            A = PC;
              
            cycleStep++;
        } else if (cycleStep == 4) {
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.add(A, Imm);
            cycleStep++;
        } else if (cycleStep == 6) {
            PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;

        // ---------------------- JALR ----------------------
    case inst::jalr:
        if (cycleStep == 3) { // T3
            A = regFile->read(currentInstruction.rs1);
            cycleStep++;
        } else if (cycleStep == 4) { // T4
            Imm = currentInstruction.immediate;
            cycleStep++;
        } else if (cycleStep == 5) { // T5
            DR = Alu.add(A, Imm);
            cycleStep++;
        } else if (cycleStep == 6) { // T6
            PC = DR;
            stage = CPUStage::Fetch1;
        }
        break;

        // ---------------------- MUL ----------------------
    case inst::mul:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            DR = Alu.multiply(A, B);
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;

        // ---------------------- MULH ----------------------
    case inst::mulh:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            int64_t tmp = static_cast<int64_t>(static_cast<int32_t>(A)) *
                          static_cast<int64_t>(static_cast<int32_t>(B));
            DR = static_cast<uint32_t>(tmp >> 32);  // ۳۲ بیت بالا
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;

        // ---------------------- DIV ----------------------
    case inst::_div:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            if (B == 0)
                DR = static_cast<uint32_t>(-1);  // RISC-V: نتیجه division by zero
            else
                DR = static_cast<uint32_t>(static_cast<int32_t>(A) / static_cast<int32_t>(B));
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;

        // ---------------------- REM ----------------------
    case inst::rem:
        if (cycleStep == 3) {
            A = regFile->read(currentInstruction.rs1);
            cycleStep++;
        } else if (cycleStep == 4) {
            B = regFile->read(currentInstruction.rs2);
            cycleStep++;
        } else if (cycleStep == 5) {
            if (B == 0)
                DR = Alu.passThrough(A);  // RISC-V: باقی‌مانده division by zero همان مقسوم
            else
                DR = static_cast<uint32_t>(static_cast<int32_t>(A) % static_cast<int32_t>(B));
            cycleStep++;
        } else if (cycleStep == 6) {
            regFile->write(currentInstruction.rd, DR);
            stage = CPUStage::Fetch1;
        }
        break;

    default:
        stage = CPUStage::HALT;
        qDebug() << "HALT!";
        //چجوری متوقف کنم برنامه رو؟
        break;
    }

    ui->updateSpecRegs("A", QString("0x%1").arg(A, 8, 16, QChar('0')).toUpper());
    ui->updateSpecRegs("B", QString("0x%1").arg(B, 8, 16, QChar('0')).toUpper());
    ui->updateSpecRegs("IR", QString("0x%1").arg(IR, 8, 16, QChar('0')).toUpper());
    ui->updateSpecRegs("DR", QString("0x%1").arg(DR, 8, 16, QChar('0')).toUpper());
    ui->updateSpecRegs("AR", QString("0x%1").arg(AR, 4, 16, QChar('0')).toUpper());
    ui->updateSpecRegs("PC", QString("0x%1").arg(PC, 8, 16, QChar('0')).toUpper());
    ui->updateSpecRegs("IMM", QString("0x%1").arg(Imm, 8, 16, QChar('0')).toUpper());


}

void CPU::setSimulator(ComputerSimulator *simulator)
{
    ui=simulator;
    ui->show();
}
