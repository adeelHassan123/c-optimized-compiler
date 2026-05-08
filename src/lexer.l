%{
#include "types.h"
#include "parser.tab.h"
#include <string.h>

int line_num = 1;
int print_tokens = 0;

void print_token(const char* type, const char* lexeme) {
    if (print_tokens) {
        printf("[TOKEN] %-15s | Lexeme: '%-10s' | Line: %d\n", type, lexeme, line_num);
    }
}
%}

%option noyywrap

%%

"int"       { print_token("KEYWORD_INT", yytext); return INT; }
"float"     { print_token("KEYWORD_FLOAT", yytext); return FLOAT; }
"char"      { print_token("KEYWORD_CHAR", yytext); return CHAR; }
"void"      { print_token("KEYWORD_VOID", yytext); return VOID; }
"if"        { print_token("KEYWORD_IF", yytext); return IF; }
"else"      { print_token("KEYWORD_ELSE", yytext); return ELSE; }
"while"     { print_token("KEYWORD_WHILE", yytext); return WHILE; }
"for"       { print_token("KEYWORD_FOR", yytext); return FOR; }
"return"    { print_token("KEYWORD_RETURN", yytext); return RETURN; }
"print"     { print_token("KEYWORD_PRINT", yytext); return PRINT; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.name = strdup(yytext); print_token("IDENTIFIER", yytext); return ID; }
[0-9]+      { yylval.int_val = atol(yytext); print_token("INT_LITERAL", yytext); return INT_LIT; }
[0-9]+\.[0-9]+ { yylval.float_val = atof(yytext); print_token("FLOAT_LITERAL", yytext); return FLOAT_LIT; }

"+"         { print_token("OP_PLUS", yytext); return PLUS; }
"-"         { print_token("OP_MINUS", yytext); return MINUS; }
"*"         { print_token("OP_MUL", yytext); return MUL; }
"/"         { print_token("OP_DIV", yytext); return DIV; }
"%"         { print_token("OP_MOD", yytext); return MOD; }
"^"         { print_token("OP_EXP", yytext); return EXP; }
"="         { print_token("OP_ASSIGN", yytext); return ASSIGN; }
"=="        { print_token("OP_EQ", yytext); return EQ; }
"!="        { print_token("OP_NEQ", yytext); return NEQ; }
"<"         { print_token("OP_LT", yytext); return LT; }
">"         { print_token("OP_GT", yytext); return GT; }
"<="        { print_token("OP_LTE", yytext); return LTE; }
">="        { print_token("OP_GTE", yytext); return GTE; }

"("         { print_token("LPAREN", yytext); return LPAREN; }
")"         { print_token("RPAREN", yytext); return RPAREN; }
"{"         { print_token("LBRACE", yytext); return LBRACE; }
"}"         { print_token("RBRACE", yytext); return RBRACE; }
";"         { print_token("SEMICOLON", yytext); return SEMICOLON; }
","         { print_token("COMMA", yytext); return COMMA; }

[ \t\r]+    { /* skip whitespace */ }
"\n"        { line_num++; }

"//".*      { print_token("COMMENT_SL", yytext); }
"/*"([^*]|\*+[^*/])*\*+"/" { print_token("COMMENT_ML", "/*...*/"); }

.           { printf("[LEX ERROR] Unknown character: %s at line %d\n", yytext, line_num); }

%%
