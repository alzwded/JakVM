#include "common.h"
#include <string.h> // mem*** functions
#include <time.h> // time functions, data types
#include <signal.h> // sig_atomic_t

/* TODO
    SWP
        this has more to do with when I actually implement the UI part of
        this here nice emu
    move code
        the unsigned char memory declaration and main should be moved
        elsewhere
        this only makes sense after I start implementing the UI since there
        are no other files at the moment
*/

sig_atomic_t keep_going = 1;

typedef struct {
    ui_callback_f func;
    unsigned frame;
} ui_callback_t;
static ui_callback_t callbacks[LAST_CB] = { { NULL, 0 }, { NULL, 0 }, };
static unsigned frameCnt = 0;
static unsigned maxFrameCnt = 1;

static const unsigned gfxs[] = { 0xE000, 0xE800 };
static size_t gfx_idx = 0;
unsigned gfx_buffer_start = 0xE000;

void set_callback(unsigned callback, ui_callback_f func, unsigned frameSkip)
{
    if(callback < LAST_CB) {
        frameSkip = (frameSkip % 60) + 1; // always 1 - 60
        callbacks[callback].func = func;
        callbacks[callback].frame = frameSkip;
        maxFrameCnt = (maxFrameCnt < frameSkip) ? frameSkip : maxFrameCnt;
    }
}

// target 250000 instructions / 60Hz frame
// updated to 125000 / 120Hz frame because of sound
// 17M ns just so happen to equal 17ms, which is about a 60th of a second
// give or take (take 20ms)
#define FRAME_LEN  8500000l
#define INSTR_PER_FRAME 125000l
// why does clock_nanosleep work as intended but nanosleep yields different
//    sleep amounts on different computers?
#define SLEEP(FIN, INI) do{\
    ts.tv_sec = 0; \
    ts.tv_nsec = FRAME_LEN - (FIN.tv_nsec - INI.tv_nsec); \
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL); \
}while(0)

#define SET_ZERO_FLAG(R) do{\
    state = ( (~(1 << Z)) & state ) | (( regs[R] == 0 ) << Z); \
}while(0)

#define SET_SIGN_FLAG(R) do{\
    state = ( (~(1 << S)) & state ) | (( (regs[R] & 0x8000) != 0 ) << S); \
}while(0)

#define SET_ZERO_FLAGH(R) do{\
    state = ( (~(1 << Z)) & state ) | (( (regs[R] & 0xFF00) == 0 ) << Z); \
}while(0)

// R here is actually a 32bit integer, not a register number
#define SET_CARRY_FLAG(R) do{\
    state = ( (~(1 << C)) & state ) | ((( (R) & 0xFFFF0000) != 0) << C); \
}while(0)

void loop(unsigned char* memory) {
    work_t work;
// alias bit positions to their symbolic names
#define Z 15
#define C 14
#define S 13
#define RS 7
#define XT 6
#define BS 5
#define SWINT 1
#define GEI 0
    unsigned char is[sizeof(work_t)];
// storage independent aliases
#define regs work.REGS
#define pc work.PC
#define sp work.REGS[3]
#define state work.STATE

    unsigned char halted = 0;

    // time and synchronization
    struct timespec t1, t2;
    struct timespec ts;
    // okay, we don't actually synchronize at 250000 instructions, but
    // after 250000 ticks (MVI for example "takes" 3 ticks)
    unsigned long ticks = 0;

    // set regs to 0
    memset(regs, 0, 4 * sizeof(short));
    memset(&state, 0, sizeof(short));
    // set RS flag, power-on-reset, GEI not set
    state |= (1 << RS);
    pc = 0x0000 + memory;
    // except for sp, which starts at 0xBFFE
    sp = 0xBFFE;
    memset(is, 0, sizeof(work_t));

    while(keep_going) {
        // begin a synchronization frame
        if(ticks++ == 0) clock_gettime(CLOCK_MONOTONIC, &t1);

        // interrupts
        if((state & (1 << GEI))
        && (state & (0x00E2)))
        {
            state &= ~(1 << SWINT); // clear virtual SWINT flag
            state &= ~(1 << GEI); // unset interrupt flag to avoid endless interrupt loop
            memcpy(is, &work, sizeof(work_t));
            pc = &0x00[memory];
            halted = 0;
        }

        if(halted) goto synchro;

        // execute current instruction
        switch(*pc) {
        case 0x00: // NOP
            ++pc;
            break;
        case 0x01: // INT
            state |= (1 << SWINT);
            ++pc;
            break;
        case 0x02: // ENI
            state = (0xFF00 & state) | (1 << GEI);
            ++pc;
            break;
        case 0x03: // DEI
            state &= (0xFFE0);
            ++pc;
            break;
        case 0x04: // RST
            state |= (1 << RS);
            pc = memory;
            sp = 0xBFFE;
            break;
        default: // unsupported op-codes default to halting
            break;
        case 0x05: // HLT
            halted = 1;
            ++pc;
            break;
        case 0x06: // SWP
            gfx_idx = (gfx_idx + 1) % (sizeof(gfxs) / sizeof(gfxs[0]));
            gfx_buffer_start = gfxs[gfx_idx];
            halted = 1;
            ++pc;
            break;
        case 0x08: // CMP
            state = (0x5FFF & state)
                | ((regs[0] == regs[1]) << Z)
                | (((signed short)regs[0] < (signed short)regs[1]) << S);
            ++pc;
            break;
        case 0x09: // RET
            state &= 0xFF01;
            memcpy(&work, is, sizeof(work_t));
            break;
        case 0x0A: // CMU
            state = (state & ~0xA0)
                | ((regs[0] == regs[1]) << Z)
                | (( (unsigned short)regs[0] < (unsigned short)regs[1] ) << S);
            ++pc;
            break;
        case 0x10: case 0x11: case 0x12: case 0x13: // PUS
            0[memory + sp] = (regs[*pc & 0x3] & 0xFF00) >> 8;
            1[memory + sp] = (regs[*pc & 0x3] & 0xFF);
            SET_ZERO_FLAG(0[pc] & 0x3);
            SET_SIGN_FLAG(0[pc] & 0x3);
            sp -= 2;
            ++pc;
            break;
        case 0x14: case 0x15: case 0x16: case 0x17: // POP
            sp += 2;
            regs[*pc & 0x3] = (0[memory + sp] << 8) | 1[memory + sp];
            SET_ZERO_FLAG(0[pc] & 0x3);
            SET_SIGN_FLAG(0[pc] & 0x3);
            ++pc;
            break;
        case 0x1F: // LJP
            pc = memory + ((1[pc] << 8) | 2[pc]);
            ++ticks;
            ++ticks;
            break;
        case 0x24: case 0x25: case 0x26: case 0x27: // JMR
            pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x28: case 0x29: case 0x2A: case 0x2B: // JZR
            if(state & (1 << Z)) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x2C: case 0x2D: case 0x2E: case 0x2F: // JNR
            if(!(state & (1 << Z))) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x30: case 0x31: case 0x32: case 0x33: // JLR
            if(state & (1 << S)) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x34: case 0x35: case 0x36: case 0x37: // JGR
            if(!(state & (1 << S))) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x38: case 0x39: case 0x3A: case 0x3B: // JOR
            if(state & (1 << C)) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x3C: case 0x3D: case 0x3E: case 0x3F: // JUR
            if(!(state & (1 << C))) pc = memory + regs[(*pc) & 0x3];
            break;
        case 0x50: case 0x51: case 0x52: case 0x53:
        case 0x54: case 0x55: case 0x56: case 0x57:
        case 0x58: case 0x59: case 0x5A: case 0x5B:
        case 0x5C: case 0x5D: case 0x5E: case 0x5F: // MOV
            regs[((*pc) & 0x0C) >> 2] = regs[(*pc) & 0x03];
            SET_ZERO_FLAG(((*pc) & 0x0C) >> 2);
            SET_SIGN_FLAG(((*pc) & 0x0C) >> 2);
            ++pc;
            break;
        case 0x60: case 0x61: case 0x62: case 0x63:
        case 0x64: case 0x65: case 0x66: case 0x67:
        case 0x68: case 0x69: case 0x6A: case 0x6B:
        case 0x6C: case 0x6D: case 0x6E: case 0x6F: // LOD
            regs[((*pc) & 0x0C) >> 2] =
                (regs[((*pc) & 0x0C) >> 2] & 0x00FF)
                | (memory[(unsigned short)regs[(*pc) & 0x03]] << 8);
            SET_ZERO_FLAGH(((*pc) & 0x0C) >> 2);
            SET_SIGN_FLAG(((*pc) & 0x0C) >> 2);
            ++pc;
            break;
        case 0x70: case 0x71: case 0x72: case 0x73:
        case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7A: case 0x7B:
        case 0x7C: case 0x7D: case 0x7E: case 0x7F: // STO
            memory[(unsigned short)regs[((*pc) & 0x0C) >> 2]] = (0xFF00 & regs[(*pc) & 0x03]) >> 8;
            SET_ZERO_FLAGH(((*pc) & 0x03));
            SET_SIGN_FLAG(((*pc) & 0x03));
            ++pc;
            break;
        case 0x80: // ADD
            if(1[pc] & 0xF0) {
                SET_CARRY_FLAG((unsigned int)regs[(1[pc] & 0xC) >> 2] + (unsigned int)((1[pc] & 0xF0) >> 4));
                regs[(1[pc] & 0xC) >> 2] += (1[pc] & 0xF0) >> 4;
            } else {
                SET_CARRY_FLAG((unsigned int)regs[(1[pc] & 0xC) >> 2] + (unsigned int)regs[1[pc] & 0x3]);
                regs[(1[pc] & 0xC) >> 2] += regs[1[pc] & 0x3];
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0x81: // SUB
            if(1[pc] & 0xF0) {
                SET_CARRY_FLAG((unsigned int)regs[(1[pc] & 0xC) >> 2] - (unsigned int)((1[pc] & 0xF0) >> 4));
                regs[(1[pc] & 0xC) >> 2] -= (1[pc] & 0xF0) >> 4;
            } else {
                SET_CARRY_FLAG((unsigned int)regs[(1[pc] & 0xC) >> 2] - (unsigned int)regs[1[pc] & 0x3]);
                regs[(1[pc] & 0xC) >> 2] -= regs[1[pc] & 0x3];
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0x82: // MUL
            if(1[pc] & 0xF0) {
                SET_CARRY_FLAG((unsigned int)regs[(1[pc] & 0xC) >> 2] * (unsigned int)((1[pc] & 0xF0) >> 4));
                regs[(1[pc] & 0xC) >> 2] *= (1[pc] & 0xF0) >> 4;
            } else {
                SET_CARRY_FLAG((unsigned int)regs[(1[pc] & 0xC) >> 2] * (unsigned int)regs[1[pc] & 0x3]);
                regs[(1[pc] & 0xC) >> 2] *= regs[1[pc] & 0x3];
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0x83: // DIV
            if(1[pc] & 0xF0) {
                if(((1[pc] & 0xF0) >> 4) == 0) {
                    state |= (1 << C);
                    regs[(1[pc] & 0xC) >> 2] = 0x7FFF | (regs[(1[pc] & 0xC) >> 2] & 0x8000);
                } else {
                    state &= ~(1 << C);
                    regs[(1[pc] & 0xC) >> 2] /= (1[pc] & 0xF0) >> 4;
                }
            } else {
                if(regs[1[pc] & 0x3] == 0) {
                    state |= (1 << C);
                    regs[(1[pc] & 0xC) >> 2] = 0x7FFF | (regs[(1[pc] & 0xC) >> 2] & 0x8000);
                } else {
                    state &= ~(1 << C);
                    regs[(1[pc] & 0xC) >> 2] /= regs[1[pc] & 0x3];
                }
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0x84: // MOD
            if(1[pc] & 0xF0) {
                if(((1[pc] & 0xF0) >> 4) == 0) {
                    state |= (1 << C);
                    regs[(1[pc] & 0xC) >> 2] = 0x7FFF | (regs[(1[pc] & 0xC) >> 2] & 0x8000);
                } else {
                    state &= ~(1 << C);
                    regs[(1[pc] & 0xC) >> 2] %= (1[pc] & 0xF0) >> 4;
                }
            } else {
                if(regs[1[pc] & 0x3] == 0) {
                    state |= (1 << C);
                    regs[(1[pc] & 0xC) >> 2] = 0x7FFF | (regs[(1[pc] & 0xC) >> 2] & 0x8000);
                } else {
                    state &= ~(1 << C);
                    regs[(1[pc] & 0xC) >> 2] %= regs[1[pc] & 0x3];
                }
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0x88: case 0x89: case 0x8A: case 0x8B: // NEG
            regs[(*pc) & 0x3] = -regs[(*pc) & 0x3];
            SET_ZERO_FLAG((1[pc] & 0x3));
            SET_SIGN_FLAG((1[pc] & 0x3));
            ++pc;
            break;
        case 0x8C: case 0x8D: case 0x8E: case 0x8F: // NOT
            regs[(*pc) & 0x3] = ~regs[(*pc) & 0x3];
            SET_ZERO_FLAG((1[pc] & 0x3));
            SET_SIGN_FLAG((1[pc] & 0x3));
            ++pc;
            break;
        case 0x90: case 0x91: case 0x92: case 0x93:
        case 0x94: case 0x95: case 0x96: case 0x97:
        case 0x98: case 0x99: case 0x9A: case 0x9B:
        case 0x9C: case 0x9D: case 0x9E: case 0x9F: // AND
            regs[(0[pc] & 0xC) >> 2] &= regs[0[pc] & 0x3];
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            break;
        case 0xA0: case 0xA1: case 0xA2: case 0xA3:
        case 0xA4: case 0xA5: case 0xA6: case 0xA7:
        case 0xA8: case 0xA9: case 0xAA: case 0xAB:
        case 0xAC: case 0xAD: case 0xAE: case 0xAF: // IOR
            regs[(0[pc] & 0xC) >> 2] |= regs[0[pc] & 0x3];
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            break;
        case 0xB0: case 0xB1: case 0xB2: case 0xB3:
        case 0xB4: case 0xB5: case 0xB6: case 0xB7:
        case 0xB8: case 0xB9: case 0xBA: case 0xBB:
        case 0xBC: case 0xBD: case 0xBE: case 0xBF: // XOR
            regs[(0[pc] & 0xC) >> 2] ^= regs[0[pc] & 0x3];
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            break;
        case 0xC0: case 0xC1: case 0xC2: case 0xC3:
        case 0xC4: case 0xC5: case 0xC6: case 0xC7:
        case 0xC8: case 0xC9: case 0xCA: case 0xCB:
        case 0xCC: case 0xCD: case 0xCE: case 0xCF: // RCL
            sp += 2 * (0[pc] & 0x0F);
            pc = memory + ((0[memory + sp] << 8) | 1[memory + sp]);
            sp += 2;
            ++ticks;
            break;
        case 0xD0: // CAL
            0[memory + sp] = ((pc - memory + 3) & 0xFF00) >> 8;
            1[memory + sp] = ((pc - memory + 3) & 0xFF);
            sp -= 2;
            pc = memory + ((1[pc] << 8) | (2[pc]));
            ++ticks;
            ++ticks;
            break;
        case 0xD4: case 0xD5: case 0xD6: case 0xD7: // RCR
            pc = ((memory[regs[0[pc] & 0x3] + 0] << 8)
                |(memory[regs[0[pc] & 0x3] + 1])) + memory;
            sp = ((memory[regs[0[pc] & 0x3] + 1] << 8)
                |(memory[regs[0[pc] & 0x3] + 2]));
            ++ticks;
            break;
        case 0xD8: case 0xD9: case 0xDA: case 0xDB: // CAR
            memory[regs[0[pc] & 0x3] + 0] = ((pc - memory + 3) & 0xFF00) >> 8;
            memory[regs[0[pc] & 0x3] + 1] = ((pc - memory + 3) & 0xFF);
            memory[regs[0[pc] & 0x3] + 1] = ((sp) & 0xFF00) >> 8;
            memory[regs[0[pc] & 0x3] + 2] = ((sp) & 0xFF);
            pc = memory + ((1[pc] << 8) | 2[pc]);
            ++ticks;
            ++ticks;
            break;
// BEGIN MACRO FUN
        case 0xDC: // SCL
            if(1[pc] & 0xF0) {
                short newState = state;
#undef state
#define state newState
                SET_CARRY_FLAG(((unsigned int)regs[(1[pc] & 0xC) >> 2] << ((1[pc] & 0xF0) >> 4) ) & 0x0001FFFF);
#undef state
#define state work.STATE
                regs[(1[pc] & 0xC) >> 2] <<= (1[pc] & 0xF0) >> 4;
                if(state & (1 << C)) regs[(1[pc] & 0xC) >> 2] |= ((1 << ((1[pc] & 0xF0) >> 4)) - 1);
                state = newState;
            } else {
                short newState = state;
#undef state
#define state newState
                SET_CARRY_FLAG(((unsigned int)regs[(1[pc] & 0xC) >> 2] << regs[1[pc] & 0x3]) & 0x0001FFFF);
#undef state
#define state work.STATE
                regs[(1[pc] & 0xC) >> 2] <<= regs[1[pc] & 0x3];
                if(state & (1 << C)) regs[(1[pc] & 0xC) >> 2] |= ((1 << regs[1[pc] & 0x3]) - 1);
                state = newState;
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xDD: // SCR
            if(1[pc] & 0xF0) {
#define R regs[(1[pc] & 0xC) >> 2]
#define AMOUNT ((1[pc] & 0xF0) >> 4)
                short newState = (0xBF & state) |
                    (((((R << 1) >> AMOUNT) & 0x1) != 0) << C);
/* let's take some time to explain the following two lines, since I imagine
        they are not entirely clear. The behaviour of the SCR instruction
        is documented in IS.md.
   CARRY_MASK essentially is a short int mask which sets the
        sign bit (0x8000) to 1 if Carry was set, or 0 if Carry was not set
   Then, this mask is OR'd over R, thus setting R's sign bit to the value of
        the Carry flag
   Then, the result is cast to a signed short. This is important for what is
        to come
   Next we expand it to a signed int. Since it was a signed short, 
        the new high 2 bytes are padded with 1's if the expanded short 
        was negative, or 0 if the expanded short was positive.
        (it depends on whether CARRY_MASK was 0x8000 or 0)
   Then, we create a mask for the bits that were shifted in. I.e.
        0xFFFF0000 >> AMOUNT. This makes sure we keep only the bits that
        were shifted in and we don't affect R's original bits. This means
        we can then safely OR this over (R >> AMOUNT), but we're not done
        yet
   Lastly, we &it with 0xFFFF and cast the result back to an unsigned short,
        thus obtaining the bits that were shifted in with the value of
        the Carry flag, as per specification
   This is a good example of the power of C and all the stuff you can do
        with it without declaring any compiler-confusing variables.

    Also note that doing a (signed int)((signed short)myUInt16) is the
        easiest way to tell the compiler to generate the movswl
        starting from an unsigned, otherwise it might zero extend it.
*/
#define CARRY_MASK ((((state) & (1 << C)) != 0) << 15)
#define R_CARRY_BITS (unsigned short)((((signed int)((signed short)((R | CARRY_MASK))) >> AMOUNT) & (0xFFFF0000 >> AMOUNT)) & 0xFFFF)
                R = R_CARRY_BITS | (R >> AMOUNT);
                state = newState;
#undef AMOUNT
            } else {
#define AMOUNT (regs[1[pc] & 0x3] % 16)
                short newState = (0xBF & state) |
                    (((((R << 1) >> AMOUNT) & 0x1) != 0) << C);
                R = R_CARRY_BITS | (R >> AMOUNT);
                state = newState;
#undef R_CARRY_BITS
#undef AMOUNT
#undef R
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
// END MACRO FUN
        case 0xDF: // IRF
            if((state & 0xFF) & 1[pc]) state |= (1 << Z);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xE0: case 0xE1: case 0xE2: case 0xE3: // LAA
            regs[0[pc] & 0x3] = (0xFF & regs[0[pc] & 0x3])
                | (memory[(1[pc] << 8) | 2[pc]] << 8);
            SET_ZERO_FLAGH(((*pc) & 0x03));
            SET_SIGN_FLAG(((*pc) & 0x03));
            ++pc;
            ++ticks;
            ++pc;
            ++ticks;
            ++pc;
            break;
// BEGIN MACRO FUN
        case 0xE4: // ROL
            if(1[pc] & 0xF0) {
#define R regs[(1[pc] & 0xC) >> 2]
#define AMOUNT ((1[pc] & 0xF0) >> 4)
                R = (R << AMOUNT) | (R >> (16 - AMOUNT));
#undef AMOUNT
            } else {
#define AMOUNT (regs[1[pc] & 0x3] % 16)
                R = (R << AMOUNT) | (R >> (16 - AMOUNT));
#undef AMOUNT
#undef R
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xE5: // ROR
            if(1[pc] & 0xF0) {
#define R regs[(1[pc] & 0xC) >> 2]
#define AMOUNT ((1[pc] & 0xF0) >> 4)
                R = (R >> AMOUNT) | (R << (16 - AMOUNT));
#undef AMOUNT
            } else {
#define AMOUNT (regs[1[pc] & 0x3] % 16)
                R = (R >> AMOUNT) | (R << (16 - AMOUNT));
#undef AMOUNT
#undef R
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xE6: // SHL
            if(1[pc] & 0xF0) {
#define R regs[(1[pc] & 0xC) >> 2]
#define AMOUNT ((1[pc] & 0xF0) >> 4)
                R = (R << AMOUNT);
#undef AMOUNT
            } else {
#define AMOUNT (regs[1[pc] & 0x3] % 16)
                R = (R << AMOUNT);
#undef AMOUNT
#undef R
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);

            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xE7: // SHR
            if(1[pc] & 0xF0) {
#define R ((signed short)(regs[(1[pc] & 0xC) >> 2]))
#define AMOUNT ((1[pc] & 0xF0) >> 4)
                regs[(1[pc] & 0xC) >> 2] = (R >> AMOUNT);
#undef AMOUNT
            } else {
#define AMOUNT (regs[1[pc] & 0x3] % 16)
                regs[(1[pc] & 0xC) >> 2] = (R >> AMOUNT);
#undef AMOUNT
#undef R
            }
            SET_ZERO_FLAG((1[pc] & 0xC) >> 2);
            SET_SIGN_FLAG((1[pc] & 0xC) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
// END MACRO FUN
        case 0xE8: // CLI
            state &= ~(1[pc] << 8) | 0xFF;
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xE9: // JMP
            pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xEA: // JIZ
            if(state & (1 << Z)) pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xEB: // JNZ
            if(!(state & (1 << Z))) pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xEC: // JLT
            if(state & (1 << S)) pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xED: // JGE
            if(!(state & (1 << S))) pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xEE: // JOF
            if(state & (1 << C)) pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xEF: // JNF
            if(!(state & (1 << C))) pc += (signed char)1[pc];
            ++ticks;
            break;
        case 0xF0: // LDI
            regs[(1[pc] & 0xC) >> 2] = (0xFF & regs[(1[pc] & 0xC) >> 2]);
            regs[(1[pc] & 0xC) >> 2] |= memory[((1[pc] & 0xF0) >> 4) + regs[1[pc] & 0x3]] << 8;
            SET_ZERO_FLAGH(((*pc) & 0x0C) >> 2);
            SET_SIGN_FLAG(((*pc) & 0x0C) >> 2);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xF1: // STI
            memory[regs[(1[pc] & 0xC) >> 2] + ((1[pc] & 0xF0) >> 4)] = 
                (regs[1[pc] & 0x3] & 0xFF00) >> 8;
            SET_ZERO_FLAGH(((*pc) & 0x03));
            SET_SIGN_FLAG(((*pc) & 0x03));
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xF2: // LRI
            regs[(1[pc] & 0x30) >> 4] = (0xFF & regs[(1[pc] & 0x30) >> 4]);
            regs[(1[pc] & 0x30) >> 4] |= memory[((1[pc] & 0xC0) >> 6) + regs[1[pc] & 0x3] + regs[(1[pc] & 0xC) >> 2]] << 8;
            SET_ZERO_FLAGH(((*pc) & 0x30) >> 4);
            SET_SIGN_FLAG(((*pc) & 0x30) >> 4);
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xF3: // SRI
            memory[regs[(1[pc] & 0x30) >> 4] + regs[(1[pc] & 0xC) >> 2] + ((1[pc] & 0xF0) >> 4)] = 
                (regs[1[pc] & 0x3] & 0xFF00) >> 8;
            SET_ZERO_FLAGH(((*pc) & 0x03));
            SET_SIGN_FLAG(((*pc) & 0x03));
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xF4: case 0xF5: case 0xF6: case 0xF7: // MVI
            regs[(*pc) & 0x03] = (1[pc] << 8) | (2[pc]);
            SET_ZERO_FLAG(((*pc) & 0x3));
            SET_SIGN_FLAG(((*pc) & 0x3));
            ++pc;
            ++ticks;
            ++pc;
            ++ticks;
            ++pc;
            break;
        case 0xF8: case 0xF9: case 0xFA: case 0xFB: // INC
            SET_CARRY_FLAG((unsigned int)regs[(*pc) & 0x3] + 1);
            ++regs[(*pc++) & 0x3];
            SET_ZERO_FLAG(((*pc) & 0x3));
            SET_SIGN_FLAG(((*pc) & 0x3));
            ++ticks;
            break;
        case 0xFC: case 0xFD: case 0xFE: case 0xFF: // DEC
            SET_CARRY_FLAG((unsigned int)regs[(*pc) & 0x3] - 1);
            --regs[(*pc++) & 0x3];
            SET_ZERO_FLAG(((*pc) & 0x3));
            SET_SIGN_FLAG(((*pc) & 0x3));
            ++ticks;
            break;
        }

synchro:
        // synchronize every 250000 instructions or so
        if(ticks >= INSTR_PER_FRAME) {
            size_t i;
            for(i = 0; i < LAST_CB; ++i) {
                if(callbacks[i].func && (frameCnt % callbacks[i].frame) == 0) {
                    callbacks[i].func(&work);
                }
            }
            frameCnt = (frameCnt + 1) % maxFrameCnt;
            ticks = 0;
            clock_gettime(CLOCK_MONOTONIC, &t2);
            SLEEP(t2, t1);
        }
    }
}

// TODO move out of here
unsigned char memory[65536];

void load_test_program1()
{
    // increments memory location 0x8000 by 7
    // these are the raw op-codes put in memory
    memory[0] = 0x02; // ENI
    memory[1] = 0xF4; // MVI AX, 1
    memory[2] = 0x01;
    memory[3] = 0x01;
    memory[4] = 0xF5; // MVI BX, 1
    memory[5] = 0x06;
    memory[6] = 0x06;
    memory[7] = 0x80; // ADD AX, BX
    memory[8] = 0x01; // 
    memory[9] = 0xF5; // MVI BX, 0x8000
    memory[10] = 0x80;
    memory[11] = 0x00;
    memory[12] = 0x11; // PUS BX
    memory[13] = 0x65; // LOD BX, BX
    memory[14] = 0x80; // ADD AX, BX
    memory[15] = 0x01;
    memory[16] = 0x15; // POP BX
    memory[17] = 0x74; // STO BX, AX
    memory[18] = 0x04; // RST
}

int main()
{
    load_test_program1();
    loop(memory);
}
