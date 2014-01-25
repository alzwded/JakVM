Registers
=========

| Name              | Description                     | AD | Width |
|-------------------|---------------------------------|----|-------|
| AX                | Accumulator                     | 00 | 16b   |
| BX                | Accumulator                     | 01 | 16b   |
| CX                | Accumulator                     | 10 | 16b   |
| SP                | Stack pointer                   | 11 | 16b   |
| PC                | Program counter (N/A)           | -- | 16b   |
| RS                | Status flags (c.f. Flags) (N/A) | -- |  8b   |
| RF                | Interrupt flags (N/A)           | -- |  8b   |
| IS                | Interrupt state                 | -- | 88b   |

* AX, BX, SP and CX can be used as general purpose register
* AX and BX are implicit in the CMP instruction
* SP is implicit in the PUS and POP instructions. PUS auto post-decrements SP, POP auto pre-increments SI.
* PC cannot be read. It can only be modified via the J** instructions
* RS and RF may be implemented in the same register. RS can only be read by the J** instructions. RF can only be read via the IRF instruction with a mask.
* RS is modified by various instructions (Arithmetic, Bitwise, IRF, CLI...)
* RF is modified by external forces and is cleared by the RET instruction
* the IS register contains the data from all of the registers (sans RF). This is used to restore data via the RET instruction after an interrupt
* the IS register is filled with data whenever an interrupt occurs. It is essentially a shadow of all of the other registers

Flags
=====

| Name          | Description       |
|---------------|-------------------|
| Z             | Zero flag         |
| C             | Overflow/Carry    |
| S             | Sign              |
| I             | Interrupt request |

The layout is: `Z``C``S``0``0``0``0``I`

Interrupt flags
===============

| Name          | Description        |
|---------------|--------------------|
| RS            | Reset              |
| XT            | External interrupt |
| BS            | Buffer swapped     |

The layout is: `RS``XT``BS``0``0``0``0``0`

Instruction set
===============

Op codes
--------

* RR means register selector (2 bits) (see Registers)
* VV means any value (imediate values)
* ?? means value is ignored; it should be kept 00 because it might be used in the future

Everything is (should be?) big endian.

0x00 - 0x7F Range, single byte: Branch&Control and Memory instructions

| Instruction | Code           | Range        |
|-------------|----------------|--------------|
| NOP         | 0b 00 00 00 00 |              |
| INT         | 0b 00 00 00 01 |              |
| ENI         | 0b 00 00 00 10 |              |
| DEI         | 0b 00 00 00 11 |              |
| RST         | 0b 00 00 01 00 |              |
| HLT         | 0b 00 00 01 01 |              |
| CMP         | 0b 00 00 10 00 |              |
| RET         | 0b 00 00 10 01 |              |
| reserved    | 0b 00 00 10 1? | 0x0A - 0x0B  |
| reserved    | 0b 00 00 11 ?? | 0x0C - 0x0F  |
| PUS         | 0b 00 01 00 RR | 0x10 - 0x13  |
| POP         | 0b 00 01 01 RR | 0x14 - 0x17  |
| reserved    | 0b 00 01 1? ?? | 0x18 - 0x1F  |
| JMP         | 0b 00 10 01 RR | 0x20 - 0x3F  |
| JIZ         | 0b 00 10 10 RR | 0x20 - 0x3F  |
| JNZ         | 0b 00 10 11 RR | 0x20 - 0x3F  |
| JLT         | 0b 00 11 00 RR | 0x20 - 0x3F  |
| JGE         | 0b 00 11 01 RR | 0x20 - 0x3F  |
| JOF         | 0b 00 11 10 RR | 0x20 - 0x3F  |
| JNF         | 0b 00 11 11 RR | 0x20 - 0x3F  |
| reserved    | 0b 01 00 ?? ?? | 0x40 - 0x4F  |
| MOV         | 0b 01 01 RR RR | 0x50 - 0x5F  |
| LOD         | 0b 01 10 RR RR | 0x60 - 0x6F  |
| STO         | 0b 01 11 RR RR | 0x70 - 0x7F  |

* NOP: does nothing
* INT: software interrupt
* ENI: enable interrupts
* DEI: disable interrupts
* RST: reset
* HLT: sleep (stop clock)
* CMP: compare AX and BX and set Z flag if they are identical
* RET: return from interrupt handler. Clears RF register.
* PUS: stack push
* POP: stack pop
* JIZ: branch if Z flag is set
* JLT: jump if S flag is set
* JOF: jump if C flag is set
* JNZ, JGE, JNF: negated versions
* MOV: move data from one register to another
* LOD: load data from address taken from LSB register to MSB register
* STO: store data from LSB register to address taken from MSB register

0x80 - 0xBF Range, single byte: Bitwise Instructions

| Instruction   | Code                | Range       |
|---------------|---------------------|-------------|
| NEG           | 0b 10 00 00 RR      | 0x80 - 0x83 |
| NOT           | 0b 10 00 01 RR      | 0x84 - 0x87 |
| reserved      | 0b 10 00 1? ??      | 0x88 - 0x8F |
| AND           | 0b 10 01 RR RR      | 0x90 - 0x9F |
| IOR           | 0b 10 10 RR RR      | 0xA0 - 0xAF |
| XOR           | 0b 10 11 RR RR      | 0xB0 - 0xBF |

* NEG: change sign
* NOT: bitwise not
* AND: bitwise AND
* IOR: bitwsie inclusive or
* XOR: bitwise exclusive or

0xC0 - 0xC3 Range, 2 bytes: Arithmetic Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| ADD           | 0b 11 00 00 00 | 0b0000RRRR |
| SUB           | 0b 11 00 00 01 | 0b0000RRRR |
| MUL           | 0b 11 00 00 10 | 0b0000RRRR |
| DIV           | 0b 11 00 00 11 | 0b0000RRRR |
| ADD           | 0b 11 00 00 00 | 0bVVVVRR?? |
| SUB           | 0b 11 00 00 01 | 0bVVVVRR?? |
| MUL           | 0b 11 00 00 10 | 0bVVVVRR?? |
| DIV           | 0b 11 00 00 11 | 0bVVVVRR?? |

* First set add value from LSB register to MSB register (VVVV=0)
* Second set add imediate value to register (VVVV!=0)

0xD0 - 0xDF Range: reserved

| Instruction   | Code           |
|---------------|----------------|
| reserved      | 0b 11 01 ?? ?? |

0xE0 - 0xE3 Range: Interrupt checking

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| IRF           | 0b 11 10 00 11 | 0bVVVVVVVV |

* IRF: check interrupt flag register agains immediate mask. Sets Z flag if any of the flags of the masked RF register are raised

0xE4 - 0xE7 Range, 2 bytes: Shifting Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| ROL           | 0b 11 10 01 00 | 0b0000RRRR |
| ROR           | 0b 11 10 01 01 | 0b0000RRRR |
| SHL           | 0b 11 10 01 10 | 0b0000RRRR |
| SHR           | 0b 11 10 01 11 | 0b0000RRRR |
| ROL           | 0b 11 10 01 00 | 0bVVVVRR?? |
| ROR           | 0b 11 10 01 01 | 0bVVVVRR?? |
| SHL           | 0b 11 10 01 10 | 0bVVVVRR?? |
| SHR           | 0b 11 10 01 11 | 0bVVVVRR?? |

* ROL, ROR: rotate bits left or right
* SHL, SHR: shift bits left or right
* The behaviour is defined as for the Arithmetic Instructions

0xE8 - 0xEF Range, 2 bytes: Local Branch Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| CLI           | 0b 11 10 10 00 | 0bVVV????? |
| JMI           | 0b 11 10 10 01 | 0bVVVVVVVV |
| JZI           | 0b 11 10 10 10 | 0bVVVVVVVV |
| JNI           | 0b 11 10 10 11 | 0bVVVVVVVV |
| JLI           | 0b 11 10 11 00 | 0bVVVVVVVV |
| JGI           | 0b 11 10 11 01 | 0bVVVVVVVV |
| JOI           | 0b 11 10 11 10 | 0bVVVVVVVV |
| JUI           | 0b 11 10 11 11 | 0bVVVVVVVV |

* CLI: clears flags according to mask from second byte (order is ZCS, I flag cannot be cleared)
* Others: immediate versions of jump instructions

0xF0 - 0xF3 Range, 2 bytes: Indexed Memory Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| LDI           | 0b 11 11 00 00 | 0bVVVVRRRR |
| STI           | 0b 11 11 00 01 | 0bVVVVRRRR |
| LRI           | 0b 11 11 00 10 | 0bVVRRRRRR |
| SRI           | 0b 11 11 00 11 | 0bVVRRRRRR |

* LDI: The value from address LSB register + immediate value is loaded in MSB register
* STI: The value from LSB register is stored at location read from MSB register + immediate value
* LRI: The value located at address LSB register + middle register + immediate value is loaded into MSB register
* SRI: The value from LSB register is stored at address middle register + MSB register + immediate value

0xF4 - 0xF7 Range, 3 bytes: Immediate Memory Instructions

| Instruction   | Code (1st B)   | 2nd & 3rd | Range          |
|---------------|----------------|-----------|----------------|
| MVI           | 0b 11 11 01 RR | 0xVV 0xVV | 0xF4 - 0xF7    |

* MVI: stores the immediate value into register RR

0xF8 - 0xFF Range, single byte: Incrementation Instructions

| Instruction   | Code           |
|---------------|----------------|
| INC           | 0b 11 11 10 RR |
| DEC           | 0b 11 11 11 RR |

* atomic incrementation and decrementation instructions applied to register

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

.int                    ; handle interrupts
    IRF 80h             ; if CPU was just reset
    MVI AX, main:       ; start with main
    JNZ AX

    IRF 40h             ; external interrupt
    JIZ refresh:
    RET                 ; return to where I was. Registers are restored
refresh:
    HLT                 ; else return to sleep

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
12 sHello       'Hello, you!' 10
8   aWords      1 64 256 1024
4   aChars      01h 10h AAh FFh
6   aHexWords   0001h 0010h FFFFh
