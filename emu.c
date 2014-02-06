sig_atomic_t keep_going = 1;

#define DIFF(FIN, INI) (FIN - INI)
// dafuq was I thinking with this?
//    ( (FIN > INI) ? (FIN - INI) : (FIN + ( ((clock_t)(0) - 1) - INI)) )

void loop(unsigned char* memory) {
    short regs[4];
    unsigned char* pc,* sp;
#define Z 15
#define C 14
#define S 13
#define RS 7
#define XT 6
#define BS 5
    short state;
    unsigned char is[104];

    clock_t t1, t2;

    memset(regs, 0, 4 * sizeof(short));
    pc = 0x0000[memory];
    sp = 0xBFFE[memory];
    memset(is, 0, 104 * sizeof(unsigned char));

    while(keep_going) {
        t1 = clock();
        switch(*pc) {
        case 0x00:
            ++pc;
            break;
        case 0x08: // CMP
            state = (0x5FFF & state)
                | ((regs[0] == regs[1]) << Z)
                | ((regs[0] < regs[1]) << S);
            ++pc;
            break;
        case 0x10: // PUS
        case 0x11:
        case 0x12:
        case 0x13:
            0[sp] = regs[*pc & 0x3] & 0x0F;
            1[sp] = (regs[*pc & 0x3] & 0xF0) >> 8;
            sp -= 2;
            ++pc;
            break;
        case 0x14: // POP
        case 0x15:
        case 0x16:
        case 0x17:
            sp += 2;
            regs[*pc & 0x3] = (0[sp] << 8) | 1[sp];
            ++pc;
            break;
        case 0x1F: // LJP
            pc = memory + ((1[pc] << 8) | 2[pc]);
            break;
        case 0x50: // MOV
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5F:
            regs[((*pc) & 0x0C) >> 2] = regs[(*pc) & 0x03];
            pc += 2;
            break;
        case 0x60: // LOD
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6F:
            regs[((*pc) & 0x0C) >> 2] = (memory[regs[(*pc) & 0x03]] << 8) | (memory[regs[(*pc) & 0x03] + 1]);
            pc += 2;
            break;
        case 0x70: // STO
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7F:
            memory[regs[((*pc) & 0x0C)] >> 2] = (0xF0 & regs[(*pc) & 0x03]) >> 8;
            memory[(regs[((*pc) & 0x0C)] >> 2) + 1] = (0x0F & regs[(*pc) & 0x03]);
            ++pc;
            break;
        case 0xE8: // CLI
            state &= ~(1[pc] << 8);
            pc += 2;
            break;
        case 0xE9: // JMP
            pc += 1[pc];
            break;
        case 0xEA: // JIZ
            if(state & (1 << Z)) pc += 1[pc];
            break;
        case 0xEB: // JNZ
            if(!(state & (1 << Z))) pc += 1[pc];
            break;
        case 0xEC: // JLT
            if(state & (1 << S)) pc += 1[pc];
            break;
        case 0xED: // JGE
            if(!(state & (1 << S))) pc += 1[pc];
            break;
        case 0xEE: // JOF
            if(state & (1 << C)) pc += 1[pc];
            break;
        case 0xEF: // JNF
            if(!(state & (1 << C))) pc += 1[pc];
            break;
        case 0xF4: // MVI
        case 0xF5:
        case 0xF6:
        case 0xF7:
            regs[(*pc) & 0x03] = (1[pc] << 8) | (2[pc]);
            pc += 3;
            break;
        case 0xF8: // INC
        case 0xF9:
        case 0xFA:
        case 0xFB:
            ++regs[(*pc++) & 0x3];
            break;
        case 0xFC: // DEC
        case 0xFD:
        case 0xFE:
        case 0xFF:
            --regs[(*pc++) & 0x3];
            break;
        }
        t2 = clock();
        nanosleep((30l - DIFF(t2 + 1, t1)) & 0x1F);
    }
}
