instruction_set = {
    # R-type
    'add':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x0, 'funct7': 0x00},
    'sub':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x0, 'funct7': 0x20},
    'xor':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x4, 'funct7': 0x00},
    'or':   {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x6, 'funct7': 0x00},
    'and':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x7, 'funct7': 0x00},
    'sll':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x1, 'funct7': 0x00},
    'srl':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x5, 'funct7': 0x00},
    'sra':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x5, 'funct7': 0x20},
    'slt':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x2, 'funct7': 0x00},
    'sltu': {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x3, 'funct7': 0x00},
    'mul':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x0, 'funct7': 0x01},
    'mulh': {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x1, 'funct7': 0x01},
    'div':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x4, 'funct7': 0x01},
    'rem':  {'fmt': 'R', 'opcode': 0x33, 'funct3': 0x6, 'funct7': 0x01},

    # I-type
    'addi': {'fmt': 'I', 'opcode': 0x13, 'funct3': 0x0},
    'xori': {'fmt': 'I', 'opcode': 0x13, 'funct3': 0x4},
    'lh':   {'fmt': 'I', 'opcode': 0x03, 'funct3': 0x1},
    'lw':   {'fmt': 'I', 'opcode': 0x03, 'funct3': 0x2},
    'jalr': {'fmt': 'I', 'opcode': 0x67, 'funct3': 0x0},

    # S-type
    'sh':   {'fmt': 'S', 'opcode': 0x23, 'funct3': 0x1},
    'sw':   {'fmt': 'S', 'opcode': 0x23, 'funct3': 0x2},

    # B-type
    'beq':  {'fmt': 'B', 'opcode': 0x63, 'funct3': 0x0},
    'bne':  {'fmt': 'B', 'opcode': 0x63, 'funct3': 0x1},
    'blt':  {'fmt': 'B', 'opcode': 0x63, 'funct3': 0x4},
    'bge':  {'fmt': 'B', 'opcode': 0x63, 'funct3': 0x5},
    'bltu': {'fmt': 'B', 'opcode': 0x63, 'funct3': 0x6},
    'bgeu': {'fmt': 'B', 'opcode': 0x63, 'funct3': 0x7},

    # U-type
    'lui':   {'fmt': 'U', 'opcode': 0x37},
    'auipc': {'fmt': 'U', 'opcode': 0x17},

    # J-type
    'jal':  {'fmt': 'J', 'opcode': 0x6F},
}

reg_alias = lambda r: int(r.replace('x', '')) if r.startswith('x') and r[1:].isdigit() else None

def parse_immediate(imm):
    if imm.startswith("'") and imm.endswith("'"):
        return ord(imm[1])
    if imm.startswith('"') and imm.endswith('"'):
        return [ord(c) for c in imm[1:-1]]
    imm = imm.lower()
    if imm.startswith('0x'):
        return int(imm, 16)
    elif imm.startswith('x'):
        return int('0x' + imm[1:], 16)
    elif imm.startswith('0b'):
        return int(imm, 2)
    elif imm.startswith('b'):
        return int('0b' + imm[1:], 2)
    else:
        return int(imm)


def expand_pseudo(tokens):
    if tokens[0] == 'nop':
        return [['addi', 'x0', 'x0', '0']]
    elif tokens[0] == 'li':
        rd, imm = tokens[1], parse_immediate(tokens[2])
        if isinstance(imm, list): imm = sum([b << (8*i) for i,b in enumerate(imm)])
        if -2048 <= imm < 2048:
            return [['addi', rd, 'x0', str(imm)]]
        else:
            upper = imm >> 12
            lower = imm & 0xFFF
            return [['lui', rd, str(upper)], ['addi', rd, rd, str(lower)]]
    elif tokens[0] == 'mv':
        return [['addi', tokens[1], tokens[2], '0']]
    elif tokens[0] == 'not':
        return [['xori', tokens[1], tokens[2], '-1']]
    elif tokens[0] == 'neg':
        return [['sub', tokens[1], 'x0', tokens[2]]]
    return [tokens]

def encode_r(funct7, rs2, rs1, funct3, rd, opcode):
    return (
        (funct7 & 0x7F) << 25 |
        (rs2 & 0x1F) << 20 |
        (rs1 & 0x1F) << 15 |
        (funct3 & 0x07) << 12 |
        (rd & 0x1F) << 7 |
        (opcode & 0x7F)
    )

def encode_i(imm, rs1, funct3, rd, opcode):
    return (
        (imm & 0xFFF) << 20 |
        (rs1 & 0x1F) << 15 |
        (funct3 & 0x07) << 12 |
        (rd & 0x1F) << 7 |
        (opcode & 0x7F)
    )

def encode_s(imm, rs2, rs1, funct3, opcode):
    imm11_5 = (imm >> 5) & 0x7F
    imm4_0 = imm & 0x1F
    return (
        (imm11_5) << 25 |
        (rs2 & 0x1F) << 20 |
        (rs1 & 0x1F) << 15 |
        (funct3 & 0x07) << 12 |
        (imm4_0) << 7 |
        (opcode & 0x7F)
    )

def encode_b(imm, rs2, rs1, funct3, opcode):
    imm = imm >> 1  # RISC-V branch offsets are multiples of 2
    imm12 = (imm >> 11) & 0x1
    imm10_5 = (imm >> 5) & 0x3F
    imm4_1 = (imm >> 1) & 0xF
    imm11 = (imm >> 10) & 0x1
    return (
        (imm12) << 31 |
        (imm11) << 7 |
        (imm10_5) << 25 |
        (imm4_1) << 8 |
        (rs2 & 0x1F) << 20 |
        (rs1 & 0x1F) << 15 |
        (funct3 & 0x07) << 12 |
        (opcode & 0x7F)
    )

def encode_u(imm, rd, opcode):
    return (
        (imm & 0xFFFFF000) |
        (rd & 0x1F) << 7 |
        (opcode & 0x7F)
    )

def encode_j(imm, rd, opcode):
    imm = imm >> 1  # offset is multiple of 2
    imm20 = (imm >> 19) & 0x1
    imm10_1 = (imm >> 1) & 0x3FF
    imm11 = (imm >> 10) & 0x1
    imm19_12 = (imm >> 11) & 0xFF
    return (
        (imm20) << 31 |
        (imm19_12) << 12 |
        (imm11) << 20 |
        (imm10_1) << 21 |
        (rd & 0x1F) << 7 |
        (opcode & 0x7F)
    )


def assemble_file(input_filename, output_filename):
    with open(input_filename, 'r') as f:
        lines = [line.strip().split('#')[0].strip() for line in f if line.strip()]

    symbol_table = {}
    instructions = []
    data_segment = {}
    pc = 0

    for line in lines:
        if line.endswith(':'):
            label = line[:-1].strip()
            symbol_table[label] = pc
        elif line.startswith('.org'):
            pc = parse_immediate(line.split()[1])
        elif line.startswith('.align'):
            align = int(line.split()[1])
            while pc % (2**align): pc += 1
        elif line.startswith('.word'):
            val = parse_immediate(line.split()[1])
            data_segment[pc] = val & 0xFFFFFFFF
            pc += 4
        elif line.startswith('.half'):
            val = parse_immediate(line.split()[1])
            data_segment[pc] = val & 0xFFFF
            pc += 2
        elif line.startswith('.byte'):
            val = parse_immediate(line.split()[1])
            if isinstance(val, list):
                for b in val:
                    data_segment[pc] = b & 0xFF
                    pc += 1
            else:
                data_segment[pc] = val & 0xFF
                pc += 1
        else:
            tokens = line.replace(',', '').split()
            for instr_tokens in expand_pseudo(tokens):
                instructions.append((pc, instr_tokens))
                pc += 4

    output = {}
    for pc, tokens in instructions:
        try:
            mnemonic = tokens[0]
            info = instruction_set.get(mnemonic)
            if not info:
                raise ValueError(f"Unknown instruction: {mnemonic}")

            fmt = info['fmt']
            if fmt == 'R':
                rd, rs1, rs2 = map(reg_alias, tokens[1:4])
                instr = encode_r(info['funct7'], rs2, rs1, info['funct3'], rd, info['opcode'])
            elif fmt == 'I':
                rd = reg_alias(tokens[1])
                if mnemonic in ['lw', 'lh', 'jalr']:
                    imm, rs1 = tokens[2].split('(')
                    rs1 = reg_alias(rs1[:-1])
                else:
                    rs1 = reg_alias(tokens[2])
                    imm = tokens[3]
                instr = encode_i(parse_immediate(imm), rs1, info['funct3'], rd, info['opcode'])
            elif fmt == 'S':
                rs2 = reg_alias(tokens[1])
                imm, rs1 = tokens[2].split('(')
                rs1 = reg_alias(rs1[:-1])
                instr = encode_s(parse_immediate(imm), rs2, rs1, info['funct3'], info['opcode'])
            elif fmt == 'B':
                rs1 = reg_alias(tokens[1])
                rs2 = reg_alias(tokens[2])
                label = tokens[3]
                offset = symbol_table[label] - pc
                instr = encode_b(offset, rs2, rs1, info['funct3'], info['opcode'])
            elif fmt == 'U':
                rd = reg_alias(tokens[1])
                imm = parse_immediate(tokens[2])
                instr = encode_u(imm, rd, info['opcode'])
            elif fmt == 'J':
                rd = reg_alias(tokens[1])
                label = tokens[2]
                offset = symbol_table[label] - pc
                instr = encode_j(offset, rd, info['opcode'])
            output[pc] = instr
        except Exception as e:
            print(f"Error on line '{' '.join(tokens)}': {e}")
            output[pc] = 0

    output.update(data_segment)

    with open(output_filename, 'wb') as f:
        for addr in sorted(output.keys()):
            f.write(output[addr].to_bytes(4, 'little'))

assemble_file('assembletest.s', 'assembletest.bin')


with open('assembletest.bin', 'rb') as f:
    data = f.read()
    for i in range(0, len(data), 4):
        word = data[i:i+4]
        print(f"{i:04x}: {word.hex()}")
