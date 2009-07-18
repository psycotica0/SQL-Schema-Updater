%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"

extern yylval;
%}

%%

CREATE return CREATE;
TABLE return TABLE;
INSERT return INSERT;
INTO return INTO;
VALUES return VALUES;
NULL return SQL_NULL;
\( return LPAREN;
\) return RPAREN;
, return COMMA;
; return SEMI;
--[^\n]*\n /* IGNORE */;
'([^']*('')*)*' yylval=strdup(yytext); return STRING;
\" return DOUBLE_QUOTE;
[a-zA-Z][a-zA-Z0-9_]* yylval=strdup(yytext); return WORD;
[0-9]* yylval=strdup(yytext); return NUMBER;
\n /* IGNORE */;
[ \t]+ /* IGNORE */;

%% 
