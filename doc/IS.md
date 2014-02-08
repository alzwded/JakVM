Registers
=========

| Name              | Description                     | AD | Width |
|-------------------|---------------------------------|----|-------|
| AX                | Accumulator                     | 00 |  16b  |
| BX                | Accumulator                     | 01 |  16b  |
| CX                | Accumulator                     | 10 |  16b  |
| SP                | Stack pointer                   | 11 |  16b  |
| PC                | Program counter (N/A)           | -- |  16b  |
| RS                | Status flags (c.f. Flags) (N/A) | -- |   8b  |
| RF                | Interrupt flags (N/A)           | -- |   8b  |
| IS                | Interrupt state                 | -- | 104b  |

* AX, BX, SP and CX can be used as general purpose register
* AX and BX are implicit in the CMP instruction
* SP is implicit in the PUS and POP instructions. PUS auto post-decrements SP, POP auto pre-increments SI.
* PC cannot be read. It can only be modified via the Jxx instructions
* RS and RF may be implemented in the same register. RS can only be read by the Jxx instructions. RF can only be read via the IRF instruction with a mask.
* RS is modified by various instructions (Arithmetic, Bitwise, IRF, CLI...)
* RF is modified by external forces and is cleared by the RET instruction or by a call to ENI
* the IS register contains the data from all of the registers (sans RF). This is used to restore data via the RET instruction after an interrupt
* the IS register is filled with data whenever an interrupt occurs. It is essentially a shadow of all of the other registers
* the size of the IS register is 16 * 4 (AX,BX,CX,SP) + 16 (PC) + 16 (RS, RF) + 8 (return address) bits = 104 bits

Flags
=====

c.f. FLAGS.md

Instruction set
===============

Op codes
--------

* RR means register selector (2 bits) (see Registers)
* VV means any value (imediate values)
* ?? means value is ignored; it should be kept 00 because it might be used in the future

Everything is (should be?) big endian.

0x00 - 0x7F Range, single byte: Branch&Control and Memory instructions

| Instruction | Code           | Range        | Notes          |
|-------------|----------------|--------------|----------------|
| NOP         | 0b 00 00 00 00 |              |                |
| INT         | 0b 00 00 00 01 |              |                |
| ENI         | 0b 00 00 00 10 |              |                |
| DEI         | 0b 00 00 00 11 |              |                |
| RST         | 0b 00 00 01 00 |              |                |
| HLT         | 0b 00 00 01 01 |              |                |
| SWP         | 0b 00 00 01 10 |              |                |
| reserved    | 0b 00 00 01 11 |              |                |
| CMP         | 0b 00 00 10 00 |              |                |
| RET         | 0b 00 00 10 01 |              |                |
| CMU         | 0b 00 00 10 10 |              |                |
| reserved    | 0b 00 00 10 11 |              |                |
| reserved    | 0b 00 00 11 ?? | 0x0C - 0x0F  |                |
| PUS         | 0b 00 01 00 RR | 0x10 - 0x13  |                |
| POP         | 0b 00 01 01 RR | 0x14 - 0x17  |                |
| reserved    | 0b 00 01 1? ?? | 0x18 - 0x1E  |                |
| LJP         | 0b 00 01 11 11 | 0x1F         | 3B jmp         |
| JMR         | 0b 00 10 01 RR | 0x20 - 0x3F  |                |
| JZR         | 0b 00 10 10 RR | 0x20 - 0x3F  |                |
| JNR         | 0b 00 10 11 RR | 0x20 - 0x3F  |                |
| JLR         | 0b 00 11 00 RR | 0x20 - 0x3F  |                |
| JGR         | 0b 00 11 01 RR | 0x20 - 0x3F  |                |
| JOR         | 0b 00 11 10 RR | 0x20 - 0x3F  |                |
| JUR         | 0b 00 11 11 RR | 0x20 - 0x3F  |                |
| reserved    | 0b 01 00 ?? ?? | 0x40 - 0x4F  |                |
| MOV         | 0b 01 01 RR RR | 0x50 - 0x5F  |                |
| LOD         | 0b 01 10 RR RR | 0x60 - 0x6F  |                |
| STO         | 0b 01 11 RR RR | 0x70 - 0x7F  |                |

* NOP: does nothing
* INT: software interrupt
* ENI: enable interrupts. Resets interrupt flags
* DEI: disable interrupts
* RST: reset
* HLT: sleep (stop clock). Clears RF register
* SWP: swap video buffers
* CMP: compare AX and BX and set Z flag if they are identical, set the S flag if AX is smaller than BX, signed
* RET: return from interrupt handler. Clears RF register.
* CMU: same as CMP, but unsigned
* PUS: stack push
* POP: stack pop
* LJP: long jump. It is a three byte instruction that receives an absolute address.
* JMR: unconditional branching with offset.
* JZR: branch if Z flag is set
* JLR: jump if S flag is set
* JOR: jump if C flag is set
* JNR, JGR, JUR: negated versions
* MOV: move data from one register to another
* LOD: load data from address taken from LSB register to MSB register, high byte
* STO: store data from LSB register to address taken from MSB register, high byte

0x80 - 0x87 Range, 2 bytes: Arithmetic Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| ADD           | 0b 10 00 00 00 | 0b0000RRRR |
| SUB           | 0b 10 00 00 01 | 0b0000RRRR |
| MUL           | 0b 10 00 00 10 | 0b0000RRRR |
| DIV           | 0b 10 00 00 11 | 0b0000RRRR |
| MOD           | 0b 10 00 01 00 | 0b0000RRRR |
| ADD           | 0b 10 00 00 00 | 0bVVVVRR?? |
| SUB           | 0b 10 00 00 01 | 0bVVVVRR?? |
| MUL           | 0b 10 00 00 10 | 0bVVVVRR?? |
| DIV           | 0b 10 00 00 11 | 0bVVVVRR?? |
| MOD           | 0b 10 00 01 00 | 0bVVVVRR?? |
| reserved      | 0b 10 00 01 01 |            |
| reserved      | 0b 10 00 01 1? |            |

* First set add value from LSB register to MSB register (VVVV=0)
* Second set add imediate value to register (VVVV!=0)


0x88 - 0xBF Range, single byte: Bitwise Instructions

| Instruction   | Code                | Range       |
|---------------|---------------------|-------------|
| NEG           | 0b 10 00 10 RR      | 0x88 - 0x8B |
| NOT           | 0b 10 00 11 RR      | 0x8C - 0x8F |
| AND           | 0b 10 01 RR RR      | 0x90 - 0x9F |
| IOR           | 0b 10 10 RR RR      | 0xA0 - 0xAF |
| XOR           | 0b 10 11 RR RR      | 0xB0 - 0xBF |

* NEG: change sign / 2's complement
* NOT: bitwise not
* AND: bitwise AND
* IOR: bitwsie inclusive or
* XOR: bitwise exclusive or

0xC0 - 0xDB Range, 3 bytes: subroutine instructions

| Instruction   | Code           | 2nd & 3rd  |
|---------------|----------------|------------|
| RCL           | 0b 11 00 VV VV |            |
| CAL           | 0b 11 01 00 00 | 0xADDR     |
| RCR           | 0b 11 01 01 RR |            |
| CAR           | 0b 11 01 10 RR | 0xADDR     |

* CAL: pushes PC+3 on the stack and long jumps to ADDR, 3 bytes
* RCL: pops V values off the stack, then pops another value and sets PC to that, one byte
* CAR: stores PC+3 and SP at the address pointed to by register R and long-jumps to ADDR, three bytes
* RCR: it restores PC and SP from the address pointed to by register R, and then continues, one byte

0xDC - 0xDD Range, 2 bytes: shift with carry instructions
| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| SCL           | 0b 11 01 11 00 | 0b0000RRRR |
| SCR           | 0b 11 01 11 01 | 0b0000RRRR |
| SCL           | 0b 11 01 11 00 | 0bVVVVRR?? |
| SCR           | 0b 11 01 11 01 | 0bVVVVRR?? |

* SCL: Shift Carry Left -- the bits that are shifted in have the value of the carry flag. These instructions set the carry flag.
* SCR: Shift 2 Right -- the bits that are shifted in have the value of the carry flag. These instructions set the carry flag.

0xDE - 0xDF Range: Interrupt checking

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| reserved      | 0b 11 01 11 10 |            |
| IRF           | 0b 11 01 11 11 | 0bVVVVVVVV |

* IRF: check interrupt flag register agains immediate mask. Sets Z flag if any of the flags of the masked RF register are raised

0xE0 - 0xE3 Range: Load Absolute Address, 3 bytes
| Instruction   | Code           | 2nd & 3rd  |
|---------------|----------------|------------|
| LAA           | 0b 11 10 00 RR | 0xADDR     |

* LAA: loads the value from an absolute address into register R, high byte

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
* SHL: shift bits left
* SHR: shift bits right. The MSBs that are shifted in have the value of the Sign flag
* The behaviour is defined as for the Arithmetic Instructions

0xE8 - 0xEF Range, 2 bytes: Local Branch Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| CLI           | 0b 11 10 10 00 | 0bVVV????? |
| JMP           | 0b 11 10 10 01 | 0bVVVVVVVV |
| JIZ           | 0b 11 10 10 10 | 0bVVVVVVVV |
| JNZ           | 0b 11 10 10 11 | 0bVVVVVVVV |
| JLT           | 0b 11 10 11 00 | 0bVVVVVVVV |
| JGE           | 0b 11 10 11 01 | 0bVVVVVVVV |
| JOF           | 0b 11 10 11 10 | 0bVVVVVVVV |
| JNF           | 0b 11 10 11 11 | 0bVVVVVVVV |

* CLI: clears flags according to mask from second byte (order is ZCS~~, I flag cannot be cleared~~)
* Others: immediate versions of jump instructions

0xF0 - 0xF3 Range, 2 bytes: Indexed Memory Instructions

| Instruction   | Code           | 2nd B      |
|---------------|----------------|------------|
| LDI           | 0b 11 11 00 00 | 0bVVVVRRRR |
| STI           | 0b 11 11 00 01 | 0bVVVVRRRR |
| LRI           | 0b 11 11 00 10 | 0bVVRRRRRR |
| SRI           | 0b 11 11 00 11 | 0bVVRRRRRR |

* LDI: The value from address LSB register + immediate value is loaded in MSB register, high byte
* STI: The value from LSB register is stored at location read from MSB register + immediate value, high byte
* LRI: The value located at address middle register + LSB register + immediate value is loaded into MSB register, high byte
* SRI: The value from LSB register is stored at address middle register + MSB register + immediate value, high byte

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

Notes on each instruction
=========================

1. NOP
    * utility: high, maybe padding code a lot since it is NUL
    * usage: NOP ; pad code
    * operands: none
2. INT
    * utility: high, inter routine communication, context switching, minimal operating system support etc.
    * usage: MVI AX, 03 MVI BX, 0FFFFh INT ; invoke syscall 3 with parameter 65535
    * operands: none
3. ENI
    * utility: medium, see DEI
    * usage: ENI
    * operands: none
4. DEI
    * utility: medium. Could be useful to disable interrupts while processing some interesting things, I guess.
    * usage: DEI
    * operands: none
5. RST
    * utility: high, there needs to be an instruction to reset the software. Then again, you can just MVI AX, 0 JMR AX or LJP 0 or something like that
    * usage: RST
    * operands: none
6. HLT
    * utility: ultra, there needs to be a way to make the processor sleep, to be awakened by interrupts. This is the only way
    * usage: HLT
    * operands: none
7. SWP
    * utility: ultra, display output; it swaps buffers
    * usage: SWP ; done filling in display buffer, now display it
    * operands: none
8. CMP
    * utility: high, single instruction to get both LT/GE AND equality checking. Otherwise, I guess a SUB would do but that's 2 bytes wide
    * usage: LOD AX, 073C MVI BX, 12 CMP JGE :label:
    * operands: implicit AX and BX
9. RET
    * utility: ultra, since we have an interrupt routine, might as well allow returning from it (or not)
    * usage: RET ; in .int section, preferably
    * operands: none
10. CMU
    * utility: not so much, but it reduces the number of instructions and reduces the number of polluted registers to do an unsigned comparisson (i.e. where 0FFFFh is greater than 07FFFh
    * usage: CMU
    * operands: implicit AX and BX
11. PUS
    * utility: ultra, stack manipulation
    * usage: PUS AX ; push AX onto stack
    * operands: reg, implicit SP
12. POP
    * utility: ultra, stack manipulation
    * usage: POP AX ; pop into AX
    * operands: reg, implicit SP
13. LJP
    * utility: ultra, jump to absolute address
    * usage: LJP :some far away label:
    * operands: imm/16
14. JMR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
15. JZR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
16. JNR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
17. JLR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
18. JGR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
19. JOR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
20. JUR
    * utility: ultra, jump to computed address
    * usage: SUB AX, BX LOD AX, AX JMR AX
    * operands: reg
21. MOV
    * utility: ultra
    * usage: MOV AX, BX ; copy register BX contents into AX
    * operands: reg, reg
22. LOD
    * utility: ultra
    * usage: LOD AX, AX ; dereference address stored in AX into the same register
    * operands: reg, reg
23. STO
    * utility: ultra
    * usage: STO AX, BX
    * opreands: reg, reg
24. NEG
    * utility: ultra
    * usage: NEG AX
    * operands: reg
25. NOT
    * utility: ultra
    * usage: NOT AX
    * operands: reg
26. AND
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg
27. IOR
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg
28. XOR
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg
29. Arithmetic/bitwise
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg || reg, imm/4
30. Arithmetic/bitwise
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg || reg, imm/4
31. Arithmetic/bitwise
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg || reg, imm/4
32. Arithmetic/bitwise
    * utility: ultra
    * usage: NEG AX
    * operands: reg, reg || reg, imm/4
33. CAL
    * utility: ultra, procedural call
    * usage: CAL :memcpy:
    * operands: imm/16
34. RCL
    * utility: ultra, procedural call
    * usage: RCL 4 ; pop 4 bytes off the stack and return
    * operands: imm/4
35. CAR
    * utility: high, ability to save calling context to a predefined address
    * usage: CAR AX, :myfunc: ; store context at address specified in AX
    * operands: reg, imm/16
36. RCR
    * utility: high, see CAR
    * usage: RCR AX ; restore context from address in AX
    * operands: reg
37. SCL
    * utility: high, shift operation with carry
    * usage: SCL AX, 3
    * operands: reg, reg || reg, imm/4
38. SCR
    * utility: high, shift operation with carry
    * usage: SCR AX, 3
    * operands: reg, reg || reg, imm/4
39. IRF
    * utility: high, check which interrupts happened
    * usage: IRF 080h
    * operands: imm/8
40. LAA
    * utility: high, reduces register pollution when working with global data
    * usage: LAA AX, (data statement)
    * operands: reg, imm/16
41. ROL
    * utility: ultra, roll operation
    * usage: ROL AX, 8 ; swap high and low bytes
    * operands: reg, reg || reg, imm/4
42. ROR
    * utility: ultra, roll operation
    * usage: ROR AX, 8 ; swap high and low bytes
    * operands: reg, reg || reg, imm/4
43. SHL
    * utility: ultra
    * usage: SHL AX, 4 ; multiply by 16
    * operands: reg, reg || reg, imm/4
44. SHR
    * utility: ultra
    * usage: SHR AX, 2 ; divide by 4
    * operands: reg, reg || reg, imm/4
45. CLI
    * utility: high, clear flags
    * usage: CLI 0FFh
    * operands: imm/8
46. JMP
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
47. JIZ
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
48. JNZ
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
49. JLT
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
50. JGE
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
51. JOF
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
52. JNF
    * utility: ultra, jump to address relative to current PC
    * usage: JMP :label:
    * operands: imm/8
53. LDI
    * utility: high, indexed addressing, useful in procedure calling and local variables and whatnot
    * usage: LDI AX, SP, 15 SHR AX, 8 LDI AX, SP, 14 ; load into AX the 15th and 16th bytes up (i.e. the 8th word)
    * operands: reg, reg, imm/4
54. STI
    * utility: high, indexed addressing, see LDI
    * usage: STI SP, AX, 14 SHL AX, 8 STI SP, AX, 15 ; store AX as 8th word up the stack
    * operands: reg, reg, imm/4
55. LRI
    * utility: medium, triple indexed address, useful for functions with a bazillion parameters but reduces the number of available registers
    * usage: LRI AX, SP, CX, 3 ; load into AX the address SP + CX + 3
    * operands: reg, reg, reg, imm/2
56. STI
    * utility: medium, see LRI
    * usage: SRI SP, CX, AX, 3 ; store AX high byte at address SP + CX + 3
    * operands: reg, reg, reg, imm/2
57. MVI
    * utility: ultra, the only non-convoluted & hackish way to load constants
    * usage: MVI AX, 3 MVI BX, @constant ; load 3 into AX and load the address of constant from the data section into bx
    * operands: imm/16
58. INC
    * utility: ultra
    * usage: INC SP ; pop value off of the stack without polluting registers
    * operands: reg
59. DEC
    * utility: ultra
    * usage: DEC CX ; decrease counter
    * operands: reg

Number of ultra needed instructions: 41
Number of highly needed instructions: 13
Number of not so needed instructions: 5

These instructions are very important:
* NOP
* INT
* RST
* CMP
* CAR
* RCR
* SCL
* SCR
* IRF
* LAA
* CLI
* LDI
* STI

The not so needed instructions:
* ENI   -- interrupts can be re-enabled by changing the byte at location 0 to NOP
* DEI   -- interrupts can be disabled by replacing the byte at location 0 with RET
* CMU   -- but how useful is this really?
* LRI   -- you could just increment the base register...
* STI   -- idem...
