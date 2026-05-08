#include "optimizer.h"
#include "ast.h"

static OptimizationStats opt_stats = {0};

static ASTNode *fold_constants(ASTNode *node) {
    if (!node || node->type != NODE_BINOP) return node;
    node->data.binop.left = fold_constants(node->data.binop.left);
    node->data.binop.right = fold_constants(node->data.binop.right);
    
    ASTNode *left = node->data.binop.left;
    ASTNode *right = node->data.binop.right;
    
    if (left->type == NODE_INT && right->type == NODE_INT) {
        long res = 0;
        const char *op_str = "";
        switch (node->data.binop.op) {
            case TOK_PLUS: res = left->data.int_val + right->data.int_val; op_str = "+"; break;
            case TOK_MINUS: res = left->data.int_val - right->data.int_val; op_str = "-"; break;
            case TOK_MUL: res = left->data.int_val * right->data.int_val; op_str = "*"; break;
            case TOK_DIV: 
                if (right->data.int_val != 0) {
                    res = left->data.int_val / right->data.int_val; 
                    op_str = "/";
                } break;
            default: return node;
        }
        printf("  [OPTIMIZE] Constant Folding: %ld %s %ld -> %ld (Line: %d)\n", 
               left->data.int_val, op_str, right->data.int_val, res, node->line);
        opt_stats.constant_folds++;
        return ast_new_int(res, node->line);
    }
    return node;
}

ASTNode *optimizer_optimize(ASTNode *ast) {
    if (!ast) return NULL;
    if (ast->type == NODE_PROGRAM) {
        for (int i = 0; i < ast->data.program.count; i++)
            ast->data.program.nodes[i] = optimizer_optimize(ast->data.program.nodes[i]);
        return ast;
    }
    if (ast->type == NODE_DECL && ast->data.decl.init_value)
        ast->data.decl.init_value = fold_constants(ast->data.decl.init_value);
    if (ast->type == NODE_ASSIGN)
        ast->data.assign.value = fold_constants(ast->data.assign.value);
    return ast;
}

OptimizationStats optimizer_get_stats(void) { return opt_stats; }
void optimizer_print_stats(void) {
    printf("\n============ OPTIMIZATION REPORT ============\n");
    printf("Constant folds: %d\n", opt_stats.constant_folds);
}
