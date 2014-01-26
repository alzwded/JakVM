#include <common.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typed_t* clone(typed_t* t)
{
    typed_t* similar = (typed_t*)malloc(sizeof(typed_t));
    memcpy(similar, t, sizeof(typed_t));
    return similar;
}

void push(listNode_t** list, typed_t* value)
{
    printf("%p: ", value);
    printf("%s", value->mnemonic);
    printf("(%d,%c) ", value->opcodeSize, ((value->addOrImed > 0) ? value->addOrImed + '0' : '-'));
    if(value->param1 != NULL && *value->param1 != 0) printf("\t%s", value->param1);
    if(value->param2 != NULL && *value->param2 != 0) printf(", %s", value->param2);
    printf("\n");
}
