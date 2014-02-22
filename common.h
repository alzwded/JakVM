#ifndef COMMON_H
#define COMMON_H

typedef struct {
    unsigned short REGS[4]; // AX, BX, CX, SP
    unsigned char* PC;
    short STATE; // RS:RF
} work_t;

typedef void (*ui_callback_f)(work_t*);

#define GFX_CB 0
#define SFX_CB 1
#define LAST_CB 2
extern void set_callback(unsigned callback, ui_callback_f func, unsigned frameSkip);

extern unsigned gfx_buffer_start;

// memory needs to be 64k large
extern void loop(unsigned char* memory);

#endif
