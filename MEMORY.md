Memory layout
=============

This describes the memory layout of my dream machine.

There are 32k addresses. They are split into a number of groups:

| description   | ranges          | size  | KiB |
|---------------|-----------------|-------|-----|
| Code and data | 0x0000 - 0xBFFF | 49152 | 48k |
| Display I/O   | 0xC000 - 0xE7FF | 10240 | 10k |
| reserved      | 0xE800 - 0xEFFF |  2048 |  2k |
| General I/O   | 0xFC00 - 0xFFFF |  1024 |  1k |

Code and data
=============

Code starts at 0x0000. This is the address where the PC is set after an interrupt.

The stack pointer (SP) by default is initialized to 0xBFFF (stack empty).

Display
=======

| description           | range             | size  | count          |
|-----------------------|-------------------|-------|----------------|
| Pallettes             | 0xC000 - 0xCFFF   | 4096B | 128 x 32       |
| Block descriptors     | 0xD000 - 0xD7FF   | 2048B | 64 x 32        |
| Background            | 0xD800 - 0xDFFF   | 2048B | 1024 x 2       |
| Sprites               | 0xE000 - 0xE7FF   | 2040B | 60 x 34        |

The screen background is split into 1024 8x8 blocks (2 indexes). A block index and a pallette index. That's 2048.

A block is 8x8x4 (256 bits, 32 bytes). The 4 bits are indexes in a color pallette.

A pallette is 32 bytes (256 bits) and features 4 colours from a maximum of... 256.

Sprites are composed of a screen location (x 1B, y 1B), a pallette index (2B) and a block descriptor (32 bytes). The location refers to their top-left pixel. The size is 34 bytes.

The total size of this block is 10240.
