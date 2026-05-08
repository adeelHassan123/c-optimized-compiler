/* Module 8: LLVM IR Generator
 * CS-346 Compiler Construction - Lab Project
 * 
 * Generates LLVM IR from AST for compilation with Clang.
 */

#ifndef LLVM_IR_H
#define LLVM_IR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* LLVM Value types */
typedef enum {
    LLVM_I1,        /* 1-bit boolean */
    LLVM_I8,        /* 8-bit integer */
    LLVM_I32,       /* 32-bit integer */
    LLVM_I64,       /* 64-bit integer */
    LLVM_FLOAT,     /* 32-bit float */
    LLVM_DOUBLE,    /* 64-bit double */
    LLVM_PTR        /* Pointer type */
} LLVMType;

/* LLVM Value representation */
typedef struct LLVMValue {
    char name[64];
    LLVMType type;
    int is_constant;
    union {
        long int_val;
        double float_val;
    } constant;
} LLVMValue;

/* LLVM Instruction types */
typedef enum {
    LLVM_ALLOCA,    /* Allocate stack space: %x = alloca i32 */
    LLVM_LOAD,      /* Load from memory: %val = load i32, i32* %x */
    LLVM_STORE,     /* Store to memory: store i32 %val, i32* %x */
    LLVM_ADD,       /* Addition: %res = add i32 %a, %b */
    LLVM_SUB,       /* Subtraction */
    LLVM_MUL,       /* Multiplication */
    LLVM_SDIV,      /* Signed division */
    LLVM_SREM,      /* Signed remainder */
    LLVM_FADD,      /* Float addition */
    LLVM_FSUB,      /* Float subtraction */
    LLVM_FMUL,      /* Float multiplication */
    LLVM_FDIV,      /* Float division */
    LLVM_FREM,      /* Float remainder */
    LLVM_ICMP,      /* Integer comparison */
    LLVM_FCMP,      /* Float comparison */
    LLVM_BR,        /* Unconditional branch */
    LLVM_COND_BR,   /* Conditional branch */
    LLVM_LABEL,     /* Label */
    LLVM_CALL,      /* Function call */
    LLVM_RET,       /* Return */
    LLVM_GETELEMENTPTR, /* Array access */
    LLVM_PHI        /* PHI node for SSA */
} LLVMInstType;

/* LLVM Instruction */
typedef struct LLVMInstruction {
    LLVMInstType type;
    char result[64];
    LLVMType result_type;
    char operand1[64];
    char operand2[64];
    char operand3[64];
    int cmp_pred;       /* Comparison predicate */
    int label_true;     /* For conditional branch */
    int label_false;
    int label_num;      /* For label instruction */
    char comment[256];
} LLVMInstruction;

/* LLVM Module - represents a single .ll file */
typedef struct LLVMModule {
    char filename[256];
    
    /* Global declarations */
    char global_decls[4096];
    int global_count;
    
    /* Function code */
    LLVMInstruction *instructions;
    int instruction_count;
    int max_instructions;
    
    /* Variable tracking */
    LLVMValue *variables;
    int var_count;
    int max_vars;
    
    /* Temp counter */
    int temp_counter;
    int label_counter;
    
    /* Current function */
    char current_function[64];
    LLVMType return_type;
} LLVMModule;

/* Function prototypes */

/* Module creation */
LLVMModule *llvm_module_create(const char *filename);
void llvm_module_destroy(LLVMModule *mod);
void llvm_module_print(LLVMModule *mod, FILE *output);

/* Variable management */
LLVMValue *llvm_declare_global(LLVMModule *mod, const char *name, LLVMType type);
LLVMValue *llvm_alloca(LLVMModule *mod, const char *name, LLVMType type);
const char *llvm_get_temp(LLVMModule *mod);
int llvm_get_label(LLVMModule *mod);

/* Instruction emission */
void llvm_emit_load(LLVMModule *mod, const char *result, LLVMType type, const char *ptr);
void llvm_emit_store(LLVMModule *mod, LLVMType type, const char *value, const char *ptr);
void llvm_emit_binary(LLVMModule *mod, LLVMInstType op, const char *result, 
                       LLVMType type, const char *op1, const char *op2);
void llvm_emit_icmp(LLVMModule *mod, const char *result, const char *pred,
                     LLVMType type, const char *op1, const char *op2);
void llvm_emit_br(LLVMModule *mod, int label);
void llvm_emit_cond_br(LLVMModule *mod, const char *cond, int true_label, int false_label);
void llvm_emit_label(LLVMModule *mod, int label_num);
void llvm_emit_ret(LLVMModule *mod, LLVMType type, const char *value);
void llvm_emit_ret_void(LLVMModule *mod);
void llvm_emit_call(LLVMModule *mod, const char *result, LLVMType ret_type,
                     const char *func_name, int arg_count, const char **args);

/* Type conversion */
const char *llvm_type_string(LLVMType type);
const char *llvm_cmp_predicate(const char *op);

/* Code generation from AST */
void llvm_generate_program(LLVMModule *mod, void *ast);  /* ast is ASTNode* */
void llvm_generate_example(LLVMModule *mod);             /* Generate sample IR */
void llvm_print_to_file(LLVMModule *mod, const char *filename);

#endif /* LLVM_IR_H */
