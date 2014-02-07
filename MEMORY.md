Memory layout
=============

This describes the memory layout of my dream machine.

There are 64k addresses. They are split into a number of groups:

| description   | ranges          | size  | KiB |
|---------------|-----------------|-------|-----|
| Code and data | 0x0000 - 0xBFFF | 49152 | 48k |
| Display I/O   | 0xC000 - 0xEFFF | 12288 | 12k |
| IO Ctrl       | 0xF000 - 0xFBFF |  3072 |  3k |
| General I/O   | 0xFC00 - 0xFFFF |  1024 |  1k |

Code and data
=============

Code starts at 0x0000. This is the address where the PC is set after an interrupt.

The stack pointer (SP) by default is initialized to 0xBFFF (stack empty).

Display
=======

| description           | range             | size  | count          |
|-----------------------|-------------------|-------|----------------|
| Block descriptors     | 0xC000 - 0xD3FF   | 5120B | 320 x 16       |
| Pallettes             | 0xD400 - 0xD7FF   | 1024B | 256 x 4        |
| Sprites               | 0xD800 - 0xD7FF   | 2040B | 60 x 34        |
| Background1           | 0xE000 - 0xE7FF   | 2048B | 1024 x 2       |
| Background2           | 0xD800 - 0xEFFF   | 2048B | 1024 x 2       |

The screen background is split into 1024 8x8 blocks (2 indexes). A block index and a pallette index. That's 2048.

A block is 8x8x2 (128 bits, 32 bytes). The 2 bits are indexes in a color pallette. (0..3)

A pallette is 16 bytes (128 bits) and features 4 colours. (colours are 16 bit, so 4x16bits = 64bits = 4 bytes)

Sprites are composed of a screen location (x 1B, y 1B), a pallette index (2B) and a block descriptor (32 bytes). The location refers to their top-left pixel. The size is 34 bytes.

The total size of this block is 12288.
