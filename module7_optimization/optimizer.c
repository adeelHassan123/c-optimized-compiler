#include "optimizer.h"

/* ====================== OPTIMIZATION STATISTICS ====================== */

static OptimizationStats opt_stats = {0};

/* ====================== CONSTANT FOLDING ====================== */

static ASTNode *evaluate_constant_binop(ASTNode *node) {
    if (!node || node->type != NODE_BINOP) return node;
    if (!node->data.binop.left || !node->data.binop.right) return node;
    
    ASTNode *left = node->data.binop.left;
    ASTNode *right = node->data.binop.right;
    
    /* Both operands must be constants */
    if ((left->type != NODE_INT && left->type != NODE_FLOAT) ||
        (right->type != NODE_INT && right->type != NODE_FLOAT)) {
        return node;
    }
    
    int op = node->data.binop.op;
    long left_int = (left->type == NODE_INT) ? left->data.int_val : (long)left->data.float_val;
    long right_int = (right->type == NODE_INT) ? right->data.int_val : (long)right->data.float_val;
    double left_float = (left->type == NODE_INT) ? (double)left->data.int_val : left->data.float_val;
    double right_float = (right->type == NODE_INT) ? (double)right->data.int_val : right->data.float_val;
    
    ASTNode *result = NULL;
    
    switch (op) {
        case TOK_PLUS:
            if (left->type == NODE_FLOAT || right->type == NODE_FLOAT) {
                result = ast_alloc_node(NODE_FLOAT, node->line);
                result->data.float_val = left_float + right_float;
            } else {
                result = ast_alloc_node(NODE_INT, node->line);
                result->data.int_val = left_int + right_int;
            }
            break;
        
        case TOK_MINUS:
            if (left->type == NODE_FLOAT || right->type == NODE_FLOAT) {
                result = ast_alloc_node(NODE_FLOAT, node->line);
                result->data.float_val = left_float - right_float;
            } else {
                result = ast_alloc_node(NODE_INT, node->line);
                result->data.int_val = left_int - right_int;
            }
            break;
        
        case TOK_MUL:
            if (left->type == NODE_FLOAT || right->type == NODE_FLOAT) {
                result = ast_alloc_node(NODE_FLOAT, node->line);
                result->data.float_val = left_float * right_float;
            } else {
                result = ast_alloc_node(NODE_INT, node->line);
                result->data.int_val = left_int * right_int;
            }
            break;
        
        case TOK_DIV:
            if (right_int == 0 && right_float == 0.0) {
                return node;  /* Division by zero */
            }
            if (left->type == NODE_FLOAT || right->type == NODE_FLOAT) {
                result = ast_alloc_node(NODE_FLOAT, node->line);
                result->data.float_val = left_float / right_float;
            } else {
                result = ast_alloc_node(NODE_INT, node->line);
                result->data.int_val = left_int / right_int;
            }
            break;
        
        case TOK_MOD:
            if (right_int == 0) return node;
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = left_int % right_int;
            break;
        
        case TOK_EXP:  /* Module 3: Exponentiation */
            if (left->type == NODE_FLOAT || right->type == NODE_FLOAT) {
                result = ast_alloc_node(NODE_FLOAT, node->line);
                result->data.float_val = pow(left_float, right_float);
            } else {
                result = ast_alloc_node(NODE_INT, node->line);
                result->data.int_val = (long)pow(left_int, right_int);
            }
            break;
        
        case TOK_EQ:
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = (left_int == right_int) ? 1 : 0;
            break;
        
        case TOK_NEQ:
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = (left_int != right_int) ? 1 : 0;
            break;
        
        case TOK_LT:
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = (left_float < right_float) ? 1 : 0;
            break;
        
        case TOK_LE:
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = (left_float <= right_float) ? 1 : 0;
            break;
        
        case TOK_GT:
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = (left_float > right_float) ? 1 : 0;
            break;
        
        case TOK_GE:
            result = ast_alloc_node(NODE_INT, node->line);
            result->data.int_val = (left_float >= right_float) ? 1 : 0;
            break;
        
        default:
            return node;
    }
    
    if (result) {
        result->optimized = 1;
        opt_stats.constant_folds++;
    }
    
    return result ? result : node;
}

/* Module 3: Constant folding for function calls (log and exp) */
static ASTNode *evaluate_constant_call(ASTNode *node) {
    if (!node || node->type != NODE_CALL) return node;
    
    ASTNode *arg = node->data.unop.operand;
    int func_type = node->data.unop.op;
    
    /* Argument must be constant */
    if (arg->type != NODE_INT && arg->type != NODE_FLOAT) {
        return node;
    }
    
    double arg_val = (arg->type == NODE_INT) ? (double)arg->data.int_val : arg->data.float_val;
    ASTNode *result = NULL;
    
    switch (func_type) {
        case TOK_LOG:
            if (arg_val > 0) {
                result = ast_alloc_node(NODE_FLOAT, node->line);
                result->data.float_val = log(arg_val);
            }
            break;
        
        case TOK_EXP_FUNC:
            result = ast_alloc_node(NODE_FLOAT, node->line);
            result->data.float_val = exp(arg_val);
            break;
    }
    
    if (result) {
        result->optimized = 1;
        opt_stats.constant_folds++;
    }
    
    return result ? result : node;
}

ASTNode *optimizer_constant_folding(ASTNode *node) {
    if (!node) return node;
    
    switch (node->type) {
        case NODE_BINOP:
            node->data.binop.left = optimizer_constant_folding(node->data.binop.left);
            node->data.binop.right = optimizer_constant_folding(node->data.binop.right);
            return evaluate_constant_binop(node);
        
        case NODE_UNOP:
            node->data.unop.operand = optimizer_constant_folding(node->data.unop.operand);
            return node;
        
        case NODE_CALL:
            node->data.unop.operand = optimizer_constant_folding(node->data.unop.operand);
            return evaluate_constant_call(node);
        
        case NODE_ASSIGN:
            node->data.assign.value = optimizer_constant_folding(node->data.assign.value);
            return node;
        
        case NODE_IF:
            node->data.if_node.condition = optimizer_constant_folding(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_constant_folding(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_constant_folding(node->data.if_node.else_branch);
            return node;
        
        case NODE_WHILE:
            node->data.while_node.condition = optimizer_constant_folding(node->data.while_node.condition);
            node->data.while_node.body = optimizer_constant_folding(node->data.while_node.body);
            return node;
        
        case NODE_FOR:
            node->data.for_node.init = optimizer_constant_folding(node->data.for_node.init);
            node->data.for_node.condition = optimizer_constant_folding(node->data.for_node.condition);
            node->data.for_node.update = optimizer_constant_folding(node->data.for_node.update);
            node->data.for_node.body = optimizer_constant_folding(node->data.for_node.body);
            return node;
        
        case NODE_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_constant_folding(node->data.block.stmts[i]);
            }
            return node;
        
        case NODE_PRINT:
            node->data.print.arg = optimizer_constant_folding(node->data.print.arg);
            return node;
        
        case NODE_RETURN:
            node->data.ret.value = optimizer_constant_folding(node->data.ret.value);
            return node;
        
        case NODE_DECL:
            node->data.decl.init_value = optimizer_constant_folding(node->data.decl.init_value);
            return node;
        
        default:
            return node;
    }
}

/* ====================== UNREACHABLE CODE ELIMINATION ====================== */

ASTNode *optimizer_unreachable_code_elimination(ASTNode *node) {
    if (!node) return node;

    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK: {
            ASTNode **filtered = malloc(node->data.block.stmt_count * sizeof(ASTNode*));
            int new_count = 0;
            int unreachable = 0;

            for (int i = 0; i < node->data.block.stmt_count; i++) {
                ASTNode *stmt = node->data.block.stmts[i];

                /* Statements after return are unreachable in this block */
                if (unreachable) {
                    opt_stats.unreachable_eliminations++;
                    continue;
                }

                stmt = optimizer_unreachable_code_elimination(stmt);
                filtered[new_count++] = stmt;

                if (stmt && stmt->type == NODE_RETURN) {
                    unreachable = 1;
                }
            }

            free(node->data.block.stmts);
            node->data.block.stmts = filtered;
            node->data.block.stmt_count = new_count;
            return node;
        }

        case NODE_IF:
            node->data.if_node.condition = optimizer_unreachable_code_elimination(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_unreachable_code_elimination(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_unreachable_code_elimination(node->data.if_node.else_branch);
            return node;

        case NODE_WHILE:
            node->data.while_node.condition = optimizer_unreachable_code_elimination(node->data.while_node.condition);
            node->data.while_node.body = optimizer_unreachable_code_elimination(node->data.while_node.body);
            return node;

        case NODE_FOR:
            node->data.for_node.init = optimizer_unreachable_code_elimination(node->data.for_node.init);
            node->data.for_node.condition = optimizer_unreachable_code_elimination(node->data.for_node.condition);
            node->data.for_node.update = optimizer_unreachable_code_elimination(node->data.for_node.update);
            node->data.for_node.body = optimizer_unreachable_code_elimination(node->data.for_node.body);
            return node;

        case NODE_ASSIGN:
            node->data.assign.value = optimizer_unreachable_code_elimination(node->data.assign.value);
            return node;

        case NODE_UNOP:
            node->data.unop.operand = optimizer_unreachable_code_elimination(node->data.unop.operand);
            return node;
        
        case NODE_CALL:
            node->data.unop.operand = optimizer_unreachable_code_elimination(node->data.unop.operand);
            return node;

        case NODE_BINOP:
            node->data.binop.left = optimizer_unreachable_code_elimination(node->data.binop.left);
            node->data.binop.right = optimizer_unreachable_code_elimination(node->data.binop.right);
            return node;

        case NODE_PRINT:
            node->data.print.arg = optimizer_unreachable_code_elimination(node->data.print.arg);
            return node;

        case NODE_RETURN:
            node->data.ret.value = optimizer_unreachable_code_elimination(node->data.ret.value);
            return node;

        case NODE_DECL:
            node->data.decl.init_value = optimizer_unreachable_code_elimination(node->data.decl.init_value);
            return node;

        default:
            return node;
    }

    return node;
}

/* ====================== ALGEBRAIC SIMPLIFICATION ====================== */

ASTNode *optimizer_algebraic_simplification(ASTNode *node) {
    if (!node) return node;

    /* If program node, apply to each statement */
    if (node->type == NODE_PROGRAM && node->data.block.stmts) {
        for (int i = 0; i < node->data.block.stmt_count; i++) {
            node->data.block.stmts[i] = optimizer_algebraic_simplification(node->data.block.stmts[i]);
        }
        return node;
    }

    switch (node->type) {
        case NODE_BINOP: {
            node->data.binop.left = optimizer_algebraic_simplification(node->data.binop.left);
            node->data.binop.right = optimizer_algebraic_simplification(node->data.binop.right);
            ASTNode *left = node->data.binop.left;
            ASTNode *right = node->data.binop.right;
            int op = node->data.binop.op;

            /* x + 0 = x */
            if (op == TOK_PLUS && right && right->type == NODE_INT && right->data.int_val == 0) {
                opt_stats.algebraic_simplifications++;
                node->optimized = 1;
                return left;
            }

            /* 0 + x = x */
            if (op == TOK_PLUS && left && left->type == NODE_INT && left->data.int_val == 0) {
                opt_stats.algebraic_simplifications++;
                node->optimized = 1;
                return right;
            }

            /* x * 0 = 0 */
            if (op == TOK_MUL && right && right->type == NODE_INT && right->data.int_val == 0) {
                opt_stats.algebraic_simplifications++;
                ASTNode *zero = ast_alloc_node(NODE_INT, node->line);
                zero->data.int_val = 0;
                return zero;
            }

            /* x * 1 = x */
            if (op == TOK_MUL && right && right->type == NODE_INT && right->data.int_val == 1) {
                opt_stats.algebraic_simplifications++;
                node->optimized = 1;
                return left;
            }

            /* x / 1 = x */
            if (op == TOK_DIV && right && right->type == NODE_INT && right->data.int_val == 1) {
                opt_stats.algebraic_simplifications++;
                node->optimized = 1;
                return left;
            }

            return node;
        }

        case NODE_UNOP:
            node->data.unop.operand = optimizer_algebraic_simplification(node->data.unop.operand);
            return node;
        
        case NODE_CALL:
            node->data.unop.operand = optimizer_algebraic_simplification(node->data.unop.operand);
            return node;

        case NODE_ASSIGN:
            node->data.assign.value = optimizer_algebraic_simplification(node->data.assign.value);
            return node;

        case NODE_IF:
            node->data.if_node.condition = optimizer_algebraic_simplification(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_algebraic_simplification(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_algebraic_simplification(node->data.if_node.else_branch);
            return node;

        case NODE_WHILE:
            node->data.while_node.condition = optimizer_algebraic_simplification(node->data.while_node.condition);
            node->data.while_node.body = optimizer_algebraic_simplification(node->data.while_node.body);
            return node;

        case NODE_FOR:
            node->data.for_node.init = optimizer_algebraic_simplification(node->data.for_node.init);
            node->data.for_node.condition = optimizer_algebraic_simplification(node->data.for_node.condition);
            node->data.for_node.update = optimizer_algebraic_simplification(node->data.for_node.update);
            node->data.for_node.body = optimizer_algebraic_simplification(node->data.for_node.body);
            return node;

        case NODE_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_algebraic_simplification(node->data.block.stmts[i]);
            }
            return node;

        case NODE_PRINT:
            node->data.print.arg = optimizer_algebraic_simplification(node->data.print.arg);
            return node;

        case NODE_RETURN:
            node->data.ret.value = optimizer_algebraic_simplification(node->data.ret.value);
            return node;

        case NODE_DECL:
            node->data.decl.init_value = optimizer_algebraic_simplification(node->data.decl.init_value);
            return node;

        default:
            return node;
    }
}

/* ====================== STRENGTH REDUCTION ====================== */

ASTNode *optimizer_strength_reduction(ASTNode *node) {
    if (!node) return node;

    if (node->type == NODE_PROGRAM && node->data.block.stmts) {
        for (int i = 0; i < node->data.block.stmt_count; i++) {
            node->data.block.stmts[i] = optimizer_strength_reduction(node->data.block.stmts[i]);
        }
        return node;
    }

    switch (node->type) {
        case NODE_BINOP: {
            node->data.binop.left = optimizer_strength_reduction(node->data.binop.left);
            node->data.binop.right = optimizer_strength_reduction(node->data.binop.right);
            ASTNode *left = node->data.binop.left;
            ASTNode *right = node->data.binop.right;
            int op = node->data.binop.op;

            /* x * 2 = x + x (strength reduction) */
            if (op == TOK_MUL && right && right->type == NODE_INT && right->data.int_val == 2) {
                opt_stats.strength_reductions++;
                node->optimized = 1;
                node->data.binop.op = TOK_PLUS;
                node->data.binop.right = left;  /* Replace * 2 with + x */
                return node;
            }

            /* x / 2 = x * 0.5 (strength reduction for division) */
            if (op == TOK_DIV && right && right->type == NODE_INT && right->data.int_val == 2) {
                opt_stats.strength_reductions++;
                node->optimized = 1;
                node->data.binop.op = TOK_MUL;
                ASTNode *half = ast_alloc_node(NODE_FLOAT, node->line);
                half->data.float_val = 0.5;
                node->data.binop.right = half;
                return node;
            }

            return node;
        }

        case NODE_UNOP:
            node->data.unop.operand = optimizer_strength_reduction(node->data.unop.operand);
            return node;

        case NODE_ASSIGN:
            node->data.assign.value = optimizer_strength_reduction(node->data.assign.value);
            return node;

        case NODE_IF:
            node->data.if_node.condition = optimizer_strength_reduction(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_strength_reduction(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_strength_reduction(node->data.if_node.else_branch);
            return node;

        case NODE_WHILE:
            node->data.while_node.condition = optimizer_strength_reduction(node->data.while_node.condition);
            node->data.while_node.body = optimizer_strength_reduction(node->data.while_node.body);
            return node;

        case NODE_FOR:
            node->data.for_node.init = optimizer_strength_reduction(node->data.for_node.init);
            node->data.for_node.condition = optimizer_strength_reduction(node->data.for_node.condition);
            node->data.for_node.update = optimizer_strength_reduction(node->data.for_node.update);
            node->data.for_node.body = optimizer_strength_reduction(node->data.for_node.body);
            return node;

        case NODE_BLOCK:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_strength_reduction(node->data.block.stmts[i]);
            }
            return node;

        case NODE_PRINT:
            node->data.print.arg = optimizer_strength_reduction(node->data.print.arg);
            return node;

        case NODE_RETURN:
            node->data.ret.value = optimizer_strength_reduction(node->data.ret.value);
            return node;

        case NODE_DECL:
            node->data.decl.init_value = optimizer_strength_reduction(node->data.decl.init_value);
            return node;

        default:
            return node;
    }
}

/* ====================== DEAD CODE ELIMINATION ====================== */

ASTNode *optimizer_dead_code_elimination(ASTNode *node) {
    if (!node) return node;
    
    if (node->type == NODE_BLOCK) {
        ASTNode **filtered = malloc(node->data.block.stmt_count * sizeof(ASTNode*));
        int new_count = 0;
        
        for (int i = 0; i < node->data.block.stmt_count; i++) {
            ASTNode *stmt = node->data.block.stmts[i];
            
            /* Skip variable declarations that are never used */
            if (stmt->type == NODE_DECL) {
                Symbol *s = symtab_lookup(stmt->data.decl.name);
                if (s && !s->used && !s->initialized) {
                    opt_stats.dead_code_eliminations++;
                    continue;
                }
            }
            
            filtered[new_count++] = stmt;
        }
        
        free(node->data.block.stmts);
        node->data.block.stmts = filtered;
        node->data.block.stmt_count = new_count;
    }
    
    return node;
}

/* ====================== CONSTANT PROPAGATION ====================== */

ASTNode *optimizer_constant_propagation(ASTNode *node) {
    if (!node) return node;

    switch (node->type) {
        case NODE_VAR: {
            Symbol *s = symtab_lookup(node->data.var.name);
            if (s && s->is_constant) {
                ASTNode *const_node = NULL;
                if (s->type == TYPE_INT) {
                    const_node = ast_alloc_node(NODE_INT, node->line);
                    const_node->data.int_val = s->value.int_val;
                } else if (s->type == TYPE_FLOAT) {
                    const_node = ast_alloc_node(NODE_FLOAT, node->line);
                    const_node->data.float_val = s->value.float_val;
                }
                if (const_node) {
                    const_node->optimized = 1;
                    opt_stats.constant_propagations++;
                    return const_node;
                }
            }
            return node;
        }
        case NODE_BINOP:
            node->data.binop.left = optimizer_constant_propagation(node->data.binop.left);
            node->data.binop.right = optimizer_constant_propagation(node->data.binop.right);
            return node;
        case NODE_UNOP:
            node->data.unop.operand = optimizer_constant_propagation(node->data.unop.operand);
            return node;
        case NODE_ASSIGN:
            node->data.assign.value = optimizer_constant_propagation(node->data.assign.value);
            return node;
        case NODE_DECL:
            node->data.decl.init_value = optimizer_constant_propagation(node->data.decl.init_value);
            return node;
        case NODE_IF:
            node->data.if_node.condition = optimizer_constant_propagation(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_constant_propagation(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_constant_propagation(node->data.if_node.else_branch);
            return node;
        case NODE_WHILE:
            node->data.while_node.condition = optimizer_constant_propagation(node->data.while_node.condition);
            node->data.while_node.body = optimizer_constant_propagation(node->data.while_node.body);
            return node;
        case NODE_FOR:
            node->data.for_node.init = optimizer_constant_propagation(node->data.for_node.init);
            node->data.for_node.condition = optimizer_constant_propagation(node->data.for_node.condition);
            node->data.for_node.update = optimizer_constant_propagation(node->data.for_node.update);
            node->data.for_node.body = optimizer_constant_propagation(node->data.for_node.body);
            return node;
        case NODE_BLOCK:
        case NODE_PROGRAM:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_constant_propagation(node->data.block.stmts[i]);
            }
            return node;
        case NODE_PRINT:
            node->data.print.arg = optimizer_constant_propagation(node->data.print.arg);
            return node;
        case NODE_RETURN:
            node->data.ret.value = optimizer_constant_propagation(node->data.ret.value);
            return node;
        default:
            return node;
    }
}

/* ====================== COPY PROPAGATION ====================== */

ASTNode *optimizer_copy_propagation(ASTNode *node) {
    if (!node) return node;

    switch (node->type) {
        case NODE_ASSIGN: {
            node->data.assign.value = optimizer_copy_propagation(node->data.assign.value);
            ASTNode *value = node->data.assign.value;
            if (value && (value->type == NODE_INT || value->type == NODE_FLOAT)) {
                if (value->type == NODE_INT) {
                    symtab_set_constant_value(node->data.assign.varname, value->data.int_val, (double)value->data.int_val);
                } else {
                    symtab_set_constant_value(node->data.assign.varname, (long)value->data.float_val, value->data.float_val);
                }
                opt_stats.copy_propagations++;
            } else {
                symtab_clear_constant_value(node->data.assign.varname);
            }
            return node;
        }
        case NODE_DECL:
            node->data.decl.init_value = optimizer_copy_propagation(node->data.decl.init_value);
            return node;
        case NODE_BINOP:
            node->data.binop.left = optimizer_copy_propagation(node->data.binop.left);
            node->data.binop.right = optimizer_copy_propagation(node->data.binop.right);
            return node;
        case NODE_UNOP:
            node->data.unop.operand = optimizer_copy_propagation(node->data.unop.operand);
            return node;
        case NODE_IF:
            node->data.if_node.condition = optimizer_copy_propagation(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_copy_propagation(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_copy_propagation(node->data.if_node.else_branch);
            return node;
        case NODE_WHILE:
            node->data.while_node.condition = optimizer_copy_propagation(node->data.while_node.condition);
            node->data.while_node.body = optimizer_copy_propagation(node->data.while_node.body);
            return node;
        case NODE_FOR:
            node->data.for_node.init = optimizer_copy_propagation(node->data.for_node.init);
            node->data.for_node.condition = optimizer_copy_propagation(node->data.for_node.condition);
            node->data.for_node.update = optimizer_copy_propagation(node->data.for_node.update);
            node->data.for_node.body = optimizer_copy_propagation(node->data.for_node.body);
            return node;
        case NODE_BLOCK:
        case NODE_PROGRAM:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_copy_propagation(node->data.block.stmts[i]);
            }
            return node;
        case NODE_PRINT:
            node->data.print.arg = optimizer_copy_propagation(node->data.print.arg);
            return node;
        case NODE_RETURN:
            node->data.ret.value = optimizer_copy_propagation(node->data.ret.value);
            return node;
        default:
            return node;
    }
}

/* ====================== COMMON SUBEXPRESSION ELIMINATION (CSE) ====================== */

/* Expression table entry for CSE */
typedef struct {
    char expr_key[256];      /* String representation of expression */
    char var_name[128];      /* Variable holding the computed result */
    int available;           /* Is this expression still valid? */
} CSEEntry;

#define MAX_CSE_ENTRIES 100
static CSEEntry cse_table[MAX_CSE_ENTRIES];
static int cse_count = 0;

static void cse_init(void) {
    cse_count = 0;
    memset(cse_table, 0, sizeof(cse_table));
}

/* Create a key for an expression tree */
static void build_expr_key(ASTNode *node, char *key, int max_len) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_INT:
            snprintf(key + strlen(key), max_len - strlen(key), "INT_%ld", node->data.int_val);
            break;
        case NODE_FLOAT:
            snprintf(key + strlen(key), max_len - strlen(key), "FLT_%f", node->data.float_val);
            break;
        case NODE_VAR:
            snprintf(key + strlen(key), max_len - strlen(key), "VAR_%s", node->data.var.name);
            break;
        case NODE_BINOP:
            strncat(key, "(", max_len - strlen(key) - 1);
            build_expr_key(node->data.binop.left, key, max_len);
            snprintf(key + strlen(key), max_len - strlen(key), "_OP%d_", node->data.binop.op);
            build_expr_key(node->data.binop.right, key, max_len);
            strncat(key, ")", max_len - strlen(key) - 1);
            break;
        default:
            strncat(key, "UNKNOWN", max_len - strlen(key) - 1);
    }
}

/* Look up expression in CSE table - returns var name if found, NULL otherwise */
static const char *cse_lookup(ASTNode *expr) {
    char key[256] = "";
    build_expr_key(expr, key, sizeof(key));
    
    for (int i = 0; i < cse_count; i++) {
        if (cse_table[i].available && strcmp(cse_table[i].expr_key, key) == 0) {
            return cse_table[i].var_name;
        }
    }
    return NULL;
}

/* Add expression to CSE table */
static void cse_add(ASTNode *expr, const char *var_name) {
    if (cse_count >= MAX_CSE_ENTRIES) return;
    
    char key[256] = "";
    build_expr_key(expr, key, sizeof(key));
    
    strncpy(cse_table[cse_count].expr_key, key, 255);
    strncpy(cse_table[cse_count].var_name, var_name, 127);
    cse_table[cse_count].available = 1;
    cse_count++;
}

/* Invalidate CSE entries that depend on a variable */
static void cse_invalidate(const char *var_name) {
    for (int i = 0; i < cse_count; i++) {
        if (cse_table[i].available && strstr(cse_table[i].expr_key, var_name) != NULL) {
            cse_table[i].available = 0;  /* Mark as invalid */
        }
    }
}

ASTNode *optimizer_common_subexpression_elimination(ASTNode *node) {
    if (!node) return node;
    
    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK: {
            /* Initialize CSE table at block scope */
            int saved_cse_count = cse_count;
            
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_common_subexpression_elimination(node->data.block.stmts[i]);
            }
            
            /* Restore CSE table */
            cse_count = saved_cse_count;
            return node;
        }
        
        case NODE_ASSIGN: {
            node->data.assign.value = optimizer_common_subexpression_elimination(node->data.assign.value);
            
            /* Invalidate entries using this variable */
            cse_invalidate(node->data.assign.varname);
            
            /* Check if the assigned expression can be CSE'd */
            if (node->data.assign.value->type == NODE_BINOP) {
                const char *existing = cse_lookup(node->data.assign.value);
                if (existing) {
                    /* Replace with reference to existing computation */
                    ASTNode *var_ref = ast_alloc_node(NODE_VAR, node->line);
                    strncpy(var_ref->data.var.name, existing, 127);
                    node->data.assign.value = var_ref;
                    opt_stats.cse_eliminations++;
                    node->optimized = 1;
                } else {
                    /* Add this expression to CSE table */
                    cse_add(node->data.assign.value, node->data.assign.varname);
                }
            }
            return node;
        }
        
        case NODE_BINOP: {
            node->data.binop.left = optimizer_common_subexpression_elimination(node->data.binop.left);
            node->data.binop.right = optimizer_common_subexpression_elimination(node->data.binop.right);
            
            /* Check if this expression was computed before */
            const char *existing = cse_lookup(node);
            if (existing) {
                /* Replace with variable reference */
                ASTNode *var_ref = ast_alloc_node(NODE_VAR, node->line);
                strncpy(var_ref->data.var.name, existing, 127);
                opt_stats.cse_eliminations++;
                node->optimized = 1;
                return var_ref;
            }
            return node;
        }
        
        case NODE_DECL: {
            if (node->data.decl.init_value) {
                node->data.decl.init_value = optimizer_common_subexpression_elimination(node->data.decl.init_value);
                
                /* Add to CSE table if it's a binary expression */
                if (node->data.decl.init_value->type == NODE_BINOP) {
                    cse_add(node->data.decl.init_value, node->data.decl.name);
                }
            }
            return node;
        }
        
        case NODE_IF: {
            int saved_cse_count = cse_count;
            node->data.if_node.condition = optimizer_common_subexpression_elimination(node->data.if_node.condition);
            node->data.if_node.then_branch = optimizer_common_subexpression_elimination(node->data.if_node.then_branch);
            node->data.if_node.else_branch = optimizer_common_subexpression_elimination(node->data.if_node.else_branch);
            cse_count = saved_cse_count;
            return node;
        }
        
        case NODE_WHILE: {
            int saved_cse_count = cse_count;
            node->data.while_node.condition = optimizer_common_subexpression_elimination(node->data.while_node.condition);
            node->data.while_node.body = optimizer_common_subexpression_elimination(node->data.while_node.body);
            cse_count = saved_cse_count;
            return node;
        }
        
        case NODE_FOR: {
            int saved_cse_count = cse_count;
            node->data.for_node.init = optimizer_common_subexpression_elimination(node->data.for_node.init);
            node->data.for_node.condition = optimizer_common_subexpression_elimination(node->data.for_node.condition);
            node->data.for_node.update = optimizer_common_subexpression_elimination(node->data.for_node.update);
            node->data.for_node.body = optimizer_common_subexpression_elimination(node->data.for_node.body);
            cse_count = saved_cse_count;
            return node;
        }
        
        default:
            return node;
    }
}

/* ====================== LOOP INVARIANT CODE MOTION ====================== */

static int is_constant_expr(ASTNode *node) {
    if (!node) return 0;
    switch (node->type) {
        case NODE_INT:
        case NODE_FLOAT:
        case NODE_CHAR:
            return 1;
        case NODE_UNOP:
            return is_constant_expr(node->data.unop.operand);
        case NODE_BINOP:
            return is_constant_expr(node->data.binop.left) &&
                   is_constant_expr(node->data.binop.right);
        default:
            return 0;
    }
}

static int stmt_assigns_var(ASTNode *stmt, const char *varname) {
    if (!stmt) return 0;
    switch (stmt->type) {
        case NODE_ASSIGN:
            return strcmp(stmt->data.assign.varname, varname) == 0;
        case NODE_IF:
            return stmt_assigns_var(stmt->data.if_node.then_branch, varname) ||
                   stmt_assigns_var(stmt->data.if_node.else_branch, varname);
        case NODE_WHILE:
            return stmt_assigns_var(stmt->data.while_node.body, varname);
        case NODE_FOR:
            return stmt_assigns_var(stmt->data.for_node.init, varname) ||
                   stmt_assigns_var(stmt->data.for_node.update, varname) ||
                   stmt_assigns_var(stmt->data.for_node.body, varname);
        case NODE_BLOCK:
        case NODE_PROGRAM:
            for (int i = 0; i < stmt->data.block.stmt_count; i++) {
                if (stmt_assigns_var(stmt->data.block.stmts[i], varname)) return 1;
            }
            return 0;
        default:
            return 0;
    }
}

static int var_assigned_later(ASTNode *block, int start_idx, const char *varname) {
    if (!block || (block->type != NODE_BLOCK && block->type != NODE_PROGRAM)) return 0;
    for (int i = start_idx; i < block->data.block.stmt_count; i++) {
        if (stmt_assigns_var(block->data.block.stmts[i], varname)) {
            return 1;
        }
    }
    return 0;
}

static int hoist_loop_invariants(ASTNode *loop_node, ASTNode ***hoisted_out) {
    *hoisted_out = NULL;
    if (!loop_node || (loop_node->type != NODE_WHILE && loop_node->type != NODE_FOR)) {
        return 0;
    }

    ASTNode *body = (loop_node->type == NODE_WHILE)
        ? loop_node->data.while_node.body
        : loop_node->data.for_node.body;

    if (!body || body->type != NODE_BLOCK || body->data.block.stmt_count == 0) {
        return 0;
    }

    int move_count = 0;
    while (move_count < body->data.block.stmt_count) {
        ASTNode *s = body->data.block.stmts[move_count];
        if (!s || s->type != NODE_ASSIGN) break;
        if (!is_constant_expr(s->data.assign.value)) break;
        if (var_assigned_later(body, move_count + 1, s->data.assign.varname)) break;
        move_count++;
    }

    if (move_count == 0) return 0;

    ASTNode **hoisted = malloc(move_count * sizeof(ASTNode *));
    for (int i = 0; i < move_count; i++) {
        hoisted[i] = body->data.block.stmts[i];
        hoisted[i]->optimized = 1;
    }

    int remaining = body->data.block.stmt_count - move_count;
    ASTNode **new_body = NULL;
    if (remaining > 0) {
        new_body = malloc(remaining * sizeof(ASTNode *));
        for (int i = 0; i < remaining; i++) {
            new_body[i] = body->data.block.stmts[move_count + i];
        }
    }

    free(body->data.block.stmts);
    body->data.block.stmts = new_body;
    body->data.block.stmt_count = remaining;
    *hoisted_out = hoisted;
    return move_count;
}

ASTNode *optimizer_loop_invariant_motion(ASTNode *node) {
    if (!node) return node;
    if (node->type == NODE_WHILE) {
        node->data.while_node.condition = optimizer_loop_invariant_motion(node->data.while_node.condition);
        node->data.while_node.body = optimizer_loop_invariant_motion(node->data.while_node.body);
    } else if (node->type == NODE_FOR) {
        node->data.for_node.init = optimizer_loop_invariant_motion(node->data.for_node.init);
        node->data.for_node.condition = optimizer_loop_invariant_motion(node->data.for_node.condition);
        node->data.for_node.update = optimizer_loop_invariant_motion(node->data.for_node.update);
        node->data.for_node.body = optimizer_loop_invariant_motion(node->data.for_node.body);
    } else if (node->type == NODE_BLOCK || node->type == NODE_PROGRAM) {
        int cap = node->data.block.stmt_count + 8;
        ASTNode **rebuilt = malloc(cap * sizeof(ASTNode *));
        int rebuilt_count = 0;

        for (int i = 0; i < node->data.block.stmt_count; i++) {
            ASTNode *stmt = optimizer_loop_invariant_motion(node->data.block.stmts[i]);
            if (stmt && (stmt->type == NODE_WHILE || stmt->type == NODE_FOR)) {
                ASTNode **hoisted = NULL;
                int moved = hoist_loop_invariants(stmt, &hoisted);
                for (int h = 0; h < moved; h++) {
                    if (rebuilt_count >= cap) {
                        cap *= 2;
                        rebuilt = realloc(rebuilt, cap * sizeof(ASTNode *));
                    }
                    rebuilt[rebuilt_count++] = hoisted[h];
                    opt_stats.loop_invariants_moved++;
                }
                if (hoisted) free(hoisted);
            }
            if (rebuilt_count >= cap) {
                cap *= 2;
                rebuilt = realloc(rebuilt, cap * sizeof(ASTNode *));
            }
            rebuilt[rebuilt_count++] = stmt;
        }
        free(node->data.block.stmts);
        node->data.block.stmts = rebuilt;
        node->data.block.stmt_count = rebuilt_count;
    }
    return node;
}

/* ====================== INDUCTION VARIABLE ELIMINATION ====================== */

/* Detect if a variable is a simple induction variable in a for loop */
static int is_induction_variable(ASTNode *for_node, const char *var_name) {
    if (!for_node || for_node->type != NODE_FOR) return 0;
    
    /* Check if update increments by constant */
    ASTNode *update = for_node->data.for_node.update;
    if (!update) return 0;
    
    /* Look for patterns like: i = i + 1, i = i + k (where k is constant) */
    if (update->type == NODE_ASSIGN) {
        if (strcmp(update->data.assign.varname, var_name) == 0) {
            ASTNode *value = update->data.assign.value;
            if (value->type == NODE_BINOP) {
                /* Check for i + constant or i - constant */
                if (value->data.binop.left->type == NODE_VAR &&
                    strcmp(value->data.binop.left->data.var.name, var_name) == 0) {
                    if (value->data.binop.right->type == NODE_INT || 
                        value->data.binop.right->type == NODE_FLOAT) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/* Replace induction variable uses with computed value */
static ASTNode *replace_induction_uses(ASTNode *node, const char *ind_var, 
                                        const char *init_val, int step) {
    if (!node) return node;
    
    switch (node->type) {
        case NODE_VAR:
            if (strcmp(node->data.var.name, ind_var) == 0) {
                /* Replace with expression: init + iteration * step */
                /* This is simplified - full implementation would track loop iteration */
                opt_stats.induction_vars_eliminated++;
                node->optimized = 1;
            }
            return node;
            
        case NODE_BINOP:
            node->data.binop.left = replace_induction_uses(node->data.binop.left, ind_var, init_val, step);
            node->data.binop.right = replace_induction_uses(node->data.binop.right, ind_var, init_val, step);
            return node;
            
        case NODE_ASSIGN:
            node->data.assign.value = replace_induction_uses(node->data.assign.value, ind_var, init_val, step);
            return node;
            
        case NODE_PRINT:
            node->data.print.arg = replace_induction_uses(node->data.print.arg, ind_var, init_val, step);
            return node;
            
        case NODE_BLOCK:
        case NODE_PROGRAM:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = replace_induction_uses(node->data.block.stmts[i], ind_var, init_val, step);
            }
            return node;
            
        default:
            return node;
    }
}

ASTNode *optimizer_induction_elimination(ASTNode *node) {
    if (!node) return node;
    
    if (node->type == NODE_FOR) {
        node->data.for_node.init = optimizer_induction_elimination(node->data.for_node.init);
        node->data.for_node.condition = optimizer_induction_elimination(node->data.for_node.condition);
        node->data.for_node.update = optimizer_induction_elimination(node->data.for_node.update);
        node->data.for_node.body = optimizer_induction_elimination(node->data.for_node.body);
        
        /* Try to identify and optimize induction variables */
        ASTNode *init = node->data.for_node.init;
        if (init && init->type == NODE_ASSIGN) {
            const char *loop_var = init->data.assign.varname;
            
            if (is_induction_variable(node, loop_var)) {
                /* Get initial value as string for replacement tracking */
                ASTNode *init_val = init->data.assign.value;
                if (init_val->type == NODE_INT) {
                    char val_str[32];
                    snprintf(val_str, sizeof(val_str), "%ld", init_val->data.int_val);
                    node->data.for_node.body = replace_induction_uses(node->data.for_node.body, loop_var, val_str, 1);
                }
            }
        }
    } else if (node->type == NODE_WHILE) {
        node->data.while_node.condition = optimizer_induction_elimination(node->data.while_node.condition);
        node->data.while_node.body = optimizer_induction_elimination(node->data.while_node.body);
    } else if (node->type == NODE_BLOCK || node->type == NODE_PROGRAM) {
        for (int i = 0; i < node->data.block.stmt_count; i++) {
            node->data.block.stmts[i] = optimizer_induction_elimination(node->data.block.stmts[i]);
        }
    } else if (node->type == NODE_IF) {
        node->data.if_node.condition = optimizer_induction_elimination(node->data.if_node.condition);
        node->data.if_node.then_branch = optimizer_induction_elimination(node->data.if_node.then_branch);
        node->data.if_node.else_branch = optimizer_induction_elimination(node->data.if_node.else_branch);
    } else if (node->type == NODE_BINOP) {
        node->data.binop.left = optimizer_induction_elimination(node->data.binop.left);
        node->data.binop.right = optimizer_induction_elimination(node->data.binop.right);
    } else if (node->type == NODE_UNOP) {
        node->data.unop.operand = optimizer_induction_elimination(node->data.unop.operand);
    } else if (node->type == NODE_ASSIGN) {
        node->data.assign.value = optimizer_induction_elimination(node->data.assign.value);
    } else if (node->type == NODE_DECL) {
        if (node->data.decl.init_value) {
            node->data.decl.init_value = optimizer_induction_elimination(node->data.decl.init_value);
        }
    }
    
    return node;
}

/* ====================== LOOP STRENGTH REDUCTION ====================== */

static ASTNode *optimizer_loop_strength_reduction_impl(ASTNode *node, int in_loop) {
    if (!node) return node;

    switch (node->type) {
        case NODE_BINOP: {
            node->data.binop.left = optimizer_loop_strength_reduction_impl(node->data.binop.left, in_loop);
            node->data.binop.right = optimizer_loop_strength_reduction_impl(node->data.binop.right, in_loop);

            if (in_loop && node->data.binop.right && node->data.binop.right->type == NODE_INT) {
                if (node->data.binop.op == TOK_MUL && node->data.binop.right->data.int_val == 2) {
                    node->optimized = 1;
                    node->data.binop.op = TOK_PLUS;
                    node->data.binop.right = node->data.binop.left;
                    opt_stats.loop_strength_reductions++;
                } else if (node->data.binop.op == TOK_DIV && node->data.binop.right->data.int_val == 2) {
                    ASTNode *half = ast_alloc_node(NODE_FLOAT, node->line);
                    half->data.float_val = 0.5;
                    node->optimized = 1;
                    node->data.binop.op = TOK_MUL;
                    node->data.binop.right = half;
                    opt_stats.loop_strength_reductions++;
                }
            }
            return node;
        }
        case NODE_UNOP:
            node->data.unop.operand = optimizer_loop_strength_reduction_impl(node->data.unop.operand, in_loop);
            return node;
        case NODE_ASSIGN:
            node->data.assign.value = optimizer_loop_strength_reduction_impl(node->data.assign.value, in_loop);
            return node;
        case NODE_DECL:
            node->data.decl.init_value = optimizer_loop_strength_reduction_impl(node->data.decl.init_value, in_loop);
            return node;
        case NODE_IF:
            node->data.if_node.condition = optimizer_loop_strength_reduction_impl(node->data.if_node.condition, in_loop);
            node->data.if_node.then_branch = optimizer_loop_strength_reduction_impl(node->data.if_node.then_branch, in_loop);
            node->data.if_node.else_branch = optimizer_loop_strength_reduction_impl(node->data.if_node.else_branch, in_loop);
            return node;
        case NODE_WHILE:
            node->data.while_node.condition = optimizer_loop_strength_reduction_impl(node->data.while_node.condition, in_loop);
            node->data.while_node.body = optimizer_loop_strength_reduction_impl(node->data.while_node.body, 1);
            return node;
        case NODE_FOR:
            node->data.for_node.init = optimizer_loop_strength_reduction_impl(node->data.for_node.init, in_loop);
            node->data.for_node.condition = optimizer_loop_strength_reduction_impl(node->data.for_node.condition, in_loop);
            node->data.for_node.update = optimizer_loop_strength_reduction_impl(node->data.for_node.update, 1);
            node->data.for_node.body = optimizer_loop_strength_reduction_impl(node->data.for_node.body, 1);
            return node;
        case NODE_BLOCK:
        case NODE_PROGRAM:
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                node->data.block.stmts[i] = optimizer_loop_strength_reduction_impl(node->data.block.stmts[i], in_loop);
            }
            return node;
        case NODE_PRINT:
            node->data.print.arg = optimizer_loop_strength_reduction_impl(node->data.print.arg, in_loop);
            return node;
        case NODE_RETURN:
            node->data.ret.value = optimizer_loop_strength_reduction_impl(node->data.ret.value, in_loop);
            return node;
        default:
            return node;
    }
}

ASTNode *optimizer_loop_strength_reduction(ASTNode *node) {
    return optimizer_loop_strength_reduction_impl(node, 0);
}

/* ====================== LOOP UNROLLING ====================== */

/* Duplicate an AST node (shallow copy for unrolling) */
static ASTNode *ast_clone_node(ASTNode *node) {
    if (!node) return NULL;
    
    ASTNode *clone = ast_alloc_node(node->type, node->line);
    clone->optimized = node->optimized;
    
    /* Copy the data union based on node type */
    switch (node->type) {
        case NODE_INT:
            clone->data.int_val = node->data.int_val;
            break;
        case NODE_FLOAT:
            clone->data.float_val = node->data.float_val;
            break;
        case NODE_VAR:
            strncpy(clone->data.var.name, node->data.var.name, 127);
            break;
        case NODE_BINOP:
            clone->data.binop.op = node->data.binop.op;
            clone->data.binop.left = node->data.binop.left;
            clone->data.binop.right = node->data.binop.right;
            break;
        case NODE_UNOP:
            clone->data.unop.op = node->data.unop.op;
            clone->data.unop.operand = node->data.unop.operand;
            break;
        case NODE_ASSIGN:
            strncpy(clone->data.assign.varname, node->data.assign.varname, 127);
            clone->data.assign.value = node->data.assign.value;
            break;
        case NODE_PRINT:
            clone->data.print.arg = node->data.print.arg;
            break;
        case NODE_RETURN:
            clone->data.ret.value = node->data.ret.value;
            break;
        case NODE_DECL:
            clone->data.decl.dtype = node->data.decl.dtype;
            strncpy(clone->data.decl.name, node->data.decl.name, 127);
            clone->data.decl.init_value = node->data.decl.init_value;
            break;
        case NODE_BLOCK:
        case NODE_PROGRAM:
            clone->data.block.stmt_count = node->data.block.stmt_count;
            if (node->data.block.stmts) {
                clone->data.block.stmts = malloc(node->data.block.stmt_count * sizeof(ASTNode *));
                for (int i = 0; i < node->data.block.stmt_count; i++) {
                    clone->data.block.stmts[i] = node->data.block.stmts[i];
                }
            }
            break;
        default:
            break;
    }
    
    return clone;
}

/* Check if a for loop can be unrolled (simple iteration with known bounds) */
static int can_unroll_loop(ASTNode *for_node) {
    if (!for_node || for_node->type != NODE_FOR) return 0;
    
    ASTNode *init = for_node->data.for_node.init;
    ASTNode *update = for_node->data.for_node.update;
    
    /* Need init and update to be simple assignments */
    if (!init || init->type != NODE_ASSIGN) return 0;
    if (!update || update->type != NODE_ASSIGN) return 0;
    
    /* Update should be simple increment */
    ASTNode *update_val = update->data.assign.value;
    if (update_val->type != NODE_BINOP) return 0;
    if (update_val->data.binop.op != TOK_PLUS && update_val->data.binop.op != TOK_MINUS) return 0;
    if (update_val->data.binop.right->type != NODE_INT) return 0;
    
    return 1;
}

ASTNode *optimizer_loop_unrolling(ASTNode *node, int unroll_factor) {
    if (!node || node->type != NODE_FOR) {
        return node;
    }
    
    /* Check if loop can be unrolled */
    if (!can_unroll_loop(node)) {
        return node;
    }
    
    ASTNode *body = node->data.for_node.body;
    if (!body || body->type != NODE_BLOCK || body->data.block.stmt_count == 0) {
        return node;
    }
    
    /* Don't unroll if body is too large */
    if (body->data.block.stmt_count > 5) {
        return node;
    }
    
    /* For now, perform 2x unrolling */
    int unroll_count = 2;
    
    /* Create unrolled body with duplicated statements */
    int orig_count = body->data.block.stmt_count;
    int new_count = orig_count * unroll_count;
    
    ASTNode **new_stmts = malloc(new_count * sizeof(ASTNode *));
    
    for (int u = 0; u < unroll_count; u++) {
        for (int i = 0; i < orig_count; i++) {
            /* Clone each statement for the unrolled iteration */
            new_stmts[u * orig_count + i] = ast_clone_node(body->data.block.stmts[i]);
        }
    }
    
    /* Mark as optimized */
    for (int i = 0; i < new_count; i++) {
        if (new_stmts[i]) new_stmts[i]->optimized = 1;
    }
    
    /* Update body with unrolled statements */
    free(body->data.block.stmts);
    body->data.block.stmts = new_stmts;
    body->data.block.stmt_count = new_count;
    
    /* Adjust update step for unrolling (e.g., i = i + 1 becomes i = i + 2) */
    ASTNode *update = node->data.for_node.update;
    if (update && update->type == NODE_ASSIGN) {
        ASTNode *update_val = update->data.assign.value;
        if (update_val->type == NODE_BINOP && update_val->data.binop.right->type == NODE_INT) {
            update_val->data.binop.right->data.int_val *= unroll_count;
        }
    }
    
    node->optimized = 1;
    return node;
}

/* ====================== MAIN OPTIMIZATION FUNCTION ====================== */

ASTNode *optimizer_optimize(ASTNode *ast) {
    if (!ast) return ast;
    
    /* Reset statistics */
    memset(&opt_stats, 0, sizeof(opt_stats));
    /* If the AST is a program node, run passes on each top-level statement */
    if (ast->type == NODE_PROGRAM && ast->data.block.stmts) {
        for (int i = 0; i < ast->data.block.stmt_count; i++) {
            ASTNode *s = ast->data.block.stmts[i];
            s = optimizer_constant_folding(s);
            s = optimizer_algebraic_simplification(s);
            s = optimizer_loop_invariant_motion(s);
            s = optimizer_loop_strength_reduction(s);
            s = optimizer_strength_reduction(s);
            s = optimizer_constant_propagation(s);
            s = optimizer_copy_propagation(s);
            s = optimizer_dead_code_elimination(s);
            s = optimizer_unreachable_code_elimination(s);
            s = optimizer_common_subexpression_elimination(s);
            s = optimizer_induction_elimination(s);
            s = optimizer_loop_unrolling(s, 2);
            ast->data.block.stmts[i] = s;
        }
        return ast;
    }

    /* Apply optimization passes for non-program roots */
    ast = optimizer_constant_folding(ast);
    ast = optimizer_algebraic_simplification(ast);
    ast = optimizer_loop_invariant_motion(ast);
    ast = optimizer_loop_strength_reduction(ast);
    ast = optimizer_strength_reduction(ast);
    ast = optimizer_constant_propagation(ast);
    ast = optimizer_copy_propagation(ast);
    ast = optimizer_dead_code_elimination(ast);
    ast = optimizer_unreachable_code_elimination(ast);
    ast = optimizer_common_subexpression_elimination(ast);
    ast = optimizer_induction_elimination(ast);
    ast = optimizer_loop_unrolling(ast, 2);
    
    return ast;
}

/* ====================== STATISTICS ====================== */

OptimizationStats optimizer_get_stats(void) {
    return opt_stats;
}

void optimizer_print_stats(void) {
    printf("\n============ OPTIMIZATION STATISTICS ============\n");
    printf("Constant Foldings:           %d\n", opt_stats.constant_folds);
    printf("Constant Propagations:       %d\n", opt_stats.constant_propagations);
    printf("Dead Code Eliminations:      %d\n", opt_stats.dead_code_eliminations);
    printf("Unreachable Code Eliminations: %d\n", opt_stats.unreachable_eliminations);
    printf("Algebraic Simplifications:   %d\n", opt_stats.algebraic_simplifications);
    printf("Strength Reductions:         %d\n", opt_stats.strength_reductions);
    printf("Copy Propagations:           %d\n", opt_stats.copy_propagations);
    printf("Common Subexpression Elmns:  %d\n", opt_stats.cse_eliminations);
    printf("Loop Invariant Motions:      %d\n", opt_stats.loop_invariants_moved);
    printf("Induction Variable Elmns:    %d\n", opt_stats.induction_vars_eliminated);
    printf("Loop Strength Reductions:    %d\n", opt_stats.loop_strength_reductions);
    printf("=================================================\n");
}
