%{
#include <parser.tab.h>
%}

%option reentrant
%option noyywrap
%option case-insensitive

%%

"'"(\\.|[^"])*"'"   {
    yylval = yytext[1]; 
    yylval[strlen(yylval) - 1] = (char*)0; // minimum is '', so it's safe
    return LITERAL_STRING;
}

","         { return COMMA; }
[\r\n]      { return EOL_ATOM; }

[a-zA-Z_][a-zA-Z_0-9]*: { return LABEL; }
[a-zA-Z_][a-zA-Z_0-9]*   { return IDENT; }

0[xX][0-9a-fA-F]* { return HEX; }
0[oO][0-7]*     { return OCT; }
0[dD][0-9]*     { return DEC; }
0[bB][01]*      { return BIN; }

[ \t]           {}
;.\n            {}
