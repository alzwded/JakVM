%{
//#define YYSTYPE char*
#include <stdio.h>
#include <ctype.h>
#include <common.h>
#include <asmGenerator.h>
#include <rtGenerator.h>

#define YYDEBUG 2

int yyerror(char* s)
{
    fprintf(stderr, "Failed to parse: %s\n", s);
}

%}

%error-verbose
%debug

%union {
    char* stringy;
    typed_t typed;
    listNode_t* list;
    int garbage;
}

%token UNEXPECTED ASM EOL ADDRESS IMEDIATE REGISTER OTHER WS_PART NONE HEX HEOF COMMA LABEL

%type <stringy> UNEXPECTED ASM EOL ADDRESS IMEDIATE REGISTER OTHER WS_PART NONE HEX HEOF COMMA LABEL
%type <stringy> param
%type <garbage> opcode_part
%type <list> file
%type <typed> opcode
%type <typed> statement
%type <typed> paramlist

%start file

%%

file:   file statement
        { if($2.mnemonic) push(&$$, clone(&$2)); }
    |   statement
        { if($1.mnemonic) push(&$$, clone(&$1)); }
    ;
statement:  TERM
            {
                $$.mnemonic = NULL;
            }
        |   ASM WS paramlist opcode TERM
            {
                $$.mnemonic = $1;
                $$.param1 = $3.param1; 
                $$.param2 = $3.param2;
                $$.opcodeSize = $4.opcodeSize;
                $$.addOrImed = $4.addOrImed;
            }
        ;
paramlist:  REGISTER COMMA WS param WS
            { $$.param1 = $1; $$.param2 = $4; }
        |   REGISTER WS
            { $$.param1 = $1; $$.param2 = NULL; }
        |   NONE WS
            { $$.param1 = NULL; $$.param2 = NULL; }
        |   LABEL WS
            { $$.param1 = $1; $$.param2 = NULL; }
        ;
param:  REGISTER
    |   IMEDIATE
    |   ADDRESS
    ;
opcode: opcode_part opcode_part
        {
            $$.opcodeSize = 2;
            if($1) { $$.addOrImed = 0; }
            if($2) { $$.addOrImed = 1; }
            else { $$.addOrImed = -1; }
        }
    |   opcode_part opcode_part opcode_part opcode_part
        {
            $$.opcodeSize = 4;
            if($2) { $$.addOrImed = 1; }
            else if ($3) { $$.addOrImed = 2; }
            else if ($4) { $$.addOrImed = 2; }
            else { $$.addOrImed = -1; }
        }
    ;
opcode_part:    HEX
                { $$ = 0; }
        |       OTHER
                { $$ = 1; }
        ;
WS:     WS WS_PART 
    |   WS_PART
    ;
TERM:   EOL | HEOF ;

%%

int main(int argc, char* argv[])
{
    yyparse();
    generateAsmFile();
    generateRTFile();
}
