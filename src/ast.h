#ifndef AST_H
#define AST_H

#include "types.h"

ASTNode *ast_alloc_node(NodeType type, int line);
ASTNode *ast_new_int(long val, int line);
ASTNode *ast_new_float(double val, int line);
ASTNode *ast_new_var(const char *name, int line);
ASTNode *ast_new_binop(int op, ASTNode *left, ASTNode *right, int line);
ASTNode *ast_new_assign(const char *name, ASTNode *value, int line);
ASTNode *ast_new_decl(DataType type, const char *name, ASTNode *init, int line);
ASTNode *ast_new_program(int line);
ASTNode *ast_new_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch, int line);
ASTNode *ast_new_while(ASTNode *condition, ASTNode *body, int line);
ASTNode *ast_new_for(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *body, int line);
ASTNode *ast_new_block(int line);
ASTNode *ast_new_print(ASTNode *value, int line);
ASTNode *ast_new_return(ASTNode *value, int line);
ASTNode *ast_new_call(const char *name, ASTNode *arg, int line);
void ast_add_node(ASTNode *parent, ASTNode *child);
void ast_print(ASTNode *node, int indent);

#endif
