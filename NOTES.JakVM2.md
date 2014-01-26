Instruction cheat sheet
=======================

* NOP ; No OP
* INT ; INTerrupt
* ENI ; ENable Interrupts
* DEI ; DisablE Interrupts
* RST ; ReSeT
* HLT ; HaLT
* CMP ; CoMPare
* RET ; RETurn
* PUS AX ; PUSh
* POP AX ; POP
* LJP 2249h ; Long JumP
* JMP AX ; JuMP     ; MVI AX, label: JMP AX
* JUZ AX ; JUmp if Zero
* JNZ BX ; Jump if Not Zero
* JLT CX ; Jump if Less Than
* JGE SP ; Jump if Greater than or Equal
* JOF AX ; Jump if OverFlow
* JNF AX ; Jump if No Overflow
* MOV AX, BX ; MOVe
* LOD AX, AX ; LOaD
* STO AX, BX ; STOre
* NEG AX ; NEGate
* NOT AX ; NOT
* AND AX, BX ; AND
* IOR BX, CX ; Inclusive OR
* XOR AX, SP ; eXclusive OR
* ADD AX, BX ; ADD
* SUB AX, 16 ; SUBstract
* MUL AX, CX ; MULtiply
* DIV CX, 4 ; DIVide
* CAL 400h ; CALl
* RCL 200h ; ReCalL
* CAR AX ; CAll Register
* RCR AX ; ReCall Register
* IRF 80h ; InterRupt Flags
* ROL AX, BX ; ROtate Left
* SHL AX, 2 ; SHift Left
* SHR BX, 15 ; SHift Right
* ROR CX, AX ; ROtate Right
* CLI FFh ; CLear flags (I?)
* JMI -60 ; JuMp to Immediate value
* JZI loop: ; Zero
* JNI l: ; Nonzero
* JLI l: ; Less
* JGI l: ; Greater
* JOI l: ; Overflow
* JUI l: ; Underflow
* LDI AX, SP, 2 ; LoaD immediate value
* STI SP, AX, 15 ; STore
* SRI AX, SP, BX, 3 ; Store value at Register plus Immediate
* LRI SP, CX, AX, 0 ; Load
* MVI AX, 329 ; MoVe Immediate value
* INC AX ; INCrement
* DEC AX ; DECrement

jasm format
===========

```
.int                    ; handle interrupts
    IRF 80h             ; if CPU was just reset
    JNI gotoMain:
    IRF 40h             ; external interrupt
    JIZ refresh:
    RET                 ; return to where I was. Registers are restored
refresh:
    HLT                 ; else return to sleep
gotoMain:
    MVI AX, main:       ; start with main
    ENI                 ; clear flags. Reset takes precedence
    JMP AX              ; execute jump

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
    JZI lf:             ; if counter is zero, jump to line feed
    JMI loop:           ; else, jump back to loop

lf:
    MVI AX, 20h         ; move value 32 (space) to AX
    STO BX, AX          ; store space at 255

xor:
    MVI BX, 400h
    LOD AX, BX
    MVI BX, $wAwesome
    XOR AX, BX
    JNI hello:
    MVI BX, 400h
    LOD AX, BX
    INC AX
    STO BX, AX
    JMI xor:

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
    JZI end:            ; break out of loop if end

    LDI BX, SP, 2       ; load the second value from the stack
    LOD BX, BX          ; it's a pointer, so dereference the pointer
    LDI AX, SP, 4       ; load the third value from the stack
    STO AX, BX          ; store BX at that address
    INC AX              ; increment AX
    STI SP, AX, 4       ; and store it as the third stack value
    LDI BX, SP, 2       ; load the second stack value
    INC BX              ; increment it
    STI SP, BX, 2       ; store it again
    JMI helloLoop:      ; loop

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
