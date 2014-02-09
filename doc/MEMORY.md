Memory layout
=============

This describes the memory layout of my dream machine.

There are 64k addresses. They are split into a number of groups:

| description   | ranges          | size  | KiB |
|---------------|-----------------|-------|-----|
| Code and data | 0x0000 - 0xBFFF | 49152 | 48k |
| Display I/O   | 0xC000 - 0xEFFF | 12288 | 12k |
| Ext. Memory   | 0xF000 - 0xFBFF |  3072 |  3k |
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

External Memory
===============

| description           | range             | size  | count          |
|-----------------------|-------------------|-------|----------------|
| Read-only data        | 0xF000 - 0xF7FF   | 2048B | 256 * 2k  max  |
| R/O data bank selector| 0xF800            |    1B | = up to 512k   |
| reserved              | 0xF801 - 0xF9FF   |  511B |                |
| R/W data              | 0xFA00 - 0xFAFF   |  256B | 256 * 256 max  |
| R/W data bank selector| 0xFB00            |    1B | = up to 64k    |
| reserved              | 0xFB01 - 0xFBFF   |  255B |                |

External memory is mapped into 0xF000 - 0xF7FF (2048 blocks) split in banks selectable by writing to 0xF800 which selects the bank. The bank needs to be available, if not, it is underfined what happens, you will probably read all 0s.

Writing to 0xF000-0xF7FF does nothing. Or rather, it should, because of the way the emu is (read: will be) implemented that might actually screw up your data until you bank switch again `:D`

Example:
```
.code
    MVI BX, 0F800h
; select bank 1
    MVI AX, 0100h
    STO BX, AX
    NOP
; read some data
    MVI AX, 08000h
    MVI BX, 0F000h
    MVI CX, 0800h
    CAL :memcpy:
; let's assume that was code, now run it!
    LJP 08000h
```

Persistent external memory is accessed by selecting the bank in 0xFB00. Memory is accessed via 0xFA00 - 0xFAFF.

Example:
```
.code
    MVI BX, 0F800h
; select bank 1
    MVI AX, 0100h
    STO BX, AX
    NOP
; read some persisted data
    LAA AX, 0FA00h
; apply some operation to it
    MVI CX, 0AAAAh
    XOR AX, CX
; persist the value
    MVI BX, 0FA00h
    STO BX, AX
```

Accomplishing this
------------------

Apparently memcpy(2048) is fast enough 200000 times a 17ms cycle.

Use `mmap` everywhere, with `MAP_PRIVATE` for the read only stuff and `MAP_SHARED` for the persistent stuff.

Now, the question is how often to we update? The first solution that comes to mind is to "update on address change" whenever the addresses at 0xF800 and 0xFB00 change.

For example, if 0xFB00 was 0x0001, changes were written to 0xFA00:0xFA80, then the bank is switched, the range is copied back over the mmap'd region and msync is called.

If no files are provided for a bank that is selected, just memcpy in some 0s and ignore changes.

Changes to mmap'd files should be resynced every 60 frames to make sure we don't lose data in case of, you know, someone exitting the emulator before the 0xFB00 value is changed.

However, because of mmap overhead (data may not be in memory, read-on-demand, blahblahblah) is might be safer to just fread the R/O data in, and only mmap the R/W data.

TODO To be concluded.

How this maps to the real world
-------------------------------

Well, you'll need to provide some 2k or 256 byte files and pass them as arguments to the emu while noting their bank id's. Something like:

`emu program.bin --ro-bank 1:./data/2kBlock1.img --ro-bank 2:./data/2kBlock2.img --rw-bank 28:./save/save.sav`

General I/O
===========

| description           | range             | size (bytes)      |
|-----------------------|-------------------|-------------------|
| Joypad                | 0xFC00 - 0xFC0F   | 16                |
| Sound                 | 0xFC10 - 0xFC1F   | 16                |
| reserved              | 0xFC20 - 0xFFFF   | 992               |

Joypad
------

| description           | range             |
|-----------------------|-------------------|
| reserved              | 0xFC00            |
| reserved              | 0xFC01            |
| reserved              | 0xFC02            |
| reserved              | 0xFC03            |
| reserved              | 0xFC04            |
| reserved              | 0xFC05            |
| reserved              | 0xFC06            |
| D-pad                 | 0xFC07            |
| ABCDLRStSel           | 0xFC08            |
| reserved              | 0xFC09 - 0xFC0F   |

0xFC07 is the D-pad in the form of `UP``LEFT``DOWN``RIGHT` where each is two bits wide.
0xFC08 are the action buttons, in the order `A``B``C``D``L``R``start``select`.

They are set each frame based on real-world input.

Sound
-----

TODO more on sound, I think there might be a need for freq/ampl/fill vectors or something like that...

| description           | range             |
|-----------------------|-------------------|
| sound ch. 1 freq.     | 0xFC10            |
| sound ch. 1 ampl.     | 0xFC11            |
| sound ch. 1 fill      | 0xFC12            |
| reserved              | 0xFC13            |
| sound ch. 2 freq.     | 0xFC14            |
| sound ch. 2 ampl.     | 0xFC15            |
| sound ch. 2 fill      | 0xFC16            |
| reserved              | 0xFC17            |
| sound ch. 3 freq.     | 0xFC18            |
| sound ch. 3 ampl.     | 0xFC19            |
| sound ch. 3 fill      | 0xFC1A            |
| reserved              | 0xFC1B            |
| sound ch. 4 freq.     | 0xFC1C            |
| sound ch. 4 ampl.     | 0xFC1D            |
| sound ch. 5 freq.     | 0xFC1E            |
| sound ch. 5 ampl.     | 0xFC1F            |

~~Channels 1 and 2 are square waves. Channel 3 is triangle/sawtooth (the fill factor set to 128 makes it triangle, setting it to 255 makes it sawtooth). Channels 4 and 5 are sine waves.~~

The audio channels are square waves.

The freq. values are badly named. They are actual notes, with 128 being C4 (middle C), and each increment or decrement marks a quarter-tone above or under C4. For example:

```
.code
; play E5, E above middle C, square
;  note height
    MVI BX, 0FC10h
    MVI AX, 0A000h  ; 160 << 8
;  note intensity
    INC BX
    MVI AX, 08000h  ; meh amplitude
    STO BX, AX
```

The values of values of 0x0 set to any of the fields silence that channel (because an amplitude of 0 means you can't hear it, a frequency of 0 means it doesn't exit, a fill of 0 means you are outputting for 0 seconds (or some other interpretation, it's still a frequency of 0)).

In practice, only C0-B9 are (somewhat) supported. That's 240 values centered in 128, so 8-248 for the freq. fields.

These are read on each frame and the output is adjusted.
