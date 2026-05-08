/* Module 8: IR to LLVM Converter
 * Converts the compiler's three-address code to LLVM IR format
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_INSTRUCTIONS 1000

typedef struct {
    char op[64];
    char result[64];
    char arg1[64];
    char arg2[64];
    char arg3[64];
} Instruction;

Instruction instructions[MAX_INSTRUCTIONS];
int instr_count = 0;
int temp_counter = 0;
int label_counter = 0;

void parse_ir_line(char *line) {
    if (strlen(line) < 3) return;
    if (line[0] == '=' || line[0] == '-') return;  /* Skip headers */
    
    Instruction *instr = &instructions[instr_count++];
    memset(instr, 0, sizeof(Instruction));
    
    /* Parse IR line format: "  0: ASSIGN x = 5" */
    char *p = line;
    while (isspace(*p)) p++;
    
    /* Skip line number */
    while (isdigit(*p)) p++;
    if (*p == ':') p++;
    while (isspace(*p)) p++;
    
    /* Get operation */
    char *op_start = p;
    while (*p && !isspace(*p)) p++;
    strncpy(instr->op, op_start, p - op_start);
    
    /* Parse based on operation */
    if (strcmp(instr->op, "ASSIGN") == 0) {
        sscanf(p, "%s = %s", instr->result, instr->arg1);
    } else if (strcmp(instr->op, "BINOP") == 0) {
        sscanf(p, "%s = %s %s %s", instr->result, instr->arg1, instr->arg2, instr->arg3);
    } else if (strcmp(instr->op, "PRINT") == 0) {
        sscanf(p, "%s", instr->result);
    } else if (strcmp(instr->op, "LABEL") == 0) {
        sscanf(p, "label %s", instr->result);
    } else if (strcmp(instr->op, "GOTO") == 0) {
        sscanf(p, "goto %s", instr->result);
    } else if (strcmp(instr->op, "IF_GOTO") == 0) {
        sscanf(p, "if %s goto %s", instr->arg1, instr->result);
    } else if (strcmp(instr->op, "RETURN") == 0) {
        sscanf(p, "%s", instr->result);
    } else if (strcmp(instr->op, "CALL") == 0) {
        sscanf(p, "%s = %s(%s)", instr->result, instr->arg1, instr->arg2);
    }
}

void generate_llvm_ir(FILE *output) {
    fprintf(output, "; ModuleID = 'compiler_output'\n");
    fprintf(output, "source_filename = \"compiler_output.c\"\n\n");
    fprintf(output, "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n");
    fprintf(output, "target triple = \"x86_64-pc-linux-gnu\"\n\n");
    
    fprintf(output, "; Function Attrs: noinline nounwind optnone uwtable\n");
    fprintf(output, "define dso_local i32 @main() #0 {\n");
    fprintf(output, "entry:\n");
    
    /* Allocate space for variables */
    fprintf(output, "  ; Variable declarations\n");
    for (int i = 0; i < instr_count; i++) {
        Instruction *instr = &instructions[i];
        if (strcmp(instr->op, "ASSIGN") == 0 && instr->result[0] && !isdigit(instr->result[0])) {
            fprintf(output, "  %%%s = alloca i32, align 4\n", instr->result);
        }
    }
    
    fprintf(output, "\n  ; Program body\n");
    
    /* Generate LLVM instructions */
    for (int i = 0; i < instr_count; i++) {
        Instruction *instr = &instructions[i];
        
        if (strcmp(instr->op, "ASSIGN") == 0) {
            if (isdigit(instr->arg1[0])) {
                fprintf(output, "  store i32 %s, i32* %%%s, align 4\n", 
                        instr->arg1, instr->result);
            } else {
                fprintf(output, "  %%t%d = load i32, i32* %%%s, align 4\n", 
                        temp_counter++, instr->arg1);
                fprintf(output, "  store i32 %%t%d, i32* %%%s, align 4\n", 
                        temp_counter - 1, instr->result);
            }
        } else if (strcmp(instr->op, "BINOP") == 0) {
            /* Load operands if they are variables */
            char op1[64], op2[64];
            int op1_temp = -1, op2_temp = -1;
            
            if (!isdigit(instr->arg1[0]) && instr->arg1[0] != '%') {
                fprintf(output, "  %%t%d = load i32, i32* %%%s, align 4\n", 
                        temp_counter, instr->arg1);
                sprintf(op1, "%%t%d", temp_counter++);
            } else {
                strcpy(op1, instr->arg1);
            }
            
            if (!isdigit(instr->arg3[0]) && instr->arg3[0] != '%') {
                fprintf(output, "  %%t%d = load i32, i32* %%%s, align 4\n", 
                        temp_counter, instr->arg3);
                sprintf(op2, "%%t%d", temp_counter++);
            } else {
                strcpy(op2, instr->arg3);
            }
            
            /* Perform operation */
            const char *llvm_op = "add";
            if (strcmp(instr->arg2, "+") == 0) llvm_op = "add";
            else if (strcmp(instr->arg2, "-") == 0) llvm_op = "sub";
            else if (strcmp(instr->arg2, "*") == 0) llvm_op = "mul";
            else if (strcmp(instr->arg2, "/") == 0) llvm_op = "sdiv";
            else if (strcmp(instr->arg2, "^") == 0) llvm_op = "xor";  /* Simplified */
            
            fprintf(output, "  %%t%d = %s nsw i32 %s, %s\n", 
                    temp_counter++, llvm_op, op1, op2);
            fprintf(output, "  store i32 %%t%d, i32* %%%s, align 4\n",
                    temp_counter - 1, instr->result);
        } else if (strcmp(instr->op, "PRINT") == 0) {
            fprintf(output, "  %%t%d = load i32, i32* %%%s, align 4\n",
                    temp_counter, instr->result);
            fprintf(output, "  %%t%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %%t%d)\n",
                    temp_counter + 1, temp_counter);
            temp_counter += 2;
        } else if (strcmp(instr->op, "LABEL") == 0) {
            fprintf(output, "\n%s:\n", instr->result);
        } else if (strcmp(instr->op, "GOTO") == 0) {
            fprintf(output, "  br label %%%s\n", instr->result);
        } else if (strcmp(instr->op, "IF_GOTO") == 0) {
            fprintf(output, "  %%t%d = icmp ne i32 %%%s, 0\n", temp_counter, instr->arg1);
            fprintf(output, "  br i1 %%t%d, label %%%s, label %%end%d\n",
                    temp_counter++, instr->result, label_counter++);
        } else if (strcmp(instr->op, "RETURN") == 0) {
            if (strlen(instr->result) > 0) {
                fprintf(output, "  ret i32 %s\n", instr->result);
            } else {
                fprintf(output, "  ret i32 0\n");
            }
        } else if (strcmp(instr->op, "CALL") == 0) {
            /* Function calls like log, exp */
            if (strcmp(instr->arg1, "log") == 0) {
                fprintf(output, "  %%t%d = call double @log(double %%%s)\n",
                        temp_counter++, instr->arg2);
            } else if (strcmp(instr->arg1, "exp") == 0) {
                fprintf(output, "  %%t%d = call double @exp(double %%%s)\n",
                        temp_counter++, instr->arg2);
            }
        }
    }
    
    fprintf(output, "}\n\n");
    fprintf(output, "declare i32 @printf(i8*, ...)\n");
    fprintf(output, "declare double @log(double)\n");
    fprintf(output, "declare double @exp(double)\n\n");
    fprintf(output, "@.str = private unnamed_addr constant [4 x i8] c\"%%d\\0A\\00\", align 1\n\n");
    fprintf(output, "attributes #0 = { noinline nounwind optnone uwtable \"frame-pointer\"=\"all\" }\n");
}

int main(int argc, char **argv) {
    printf("==================================\n");
    printf("Module 8: IR to LLVM Converter\n");
    printf("==================================\n\n");
    
    if (argc < 2) {
        printf("Usage: %s <ir_file.txt> [output.ll]\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s ir_output.txt output.ll\n", argv[0]);
        printf("  clang output.ll -o program -lm\n");
        return 1;
    }
    
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "ERROR: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    char *output_file = (argc > 2) ? argv[2] : "output.ll";
    FILE *output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "ERROR: Cannot create %s\n", output_file);
        fclose(input);
        return 1;
    }
    
    /* Parse input IR */
    char line[MAX_LINE];
    int in_ir_section = 0;
    
    while (fgets(line, sizeof(line), input)) {
        /* Detect IR section */
        if (strstr(line, "INTERMEDIATE REPRESENTATION")) {
            in_ir_section = 1;
            continue;
        }
        if (strstr(line, "=======") && in_ir_section) {
            continue;
        }
        
        if (in_ir_section && strlen(line) > 1) {
            parse_ir_line(line);
        }
    }
    
    fclose(input);
    
    printf("Parsed %d instructions\n", instr_count);
    printf("Generating LLVM IR...\n");
    
    /* Generate LLVM IR */
    generate_llvm_ir(output);
    fclose(output);
    
    printf("\n==================================\n");
    printf("Success! Output written to: %s\n", output_file);
    printf("==================================\n");
    printf("\nTo compile:\n");
    printf("  clang %s -o program -lm\n", output_file);
    
    return 0;
}
