jasm format
===========

```
.int                    ; handle interrupts
    IRF 80h             ; if CPU was just reset
    JNZ gotoMain:
    IRF 40h             ; external interrupt
    JIZ refresh:
    RET                 ; return to where I was. Registers are restored
refresh:
    HLT                 ; else return to sleep
gotoMain:
    MVI AX, main:       ; start with main
    ENI                 ; clear flags. Reset takes precedence
    JMR AX              ; execute jump

.code                   ; main code
main:                   ;
    MOV AX, 5           ; init loop counter to 5
loop:
    MOV BX, $cZero      ; load value from data section cZero into BX
    PUS AX              ; push counter on stack
    ADD AX, BX          ; add cZero to counter
    MOV BX, FFh         ; move value 255 to BX
    STO BX, AX          ; store digit character at address 255
    POP AX              ; restore counter
    DEC AX              ; decrement
    JIZ lf:             ; if counter is zero, jump to line feed
    JMP loop:           ; else, jump back to loop

lf:
    MVI AX, 20h         ; move value 32 (space) to AX
    STO BX, AX          ; store space at 255

xor:
    MVI BX, 400h
    LOD AX, BX
    MVI BX, $wAwesome
    XOR AX, BX
    JNZ hello:
    MVI BX, 400h
    LOD AX, BX
    INC AX
    STO BX, AX
    JMP xor:

hello:
    MVI AX, 100h        ; store address 0x100 on the stack
    PUS AX

    MVI AX, @sHello     ; init AX to point at the start of sHello
    PUS AX              ; store it on the stack
    MVI BX, 12          ; add 12 to AX to point at the end of sHell
    ADD AX, BX
    PUS AX              ; push that on the stack

helloLoop:
    POP AX              ; pop the end pointer
    DEC AX              ; decrement it
    PUS AX              ; push it again
    INC AX              ; re-increment it to get the original value
    MVI BX, @sHello     ; in order to compare it to the start of sHello
    DEC AX, BX          ; which we do here
    JIZ end:            ; break out of loop if end

    LDI BX, SP, 2       ; load the second value from the stack
    LOD BX, BX          ; it's a pointer, so dereference the pointer
    LDI AX, SP, 4       ; load the third value from the stack
    STO AX, BX          ; store BX at that address
    INC AX              ; increment AX
    STI SP, AX, 4       ; and store it as the third stack value
    LDI BX, SP, 2       ; load the second stack value
    INC BX              ; increment it
    STI SP, BX, 2       ; store it again
    JMP helloLoop:      ; loop

end:
    POP AX              ; some cleanup
    POP AX
    POP AX
    HLT                 ; halt processor

.data
1   cZero       '0'
2   wAwesome    2048
12 sHello       'Hello, you!', 10
8   aWords      1, 64, 256, 1024
4   aChars      01h, 10h, AAh, FFh
6   aHexWords   0001h, 0010h, FFFFh
```

ASM implementation
==================

```C
token_t getNextToken(i_t* instruction, char* first, char* second, char* third);
token_t getNextToken(int* num, char* first, char* second, char* third)
{
    // LJP :label:
    //      num = find(mnemonics, "LJP");
    //      first = ":label:";
    //      return INSTRUCTION;
    // 4 wNum 1024, 2048
    //      num = 2;
    //      first = "wNum";
    //      second = "1024, 2048";
    //      return DATA;
}

typedef enum { INSTRUCTION, LABEL, CODE_SEC, DATA_SEC, INT_SEC, DATA, DATALISTITEM, UNKNOWN } token_t;
typedef enum { JMP, MOV, MVI, ... etc } i_t;

example() {
    i_t i;
    char first[32], second[32], third[32];
    short num;
    switch(state) {
    case codeSection:
        token_t t = getNextToken(&i, first, second, third);
        switch(t) {
        case INSTRUCTION:
            addInstruction(i, first, second, third);
            break;
        case LABEL:
            beginLabel(first);
            break;
        case DATA_SECTION:
            state = DATA_SECTION;
            //startNewDataSection();
            break;
        }
        break;
    case dataSection:
        token_t t = getNextToken(&num, first, second, third);
        switch(t) {
        case DATA:
            beginNewData(num, first);
            char* p = strok(second, ",");
            while(p) {
                appendDataItem(p);
                p = strok(NULL, ",");
            }
            endData();
            break;
        case CODE_SEC:    
            state = t;
            break;
        }
    }
}

typedef struct {
    enum { CODE, DATA } type;
    short size; // opcode size for I, data length for D
    char* label; // optional label for I, label for D
    unsigned char* data; // opcode for I, data for D
} node_t;
```
