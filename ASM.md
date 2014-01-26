Assembler specification
=======================

This describes the behaviour of the assembler.

Major Sections
==============

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

These can be multiple. They are appended in the order they appear, noting that the .int section always comes first.

Format:
```
.data ::= INT<size> STR<name> LIST<initializers> ;
```

Data statements can be referenced in .code and .int sections like so:
```
.data
2 w 1024
.code
    MVI AX, $w          ; AX = 1024

    MVI AX, @w          ; AX = addressof(w)
    LOD AX, AX          ; AX = 1024
```

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
