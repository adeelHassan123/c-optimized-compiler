#ifndef IR_H
#define IR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================== STANDALONE TYPE DEFINITIONS ====================== */
/* These would normally be in ast.h/types.h but embedded here for module independence */

#define IDENTIFIER_MAX 64
#define MAX_IR_NODES 1000

/* AST Node Types - simplified for IR module */
typedef enum {
    NODE_INT,
    NODE_FLOAT,
    NODE_VAR,
    NODE_BINOP,
    NODE_UNOP,
    NODE_ASSIGN,
    NODE_DECL,
    NODE_PRINT,
    NODE_RETURN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_BLOCK,
    NODE_PROGRAM,
    NODE_CALL
} NodeType;

/* Forward declaration - ASTNode is defined in ir.c for this standalone module */
struct ASTNode;
typedef struct ASTNode ASTNode;

/* ====================== IR NODE TYPES ====================== */

typedef enum {
    IR_ASSIGN,      /* Assignment: temp = value */
    IR_BINOP,       /* Binary operation: result = left OP right */
    IR_UNOP,        /* Unary operation: result = OP operand */
    IR_IF_GOTO,     /* Conditional jump: if (cond) goto label */
    IR_GOTO,        /* Unconditional jump */
    IR_LABEL,       /* Label: label: */
    IR_CALL,        /* Function call */
    IR_RETURN,      /* Return statement */
    IR_PARAM,       /* Parameter passing */
    IR_PRINT        /* Print statement */
} IROpType;

/* ====================== IR VALUE STRUCTURE ====================== */

typedef enum {
    IR_VAL_CONST,   /* Constant value */
    IR_VAL_VAR,     /* Variable */
    IR_VAL_TEMP     /* Temporary variable */
} IRValueType;

typedef struct {
    IRValueType type;
    char name[IDENTIFIER_MAX];
    union {
        long int_val;
        double float_val;
    } value;
} IRValue;

/* ====================== IR INSTRUCTION ====================== */

typedef struct {
    IROpType op;
    IRValue result;
    IRValue operand1;
    IRValue operand2;
    int op_type;    /* For binary/unary operations (TOK_PLUS, TOK_MINUS, etc) */
    int label;      /* For jumps */
} IRInstruction;

/* ====================== IR CODE ====================== */

typedef struct {
    IRInstruction *instructions;
    int instruction_count;
    int max_instructions;
    int temp_count;
    int label_count;
} IRCode;

/* ====================== IR INTERFACE ====================== */

/* Create new IR code generator */
IRCode *ir_create(void);

/* Generate IR from AST */
IRCode *ir_generate(ASTNode *ast);

/* Allocate temporary variable */
IRValue ir_alloc_temp(void);

/* Allocate label */
int ir_alloc_label(void);

/* Emit IR instruction */
void ir_emit(IRCode *code, IROpType op, IRValue result, IRValue op1, IRValue op2, int op_type);

/* Print IR code */
void ir_print(IRCode *code);

/* Free IR code */
void ir_free(IRCode *code);

#endif /* IR_H */
