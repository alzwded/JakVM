Assembler specification
=======================

This describes the behaviour of the assembler.

Major Sections
==============

.const
------

This section is not assembled, but contains constants useful while assembling e.g. offsets, magic numbers. This will not be part of the outputted code. These are accessed using the $ operator.

.int
----

The code that will be aligned to the interrupt handler location (00h). This section gets executed whenever an interrupt occurs. There can only be one .int section.

.code
-----

These can be multiple. They contain executable instructions, executed in order unless an instruction demands an explicit jump. Code sections are appended in the object code in the order they appear, noting that the .int section always comes first.

Can contain labels to assist in coding jump or call instructions.

```
label ::= ":" STR<name> ":" ;
```

Instructions are optionally separated by whitespace.

Comments tart with `;` and last until end of line.

```
comment ::= ";" ANY* EOL ;
```

The same definitions apply to the .int section as well.

In jump or call instructions you can reference labels as your offsets or absolute addresses.

```
    CMP                     ; compare AX and BX
    JIZ :a:                 ; if they are equal jump 7 bytes forward
    MVI CX, :procB:         ; load CX with the location of :procB:
    JMP :call:              ; jump 5 bytes forward
:a:
    MVI CX, :procA:         ; load CX with the location of :procA:
:call:
    CAR CX                  ; call whatever's at the location in CX
```

.data
-----

These can be multiple. They are appended in the order they appear, noting that the .int section always comes first. Addresses of data entries are specified in brackets and shall support simple arithmetics.

Format:
```
.data ::= INT<size> STR<name> LIST<initializers> ;
```

Data statements can be referenced in .code and .int sections like so:
```
.const
magic 1024
.data
2 w 1024
64 asd
.code
    MVI AX, $magic      ; AX = 1024

    MVI AX, (w + 1)     ; AX = addressof(w)
    LOD AX, AX
    SHR AX, 8
    MVI AX, (w)         ; AX = 1024

    LAA AX, (w + 1)
    SHR AX, 8
    LAA AX, (w)         ; AX = 1024

    MVI BX, (asd)
    CAL :computeoffset: ; result stored in AX
    LRI AX, BX, AX      ; AX high byte is loaded with (asd) + value from AX

    MVI BX, (asd)
    CAL :computeoffset: ; result stored in AX
    ADD BX, AX
    LOD AX, BX          ; AX high byte is loaded with (asd) + value from AX
```

You can use the .at directive to set the start address of a block. For example
```
.data
.at 0C00h
4 pallette1 0h, 0A100h, 0FF00h, 03Eh    ; store pallette directly in video memory
```

These .at directives are valid throughout the file. Be careful when using them. They are usually only useful for padding your object code in order to align with the video memory directly (which comes after all of your code anyway).

Assembling
==========

Single file
-----------

If one file is passed as input, the assembler shall work like so:

1. Each section is parsed and hashmap entries are created for all data references, labels and relative or absolute-immediate jump instructinos.
2. The .int section is moved at the start.
3. .code and .data sections are appended to the end of the object. Whenever data or label references become apparent, replace the placeholder hash with the actual byte code.
4. Go over the code again and replace any remaining place holders with their actual value (labels, data references, calls etc).

Multi-file
----------

Multiple .int sections shall result in an error.

Otherwise, the files are concatenated in the order which they are passed and assembly works as per single files.

Constants and .data section
---------------------------

There shall be permitted to write stuff like
```
LAA AX, (const + 1)
SHR AX, 8
LAA AX, (const)
```

This means that at least simple address addition or substraction should be supported.


