/* Module 8: LLVM IR Generator Driver
 * 
 * This program generates LLVM IR from the compiler's AST.
 * It demonstrates LLVM IR generation as required by CS-346.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llvm_ir.h"

/* Include the types from the main compiler */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID,
    TYPE_UNKNOWN
} DataType;

typedef enum {
    NODE_INT, NODE_FLOAT, NODE_CHAR, NODE_STRING,
    NODE_VAR, NODE_BINOP, NODE_UNOP,
    NODE_ASSIGN, NODE_IF, NODE_WHILE, NODE_FOR,
    NODE_PRINT, NODE_RETURN,
    NODE_BLOCK, NODE_PROGRAM,
    NODE_DECL
} NodeType;

/* Simplified AST structure matching the compiler */
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
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_node;
        
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_node;
        
        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *update;
            struct ASTNode *body;
        } for_node;
        
        struct {
            struct ASTNode **stmts;
            int stmt_count;
        } block;
        
        struct {
            struct ASTNode *arg;
        } print;
        
        struct {
            struct ASTNode *value;
        } ret;
        
        struct {
            DataType dtype;
            char name[128];
            struct ASTNode *init_value;
        } decl;
        
    } data;
} ASTNode;

/* Forward declarations */
static const char *generate_llvm_expr(LLVMModule *mod, ASTNode *node);
static void generate_llvm_stmt(LLVMModule *mod, ASTNode *node);

/* Map our types to LLVM types */
static LLVMType map_type(DataType dt) {
    switch (dt) {
        case TYPE_INT: return LLVM_I32;
        case TYPE_FLOAT: return LLVM_FLOAT;
        case TYPE_CHAR: return LLVM_I8;
        default: return LLVM_I32;
    }
}

/* Get operator string */
static const char *get_op_str(int op) {
    switch (op) {
        case 29: return "+";  /* TOK_PLUS */
        case 30: return "-";  /* TOK_MINUS */
        case 31: return "*";  /* TOK_MUL */
        case 32: return "/";  /* TOK_DIV */
        case 33: return "%";  /* TOK_MOD */
        case 34: return "<";  /* TOK_LT */
        case 35: return "<="; /* TOK_LE */
        case 36: return ">";  /* TOK_GT */
        case 37: return ">="; /* TOK_GE */
        case 38: return "=="; /* TOK_EQ */
        case 39: return "!="; /* TOK_NEQ */
        case 40: return "&&"; /* TOK_AND */
        case 41: return "||"; /* TOK_OR */
        default: return "+";
    }
}

/* Get LLVM instruction type for binary op */
static LLVMInstType get_llvm_binop(int op, int is_float) {
    if (is_float) {
        switch (op) {
            case 29: return LLVM_FADD;
            case 30: return LLVM_FSUB;
            case 31: return LLVM_FMUL;
            case 32: return LLVM_FDIV;
            default: return LLVM_FADD;
        }
    } else {
        switch (op) {
            case 29: return LLVM_ADD;
            case 30: return LLVM_SUB;
            case 31: return LLVM_MUL;
            case 32: return LLVM_SDIV;
            case 33: return LLVM_SREM;
            default: return LLVM_ADD;
        }
    }
}

/* Generate LLVM code for an expression */
static const char *generate_llvm_expr(LLVMModule *mod, ASTNode *node) {
    static char result_str[256];
    
    if (!node) {
        strcpy(result_str, "0");
        return result_str;
    }
    
    switch (node->type) {
        case NODE_INT: {
            snprintf(result_str, sizeof(result_str), "%ld", node->data.int_val);
            return result_str;
        }
        
        case NODE_FLOAT: {
            snprintf(result_str, sizeof(result_str), "%f", node->data.float_val);
            return result_str;
        }
        
        case NODE_VAR: {
            /* Load variable */
            const char *temp = llvm_get_temp(mod);
            char var_ptr[256];
            snprintf(var_ptr, sizeof(var_ptr), "%%%s", node->data.var.name);
            llvm_emit_load(mod, temp, LLVM_I32, var_ptr);
            strncpy(result_str, temp, sizeof(result_str)-1);
            return result_str;
        }
        
        case NODE_BINOP: {
            const char *left = generate_llvm_expr(mod, node->data.binop.left);
            const char *right = generate_llvm_expr(mod, node->data.binop.right);
            const char *result = llvm_get_temp(mod);
            
            LLVMInstType op = get_llvm_binop(node->data.binop.op, 0);
            llvm_emit_binary(mod, op, result, LLVM_I32, left, right);
            
            strncpy(result_str, result, sizeof(result_str)-1);
            return result_str;
        }
        
        case NODE_UNOP: {
            const char *operand = generate_llvm_expr(mod, node->data.unop.operand);
            const char *result = llvm_get_temp(mod);
            
            if (node->data.unop.op == 30) { /* MINUS */
                llvm_emit_binary(mod, LLVM_SUB, result, LLVM_I32, "0", operand);
            }
            
            strncpy(result_str, result, sizeof(result_str)-1);
            return result_str;
        }
        
        default:
            strcpy(result_str, "0");
            return result_str;
    }
}

/* Generate LLVM code for a statement */
static void generate_llvm_stmt(LLVMModule *mod, ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_DECL: {
            /* Allocate space for variable */
            llvm_alloca(mod, node->data.decl.name, LLVM_I32);
            
            /* Initialize if there's a value */
            if (node->data.decl.init_value) {
                const char *init_val = generate_llvm_expr(mod, node->data.decl.init_value);
                char var_ptr[256];
                snprintf(var_ptr, sizeof(var_ptr), "%%%s", node->data.decl.name);
                llvm_emit_store(mod, LLVM_I32, init_val, var_ptr);
            }
            break;
        }
        
        case NODE_ASSIGN: {
            const char *value = generate_llvm_expr(mod, node->data.assign.value);
            char var_ptr[256];
            snprintf(var_ptr, sizeof(var_ptr), "%%%s", node->data.assign.varname);
            llvm_emit_store(mod, LLVM_I32, value, var_ptr);
            break;
        }
        
        case NODE_IF: {
            int then_label = llvm_get_label(mod);
            int else_label = llvm_get_label(mod);
            int end_label = llvm_get_label(mod);
            
            /* Generate condition */
            const char *cond = generate_llvm_expr(mod, node->data.if_node.condition);
            const char *cond_result = llvm_get_temp(mod);
            llvm_emit_icmp(mod, cond_result, "ne", LLVM_I32, cond, "0");
            
            /* Conditional branch */
            llvm_emit_cond_br(mod, cond_result, then_label, else_label);
            
            /* Then branch */
            llvm_emit_label(mod, then_label);
            generate_llvm_stmt(mod, node->data.if_node.then_branch);
            llvm_emit_br(mod, end_label);
            
            /* Else branch */
            llvm_emit_label(mod, else_label);
            if (node->data.if_node.else_branch) {
                generate_llvm_stmt(mod, node->data.if_node.else_branch);
            }
            llvm_emit_br(mod, end_label);
            
            /* End label */
            llvm_emit_label(mod, end_label);
            break;
        }
        
        case NODE_WHILE: {
            int loop_label = llvm_get_label(mod);
            int body_label = llvm_get_label(mod);
            int end_label = llvm_get_label(mod);
            
            /* Loop entry */
            llvm_emit_br(mod, loop_label);
            
            /* Loop condition */
            llvm_emit_label(mod, loop_label);
            const char *cond = generate_llvm_expr(mod, node->data.while_node.condition);
            const char *cond_result = llvm_get_temp(mod);
            llvm_emit_icmp(mod, cond_result, "ne", LLVM_I32, cond, "0");
            llvm_emit_cond_br(mod, cond_result, body_label, end_label);
            
            /* Loop body */
            llvm_emit_label(mod, body_label);
            generate_llvm_stmt(mod, node->data.while_node.body);
            llvm_emit_br(mod, loop_label);
            
            /* End */
            llvm_emit_label(mod, end_label);
            break;
        }
        
        case NODE_FOR: {
            /* Generate init */
            if (node->data.for_node.init) {
                generate_llvm_stmt(mod, node->data.for_node.init);
            }
            
            int loop_label = llvm_get_label(mod);
            int body_label = llvm_get_label(mod);
            int end_label = llvm_get_label(mod);
            
            llvm_emit_br(mod, loop_label);
            
            /* Loop condition */
            llvm_emit_label(mod, loop_label);
            if (node->data.for_node.condition) {
                const char *cond = generate_llvm_expr(mod, node->data.for_node.condition);
                const char *cond_result = llvm_get_temp(mod);
                llvm_emit_icmp(mod, cond_result, "ne", LLVM_I32, cond, "0");
                llvm_emit_cond_br(mod, cond_result, body_label, end_label);
            } else {
                llvm_emit_br(mod, body_label);
            }
            
            /* Loop body */
            llvm_emit_label(mod, body_label);
            generate_llvm_stmt(mod, node->data.for_node.body);
            
            /* Update */
            if (node->data.for_node.update) {
                generate_llvm_stmt(mod, node->data.for_node.update);
            }
            
            llvm_emit_br(mod, loop_label);
            
            /* End */
            llvm_emit_label(mod, end_label);
            break;
        }
        
        case NODE_BLOCK: {
            for (int i = 0; i < node->data.block.stmt_count; i++) {
                generate_llvm_stmt(mod, node->data.block.stmts[i]);
            }
            break;
        }
        
        case NODE_RETURN: {
            if (node->data.ret.value) {
                const char *ret_val = generate_llvm_expr(mod, node->data.ret.value);
                llvm_emit_ret(mod, LLVM_I32, ret_val);
            } else {
                llvm_emit_ret(mod, LLVM_I32, "0");
            }
            break;
        }
        
        default:
            break;
    }
}

/* Generate LLVM IR from AST */
void llvm_generate_from_ast(LLVMModule *mod, ASTNode *ast) {
    if (!ast) return;
    
    if (ast->type == NODE_PROGRAM || ast->type == NODE_BLOCK) {
        for (int i = 0; i < ast->data.block.stmt_count; i++) {
            generate_llvm_stmt(mod, ast->data.block.stmts[i]);
        }
    } else {
        generate_llvm_stmt(mod, ast);
    }
    
    /* Ensure there's a return if not already present */
    /* In a full implementation, we'd track this properly */
}

/* Main function for standalone LLVM generator */
int main(int argc, char **argv) {
    printf("=================================================\n");
    printf("Module 8: LLVM IR Generator\n");
    printf("CS-346 Compiler Construction - Lab Project\n");
    printf("=================================================\n\n");
    
    /* Create module */
    LLVMModule *mod = llvm_module_create("output.ll");
    
    /* Generate example LLVM IR */
    printf("Generating sample LLVM IR...\n\n");
    llvm_generate_example(mod);
    
    /* Print to stdout */
    printf("; Generated LLVM IR:\n");
    printf("; ==================\n\n");
    llvm_module_print(mod, stdout);
    
    /* Write to file */
    llvm_print_to_file(mod, "output.ll");
    
    /* Also generate optimized version placeholder */
    printf("\n; To generate optimized IR, run:\n");
    printf(";   clang -S -emit-llvm -O3 output.ll -o output_optimized.ll\n\n");
    
    /* Cleanup */
    llvm_module_destroy(mod);
    
    return 0;
}
