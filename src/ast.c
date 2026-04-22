#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================== AST NODE POOL ====================== */

static ASTNode ast_pool[MAX_AST_NODES];
static int ast_node_count = 0;

/* ====================== NODE ALLOCATION ====================== */

ASTNode *ast_alloc_node(NodeType type, int line) {
    if (ast_node_count >= MAX_AST_NODES) {
        fprintf(stderr, "ERROR: AST node pool exhausted\n");
        return NULL;
    }
    
    ASTNode *node = &ast_pool[ast_node_count++];
    memset(node, 0, sizeof(*node));
    node->type = type;
    node->line = line;
    node->optimized = 0;
    node->next = NULL;
    
    return node;
}

/* ====================== NODE FREEING ====================== */

void ast_free(ASTNode *root) {
    if (!root) return;
    
    switch (root->type) {
        case NODE_BINOP:
            ast_free(root->data.binop.left);
            ast_free(root->data.binop.right);
            break;
        
        case NODE_UNOP:
            ast_free(root->data.unop.operand);
            break;
        
        case NODE_ASSIGN:
            ast_free(root->data.assign.value);
            break;
        
        case NODE_IF:
            ast_free(root->data.if_node.condition);
            ast_free(root->data.if_node.then_branch);
            ast_free(root->data.if_node.else_branch);
            break;
        
        case NODE_WHILE:
            ast_free(root->data.while_node.condition);
            ast_free(root->data.while_node.body);
            break;
        
        case NODE_FOR:
            ast_free(root->data.for_node.init);
            ast_free(root->data.for_node.condition);
            ast_free(root->data.for_node.update);
            ast_free(root->data.for_node.body);
            break;
        
        case NODE_BLOCK:
            for (int i = 0; i < root->data.block.stmt_count; i++) {
                ast_free(root->data.block.stmts[i]);
            }
            if (root->data.block.stmts) free(root->data.block.stmts);
            break;
        
        case NODE_PRINT:
            ast_free(root->data.print.arg);
            break;
        
        case NODE_RETURN:
            ast_free(root->data.ret.value);
            break;
        
        case NODE_DECL:
            ast_free(root->data.decl.init_value);
            break;
        
        case NODE_PROGRAM:
            ast_free(root->data.block.stmts[0]);
            if (root->data.block.stmts) free(root->data.block.stmts);
            break;
        
        default:
            break;
    }
}

/* ====================== AST PRINTING ====================== */

static const char *node_type_name(NodeType t) {
    switch (t) {
        case NODE_INT: return "INT";
        case NODE_FLOAT: return "FLOAT";
        case NODE_CHAR: return "CHAR";
        case NODE_STRING: return "STRING";
        case NODE_VAR: return "VAR";
        case NODE_BINOP: return "BINOP";
        case NODE_UNOP: return "UNOP";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_IF: return "IF";
        case NODE_WHILE: return "WHILE";
        case NODE_FOR: return "FOR";
        case NODE_PRINT: return "PRINT";
        case NODE_RETURN: return "RETURN";
        case NODE_BLOCK: return "BLOCK";
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_DECL: return "DECL";
        default: return "UNKNOWN";
    }
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s", node_type_name(node->type));
    if (node->optimized) printf("[OPT]");
    printf("\n");
    
    switch (node->type) {
        case NODE_INT:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("value: %ld\n", node->data.int_val);
            break;
        
        case NODE_FLOAT:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("value: %f\n", node->data.float_val);
            break;
        
        case NODE_VAR:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("name: %s\n", node->data.var.name);
            break;
        
        case NODE_BINOP:
            ast_print(node->data.binop.left, indent + 1);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("op: %d\n", node->data.binop.op);
            ast_print(node->data.binop.right, indent + 1);
            break;
        
        case NODE_UNOP:
            ast_print(node->data.unop.operand, indent + 1);
            break;
        
        case NODE_ASSIGN:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("var: %s\n", node->data.assign.varname);
            ast_print(node->data.assign.value, indent + 1);
            break;
        
        case NODE_IF:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("condition:\n");
            ast_print(node->data.if_node.condition, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("then:\n");
            ast_print(node->data.if_node.then_branch, indent + 2);
            if (node->data.if_node.else_branch) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("else:\n");
                ast_print(node->data.if_node.else_branch, indent + 2);
            }
            break;
        
        case NODE_WHILE:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("condition:\n");
            ast_print(node->data.while_node.condition, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("body:\n");
            ast_print(node->data.while_node.body, indent + 2);
            break;
        
        case NODE_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                ast_print(node->data.block.stmts[i], indent + 1);
            }
            break;
        
        case NODE_PRINT:
            ast_print(node->data.print.arg, indent + 1);
            break;
        
        case NODE_RETURN:
            if (node->data.ret.value) {
                ast_print(node->data.ret.value, indent + 1);
            }
            break;
        
        case NODE_DECL:
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("name: %s\n", node->data.decl.name);
            if (node->data.decl.init_value) {
                ast_print(node->data.decl.init_value, indent + 1);
            }
            break;
        
        default:
            break;
    }
}

/* ====================== AST TRAVERSAL ====================== */

void ast_traverse(ASTNode *node, void (*func)(ASTNode *)) {
    if (!node) return;
    
    func(node);
    
    switch (node->type) {
        case NODE_BINOP:
            ast_traverse(node->data.binop.left, func);
            ast_traverse(node->data.binop.right, func);
            break;
        
        case NODE_UNOP:
            ast_traverse(node->data.unop.operand, func);
            break;
        
        case NODE_ASSIGN:
            ast_traverse(node->data.assign.value, func);
            break;
        
        case NODE_IF:
            ast_traverse(node->data.if_node.condition, func);
            ast_traverse(node->data.if_node.then_branch, func);
            ast_traverse(node->data.if_node.else_branch, func);
            break;
        
        case NODE_WHILE:
            ast_traverse(node->data.while_node.condition, func);
            ast_traverse(node->data.while_node.body, func);
            break;
        
        case NODE_FOR:
            ast_traverse(node->data.for_node.init, func);
            ast_traverse(node->data.for_node.condition, func);
            ast_traverse(node->data.for_node.update, func);
            ast_traverse(node->data.for_node.body, func);
            break;
        
        case NODE_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                ast_traverse(node->data.block.stmts[i], func);
            }
            break;
        
        case NODE_PRINT:
            ast_traverse(node->data.print.arg, func);
            break;
        
        case NODE_RETURN:
            ast_traverse(node->data.ret.value, func);
            break;
        
        case NODE_DECL:
            ast_traverse(node->data.decl.init_value, func);
            break;
        
        default:
            break;
    }
}
