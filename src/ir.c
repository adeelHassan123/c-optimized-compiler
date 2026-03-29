#include "ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================== IR STATE ====================== */

static int temp_counter = 0;
static int label_counter = 0;

/* ====================== IR CREATION ====================== */

IRCode *ir_create(void) {
    IRCode *code = malloc(sizeof(IRCode));
    code->instructions = malloc(MAX_IR_NODES * sizeof(IRInstruction));
    code->instruction_count = 0;
    code->max_instructions = MAX_IR_NODES;
    code->temp_count = 0;
    code->label_count = 0;
    return code;
}

/* ====================== TEMPORARY AND LABEL ALLOCATION ====================== */

IRValue ir_alloc_temp(void) {
    IRValue v;
    v.type = IR_VAL_TEMP;
    sprintf(v.name, "t%d", temp_counter++);
    v.value.int_val = 0;
    v.value.float_val = 0.0;
    return v;
}

int ir_alloc_label(void) {
    return label_counter++;
}

/* ====================== INSTRUCTION EMISSION ====================== */

void ir_emit(IRCode *code, IROpType op, IRValue result, IRValue op1, IRValue op2, int op_type) {
    if (code->instruction_count >= code->max_instructions) {
        fprintf(stderr, "ERROR: IR instruction buffer exhausted\n");
        return;
    }
    
    IRInstruction *instr = &code->instructions[code->instruction_count++];
    instr->op = op;
    instr->result = result;
    instr->operand1 = op1;
    instr->operand2 = op2;
    instr->op_type = op_type;
    instr->label = -1;
}

/* ====================== IR GENERATION FROM AST ====================== */

static IRCode *code_generator(void);

static IRValue generate_ir_expr(IRCode *code, ASTNode *expr) {
    if (!expr) {
        IRValue v;
        v.type = IR_VAL_CONST;
        v.value.int_val = 0;
        v.value.float_val = 0.0;
        return v;
    }
    
    switch (expr->type) {
        case NODE_INT: {
            IRValue v;
            v.type = IR_VAL_CONST;
            v.value.int_val = expr->data.int_val;
            v.value.float_val = (double)expr->data.int_val;
            return v;
        }
        
        case NODE_FLOAT: {
            IRValue v;
            v.type = IR_VAL_CONST;
            v.value.int_val = (long)expr->data.float_val;
            v.value.float_val = expr->data.float_val;
            return v;
        }
        
        case NODE_VAR: {
            IRValue v;
            v.type = IR_VAL_VAR;
            strcpy(v.name, expr->data.var.name);
            return v;
        }

        case NODE_ASSIGN: {
            IRValue var;
            var.type = IR_VAL_VAR;
            strcpy(var.name, expr->data.assign.varname);
            IRValue value = generate_ir_expr(code, expr->data.assign.value);
            ir_emit(code, IR_ASSIGN, var, value, value, 0);
            return var;
        }
        
        case NODE_BINOP: {
            IRValue left = generate_ir_expr(code, expr->data.binop.left);
            IRValue right = generate_ir_expr(code, expr->data.binop.right);
            IRValue result = ir_alloc_temp();
            
            ir_emit(code, IR_BINOP, result, left, right, expr->data.binop.op);
            return result;
        }
        
        case NODE_UNOP: {
            IRValue operand = generate_ir_expr(code, expr->data.unop.operand);
            IRValue result = ir_alloc_temp();
            
            ir_emit(code, IR_UNOP, result, operand, operand, expr->data.unop.op);
            return result;
        }
        
        default: {
            IRValue v;
            v.type = IR_VAL_CONST;
            v.value.int_val = 0;
            v.value.float_val = 0.0;
            return v;
        }
    }
}

static void generate_ir_stmt(IRCode *code, ASTNode *stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case NODE_DECL: {
            IRValue var;
            var.type = IR_VAL_VAR;
            strcpy(var.name, stmt->data.decl.name);
            
            if (stmt->data.decl.init_value) {
                IRValue value = generate_ir_expr(code, stmt->data.decl.init_value);
                ir_emit(code, IR_ASSIGN, var, value, value, 0);
            }
            break;
        }
        
        case NODE_ASSIGN: {
            IRValue var;
            var.type = IR_VAL_VAR;
            strcpy(var.name, stmt->data.assign.varname);
            
            IRValue value = generate_ir_expr(code, stmt->data.assign.value);
            ir_emit(code, IR_ASSIGN, var, value, value, 0);
            break;
        }
        
        case NODE_PRINT: {
            IRValue arg = generate_ir_expr(code, stmt->data.print.arg);
            ir_emit(code, IR_PRINT, arg, arg, arg, 0);
            break;
        }
        
        case NODE_RETURN: {
            if (stmt->data.ret.value) {
                IRValue value = generate_ir_expr(code, stmt->data.ret.value);
                ir_emit(code, IR_RETURN, value, value, value, 0);
            } else {
                IRValue v;
                v.type = IR_VAL_CONST;
                v.value.int_val = 0;
                v.value.float_val = 0.0;
                ir_emit(code, IR_RETURN, v, v, v, 0);
            }
            break;
        }
        
        case NODE_IF: {
            IRValue cond = generate_ir_expr(code, stmt->data.if_node.condition);
            int then_label = ir_alloc_label();
            int else_label = ir_alloc_label();
            int end_label = ir_alloc_label();
            
            IRValue v;
            v.type = IR_VAL_CONST;
            v.value.int_val = 0;
            v.value.float_val = 0.0;
            ir_emit(code, IR_IF_GOTO, v, cond, v, then_label);
            ir_emit(code, IR_GOTO, v, v, v, else_label);
            ir_emit(code, IR_LABEL, v, v, v, then_label);
            
            generate_ir_stmt(code, stmt->data.if_node.then_branch);
            
            ir_emit(code, IR_GOTO, v, v, v, end_label);
            ir_emit(code, IR_LABEL, v, v, v, else_label);
            if (stmt->data.if_node.else_branch) {
                generate_ir_stmt(code, stmt->data.if_node.else_branch);
            }
            
            ir_emit(code, IR_LABEL, v, v, v, end_label);
            break;
        }
        
        case NODE_WHILE: {
            int loop_label = ir_alloc_label();
            int body_label = ir_alloc_label();
            int end_label = ir_alloc_label();
            
            IRValue v;
            v.type = IR_VAL_CONST;
            ir_emit(code, IR_LABEL, v, v, v, loop_label);
            IRValue cond = generate_ir_expr(code, stmt->data.while_node.condition);
            ir_emit(code, IR_IF_GOTO, v, cond, v, body_label);
            ir_emit(code, IR_GOTO, v, v, v, end_label);
            ir_emit(code, IR_LABEL, v, v, v, body_label);
            
            generate_ir_stmt(code, stmt->data.while_node.body);
            
            ir_emit(code, IR_GOTO, v, v, v, loop_label);
            ir_emit(code, IR_LABEL, v, v, v, end_label);
            break;
        }

        case NODE_FOR: {
            int loop_label = ir_alloc_label();
            int body_label = ir_alloc_label();
            int end_label = ir_alloc_label();
            IRValue v;
            v.type = IR_VAL_CONST;
            v.value.int_val = 0;
            v.value.float_val = 0.0;

            if (stmt->data.for_node.init) {
                generate_ir_stmt(code, stmt->data.for_node.init);
            }

            ir_emit(code, IR_LABEL, v, v, v, loop_label);

            if (stmt->data.for_node.condition) {
                IRValue cond = generate_ir_expr(code, stmt->data.for_node.condition);
                ir_emit(code, IR_IF_GOTO, v, cond, v, body_label);
                ir_emit(code, IR_GOTO, v, v, v, end_label);
            } else {
                ir_emit(code, IR_GOTO, v, v, v, body_label);
            }

            ir_emit(code, IR_LABEL, v, v, v, body_label);
            generate_ir_stmt(code, stmt->data.for_node.body);

            if (stmt->data.for_node.update) {
                generate_ir_stmt(code, stmt->data.for_node.update);
            }

            ir_emit(code, IR_GOTO, v, v, v, loop_label);
            ir_emit(code, IR_LABEL, v, v, v, end_label);
            break;
        }
        
        case NODE_BLOCK: {
            for (int i = 0; i < stmt->data.block.stmt_count; i++) {
                generate_ir_stmt(code, stmt->data.block.stmts[i]);
            }
            break;
        }
        
        default:
            break;
    }
}

IRCode *ir_generate(ASTNode *ast) {
    IRCode *code = ir_create();
    
    if (!ast) return code;
    
    if (ast->type == NODE_PROGRAM && ast->data.block.stmts) {
        for (int i = 0; i < ast->data.block.stmt_count; i++) {
            generate_ir_stmt(code, ast->data.block.stmts[i]);
        }
    } else {
        generate_ir_stmt(code, ast);
    }
    
    return code;
}

/* ====================== IR PRINTING ====================== */

static const char *ir_op_name(IROpType op) {
    switch (op) {
        case IR_ASSIGN: return "ASSIGN";
        case IR_BINOP: return "BINOP";
        case IR_UNOP: return "UNOP";
        case IR_IF_GOTO: return "IF_GOTO";
        case IR_GOTO: return "GOTO";
        case IR_LABEL: return "LABEL";
        case IR_CALL: return "CALL";
        case IR_RETURN: return "RETURN";
        case IR_PARAM: return "PARAM";
        case IR_PRINT: return "PRINT";
        default: return "UNKNOWN";
    }
}

static void ir_value_print(IRValue v) {
    switch (v.type) {
        case IR_VAL_CONST:
            if (v.value.float_val != 0.0 && (double)v.value.int_val != v.value.float_val) {
                printf("%g", v.value.float_val);
            } else {
                printf("%ld", v.value.int_val);
            }
            break;
        case IR_VAL_VAR:
            printf("%s", v.name);
            break;
        case IR_VAL_TEMP:
            printf("%s", v.name);
            break;
    }
}

static const char *tok_op_to_str(int op_type) {
    switch (op_type) {
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_MUL: return "*";
        case TOK_DIV: return "/";
        case TOK_MOD: return "%";
        case TOK_EQ: return "==";
        case TOK_NEQ: return "!=";
        case TOK_LT: return "<";
        case TOK_LE: return "<=";
        case TOK_GT: return ">";
        case TOK_GE: return ">=";
        case TOK_AND: return "&&";
        case TOK_OR: return "||";
        case TOK_NOT: return "!";
        default: return "?";
    }
}

void ir_print(IRCode *code) {
    printf("\n============ INTERMEDIATE REPRESENTATION ============\n");
    
    for (int i = 0; i < code->instruction_count; i++) {
        IRInstruction *instr = &code->instructions[i];
        printf("%3d: %s ", i, ir_op_name(instr->op));
        
        switch (instr->op) {
            case IR_LABEL:
                printf("label %d", instr->label);
                break;
            
            case IR_GOTO:
                printf("goto %d", instr->label);
                break;
            
            case IR_IF_GOTO:
                printf("if ");
                ir_value_print(instr->operand1);
                printf(" goto %d", instr->label);
                break;
            
            case IR_ASSIGN:
                ir_value_print(instr->result);
                printf(" = ");
                ir_value_print(instr->operand1);
                break;
            
            case IR_BINOP:
                ir_value_print(instr->result);
                printf(" = ");
                ir_value_print(instr->operand1);
                printf(" %s ", tok_op_to_str(instr->op_type));
                ir_value_print(instr->operand2);
                break;
            
            case IR_UNOP:
                ir_value_print(instr->result);
                printf(" = %s ", tok_op_to_str(instr->op_type));
                ir_value_print(instr->operand1);
                break;
            
            case IR_PRINT:
                ir_value_print(instr->result);
                break;
            
            case IR_RETURN:
                ir_value_print(instr->result);
                break;
            
            default:
                break;
        }
        
        printf("\n");
    }
    
    printf("====================================================\n");
}

/* ====================== IR FREEING ====================== */

void ir_free(IRCode *code) {
    if (code) {
        if (code->instructions) free(code->instructions);
        free(code);
    }
}
// Commit Marker