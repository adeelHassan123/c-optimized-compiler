#include "ir.h"

static int temp_counter = 0;
static int label_counter = 0;

IRCode *ir_create(void) {
    IRCode *code = malloc(sizeof(IRCode));
    code->instructions = malloc(MAX_IR_NODES * sizeof(IRInstruction));
    code->instruction_count = 0;
    code->max_instructions = MAX_IR_NODES;
    code->temp_count = 0;
    code->label_count = 0;
    return code;
}

IRValue ir_alloc_temp(void) {
    IRValue v;
    v.type = IR_VAL_TEMP;
    sprintf(v.name, "t%d", temp_counter++);
    return v;
}

int ir_alloc_label(void) { return label_counter++; }

void ir_emit(IRCode *code, IROpType op, IRValue result, IRValue op1, IRValue op2, int op_type) {
    if (code->instruction_count >= code->max_instructions) return;
    IRInstruction *instr = &code->instructions[code->instruction_count++];
    instr->op = op;
    instr->result = result;
    instr->operand1 = op1;
    instr->operand2 = op2;
    instr->op_type = op_type;
    instr->label = -1;

    // Diagnostic print
    printf("  [IR] Emitting instruction: ");
    switch (op) {
        case IR_ASSIGN: 
            if (op1.type == IR_VAL_CONST) printf("%s = %ld\n", result.name, op1.value.int_val);
            else printf("%s = %s\n", result.name, op1.name);
            break;
        case IR_BINOP: printf("%s = %s op(%d) %s\n", result.name, op1.name, op_type, op2.name); break;
        case IR_IF_GOTO: printf("if %s goto L%d\n", op1.name, op_type); break;
        case IR_GOTO: printf("goto L%d\n", op_type); break;
        case IR_LABEL: printf("L%d:\n", op_type); break;
        case IR_CALL: printf("%s = call %s(%s)\n", result.name, op1.name, op2.name); break;
        case IR_RETURN: printf("return %s\n", op1.name); break;
        case IR_PRINT: printf("print %s\n", result.name); break;
        default: printf("Other op\n"); break;
    }
}

static IRValue generate_ir_expr(IRCode *code, ASTNode *expr) {
    if (!expr) { IRValue v = {IR_VAL_CONST, "", {0}}; return v; }
    switch (expr->type) {
        case NODE_INT: { IRValue v = {IR_VAL_CONST, "", {0}}; v.value.int_val = expr->data.int_val; return v; }
        case NODE_FLOAT: { IRValue v = {IR_VAL_CONST, "", {0}}; v.value.float_val = expr->data.float_val; return v; }
        case NODE_VAR: { IRValue v = {IR_VAL_VAR, "", {0}}; strcpy(v.name, expr->data.var.name); return v; }
        case NODE_BINOP: {
            IRValue left = generate_ir_expr(code, expr->data.binop.left);
            IRValue right = generate_ir_expr(code, expr->data.binop.right);
            IRValue result = ir_alloc_temp();
            ir_emit(code, IR_BINOP, result, left, right, expr->data.binop.op);
            return result;
        }
        case NODE_ASSIGN: {
            IRValue var = {IR_VAL_VAR, "", {0}}; strcpy(var.name, expr->data.assign.varname);
            IRValue val = generate_ir_expr(code, expr->data.assign.value);
            ir_emit(code, IR_ASSIGN, var, val, val, 0);
            return var;
        }
        case NODE_CALL: {
            IRValue arg = generate_ir_expr(code, expr->data.call.arg);
            IRValue func = {IR_VAL_VAR, "", {0}}; strcpy(func.name, expr->data.call.name);
            IRValue result = ir_alloc_temp();
            ir_emit(code, IR_CALL, result, func, arg, 0);
            return result;
        }
        default: { IRValue v = {IR_VAL_CONST, "", {0}}; return v; }
    }
}

static void generate_ir_stmt(IRCode *code, ASTNode *stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case NODE_DECL: {
            IRValue var = {IR_VAL_VAR, "", {0}}; strcpy(var.name, stmt->data.decl.name);
            if (stmt->data.decl.init_value) {
                IRValue val = generate_ir_expr(code, stmt->data.decl.init_value);
                ir_emit(code, IR_ASSIGN, var, val, val, 0);
            }
            break;
        }
        case NODE_ASSIGN: {
            generate_ir_expr(code, stmt);
            break;
        }
        case NODE_PRINT: {
            IRValue val = generate_ir_expr(code, stmt->data.print.value);
            ir_emit(code, IR_PRINT, val, val, val, 0);
            break;
        }
        case NODE_BLOCK:
            for (int i = 0; i < stmt->data.block.count; i++) generate_ir_stmt(code, stmt->data.block.statements[i]);
            break;
        case NODE_PROGRAM:
            for (int i = 0; i < stmt->data.program.count; i++) generate_ir_stmt(code, stmt->data.program.nodes[i]);
            break;
        case NODE_IF: {
            int L_else = ir_alloc_label();
            int L_end = ir_alloc_label();
            IRValue cond = generate_ir_expr(code, stmt->data.if_stmt.condition);
            
            IRValue dummy = {IR_VAL_CONST, "", {0}};
            // Emit if_false cond goto L_else
            ir_emit(code, IR_IF_GOTO, dummy, cond, dummy, L_else);
            
            generate_ir_stmt(code, stmt->data.if_stmt.then_branch);
            ir_emit(code, IR_GOTO, dummy, dummy, dummy, L_end);
            
            ir_emit(code, IR_LABEL, dummy, dummy, dummy, L_else);
            if (stmt->data.if_stmt.else_branch) {
                generate_ir_stmt(code, stmt->data.if_stmt.else_branch);
            }
            ir_emit(code, IR_LABEL, dummy, dummy, dummy, L_end);
            break;
        }
        case NODE_WHILE: {
            int L_start = ir_alloc_label();
            int L_end = ir_alloc_label();
            IRValue dummy = {IR_VAL_CONST, "", {0}};
            
            ir_emit(code, IR_LABEL, dummy, dummy, dummy, L_start);
            IRValue cond = generate_ir_expr(code, stmt->data.while_stmt.condition);
            ir_emit(code, IR_IF_GOTO, dummy, cond, dummy, L_end);
            
            generate_ir_stmt(code, stmt->data.while_stmt.body);
            ir_emit(code, IR_GOTO, dummy, dummy, dummy, L_start);
            ir_emit(code, IR_LABEL, dummy, dummy, dummy, L_end);
            break;
        }
        case NODE_FOR: {
            int L_start = ir_alloc_label();
            int L_end = ir_alloc_label();
            IRValue dummy = {IR_VAL_CONST, "", {0}};
            
            generate_ir_stmt(code, stmt->data.for_stmt.init);
            ir_emit(code, IR_LABEL, dummy, dummy, dummy, L_start);
            IRValue cond = generate_ir_expr(code, stmt->data.for_stmt.condition);
            ir_emit(code, IR_IF_GOTO, dummy, cond, dummy, L_end);
            
            generate_ir_stmt(code, stmt->data.for_stmt.body);
            generate_ir_stmt(code, stmt->data.for_stmt.increment);
            ir_emit(code, IR_GOTO, dummy, dummy, dummy, L_start);
            ir_emit(code, IR_LABEL, dummy, dummy, dummy, L_end);
            break;
        }
        case NODE_RETURN: {
            IRValue val = generate_ir_expr(code, stmt->data.ret.value);
            IRValue dummy = {IR_VAL_CONST, "", {0}};
            ir_emit(code, IR_RETURN, dummy, val, dummy, 0);
            break;
        }
        case NODE_CALL:
            generate_ir_expr(code, stmt);
            break;
        default:
            break;
    }
}

IRCode *ir_generate(ASTNode *ast) {
    IRCode *code = ir_create();
    generate_ir_stmt(code, ast);
    return code;
}

void ir_print(IRCode *code) {
    printf("\n============ INTERMEDIATE REPRESENTATION (TAC) ============\n");
    for (int i = 0; i < code->instruction_count; i++) {
        IRInstruction *instr = &code->instructions[i];
        switch (instr->op) {
            case IR_ASSIGN: 
                if (instr->operand1.type == IR_VAL_CONST) {
                    printf("%s = %ld\n", instr->result.name, instr->operand1.value.int_val);
                } else {
                    printf("%s = %s\n", instr->result.name, instr->operand1.name);
                }
                break;
            case IR_BINOP: 
                printf("%s = %s op(%d) %s\n", instr->result.name, instr->operand1.name, instr->op_type, instr->operand2.name); 
                break;
            case IR_PRINT: 
                printf("print %s\n", instr->result.name); 
                break;
            default: break;
        }
    }
}

void ir_free(IRCode *code) { free(code->instructions); free(code); }
