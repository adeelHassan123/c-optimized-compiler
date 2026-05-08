/* Module 8: LLVM IR Generator Implementation
 * Generates LLVM IR format compatible with Clang
 */

#include "llvm_ir.h"

/* Type to string conversion */
const char *llvm_type_string(LLVMType type) {
    switch (type) {
        case LLVM_I32: return "i32";
        case LLVM_I64: return "i64";
        case LLVM_FLOAT: return "float";
        case LLVM_DOUBLE: return "double";
        case LLVM_PTR: return "ptr";
        default: return "i32";
    }
}

/* Comparison predicate conversion */
const char *llvm_cmp_predicate(const char *op) {
    if (strcmp(op, "==") == 0) return "eq";
    if (strcmp(op, "!=") == 0) return "ne";
    if (strcmp(op, "<") == 0) return "slt";
    if (strcmp(op, "<=") == 0) return "sle";
    if (strcmp(op, ">") == 0) return "sgt";
    if (strcmp(op, ">=") == 0) return "sge";
    return "eq";
}

/* Create new LLVM module */
LLVMModule *llvm_module_create(const char *filename) {
    LLVMModule *mod = malloc(sizeof(LLVMModule));
    strncpy(mod->filename, filename, 255);
    mod->filename[255] = '\0';
    
    mod->global_decls[0] = '\0';
    mod->global_count = 0;
    
    mod->max_instructions = 1000;
    mod->instructions = malloc(mod->max_instructions * sizeof(LLVMInstruction));
    mod->instruction_count = 0;
    
    mod->max_vars = 100;
    mod->variables = malloc(mod->max_vars * sizeof(LLVMValue));
    mod->var_count = 0;
    
    mod->temp_counter = 0;
    mod->label_counter = 0;
    
    mod->current_function[0] = '\0';
    mod->return_type = LLVM_I32;
    
    return mod;
}

void llvm_module_destroy(LLVMModule *mod) {
    if (mod) {
        free(mod->instructions);
        free(mod->variables);
        free(mod);
    }
}

/* Get a new temporary variable name */
const char *llvm_get_temp(LLVMModule *mod) {
    static char temp_name[64];
    snprintf(temp_name, sizeof(temp_name), "%%t%d", mod->temp_counter++);
    return temp_name;
}

/* Get a new label number */
int llvm_get_label(LLVMModule *mod) {
    return mod->label_counter++;
}

/* Emit instruction */
static void emit_instruction(LLVMModule *mod, LLVMInstruction *inst) {
    if (mod->instruction_count >= mod->max_instructions) {
        mod->max_instructions *= 2;
        mod->instructions = realloc(mod->instructions, 
                                     mod->max_instructions * sizeof(LLVMInstruction));
    }
    mod->instructions[mod->instruction_count++] = *inst;
}

/* Emit alloca instruction */
LLVMValue *llvm_alloca(LLVMModule *mod, const char *name, LLVMType type) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_ALLOCA;
    snprintf(inst.result, sizeof(inst.result), "%%%s", name);
    inst.result_type = type;
    snprintf(inst.comment, sizeof(inst.comment), "; Declare %s", name);
    emit_instruction(mod, &inst);
    
    /* Track variable */
    LLVMValue *var = &mod->variables[mod->var_count++];
    strncpy(var->name, name, 63);
    var->type = type;
    var->is_constant = 0;
    
    return var;
}

/* Emit load instruction */
void llvm_emit_load(LLVMModule *mod, const char *result, LLVMType type, const char *ptr) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_LOAD;
    strncpy(inst.result, result, 63);
    inst.result_type = type;
    snprintf(inst.operand1, sizeof(inst.operand1), "%s", ptr);
    emit_instruction(mod, &inst);
}

/* Emit store instruction */
void llvm_emit_store(LLVMModule *mod, LLVMType type, const char *value, const char *ptr) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_STORE;
    inst.result_type = type;
    snprintf(inst.operand1, sizeof(inst.operand1), "%s", value);
    snprintf(inst.operand2, sizeof(inst.operand2), "%s", ptr);
    emit_instruction(mod, &inst);
}

/* Emit binary operation */
void llvm_emit_binary(LLVMModule *mod, LLVMInstType op, const char *result, 
                       LLVMType type, const char *op1, const char *op2) {
    LLVMInstruction inst = {0};
    inst.type = op;
    strncpy(inst.result, result, 63);
    inst.result_type = type;
    snprintf(inst.operand1, sizeof(inst.operand1), "%s", op1);
    snprintf(inst.operand2, sizeof(inst.operand2), "%s", op2);
    emit_instruction(mod, &inst);
}

/* Emit integer comparison */
void llvm_emit_icmp(LLVMModule *mod, const char *result, const char *pred,
                     LLVMType type, const char *op1, const char *op2) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_ICMP;
    strncpy(inst.result, result, 63);
    inst.result_type = LLVM_I1;  /* i1 (boolean) result */
    snprintf(inst.operand1, sizeof(inst.operand1), "%s", pred);
    snprintf(inst.operand2, sizeof(inst.operand2), "%s", op1);
    snprintf(inst.operand3, sizeof(inst.operand3), "%s", op2);
    emit_instruction(mod, &inst);
}

/* Emit unconditional branch */
void llvm_emit_br(LLVMModule *mod, int label) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_BR;
    inst.label_num = label;
    emit_instruction(mod, &inst);
}

/* Emit conditional branch */
void llvm_emit_cond_br(LLVMModule *mod, const char *cond, int true_label, int false_label) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_COND_BR;
    snprintf(inst.operand1, sizeof(inst.operand1), "%s", cond);
    inst.label_true = true_label;
    inst.label_false = false_label;
    emit_instruction(mod, &inst);
}

/* Emit label */
void llvm_emit_label(LLVMModule *mod, int label_num) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_LABEL;
    inst.label_num = label_num;
    emit_instruction(mod, &inst);
}

/* Emit return with value */
void llvm_emit_ret(LLVMModule *mod, LLVMType type, const char *value) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_RET;
    inst.result_type = type;
    snprintf(inst.operand1, sizeof(inst.operand1), "%s", value);
    emit_instruction(mod, &inst);
}

/* Emit void return */
void llvm_emit_ret_void(LLVMModule *mod) {
    LLVMInstruction inst = {0};
    inst.type = LLVM_RET;
    emit_instruction(mod, &inst);
}

/* Print instruction to string */
static void print_instruction(LLVMInstruction *inst, FILE *output) {
    switch (inst->type) {
        case LLVM_ALLOCA:
            fprintf(output, "  %s = alloca %s, align 4\n", 
                    inst->result, llvm_type_string(inst->result_type));
            break;
            
        case LLVM_LOAD:
            fprintf(output, "  %s = load %s, %s* %s, align 4\n",
                    inst->result, 
                    llvm_type_string(inst->result_type),
                    llvm_type_string(inst->result_type),
                    inst->operand1);
            break;
            
        case LLVM_STORE:
            fprintf(output, "  store %s %s, %s* %s, align 4\n",
                    llvm_type_string(inst->result_type),
                    inst->operand1,
                    llvm_type_string(inst->result_type),
                    inst->operand2);
            break;
            
        case LLVM_ADD:
            fprintf(output, "  %s = add nsw %s %s, %s\n",
                    inst->result,
                    llvm_type_string(inst->result_type),
                    inst->operand1, inst->operand2);
            break;
            
        case LLVM_SUB:
            fprintf(output, "  %s = sub nsw %s %s, %s\n",
                    inst->result,
                    llvm_type_string(inst->result_type),
                    inst->operand1, inst->operand2);
            break;
            
        case LLVM_MUL:
            fprintf(output, "  %s = mul nsw %s %s, %s\n",
                    inst->result,
                    llvm_type_string(inst->result_type),
                    inst->operand1, inst->operand2);
            break;
            
        case LLVM_SDIV:
            fprintf(output, "  %s = sdiv %s %s, %s\n",
                    inst->result,
                    llvm_type_string(inst->result_type),
                    inst->operand1, inst->operand2);
            break;
            
        case LLVM_SREM:
            fprintf(output, "  %s = srem %s %s, %s\n",
                    inst->result,
                    llvm_type_string(inst->result_type),
                    inst->operand1, inst->operand2);
            break;
            
        case LLVM_ICMP:
            fprintf(output, "  %s = icmp %s %s %s, %s\n",
                    inst->result,
                    inst->operand1,  /* predicate */
                    llvm_type_string(inst->result_type),
                    inst->operand2, inst->operand3);
            break;
            
        case LLVM_BR:
            fprintf(output, "  br label %%%d\n", inst->label_num);
            break;
            
        case LLVM_COND_BR:
            fprintf(output, "  br i1 %s, label %%%d, label %%%d\n",
                    inst->operand1,
                    inst->label_true,
                    inst->label_false);
            break;
            
        case LLVM_LABEL:
            fprintf(output, "\n%d:\n", inst->label_num);
            break;
            
        case LLVM_RET:
            if (strlen(inst->operand1) > 0) {
                fprintf(output, "  ret %s %s\n",
                        llvm_type_string(inst->result_type),
                        inst->operand1);
            } else {
                fprintf(output, "  ret void\n");
            }
            break;
            
        default:
            break;
    }
}

/* Print complete module */
void llvm_module_print(LLVMModule *mod, FILE *output) {
    /* Module header */
    fprintf(output, "; ModuleID = '%s'\n", mod->filename);
    fprintf(output, "source_filename = \"%s\"\n\n", mod->filename);
    
    /* Target declarations */
    fprintf(output, "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n");
    fprintf(output, "target triple = \"x86_64-pc-linux-gnu\"\n\n");
    
    /* Global declarations (if any) */
    if (strlen(mod->global_decls) > 0) {
        fprintf(output, "%s\n", mod->global_decls);
    }
    
    /* Function definition */
    fprintf(output, "define dso_local i32 @main() #0 {\n");
    fprintf(output, "entry:\n");
    
    /* Instructions */
    for (int i = 0; i < mod->instruction_count; i++) {
        print_instruction(&mod->instructions[i], output);
    }
    
    fprintf(output, "}\n\n");
    
    /* Function attributes */
    fprintf(output, "attributes #0 = { noinline nounwind optnone uwtable \"frame-pointer\"=\"all\" }\n");
}

/* Print to file */
void llvm_print_to_file(LLVMModule *mod, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f) {
        llvm_module_print(mod, f);
        fclose(f);
        printf("LLVM IR written to: %s\n", filename);
    } else {
        fprintf(stderr, "ERROR: Cannot write to %s\n", filename);
    }
}

/* Simple example generation - creates a sample program */
void llvm_generate_example(LLVMModule *mod) {
    /* Example: int x = 10; int y = 20; return x + y; */
    
    /* Allocate x */
    llvm_alloca(mod, "x", LLVM_I32);
    /* Store 10 to x */
    llvm_emit_store(mod, LLVM_I32, "10", "%x");
    
    /* Allocate y */
    llvm_alloca(mod, "y", LLVM_I32);
    /* Store 20 to y */
    llvm_emit_store(mod, LLVM_I32, "20", "%y");
    
    /* Load x */
    const char *t1 = llvm_get_temp(mod);
    llvm_emit_load(mod, t1, LLVM_I32, "%x");
    
    /* Load y */
    const char *t2 = llvm_get_temp(mod);
    llvm_emit_load(mod, t2, LLVM_I32, "%y");
    
    /* Add */
    const char *t3 = llvm_get_temp(mod);
    llvm_emit_binary(mod, LLVM_ADD, t3, LLVM_I32, t1, t2);
    
    /* Return */
    llvm_emit_ret(mod, LLVM_I32, t3);
}
