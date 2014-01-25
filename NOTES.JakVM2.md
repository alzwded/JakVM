Registers
=========

| Name              | Description                     | AD |
|-------------------|---------------------------------|----|
| AX                | Accumulator                     | 00 |
| BX                | Accumulator                     | 01 |
| SP                | Stack pointer                   | 11 |
| SI                | Stack index                     | 10 |
| PC                | Program counter (N/A)           |    |
| RS                | Status flags (c.f. Flags) (N/A) |    |

Flags
=====

| Name          | Description       |
|---------------|-------------------|
| Z             | Zero flag         |
| C             | Overflow/Carry    |
| S             | Sign              |
| I             | Interrupt request |

Instruction set
===============

Memory
------

| Mnemonic          | Operands          | Description       |
|-------------------|-------------------|-------------------|
| MOV               | Reg, Imed         | Set Imed to Reg   |
| MOV               | Reg, Reg          | Copy Reg to Reg   |
| LOD               | Reg, Reg          | Load @Reg to Reg  |
| STO               | Reg, Reg          | Store Reg to @Reg |
| PUS               | Reg               | Push Reg on Stack; SI is decremented, Address is SP-SI |
| POP               | Reg               | Pop stack to Reg  |
| LDI               | Reg, Reg          | Reg = @(Reg + Reg)|

Branching & Control
-------------------

| Mnemonic          | Operands          | Description       |
|-------------------|-------------------|-------------------|
| JMP               | Imed              | PC += Imed        |
| JMP               | Reg               | PC := Reg         |
| JIZ               | Imed              | if RS.Z           |
| JIZ               | Reg               | if RS.Z           |
| JNZ               | Imed              | if !RS.Z          |
| JNZ               | Reg               | if !RS.Z          |
| JGE               | Imed              | if !RS.S          |
| JGE               | Reg               | if !RS.S          |
| JLT               | Imed              | if RS.S           |
| JLT               | Reg               | if RS.S           |
| JOF               | Imed              | if RS.C           |
| JOF               | Reg               | if RS.C           |
| JNF               | Imed              | if !RS.C          |
| JNF               | Reg               | if !RS.C          |
| INT               | -                 | software interrupt|
| ENI               | -                 | enable interrupts |
| DEI               | -                 | disable ints      |
| RST               | -                 | reset             |
| HLT               | -                 | halt              |
| CLI               | Imed              | clear flags (mask)|
| NOP               | -                 | no operation      |

Arithmetic
----------

| Mnemonic          | Operands          | Description       |
|-------------------|-------------------|-------------------|
| INC               | Reg, Imed         | Increment R with I|
| ADD               | Reg, Reg          |                   |
| DEC               | Reg, Imed         | Decrement R w/ I  |
| SUB               | Reg, Reg          |                   |
| MUL               | Reg, Reg          |                   |
| SHL               | Reg, Imed         | Shift left        |
| ROL               | Reg, Reg          | Rotate left       |
| DIV               | Reg, Reg          |                   |
| SHR               | Reg, Imed         | Shift right       |
| ROR               | Reg, Reg          | Rotate right      |

Tally
-----

* Void instructions: ENI, DEI, RST, HLT, INT, NOP
  - count: 6
  - extra bits: 0
* Instructions with imediate values: JMP(I), JIZ(I), JNZ(I), JGE(I), JLT(I), JOF(I), JNF(I), CLI
  - count: 8
  - extra bits: 16
* Instructions with one register: PUS, POP, JMP, JIZ, JNZ, JGE, JLT, JOF, JNF
  - count: 9
  - extra bits: 2
* Instructions with two registers: MOV, LOD, STO, LDI, ADD, SUB, MUL, ROL, DIV, ROR
  - count: 10
  - extra bits: 4
* Instructions with register and imediate: MOV(I), INC, DEC, SHL, SHR
  - count: 5
  - extra bits: 18

Theories
--------

Group VI with IWOR with IWTR as single byte; total Is: 25 ;
Group others as 3 byte instructions; total Is: 13 ;

RR means register selector (2 bits) (see Registers)
VV means any value (imediate values)
?? means value is ignored

Everything is (should be?) big endian.

0x00 - 0xDF Range, single byte:

| Instruction   | Code                | Range              |
|---------------|---------------------|--------------------|
| NOP           | 0b 00 00 00 00      |  0x00 - 0x07       |
| INT           | 0b 00 00 00 01      |  0x00 - 0x07       |
| ENI           | 0b 00 00 00 10      |  0x00 - 0x07       |
| DEI           | 0b 00 00 00 11      |  0x00 - 0x07       |
| RST           | 0b 00 00 01 00      |  0x00 - 0x07       |
| HLT           | 0b 00 00 01 01      |  0x00 - 0x07       |
| JMPR          | 0b 00 10 01 RR      |  0x20 - 0x3F       |
| JIZR          | 0b 00 10 10 RR      |  0x20 - 0x3F       |
| JNZR          | 0b 00 10 11 RR      |  0x20 - 0x3F       |
| JLTR          | 0b 00 11 00 RR      |  0x20 - 0x3F       |
| JGER          | 0b 00 11 01 RR      |  0x20 - 0x3F       |
| JOFR          | 0b 00 11 10 RR      |  0x20 - 0x3F       |
| JNFR          | 0b 00 11 11 RR      |  0x20 - 0x3F       |
| LDI           | 0b 00 10 00 RR      |  0x20 - 0x3F       |
| PUS           | 0b 01 00 00 RR      |  0x40 - 0x7F       |
| POP           | 0b 01 00 01 RR      |  0x40 - 0x7F       |
| MOVR          | 0b 01 01 RR RR      |  0x40 - 0x7F       |
| LOD           | 0b 01 10 RR RR      |  0x40 - 0x7F       |
| STO           | 0b 01 11 RR RR      |  0x40 - 0x7F       |
| ADD           | 0b 10 00 RR RR      |  0x80 - 0xBF       |
| SUB           | 0b 10 01 RR RR      |  0x80 - 0xBF       |
| MUL           | 0b 10 10 RR RR      |  0x80 - 0xBF       |
| DIV           | 0b 10 11 RR RR      |  0x80 - 0xBF       |
| ROL           | 0b 11 00 RR RR      |  0xC0 - 0xDF       |
| ROR           | 0b 11 01 RR RR      |  0xC0 - 0xDF       |

0xE0 - 0xFF Range, 3 bytes:

| Instruction   | Code (1st B)   | 2nd & 3rd | Range          |
|---------------|----------------|-----------|----------------|
| INC           | 0b 11 10 00 RR | 0xVV 0xVV | 0xE0 - 0xE3    |
| DEC           | 0b 11 10 01 RR | 0xVV 0xVV | 0xE4 - 0xE7    |
| SHL           | 0b 11 10 10 RR | 0xVV 0xVV | 0xE8 - 0xEB    |
| SHR           | 0b 11 10 11 RR | 0xVV 0xVV | 0xEC - 0xEF    |
| CLI           | 0b 11 11 00 00 | 0xVV 0x?? | 0xF0 - 0xF7    |
| JMPI          | 0b 11 11 00 01 | 0xVV 0xVV | 0xF0 - 0xF7    |
| JIZI          | 0b 11 11 00 10 | 0xVV 0xVV | 0xF0 - 0xF7    |
| JNZI          | 0b 11 11 00 11 | 0xVV 0xVV | 0xF0 - 0xF7    |
| JLTI          | 0b 11 11 01 00 | 0xVV 0xVV | 0xF0 - 0xF7    |
| JGEI          | 0b 11 11 01 01 | 0xVV 0xVV | 0xF0 - 0xF7    |
| JOFI          | 0b 11 11 01 10 | 0xVV 0xVV | 0xF0 - 0xF7    |
| JNFI          | 0b 11 11 01 11 | 0xVV 0xVV | 0xF0 - 0xF7    |
| MOVI          | 0b 11 11 10 RR | 0xVV 0xVV | 0xF8 - 0xFB    |
