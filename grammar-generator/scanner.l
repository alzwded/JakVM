%{
#include <common.h>
#include <parser.tab.h>
// <<EOF>>     { return HEOF; }
%}

%option noreentrant
%option noyywrap
%option nocase-insensitive


%%

","     { return COMMA; }

"AX" |
"AH" | 
"AL" |
"AD" |
"AC" |
"AB" |
"AA" |
"EX" |
"EH" |
"EL" |
"ED" |
"EC" |
"EB" |
"EA" |
"CX" |
"CH" |
"CL" |
"RX" |
"IX" |
"JX" |
"SP" |
"ZP" |
"PP" |
"QP" |
"IP" { yylval.stringy = strdup(yytext); return REGISTER; }


[\t ]       { return WS_PART; }
"\n"        |
"\r\r"      { return EOL; }

"xx"        |
"x-"        |
"--"        |
"-x"        { return OTHER; }

"addr"      { yylval.stringy = strdup(yytext); return ADDRESS; }
^[a-zA-Z][a-zA-Z][0-9]?  |
^[a-zA0Z][a-zA-Z][a-zA-Z][0-9]? { yylval.stringy = strdup(yytext); return ASM; }


"_"         { yylval.stringy = strdup("x"); return IMEDIATE; }
"-"         { yylval.stringy = strdup(""); return NONE; }

[0-9a-f][0-9a-f]  { return HEX; }

.           { return UNEXPECTED; }
