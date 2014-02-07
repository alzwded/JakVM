#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
sig_atomic_t keep_going = 1;

// target 250000 instructions / 60Hz frame
#define INSTR_PER_FRAME 250000
#define SLEEP(FIN, INI) do{\
    ts.tv_sec = 0; \
    ts.tv_nsec = 17000000 - (FIN.tv_nsec - INI.tv_nsec); \
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL); \
}while(0)

void loop(unsigned char* memory) {
    struct work_s {
        unsigned short REGS[4]; // AX, BX, CX, SP
        unsigned char* PC;
#define Z 15
#define C 14
#define S 13
#define RS 7
#define XT 6
#define BS 5
        short STATE; // RS:RF
    } work;
#define regs work.REGS
#define pc work.PC
#define sp work.REGS[3]
#define state work.STATE
    unsigned char is[sizeof(struct work_s)];

    // time and synchronization
    struct timespec t1, t2;
    struct timespec ts;
    unsigned long ticks = 0;

    // set regs to 0
    memset(regs, 0, 4 * sizeof(short));
    memset(&state, 0, sizeof(short));
    pc = 0x0000 + memory;
    // except for sp, which starts at 0xBFFE
    sp = 0xBFFE;
    memset(is, 0, sizeof(struct work_s));

    while(keep_going) {
        // begin a synchronization frame
        if(ticks++ == 0) clock_gettime(CLOCK_MONOTONIC, &t1);

        // execute current instruction
        switch(*pc) {
        case 0x00: // NOP
            ++pc;
            break;
        case 0x01: // INT
            memcpy(is, &work, sizeof(struct work_s));
            pc = &0x00[memory];
            break;
        case 0x02: // ENI
        case 0x03: // DEI
            ++pc;
            break;
        case 0x04: // RST
            state |= state | (1 << RS);
            pc = memory;
            sp = 0xBFFE;
            break;
        case 0x05: // HLT
            break;
        case 0x06: // SWP
            ++pc;
            break;
        case 0x08: // CMP
            state = (0x5FFF & state)
                | ((regs[0] == regs[1]) << Z)
                | (((signed short)regs[0] < (signed short)regs[1]) << S);
            ++pc;
            break;
        case 0x09: // RET
            memcpy(&work, is, sizeof(struct work_s));
            break;
        case 0x0A: // CMU
            state = (state & ~0xA0)
                | ((regs[0] == regs[1]) << Z)
                | (( (unsigned short)regs[0] < (unsigned short)regs[1] ) << S);
            ++pc;
            break;
        case 0x10: // PUS
        case 0x11:
        case 0x12:
        case 0x13:
            0[memory + sp] = (regs[*pc & 0x3] & 0xFF00) >> 8;
            1[memory + sp] = (regs[*pc & 0x3] & 0xFF);
            sp -= 2;
            ++pc;
            break;
        case 0x14: // POP
        case 0x15:
        case 0x16:
        case 0x17:
            sp += 2;
            regs[*pc & 0x3] = (0[memory + sp] << 8) | 1[memory + sp];
            ++pc;
            break;
        case 0x1F: // LJP
            pc = memory + ((1[pc] << 8) | 2[pc]);
            break;
        case 0x24: // JMR
        case 0x25:
        case 0x26:
        case 0x27:
            pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x28: // JZR
        case 0x29:
        case 0x2A:
        case 0x2B:
            if(state & (1 << Z)) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x2C: // JNR
        case 0x2D:
        case 0x2E:
        case 0x2F:
            if(!(state & (1 << Z))) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x30: // JLR
        case 0x31:
        case 0x32:
        case 0x33:
            if(state & (1 << S)) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x34: // JGR
        case 0x35:
        case 0x36:
        case 0x37:
            if(!(state & (1 << S))) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x38: // JOR
        case 0x39:
        case 0x3A:
        case 0x3B:
            if(state & (1 << C)) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x3C: // JUR
        case 0x3D:
        case 0x3E:
        case 0x3F:
            if(!(state & (1 << C))) pc = memory + regs[(*pc) & 0x3];
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
            ++pc;
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
            regs[((*pc) & 0x0C) >> 2] =
                (regs[((*pc) & 0x0C) >> 2] & 0x00FF)
                | (memory[(unsigned short)regs[(*pc) & 0x03]] << 8);
            ++pc;
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
            memory[(unsigned short)regs[((*pc) & 0x0C) >> 2]] = (0xFF00 & regs[(*pc) & 0x03]) >> 8;
            ++pc;
            break;
        case 0x80: // ADD
            if(1[pc] & 0xF0)
                regs[(1[pc] & 0xC) >> 2] += (1[pc] & 0xF0) >> 4;
            else
                regs[(1[pc] & 0xC) >> 2] += regs[1[pc] & 0x3];
            pc += 2;
            break;
        case 0x81: // SUB
            if(1[pc] & 0xF0)
                regs[(1[pc] & 0xC) >> 2] -= (1[pc] & 0xF0) >> 4;
            else
                regs[(1[pc] & 0xC) >> 2] -= regs[1[pc] & 0x3];
            pc += 2;
            break;
        case 0x82: // MUL
            if(1[pc] & 0xF0)
                regs[(1[pc] & 0xC) >> 2] *= (1[pc] & 0xF0) >> 4;
            else
                regs[(1[pc] & 0xC) >> 2] *= regs[1[pc] & 0x3];
            pc += 2;
            break;
        case 0x83: // DIV
            if(1[pc] & 0xF0)
                regs[(1[pc] & 0xC) >> 2] /= (1[pc] & 0xF0) >> 4;
            else
                regs[(1[pc] & 0xC) >> 2] /= regs[1[pc] & 0x3];
            pc += 2;
            break;
        case 0x84: // MOD
            if(1[pc] & 0xF0)
                regs[(1[pc] & 0xC) >> 2] %= (1[pc] & 0xF0) >> 4;
            else
                regs[(1[pc] & 0xC) >> 2] %= regs[1[pc] & 0x3];
            pc += 2;
            break;
        case 0x88: // NEG
        case 0x89:
        case 0x8A:
        case 0x8B:
            regs[(*pc) & 0x3] = -regs[(*pc) & 0x3];
            ++pc;
            break;
        case 0x8C: // NOT
        case 0x8D:
        case 0x8E:
        case 0x8F:
            regs[(*pc) & 0x3] = ~regs[(*pc) & 0x3];
            ++pc;
            break;
        case 0x90: // AND
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
            regs[(0[pc] & 0xC) >> 2] &= regs[0[pc] & 0x3];
            ++pc;
            break;
        case 0xA0: // IOR
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF:
            regs[(0[pc] & 0xC) >> 2] |= regs[0[pc] & 0x3];
            ++pc;
            break;
        case 0xB0: // XOR
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
            regs[(0[pc] & 0xC) >> 2] ^= regs[0[pc] & 0x3];
            ++pc;
            break;
        case 0xC0: // RCL
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7:
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
            sp += 2 * (0[pc] & 0x0F);
            pc = memory + ((0[memory + sp] << 8) | 1[memory + sp]);
            sp += 2;
            break;
        case 0xD0: // CAL
            0[memory + sp] = ((pc - memory + 3) & 0xFF00) >> 8;
            1[memory + sp] = ((pc - memory + 3) & 0xFF);
            sp -= 2;
            pc = memory + ((1[pc] << 8) | (2[pc]));
            break;
        case 0xD4: // RCR
        case 0xD5:
        case 0xD6:
        case 0xD7:
            pc = ((memory[regs[0[pc] & 0x3] + 0] << 8)
                |(memory[regs[0[pc] & 0x3] + 1])) + memory;
            sp = ((memory[regs[0[pc] & 0x3] + 1] << 8)
                |(memory[regs[0[pc] & 0x3] + 2]));
            break;
        case 0xD8: // CAR
        case 0xD9:
        case 0xDA:
        case 0xDB:
            memory[regs[0[pc] & 0x3] + 0] = ((pc - memory + 3) & 0xFF00) >> 8;
            memory[regs[0[pc] & 0x3] + 1] = ((pc - memory + 3) & 0xFF);
            memory[regs[0[pc] & 0x3] + 1] = ((sp) & 0xFF00) >> 8;
            memory[regs[0[pc] & 0x3] + 2] = ((sp) & 0xFF);
            pc = memory + ((1[pc] << 8) | 2[pc]);
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

        // synchronize every 250000 instructions or so
        if(ticks == INSTR_PER_FRAME) {
            ticks = 0;
            SLEEP(t2, t1);
        }
    }
}

unsigned char memory[65536];

void load_test_program1()
{
    // increments memory location 0x8000 by 7
    // these are the raw op-codes put in memory
    memory[0] = 0xF4; // MVI AX, 1
    memory[1] = 0x01;
    memory[2] = 0x01;
    memory[3] = 0xF5; // MVI BX, 1
    memory[4] = 0x06;
    memory[5] = 0x06;
    memory[6] = 0x80; // ADD AX, BX
    memory[7] = 0x01; // 
    memory[8] = 0xF5; // MVI BX, 0x8000
    memory[9] = 0x80;
    memory[10] = 0x00;
    memory[11] = 0x65; // LOD BX, BX
    memory[12] = 0x80; // ADD AX, BX
    memory[13] = 0x01;
    memory[14] = 0xF5; // MVI BX, 0x8000
    memory[15] = 0x80;
    memory[16] = 0x00;
    memory[17] = 0x74; // STO BX, AX
    memory[18] = 0x04; // RST
}

int main()
{
    load_test_program1();
    loop(memory);
}
