typedef enum {
    UNKNOWN,
    INSTRUCTION,
    LABEL,
    CODE_SEC,
    DATA_SEC,
    INT_SEC,
    DATA,
    DATA_LIST_ITEM
} token_t;

// assign instructions unique IDs
#define NOP 0
#define INT 1
#define ENI 2
#define DEI 3
#define RST 4
#define HLT 5
#define SWP 6
#define CMP 7
#define RET 8
#define CMU 9
#define PUS 10
#define POP 11
#define LJP 12
#define JMR 13
#define JZR 14
#define JNR 15
#define JLR 16
#define JGR 17
#define JOR 18
#define JUR 19
#define MOV 20
#define LOD 21
#define STO 22
#define NEG 23
#define NOT 24
#define AND 25
#define IOR 26
#define XOR 27
#define ADD 28
#define SUB 29
#define MUL 30
#define DIV 31
#define MOD 32
#define CAL 33
#define RCL 34
#define CAR 35
#define RCR 36
#define SCL 37
#define SCR 38
#define IRF 39
#define LAA 40
#define ROL 41
#define ROR 42
#define SHL 43
#define SHR 44
#define CLI 45
#define JMP 46
#define JIZ 47
#define JNZ 48
#define JLT 49
#define JGE 50
#define JOF 51
#define JNF 52
#define LDI 53
#define STI 54
#define LRI 55
#define SRI 56
#define MVI 57
#define INC 58
#define DEC 59

// group instructions by how they are coded
#define G_NOP 0
#define G_PUS 1 // 2b reg, 0
#define G_LJP 2 // 0, 2B imed
#define G_MOV 3 // 4b reg, 0
#define G_ADD 4 // 0, 1B imed+reg or 2reg
#define G_RCL 5 // 4b imed, 0
#define G_JMP 6 // 0, 1B imed
#define G_LDI 7 // 0, 1B 4b imed + 2reg
#define G_LRI 8 // 0, 1B 2b imed + 3reg
#define G_MVI 9 // 2b reg, 2B imed

struct {
    // the instruction's mnemonic in the assembly language
    char const mnemonic[4];
    // the internal unique id
    unsigned char id;
    // the encoding group the instruction belongs to
    unsigned char group;
    // the base opcode for an instruction not taking parameter encoding
    //     into account (e.g. PUS has base 0x10, but 0x12 is PUS CX)
    unsigned char opcodeHint;
} imap[] = {
    { "NOP", NOP, G_NOP, 0x00 },
    { "INT", INT, G_NOP, 0x01 },
    { "ENI", ENI, G_NOP, 0x02 },
    { "DEI", DEI, G_NOP, 0x03 },
    { "RST", RST, G_NOP, 0x04 },
    { "HLT", HLT, G_NOP, 0x05 },
    { "SWP", SWP, G_NOP, 0x06 },
    { "CMP", CMP, G_NOP, 0x80 },
    { "RET", RET, G_NOP, 0x09 },
    { "CMU", CMU, G_NOP, 0x0A },
    { "PUS", PUS, G_PUS, 0x10 },
    { "POP", POP, G_PUS, 0x14 },
    { "LJP", LJP, G_LJP, 0x1F },
    { "JMR", JMR, G_PUS, 0x24 },
    { "JZR", JZR, G_PUS, 0x28 },
    { "JNR", JNR, G_PUS, 0x2C },
    { "JLR", JLR, G_PUS, 0x30 },
    { "JGR", JGR, G_PUS, 0x34 },
    { "JOR", JOR, G_PUS, 0x38 },
    { "JUR", JUR, G_PUS, 0x3C },
    { "MOV", MOV, G_MOV, 0x50 },
    { "LOD", LOD, G_MOV, 0x60 },
    { "STO", STO, G_MOV, 0x70 },
    { "ADD", ADD, G_ADD, 0x80 },
    { "SUB", SUB, G_ADD, 0x81 },
    { "MUL", MUL, G_ADD, 0x82 },
    { "DIV", DIV, G_ADD, 0x83 },
    { "MOD", MOD, G_ADD, 0x84 },
    { "NEG", NEG, G_PUS, 0x88 },
    { "NOT", NOT, G_PUS, 0x8C },
    { "AND", AND, G_MOV, 0x90 },
    { "IOR", IOR, G_MOV, 0xA0 },
    { "XOR", XOR, G_MOV, 0xB0 },
    { "RCL", RCL, G_RCL, 0xC0 },
    { "CAL", CAL, G_LJP, 0xD0 },
    { "RCR", RCR, G_PUS, 0xD4 },
    { "CAR", CAR, G_MVI, 0xD8 },
    { "SCL", SCL, G_ADD, 0xDC },
    { "SCR", SCR, G_ADD, 0xDD },
    { "IRF", IRF, G_JMP, 0xDF },
    { "LAA", LAA, G_MVI, 0xE0 },
    { "ROL", ROL, G_ADD, 0xE4 },
    { "ROR", ROR, G_ADD, 0xE5 },
    { "SHL", SHL, G_ADD, 0xE6 },
    { "SHR", SHR, G_ADD, 0xE7 },
    { "CLI", CLI, G_JMP, 0xE8 },
    { "JMP", JMP, G_JMP, 0xE9 },
    { "JIZ", JIZ, G_JMP, 0xEA },
    { "JNZ", JNZ, G_JMP, 0xEB },
    { "JLT", JLT, G_JMP, 0xEC },
    { "JGE", JGE, G_JMP, 0xED },
    { "JOF", JOF, G_JMP, 0xEE },
    { "JNF", JNF, G_JMP, 0xEF },
    { "LDI", LDI, G_LDI, 0xF0 },
    { "STI", STI, G_LDI, 0xF1 },
    { "LRI", LRI, G_LRI, 0xF2 },
    { "SRI", SRI, G_LRI, 0xF3 },
    { "MVI", MVI, G_MVI, 0xF4 },
    { "INC", INC, G_PUS, 0xF8 },
    { "DEC", DEC, G_PUS, 0xFC },
} instructions;

typedef struct {
    unsigned char code[3];
    unsigned char len;
} element_t;

element_t encode_pus(char first[], char second[], char third[])
{
    size_t i;
    for(i = 0; i < sizeof(instructions)/sizeof(instructions[0]); ++i) {
        if(strcmp(first, instructions[i].mnemonic) == 0) {
            goto found;
        }
    }
    abort();
found:
    element_t ret = { { instructions[i].opcodeHint, 0, 0 }, 1 };
    if(strcmp(first, "AX") == 0) {
        return ret;
    } else if(strcmp(first, "BX") == 0) {
        ret.code[0]++;
        return ret;
    } else if(strcmp(first, "CX") == 0) {
        ret.code[0]++;
        ret.code[0]++;
        return ret;
    } else if(strcmp(first, "SP") == 0) {
        ret.code[0]++;
        ret.code[0]++;
        ret.code[0]++;
    }
    abort();
    return ret;
}
