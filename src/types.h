#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ====================== TOKEN TYPES ====================== */
typedef enum {
    TOK_EOF = 0,
    TOK_INT,
    TOK_FLOAT,
    TOK_CHAR,
    TOK_VOID,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_FOR,
    TOK_RETURN,
    TOK_PRINT,
    TOK_LOG,
    TOK_EXP_FUNC,
    TOK_ID,
    TOK_INT_LIT,
    TOK_FLOAT_LIT,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_MOD,
    TOK_EXP,
    TOK_ASSIGN,
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_GT,
    TOK_LTE,
    TOK_GTE,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_COMMA
} TokenType;

/* ====================== DATA TYPES ====================== */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_UNKNOWN
} DataType;

/* ====================== AST NODE TYPES ====================== */
typedef enum {
    NODE_INT,
    NODE_FLOAT,
    NODE_VAR,
    NODE_BINOP,
    NODE_UNOP,
    NODE_ASSIGN,
    NODE_DECL,
    NODE_PRINT,
    NODE_RETURN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_BLOCK,
    NODE_PROGRAM,
    NODE_CALL
} NodeType;

/* ====================== AST STRUCTURE ====================== */
typedef struct ASTNode {
    NodeType type;
    int line;
    int optimized;

    union {
        long int_val;
        double float_val;
        char char_val;

        struct {
            char name[128];
        } var;

        struct {
            int op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binop;

        struct {
            int op;
            struct ASTNode *operand;
        } unop;

        struct {
            char varname[128];
            struct ASTNode *value;
        } assign;

        struct {
            char name[128];
            DataType type;
            struct ASTNode *init_value;
        } decl;

        struct {
            struct ASTNode *value;
        } print;

        struct {
            struct ASTNode *value;
        } ret;

        struct {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;

        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;

        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *increment;
            struct ASTNode *body;
        } for_stmt;

        struct {
            struct ASTNode **statements;
            int count;
        } block;

        struct {
            struct ASTNode **nodes;
            int count;
        } program;

        struct {
            char name[128];
            struct ASTNode *arg;
        } call;
    } data;
} ASTNode;

#endif /* TYPES_H */
