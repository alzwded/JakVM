#include <stddef.h>
// base 16 string to int
static inline int matoi16(char const* volatile s)
{
    // make local variables volatile because the output with -O2 or
    //    -O3 was causing strings to become NULL for whatever reason
    // The s pointer is copied for the very same reason
    char const* volatile s1 = s;
    int volatile ret;
    asm volatile (   
            "xorl %%ebx, %%ebx\n\t"    // upper three bytes -> 0
            "xorl %0, %0\n"       // ret = 0

            "matoi16_loop%=:\n\t"     // while(*s1) {
            "movb (%1), %%bl\n\t"
            "cmpb $0, %%bl\n\t"
            "jz matoi16_end%=\n\t"
            "shll $4, %0\n\t"       //      ret <<= 4

            "cmpb $0x61, %%bl\n\t" //      if(*s1 > 'a') goto small_letters
            "jge matoi16_small_letters%=\n\t"

            "cmpb $0x41, %%bl\n\t" //      if(*s1 > 'A') goto big_letters
            "jge matoi16_big_letters%=\n\t"

            "subl $0x30, %%ebx\n" //      ebx = *s1 - '0'
            "matoi16_sum%=:\n\t"
            "addl %%ebx, %0\n\t"    //      ret += ebx
            "inc %1\n\t"            //      s1++
            "jmp matoi16_loop%=\n"

            "matoi16_big_letters%=:\n\t"
            "subl $0x37, %%ebx\n\t" //      ebx = *s1 - 'A'
            "jmp matoi16_sum%=\n"

            "matoi16_small_letters%=:\n\t"
            "subl $0x57, %%ebx\n\t" //      ebx = *s1 - 'a'
            "jmp matoi16_sum%=\n"
            "matoi16_end%=:\n\t"
            : "=&a"(ret)
            : "r"(s1)
            : "%ebx", "cc"
        );
    return ret;
}

// base 8 string to int
static inline int matoi8(char const* volatile s)
{
    // make local variables volatile because the output with -O2 or
    //    -O3 was causing strings to become NULL for whatever reason
    // The s pointer is copied for the very same reason
    char const* volatile s1 = s;
    int volatile ret;
    asm volatile (   
            "xorl %%ebx, %%ebx\n\t"    // upper three bytes -> 0
            "xorl %0, %0\n"       // ret = 0

            "matoi8_loop%=:\n\t"      // while(*s1) {
            "movb (%1), %%bl\n\t"
            "cmpb $0, %%bl\n\t"
            "jz matoi8_end%=\n\t"
            "shll $3, %0\n\t"       //      ret <<= 3

            "subl $0x30, %%ebx\n\t" //      ebx = *s1 - '0'
            "addl %%ebx, %0\n\t"    //      ret += ebx
            "inc %1\n\t"            //      s1++
            "jmp matoi8_loop%=\n"

            "matoi8_end%=:\n\t"
            : "=&a"(ret)             // output ret, W/O random register
            : "r"(s1)               // input s1, R/W random register
            : "%ebx", "cc"           // modifying eAX
        );
    return ret;
}

// base 10 atoi
static inline int matoi10(char const* volatile s)
{
    char const* s1 = s;
    int volatile ret;
    asm volatile(
            "xorl %%edx, %%edx\n\t"    // upper three bytes -> 0
            "xorl %0, %0\n"       // ret = 0

            "matoi10_loop%=:\n\t"      // while(*s1) {
            "movb (%1), %%dl\n\t"
            "cmpb $0, %%dl\n\t"
            "jz matoi10_end%=\n\t"
            "movl %0, %%ebx\n\t"        // ret *= 10
            "shll $1, %%ebx\n\t"
            "shll $3, %0\n\t"
            "addl %%ebx, %0\n\t"

            "subl $0x30, %%edx\n\t" //      edx = *s1 - '0'
            "addl %%edx, %0\n\t"    //      ret += edx
            "inc %1\n\t"            //      s1++
            "jmp matoi10_loop%=\n"

            "matoi10_end%=:\n\t"
            : "=&r"(ret)             // output ret, W/O random register
            : "r"(s1)               // input s1, R/W random register
            : "%edx", "%ebx", "cc"   // modifying eAX, ebx
        );
    return ret;
}

// how to use MUL:
/*
   movl $10, %ebx
   movl $10, %eax
   mul %eax
   ; result is in edx:eax
   ; in this case OF and CF are 0 (no overflow), edx is 0 and eax is 100
*/

// short versions:
// base 8 string to short
static inline short satoi8(char const* volatile s)
{
    // make local variables volatile because the output with -O2 or
    //    -O3 was causing strings to become NULL for whatever reason
    // The s pointer is copied for the very same reason
    char const* volatile s1 = s;
    short volatile ret;
    asm volatile (   
            "xorw %%ax, %%ax\n\t"    // upper three bytes -> 0
            "xorw %0, %0\n"       // ret = 0

            "satoi8_loop%=:\n\t"      // while(*s1) {
            "movb (%1), %%al\n\t"
            "cmpw $0, %%ax\n\t"
            "jz satoi8_end%=\n\t"
            "shlw $3, %0\n\t"       //      ret <<= 3

            "subw $0x30, %%ax\n\t" //      eax = *s1 - '0'
            "addw %%ax, %0\n\t"    //      ret += eax
            "inc %1\n\t"            //      s1++
            "jmp satoi8_loop%=\n"

            "satoi8_end%=:\n\t"
            : "=&r"(ret)             // output ret, W/O random register
            : "r"(s1)               // input s1, R/W random register
            : "%ax", "cc"           // modifying eAX
        );
    return ret;
}
// base 16 string to short
static inline short satoi16(char const* volatile s)
{
    // make local variables volatile because the output with -O2 or
    //    -O3 was causing strings to become NULL for whatever reason
    // The s pointer is copied for the very same reason
    char const* volatile s1 = s;
    short volatile ret;
    asm volatile (   
            "xorw %%ax, %%ax\n\t"    // upper three bytes -> 0
            "xorw %0, %0\n"       // ret = 0

            "satoi16_loop%=:\n\t"     // while(*s1) {
            "movb (%1), %%al\n\t"
            "cmpw $0, %%ax\n\t"
            "jz satoi16_end%=\n\t"
            "shlw $4, %0\n\t"       //      ret <<= 4

            "cmpw $0x61, %%ax\n\t" //      if(*s1 > 'a') goto small_letters
            "jge satoi16_small_letters%=\n\t"

            "cmpw $0x41, %%ax\n\t" //      if(*s1 > 'A') goto big_letters
            "jge satoi16_big_letters%=\n\t"

            "subw $0x30, %%ax\n" //      eax = *s1 - '0'
            "satoi16_sum%=:\n\t"
            "addw %%ax, %0\n\t"    //      ret += eax
            "inc %1\n\t"            //      s1++
            "jmp satoi16_loop%=\n"

            "satoi16_big_letters%=:\n\t"
            "subw $0x37, %%ax\n\t" //      eax = *s1 - 'A'
            "jmp satoi16_sum%=\n"

            "satoi16_small_letters%=:\n\t"
            "subw $0x57, %%ax\n\t" //      eax = *s1 - 'a'
            "jmp satoi16_sum%=\n"
            "satoi16_end%=:\n\t"
            : "=&r"(ret)
            : "r"(s1)
            : "%ax", "cc"
        );
    return ret;
}

// base 10 atos
static inline short satoi10(char const* volatile s)
{
    char const* s1 = s;
    short volatile ret;
    asm volatile(
            "xorw %%ax, %%ax\n\t"    // upper three bytes -> 0
            "xorw %0, %0\n"       // ret = 0

            "satoi10_loop%=:\n\t"      // while(*s1) {
            "movb (%1), %%al\n\t"
            "cmpw $0, %%ax\n\t"
            "jz satoi10_end%=\n\t"
            "movw %0, %%bx\n\t"        // ret *= 10
            "shlw $1, %%bx\n\t"
            "shlw $3, %0\n\t"
            "addw %%bx, %0\n\t"

            "subw $0x30, %%ax\n\t" //      eax = *s1 - '0'
            "addw %%ax, %0\n\t"    //      ret += eax
            "inc %1\n\t"            //      s1++
            "jmp satoi10_loop%=\n"

            "satoi10_end%=:\n\t"
            : "=&r"(ret)             // output ret, W/O random register
            : "r"(s1)               // input s1, R/W random register
            : "%ax", "%bx", "cc"   // modifying eAX, ebx
        );
    return ret;
}

