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
void ast_add_node(ASTNode *program, ASTNode *node);
void ast_print(ASTNode *node, int indent);

#endif
