#ifndef COMMON_H
#define COMMON_H
typedef struct typed_s {
    char* param1;
    char* param2;
    char* mnemonic;
    int opcodeSize;
    int addOrImed;
} typed_t;

typedef struct listNode_s {
    struct listNode_s* next;
    struct typed* value;
} listNode_t;

void push(listNode_t** list, typed_t* value);
typed_t* clone(typed_t*);
#endif
