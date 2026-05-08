#ifndef IR_H
#define IR_H

#include "types.h"

#define MAX_IR_NODES 1000

typedef enum {
    IR_ASSIGN,
    IR_BINOP,
    IR_UNOP,
    IR_IF_GOTO,
    IR_GOTO,
    IR_LABEL,
    IR_CALL,
    IR_RETURN,
    IR_PARAM,
    IR_PRINT
} IROpType;

typedef enum {
    IR_VAL_CONST,
    IR_VAL_VAR,
    IR_VAL_TEMP
} IRValueType;

typedef struct {
    IRValueType type;
    char name[64];
    union {
        long int_val;
        double float_val;
    } value;
} IRValue;

typedef struct {
    IROpType op;
    IRValue result;
    IRValue operand1;
    IRValue operand2;
    int op_type;
    int label;
} IRInstruction;

typedef struct {
    IRInstruction *instructions;
    int instruction_count;
    int max_instructions;
    int temp_count;
    int label_count;
} IRCode;

IRCode *ir_create(void);
IRCode *ir_generate(ASTNode *ast);
IRValue ir_alloc_temp(void);
int ir_alloc_label(void);
void ir_emit(IRCode *code, IROpType op, IRValue result, IRValue op1, IRValue op2, int op_type);
void ir_print(IRCode *code);
void ir_free(IRCode *code);

#endif
