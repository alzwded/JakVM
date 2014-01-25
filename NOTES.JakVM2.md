Registers
=========

| Name              | Description                     | AD |
|-------------------|---------------------------------|----|
| AX                | Accumulator                     | 00 |
| BX                | Accumulator                     | 01 |
| SP                | Stack pointer                   | 11 |
| SI                | Stack index                     | 10 |
| PC                | Program counter (N/A)           | -- |
| RS                | Status flags (c.f. Flags) (N/A) | -- |

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

Op codes
--------

* RR means register selector (2 bits) (see Registers)
* VV means any value (imediate values)
* ?? means value is ignored; it should be kept 00 because it might be used in the future

Everything is (should be?) big endian.

0x00 - 0x7F Range, single byte: Branch&Control and Memory instructions

| Instruction | Code           | Range        |
|-------------|----------------|--------------|
| NOP         | 0b 00 00 00 00 | 0x00 - 0x07  |
| INT         | 0b 00 00 00 01 | 0x00 - 0x07  |
| ENI         | 0b 00 00 00 10 | 0x00 - 0x07  |
| DEI         | 0b 00 00 00 11 | 0x00 - 0x07  |
| RST         | 0b 00 00 01 00 | 0x00 - 0x07  |
| HLT         | 0b 00 00 01 01 | 0x00 - 0x07  |
| reserved    | 0b 00 00 1? ?? | 0x08 - 0x0F  |
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
| reserved    | 0b 01 00 ?? ?? | 0x20 - 0x3F  |
| MOV         | 0b 01 01 RR RR | 0x40 - 0x7F  |
| LOD         | 0b 01 10 RR RR | 0x40 - 0x7F  |
| STO         | 0b 01 11 RR RR | 0x40 - 0x7F  |

* NOP: does nothing
* INT: software interrupt
* ENI: enable interrupts
* DEI: disable interrupts
* RST: reset
* HLT: sleep (stop clock)
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

0xD0 - 0xE3 Range: reserved

| Instruction   | Code           |
|---------------|----------------|
| reserved      | 0b 11 01 ?? ?? |
| reserved      | 0b 11 10 00 ?? |

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
| MVI           | 0b 11 11 01 RR | 0xVV 0xVV | 0xF0 - 0xF8    |

* MVI: stores the immediate value into register RR

0xF8 - 0xFF Range, single byte: Incrementation Instructions

| Instruction   | Code           |
|---------------|----------------|
| INC           | 0b 11 11 10 RR |
| DEC           | 0b 11 11 11 RR |

* atomic incrementation and decrementation instructions applied to register
