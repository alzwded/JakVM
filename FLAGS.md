Status flags
============

| Name          | Description       |
|---------------|-------------------|
| Z             | Zero flag         |
| C             | Overflow/Carry    |
| S             | Sign              |

The layout is: `Z``C``S``0``0``0``0``0`

Interrupt flags
===============

| Name          | Description        |
|---------------|--------------------|
| RS            | Reset              |
| XT            | External interrupt |
| BS            | Buffer swapped     |

The layout is: `RS``XT``BS``0``0``0``0``0`

These flags are all set by external events. If none of them are set, it was a software interrupt (INT instruction).

Instructions that set or reset flags
====================================

The CLI instruction can reset flags arbitrarily.

Flag Z
------

All instructions that operate on a register set the Zero flag whenever the register they operate on becomes Zero as a result of the operation. Jump instructions do not modify the Zero flag. Notes are listed below. These notes present peculiar behaviour. Also, any instructions that modify the Zero flag can reset it in the complementary situation. (e.g. LOD sets the Z flag when it loads a zero, but it resets it otherwise; basically you can add ", and it resets it otherwise" after any instruction when it comes to the Z flag)

| Instruction   | Behaviour                                             |
|---------------|-------------------------------------------------------|
| LOD           | Sets the Zero flag when it loads a zero into a Reg    |
| STO           | Sets the Zero flag when it stores a zero byte         |
| CMP           | Sets the Zero flag when AX == BX                      |
| CMU           | Sets the Zero flag when AX == BX                      |
| PUS           | Sets the Zero flag when the pushed register is 0      |
| POP           | Sets the Zero flag when the pushed register is 0      |
| LDI, LRI      | Sets the Zero flag when it loads a zero               |
| STI, SRI      | Sets the Zero flag when it stores a 0 value           |

The other instructions that modify the Z flag: MOV, NEG, NOT, AND, IOR, XOR, ADD, SUB, MUL, DIV, SCR, SCL, ROL, ROR, SHL, SHR, CLI, LDI, INC, DEC

Flag S
------

The behaviour is the same as for the Z flag, but it is set (or reset) in relation to the value of the most significant bit being set rather than the entire word being zero. E.g. if LOD loads the number 080h in the high byte of register AX, it will set the sign flag. If LDI loads the value 003Fh into register CX, the S flag will be reset.

A special note, CMP and CMU set the S flag if the value contained in the AX register is methematically smaller (signed respectively unsigned) than the value in the BX register. (and it resets it otherwise)

Flag C
------

| Instruction   | Behaviour                                             |
|---------------|-------------------------------------------------------|
| ADD, SUB, MUL, DIV | Set or reset the carry flag on over- or underflow |
| SCL, SCR | Set or reset the carry flag based on the value of the last out shifted bit |
| ROL, ROR | Use the carry flag to transport bits around, so the value will be set to whatever the last rolled bit was |
| INC | Sets the C flag on overflow, resets it otherwise |
| DEC | Sets the C flag on underflow, resets it otherwise |
