#include "asm_utils.c"

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

struct iMetaData {
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

typedef struct unprocessed_s {
    enum { UT_INSTRUCTION, UT_DATA } type;
    union {
        struct {
            struct iMetaData* md;
            char first[32];
            char second[32];
            char third[32];
            char fourth[32];
            element_t i;
        } instruction;
        struct {
            unsigned char* data;
            size_t size;
        } data;
    };
    char* label;
    size_t offset;
    struct unprocessed_s* next;
    struct unprocessed_s* prev;
} unprocessed_t;

unsigned char groupSizes[] =
{
    /* G_NOP */ 1,
    /* G_PUS */ 1,
    /* G_LJP */ 3,
    /* G_MOV */ 1,
    /* G_ADD */ 2,
    /* G_RCL */ 1,
    /* G_JMP */ 2,
    /* G_LDI */ 2,
    /* G_LRI */ 2,
    /* G_MVI */ 3,
};

static void pretranslate(unprocessed_t* instr)
{
    switch(instr->type) {
    case UT_INSTRUCTION:
        instr->offset = (instr->prev)
            ? (instr->prev->offset + groupSizes[instr->instruction.md->group])
            : 0;
         break;
    case UT_DATA:
        instr->offset = (instr->prev)
            ? (instr->prev->offset + instr->data.size)
            : 0;
        break;
    }
}

static void nop_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 1;
}

#define TRANSLATE_REG(op, str, offset) do{\
    if(strcmp(str, "AX") == 0) op |= (0 << offset); \
    else if(strcmp(str, "BX") == 0) op |= (1 << offset); \
    else if(strcmp(str, "CX") == 0) op |= (2 << offset); \
    else if(strcmp(str, "SP") == 0) op |= (3 << offset); \
    else abort(); \
}while(0)

#define TRANSLATE_IMED(op, str, mask, offset) do{\
    size_t len = strlen(str); \
    assert(len); \
    if(str[0] == '-') { \
        assert(str[1]); \
        if(str[1] == '0') { \
            if(str[len - 1] == 'h') { \
                op |= ((-satoi16(str+1)) & mask) << offset; \
            } else { \
                op |= ((-satoi8(str+1)) & mask) << offset; \
            } \
        } else { \
            op |= ((-satoi10(str+1)) & mask) << offset; \
        } \
    } else { \
        if(str[0] == '0') { \
            if(str[len - 1] == 'h') {\
                op |= (satoi16(str) & mask) << offset; \
            } else { \
                op |= (satoi8(str) & mask) << offset; \
            } \
        } else { \
            op |= (satoi10(str) & mask) << offset; \
        } \
    } \
}while(0)


static void pus_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr.instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 1;
    TRANSLATE_REG(instr->instruction.i.code[0], instr->instruction.i.first, 0);
}

static void ljp_translator(unprocessed_t* instr)
{
    unprocessed_t* fit = instr->next;
    unprocessed_t* bit = instr->prev;

    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 3;

    while(fit || bit) {
        if(strcmp(fit->label, instr->instruction.first) == 0) {
            instr->instruction.i.code[1] = (fit->offset & 0xFF00) >> 8;
            instr->instruction.i.code[2] = fit->offset & 0xFF;
            return;
        }
        if(strcmp(bit->label, instr->instruction.first) == 0) {
            instr->instruction.i.code[1] = (bit->offset & 0xFF00) >> 8;
            instr->instruction.i.code[2] = bit->offset & 0xFF;
            return;
        }
        if(fit) fit = fit->next;
        if(bit) bit = bit->prev;
    }
    abort();
}

static void mov_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 1;

    TRANSLATE_REG(intr->instruction.i.code[0], instr->instruction.first, 2);
    TRANSLATE_REG(intr->instruction.i.code[0], instr->instruction.second, 0);
}

static void add_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.size = 2;

    TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.first, 2);
    if(isImed(instr->instruction.i.second)) {
        TRANSLATE_IMED(instr->instruction.i.code[1], instr->instruction.i.second, 0xF, 4);
    } else if(isConst(instr->instruction.i.second)) {
        struct const_t* c = consts;
        while(c) {
            if(strcmp(instr->instruction.i.second, c) == 0) {
                if(isImed(c->value)) {
                    TRANSLATE_IMED(instr->instruction.i.code[1], x->value, 0xF, 4);
                }
            }
        }
    } else {
        TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.second, 0);
    }
}

static void rcl_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.size = 1;
    TRANSLATE_IMED(instr->instruction.i.code[0], instr->instruction.i.first, 0xF, 0);
}

static void jmp_translator(unprocessed_t* instr)
{   
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 2;

    if(isImed(instr->instruction.i.first)) {
        TRANSLATE_IMED(instr->instruction.i.code[1], instr->instruction.i.first, 0xFF, 0);
        return;
    } else if(isConst(instr->instruction.i.first)) {
        struct const_t* c = consts;
        while(c) {
            if(strcmp(instr->instruction.i.second, c) == 0) {
                if(isImed(c->value)) {
                    TRANSLATE_IMED(instr->instruction.i.code[1], x->value, 0xF, 4);
                } else {
                    unprocessed_t* fit = instr->next;
                    unprocessed_t* bit = instr->prev;

                    while(fit || bit) {
                        if(strcmp(fit->label, c->value) == 0) {
                            instr->instruction.i.code[1] = fit->offset - instr->offset;
                            return;
                        }
                        if(strcmp(bit->label, c->value) == 0) {
                            instr->instruction.i.code[1] = instr->offset - bit->offset;
                            return;
                        }
                        if(fit) fit = fit->next;
                        if(bit) bit = bit->prev;
                    }
                    abort();
                }
            }
        }
    } else {
        unprocessed_t* fit = instr->next;
        unprocessed_t* bit = instr->prev;

        while(fit || bit) {
            if(strcmp(fit->label, instr->instruction.first) == 0) {
                instr->instruction.i.code[1] = fit->offset - instr->offset;
                return;
            }
            if(strcmp(bit->label, instr->instruction.first) == 0) {
                instr->instruction.i.code[1] = instr->offset - bit->offset;
                return;
            }
            if(fit) fit = fit->next;
            if(bit) bit = bit->prev;
        }
        abort();
    }
}

static void ldi_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 2;
    TRANSLATE_IMED(instr->instruction.i.code[1], instr->instruction.i.first, 0xF, 4);
    TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.second, 2);
    TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.third, 0);
}

static void lri_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 2;
    TRANSLATE_IMED(instr->instruction.i.code[1], instr->instruction.i.first, 0x3, 6);
    TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.second, 4);
    TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.third, 2);
    TRANSLATE_REG(instr->instruction.i.code[1], instr->instruction.i.fourth, 0);
}

static void mvi_translator(unprocessed_t* instr)
{
    instr->instruction.i.code[0] = instr->instruction.md->opcodeHint;
    instr->instruction.i.code[1] = 0;
    instr->instruction.i.code[2] = 0;
    instr->instruction.i.size = 3;

    if(isImed(instr->instruction.i.first)) {
        TRANSLATE_IMED(instr->instruction.i.code[1], instr->instruction.i.first, 0xFF00, 0);
        TRANSLATE_IMED(instr->instruction.i.code[2], instr->instruction.i.first, 0xFF, 0);
    } else if(isConst(instr->instruction.i.first)) {
        struct const_t* c = consts;
        while(c) {
            if(strcmp(instr->instruction.i.second, c) == 0) {
                if(isImed(c->value)) {
                    TRANSLATE_IMED(instr->instruction.i.code[1], x->value, 0xF, 4);
                } else {
                    unprocessed_t* fit = instr->next;
                    unprocessed_t* bit = instr->prev;

                    while(fit || bit) {
                        if(strcmp(fit->label, c->value) == 0) {
                            instr->instruction.i.code[1] = (fit->offset & 0xFF00) >> 8;
                            instr->instruction.i.code[2] = fit->offset & 0xFF;
                            return;
                        }
                        if(strcmp(bit->label, c->value) == 0) {
                            instr->instruction.i.code[1] = (bit->offset & 0xFF00) >> 8;
                            instr->instruction.i.code[2] = bit->offset & 0xFF;
                            return;
                        }
                        if(fit) fit = fit->next;
                        if(bit) bit = bit->prev;
                    }
                    abort();
                }
            }
        }
    } else {
        unprocessed_t* fit = instr->next;
        unprocessed_t* bit = instr->prev;

        while(fit || bit) {
            if(strcmp(fit->label, instr->instruction.first) == 0) {
                instr->instruction.i.code[1] = (fit->offset & 0xFF00) >> 8;
                instr->instruction.i.code[2] = fit->offset & 0xFF;
                return;
            }
            if(strcmp(bit->label, instr->instruction.first) == 0) {
                instr->instruction.i.code[1] = (bit->offset & 0xFF00) >> 8;
                instr->instruction.i.code[2] = bit->offset & 0xFF;
                return;
            }
            if(fit) fit = fit->next;
            if(bit) bit = bit->prev;
        }
        abort();
    }
}

typedef void (*translator_fn)(unprocessed_t* instr);

translator_fn translators[] = {
    &nop_translator,
    &pus_translator,
    &ljp_translator,
    &mov_translator,
    &add_translator,
    &rcl_translator,
    &jmp_translator,
    &ldi_translator,
    &lri_translator,
    &mvi_translator,
};

unprocessed_t* unprocessed_list;
unprocessed_t** addressDependent;

struct const_t {
    const char* name;
    const char* value;
    struct const_t* next;
} *consts;

/*
   first pass:
        parse .jasm file
        if instruction, unprocessed.instruction.md = find_by_mnemonic
                        unprocessed.instruction.first,second,third,fourth = <IN>
                    if instruction not of G_LJP, G_JMP or G_MVI
                        translators[group](instruction)
                    else
                        mark size and offset
        if data,    unprocessed.data.size = <IN>
                    unprocessed.data.data = <IN>
        if label,   mark instr/data with label
    second pass:
        foreach unprocessed which is instruction
            call translators[instruction.md.group](instruction)

    a translator is of the form:
        start with instruction.md.opcodeHint
        if instruction.md.group has parameters
            foreach parameter 
            switch parameter.type
            REG: translate reg
            IMMED: translate imed
            LABEL: translate label

    translate reg:
        switch(regStr)
        AX: add 0 << offset
        BX: add 1 << offset
        CX: add 2 << offset
        SP: add 3 << offset

    translate imed:
        base = base? imed
        call satoi<base>(imed)

    translate label:
        search forward or backward for label with a signed counter
        starting from current instruction
*/
