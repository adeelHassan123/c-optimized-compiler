/* Module 2: Calculator Lexer
 * Shared lexer for postfix, prefix, and infix parsers
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "postfix.tab.h"  /* Change to prefix.tab.h or infix.tab.h as needed */
%}

%option noyywrap

%%

 /* Numbers */
[0-9]+(\.[0-9]+)?  { yylval.dval = atof(yytext); return NUMBER; }

 /* Operators */
"+"              { return PLUS; }
"-"              { return MINUS; }
"*"              { return MUL; }
"/"              { return DIV; }
"^"              { return EXP; }

 /* Functions */
"log"            { return LOG; }
"exp"            { return EXP_FUNC; }

 /* Parentheses (for infix/prefix) */
"("              { return LPAREN; }
")"              { return RPAREN; }

 /* Newline */
\n               { return NEWLINE; }

 /* Whitespace */
[ \t\r]+         { /* skip whitespace */ }

 /* Error */
.                { fprintf(stderr, "ERROR: Unknown character '%s'\n", yytext); }

%%
