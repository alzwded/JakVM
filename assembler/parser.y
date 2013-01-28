%{
/* details in assemblerDefinition */
%}

%start file

%%

file:   STACK_DIRECTIVE DATA_SEGMENT CODE_SEGMENT 
    |   STACK_DIRECTIVE CODE_SEGMENT
    ;

STACK_DIRECTIVE:    '.stack' HUMAN_READABLE_SIZE EOL
    ;

DATA_SEGMENT:       '.data' EOL
    |               '.data' EOL DATA_SEGMENT_BODY
    ;

DATA_SEGMENT_BODY:  DATA_SEGMENT_BODY ATOMIC_DATA
    |               ATOMIC_DATA
    ;

LIST_TYPE:          LIST_TYPE COMMA LITERAL
    |               LITERAL
    |               LITERAL_STRING
    ;

LITERAL_TYPE_SPECIFIER: 'n1' | 'n2' | 'n4' ; /* TODO */

LIST_TYPE_SPECIFIER: 's1' | 's2' | 's4' | 'a1' | 'a2' | 'a4' ; /* TODO */

ADOMIC_DATA:        STRING LITERAL_TYPE_SPECIFIER LITERAL EOL
    |               STRING LIST_TYPE_SPECIFIER LIST_TYPE EOL
    ;

CODE_SEGMENT:       '.code' EOL
    |               '.code' EOL CODE_SEGMENT_BODY
    ;

CODE_SEGMENT_BODY:  CODE_SEGMENT_BODY INSTRUCTION EOL 
    |               INSTRUCTION EOL
    ;

/*LABEL:          IDENT COLON
    ;*/

INSTRUCTION:    LABEL ATOMIC_INSTRUCTION EOL
    |           LABEL EOL ATOMIC_INSTRUCTION EOL
    |           ATOMIC_INSTRUCTION EOL
    ;

MNEMONIC:           IDENT
    ;

PARAM:              IDENT
    |               NUMBER 
    ;

ATOMIC_INSTRUCTION: MNEMONIC
    |               MNEMONIC PARAM
    |               MNEMONIC PARAM COMMA PARAM
    ;

EOL:    EOL EOL_ATOM
    |   EOL_ATOM
    ;

NUMBER:     NUMBER_ATOM
    |       '-' NUMBER_ATOM
    ;

NUMBER_ATOM:    HEX
    |           OCT
    |           DEC
    |           BIN
    ;

%%
