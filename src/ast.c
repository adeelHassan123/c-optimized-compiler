#include "types.h"

ASTNode *ast_alloc_node(NodeType type, int line) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "ERROR: Out of memory\n");
        exit(1);
    }
    node->type = type;
    node->line = line;
    node->optimized = 0;
    memset(&node->data, 0, sizeof(node->data));
    return node;
}

ASTNode *ast_new_int(long val, int line) {
    ASTNode *node = ast_alloc_node(NODE_INT, line);
    node->data.int_val = val;
    return node;
}

ASTNode *ast_new_float(double val, int line) {
    ASTNode *node = ast_alloc_node(NODE_FLOAT, line);
    node->data.float_val = val;
    return node;
}

ASTNode *ast_new_var(const char *name, int line) {
    ASTNode *node = ast_alloc_node(NODE_VAR, line);
    strncpy(node->data.var.name, name, 127);
    return node;
}

ASTNode *ast_new_binop(int op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = ast_alloc_node(NODE_BINOP, line);
    node->data.binop.op = op;
    node->data.binop.left = left;
    node->data.binop.right = right;
    return node;
}

ASTNode *ast_new_assign(const char *name, ASTNode *value, int line) {
    ASTNode *node = ast_alloc_node(NODE_ASSIGN, line);
    strncpy(node->data.assign.varname, name, 127);
    node->data.assign.value = value;
    return node;
}

ASTNode *ast_new_decl(DataType type, const char *name, ASTNode *init, int line) {
    ASTNode *node = ast_alloc_node(NODE_DECL, line);
    node->data.decl.type = type;
    strncpy(node->data.decl.name, name, 127);
    node->data.decl.init_value = init;
    return node;
}

ASTNode *ast_new_program(int line) {
    ASTNode *node = ast_alloc_node(NODE_PROGRAM, line);
    node->data.program.nodes = malloc(sizeof(ASTNode *) * 100);
    node->data.program.count = 0;
    return node;
}

void ast_add_node(ASTNode *program, ASTNode *node) {
    if (program->type == NODE_PROGRAM) {
        program->data.program.nodes[program->data.program.count++] = node;
    } else if (program->type == NODE_BLOCK) {
        program->data.block.statements[program->data.block.count++] = node;
    }
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case NODE_INT: printf("INT: %ld\n", node->data.int_val); break;
        case NODE_FLOAT: printf("FLOAT: %.2f\n", node->data.float_val); break;
        case NODE_VAR: printf("VAR: %s\n", node->data.var.name); break;
        case NODE_BINOP:
            printf("BINOP: %d\n", node->data.binop.op);
            ast_print(node->data.binop.left, indent + 1);
            ast_print(node->data.binop.right, indent + 1);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN: %s\n", node->data.assign.varname);
            ast_print(node->data.assign.value, indent + 1);
            break;
        case NODE_DECL:
            printf("DECL: %s (Type: %d)\n", node->data.decl.name, node->data.decl.type);
            if (node->data.decl.init_value) ast_print(node->data.decl.init_value, indent + 1);
            break;
        case NODE_PROGRAM:
            printf("PROGRAM:\n");
            for (int i = 0; i < node->data.program.count; i++)
                ast_print(node->data.program.nodes[i], indent + 1);
            break;
        case NODE_PRINT:
            printf("PRINT:\n");
            ast_print(node->data.print.value, indent + 1);
            break;
        case NODE_RETURN:
            printf("RETURN:\n");
            if (node->data.ret.value) ast_print(node->data.ret.value, indent + 1);
            break;
        case NODE_BLOCK:
            printf("BLOCK:\n");
            for (int i = 0; i < node->data.block.count; i++)
                ast_print(node->data.block.statements[i], indent + 1);
            break;
        default: printf("UNKNOWN NODE TYPE: %d at line %d\n", node->type, node->line); break;
    }
}
