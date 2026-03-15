#ifndef TYPES_H
#define TYPES_H

/* ====================== DATA TYPES ====================== */

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_UNKNOWN
} DataType;

/* ====================== TOKEN TYPES ====================== */

typedef enum {
    /* Keywords */
    TOK_INT, TOK_FLOAT, TOK_CHAR,
    TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR, TOK_RETURN,
    TOK_PRINT, TOK_MAIN,
    
    /* Identifiers and Literals */
    TOK_IDENT,
    TOK_INT_LIT, TOK_FLOAT_LIT, TOK_CHAR_LIT, TOK_STRING_LIT,
    
    /* Operators and Delimiters */
    TOK_ASSIGN, TOK_SEMI, TOK_COMMA,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_MOD,
    TOK_GT, TOK_LT, TOK_EQ, TOK_NEQ, TOK_GE, TOK_LE,
    TOK_AND, TOK_OR, TOK_NOT,
    TOK_EOF, TOK_UNKNOWN
} TokenType;

/* ====================== AST NODE TYPES ====================== */

typedef enum {
    /* Literals */
    NODE_INT, NODE_FLOAT, NODE_CHAR, NODE_STRING,
    /* Variables and Operations */
    NODE_VAR, NODE_BINOP, NODE_UNOP,
    /* Statements */
    NODE_ASSIGN, NODE_IF, NODE_WHILE, NODE_FOR,
    NODE_PRINT, NODE_RETURN,
    /* Compound */
    NODE_BLOCK, NODE_PROGRAM,
    /* Declarations */
    NODE_DECL
} NodeType;

/* ====================== CONSTANTS ====================== */

#define MAX_SOURCE       65536
#define MAX_TOKENS       10000
#define MAX_SYMBOLS      1000
#define MAX_AST_NODES    5000
#define MAX_IR_NODES     10000
#define IDENTIFIER_MAX   128
#define STRING_MAX       512

#endif /* TYPES_H */
// Commit Marker