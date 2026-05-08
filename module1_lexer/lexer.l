%{
/* Module 1: Flex Lexer
 * CS-346 Compiler Construction - Lab Project
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Token type definitions */
enum TokenType {
    TOK_INT, TOK_FLOAT, TOK_WHILE, TOK_RETURN, TOK_ELSE,
    TOK_IF, TOK_THEN, TOK_BEGIN, TOK_END, TOK_PROCEDURE, TOK_FUNCTION,
    TOK_IDENTIFIER, TOK_INT_LITERAL, TOK_FLOAT_LITERAL,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_EXP,
    TOK_ASSIGN, TOK_EQ, TOK_NEQ,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_SEMI, TOK_COMMA,
    TOK_EOF, TOK_UNKNOWN
};

/* Token structure for output */
typedef struct {
    enum TokenType type;
    char lexeme[256];
    int line;
    int int_val;
    double float_val;
} Token;

int line_num = 1;
int token_count = 0;

const char* get_token_name(enum TokenType type);
void print_token(Token t);
void create_token(Token *t, enum TokenType type, const char *lexeme, int line, int int_val, double float_val);

%}

/* Options */
%option noyywrap
%option case-sensitive

/* Definitions */
DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+

%%

 /* Keywords */
"int"           { Token t; create_token(&t, TOK_INT, "int", line_num, 0, 0.0); print_token(t); token_count++; }
"float"         { Token t; create_token(&t, TOK_FLOAT, "float", line_num, 0, 0.0); print_token(t); token_count++; }
"while"         { Token t; create_token(&t, TOK_WHILE, "while", line_num, 0, 0.0); print_token(t); token_count++; }
"return"        { Token t; create_token(&t, TOK_RETURN, "return", line_num, 0, 0.0); print_token(t); token_count++; }
"else"          { Token t; create_token(&t, TOK_ELSE, "else", line_num, 0, 0.0); print_token(t); token_count++; }
"if"            { Token t; create_token(&t, TOK_IF, "if", line_num, 0, 0.0); print_token(t); token_count++; }
"then"          { Token t; create_token(&t, TOK_THEN, "then", line_num, 0, 0.0); print_token(t); token_count++; }
"begin"         { Token t; create_token(&t, TOK_BEGIN, "begin", line_num, 0, 0.0); print_token(t); token_count++; }
"end"           { Token t; create_token(&t, TOK_END, "end", line_num, 0, 0.0); print_token(t); token_count++; }
"procedure"     { Token t; create_token(&t, TOK_PROCEDURE, "procedure", line_num, 0, 0.0); print_token(t); token_count++; }
"function"      { Token t; create_token(&t, TOK_FUNCTION, "function", line_num, 0, 0.0); print_token(t); token_count++; }

 /* Identifiers */
{IDENTIFIER}    { Token t; create_token(&t, TOK_IDENTIFIER, yytext, line_num, 0, 0.0); print_token(t); token_count++; }

 /* Numeric literals */
{FLOAT}         { Token t; create_token(&t, TOK_FLOAT_LITERAL, yytext, line_num, 0, atof(yytext)); print_token(t); token_count++; }
{INTEGER}       { Token t; create_token(&t, TOK_INT_LITERAL, yytext, line_num, atoi(yytext), 0.0); print_token(t); token_count++; }

 /* Operators */
"+"             { Token t; create_token(&t, TOK_PLUS, "+", line_num, 0, 0.0); print_token(t); token_count++; }
"-"             { Token t; create_token(&t, TOK_MINUS, "-", line_num, 0, 0.0); print_token(t); token_count++; }
"*"             { Token t; create_token(&t, TOK_MUL, "*", line_num, 0, 0.0); print_token(t); token_count++; }
"/"             { Token t; create_token(&t, TOK_DIV, "/", line_num, 0, 0.0); print_token(t); token_count++; }
"^"             { Token t; create_token(&t, TOK_EXP, "^", line_num, 0, 0.0); print_token(t); token_count++; }
"=="            { Token t; create_token(&t, TOK_EQ, "==", line_num, 0, 0.0); print_token(t); token_count++; }
"!="            { Token t; create_token(&t, TOK_NEQ, "!=", line_num, 0, 0.0); print_token(t); token_count++; }
"="             { Token t; create_token(&t, TOK_ASSIGN, "=", line_num, 0, 0.0); print_token(t); token_count++; }

 /* Delimiters */
"("             { Token t; create_token(&t, TOK_LPAREN, "(", line_num, 0, 0.0); print_token(t); token_count++; }
")"             { Token t; create_token(&t, TOK_RPAREN, ")", line_num, 0, 0.0); print_token(t); token_count++; }
"{"             { Token t; create_token(&t, TOK_LBRACE, "{", line_num, 0, 0.0); print_token(t); token_count++; }
"}"             { Token t; create_token(&t, TOK_RBRACE, "}", line_num, 0, 0.0); print_token(t); token_count++; }
";"             { Token t; create_token(&t, TOK_SEMI, ";", line_num, 0, 0.0); print_token(t); token_count++; }
","             { Token t; create_token(&t, TOK_COMMA, ",", line_num, 0, 0.0); print_token(t); token_count++; }

 /* Single-line comment */
"//".*          { /* Skip single-line comment */ }

 /* Multi-line comment */
"/*"([^*]|\*+[^*/])*\*+"/" { /* Skip multi-line comment */ }

 /* Whitespace */
[ \t\r]+        { /* Skip whitespace */ }
\n              { line_num++; }

 /* Error: unrecognized character */
.               { fprintf(stderr, "ERROR: Unrecognized character '%s' at line %d\n", yytext, line_num); }

%%

/* Helper function to create tokens */
void create_token(Token *t, enum TokenType type, const char *lexeme, int line, int int_val, double float_val) {
    t->type = type;
    strncpy(t->lexeme, lexeme, 255);
    t->lexeme[255] = '\0';
    t->line = line;
    t->int_val = int_val;
    t->float_val = float_val;
}

const char* get_token_name(enum TokenType type) {
    switch(type) {
        case TOK_INT: return "INT";
        case TOK_FLOAT: return "FLOAT";
        case TOK_WHILE: return "WHILE";
        case TOK_RETURN: return "RETURN";
        case TOK_ELSE: return "ELSE";
        case TOK_IF: return "IF";
        case TOK_THEN: return "THEN";
        case TOK_BEGIN: return "BEGIN";
        case TOK_END: return "END";
        case TOK_PROCEDURE: return "PROCEDURE";
        case TOK_FUNCTION: return "FUNCTION";
        case TOK_IDENTIFIER: return "IDENTIFIER";
        case TOK_INT_LITERAL: return "INT_LITERAL";
        case TOK_FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TOK_PLUS: return "PLUS";
        case TOK_MINUS: return "MINUS";
        case TOK_MUL: return "MUL";
        case TOK_DIV: return "DIV";
        case TOK_EXP: return "EXP";
        case TOK_ASSIGN: return "ASSIGN";
        case TOK_EQ: return "EQ";
        case TOK_NEQ: return "NEQ";
        case TOK_LPAREN: return "LPAREN";
        case TOK_RPAREN: return "RPAREN";
        case TOK_LBRACE: return "LBRACE";
        case TOK_RBRACE: return "RBRACE";
        case TOK_SEMI: return "SEMI";
        case TOK_COMMA: return "COMMA";
        case TOK_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

void print_token(Token t) {
    printf("Line %3d: %-15s '%s'", t.line, get_token_name(t.type), t.lexeme);
    if (t.type == TOK_INT_LITERAL) {
        printf(" (value: %d)", t.int_val);
    } else if (t.type == TOK_FLOAT_LITERAL) {
        printf(" (value: %f)", t.float_val);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    printf("=================================================\n");
    printf("Module 1: Flex Lexer - CS-346 Compiler Project\n");
    printf("=================================================\n\n");
    
    if (argc > 1) {
        FILE *input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "ERROR: Cannot open file '%s'\n", argv[1]);
            return 1;
        }
        yyin = input;
        printf("Input file: %s\n\n", argv[1]);
    } else {
        printf("Reading from stdin (enter tokens, Ctrl+D to end):\n\n");
    }
    
    printf("%-8s %-15s %-20s %s\n", "Line", "Token Type", "Lexeme", "Value");
    printf("----------------------------------------------------\n");
    
    yylex();
    
    printf("\n=================================================\n");
    printf("Total tokens: %d\n", token_count);
    printf("Total lines: %d\n", line_num);
    printf("=================================================\n");
    
    return 0;
}
