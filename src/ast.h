#ifndef AST_H
#define AST_H

#include "types.h"

/* Forward declaration */
typedef struct ASTNode ASTNode;

/* ====================== AST NODE STRUCTURE ====================== */

typedef struct ASTNode {
    NodeType type;
    int line;
    int optimized;  /* Flag for optimization tracking */
    
    union {
        /* Literals */
        long int_val;
        double float_val;
        char char_val;
        
        /* Variable reference */
        struct {
            char name[IDENTIFIER_MAX];
        } var;
        
        /* Binary operation */
        struct {
            int op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binop;
        
        /* Unary operation */
        struct {
            int op;
            struct ASTNode *operand;
        } unop;
        
        /* Assignment */
        struct {
            char varname[IDENTIFIER_MAX];
            struct ASTNode *value;
        } assign;
        
        /* If statement */
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_node;
        
        /* While loop */
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_node;
        
        /* For loop */
        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *update;
            struct ASTNode *body;
        } for_node;
        
        /* Block of statements */
        struct {
            struct ASTNode **stmts;
            int stmt_count;
        } block;
        
        /* Print statement */
        struct {
            struct ASTNode *arg;
        } print;
        
        /* Return statement */
        struct {
            struct ASTNode *value;
        } ret;
        
        /* Declaration */
        struct {
            DataType dtype;
            char name[IDENTIFIER_MAX];
            struct ASTNode *init_value;
        } decl;
        
    } data;
    
    struct ASTNode *next;  /* For statement lists */
} ASTNode;

/* ====================== AST INTERFACE ====================== */

/* Allocate new AST node */
ASTNode *ast_alloc_node(NodeType type, int line);

/* Free entire AST */
void ast_free(ASTNode *root);

/* Print AST (for debugging) */
void ast_print(ASTNode *node, int indent);

/* Traverse and call function on each node */
void ast_traverse(ASTNode *node, void (*func)(ASTNode *));

#endif /* AST_H */
// Commit Marker