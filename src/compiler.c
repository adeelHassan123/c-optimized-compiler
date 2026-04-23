/*
 * Modular Optimizing Compiler
 * ===========================
 * 
 * A well-structured, modular compiler with clear separation of concerns:
 * 
 * MODULES:
 *   1. Lexer (lexer.c/h)         - Tokenization
 *   2. Parser (parser.c/h)       - Syntax analysis & AST construction
 *   3. AST (ast.c/h)             - Abstract Syntax Tree management
 *   4. Symbol Table (symbol_table.c/h) - Semantic analysis & symbol management
 *   5. Optimizer (optimizer.c/h) - Code optimization passes
 *   6. IR (ir.c/h)              - Intermediate representation generation
 *   7. Types (types.h)          - Common type definitions
 * 
 * COMPILATION STAGES:
 *   1. Lexical Analysis   -> Tokens
 *   2. Parsing           -> AST
 *   3. Semantic Analysis -> Validated AST
 *   4. Optimization      -> Optimized AST
 *   5. IR Generation     -> Three-address code
 * 
 * OPTIMIZATIONS IMPLEMENTED (safe & applied):
 *   - Constant Folding
 *   - Constant Propagation
 *   - Algebraic Simplification
 *   - Strength Reduction
 *   - Dead Code Elimination (limited)
 *   - Unreachable Code Elimination
 *   - Copy/Constant Tracking (enables propagation)
 *
 * Notes:
 *   - Advanced optimizations such as common subexpression elimination and
 *     loop transformations are intentionally not implemented as real AST
 *     rewrites in this lab version (to avoid incorrect logic).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"
#include "optimizer.h"
#include "ir.h"

/* ====================== STATISTICS ====================== */

typedef struct {
    int total_tokens;
    int total_lines;
    int parse_errors;
    int semantic_errors;
    long lex_time_ms;
    long parse_time_ms;
    long opt_time_ms;
    long ir_time_ms;
} CompilationStats;

static CompilationStats stats = {0};

/* ====================== HELPER FUNCTIONS ====================== */

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size > MAX_SOURCE) {
        fprintf(stderr, "ERROR: File too large (max %d bytes)\n", MAX_SOURCE);
        fclose(f);
        return NULL;
    }
    
    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    return buffer;
}

static long get_timestamp_ms(void) {
    return (long)(clock() * 1000.0 / CLOCKS_PER_SEC);
}

typedef struct {
    char name[IDENTIFIER_MAX];
    double value;
    int used;
} ExecSlot;

typedef struct {
    int label;
    int instr_index;
} LabelSlot;

static ExecSlot *exec_get_slot(ExecSlot *slots, int max_slots, const char *name) {
    for (int i = 0; i < max_slots; i++) {
        if (slots[i].used && strcmp(slots[i].name, name) == 0) {
            return &slots[i];
        }
    }
    for (int i = 0; i < max_slots; i++) {
        if (!slots[i].used) {
            slots[i].used = 1;
            strncpy(slots[i].name, name, IDENTIFIER_MAX - 1);
            slots[i].name[IDENTIFIER_MAX - 1] = '\0';
            slots[i].value = 0.0;
            return &slots[i];
        }
    }
    return NULL;
}

static double exec_get_value(IRValue v, ExecSlot *slots, int max_slots) {
    switch (v.type) {
        case IR_VAL_CONST:
            return v.value.float_val;
        case IR_VAL_VAR:
        case IR_VAL_TEMP: {
            ExecSlot *s = exec_get_slot(slots, max_slots, v.name);
            return s ? s->value : 0.0;
        }
        default:
            return 0.0;
    }
}

static void exec_set_value(IRValue target, double value, ExecSlot *slots, int max_slots) {
    if (target.type != IR_VAL_VAR && target.type != IR_VAL_TEMP) return;
    ExecSlot *s = exec_get_slot(slots, max_slots, target.name);
    if (s) s->value = value;
}

static int exec_label_target(LabelSlot *labels, int label_count, int label) {
    for (int i = 0; i < label_count; i++) {
        if (labels[i].label == label) return labels[i].instr_index;
    }
    return -1;
}

static int execute_ir(IRCode *ir, int suppress_output) {
    if (!ir) return 0;

    ExecSlot slots[MAX_IR_NODES] = {0};
    LabelSlot labels[MAX_IR_NODES];
    int label_count = 0;

    for (int i = 0; i < ir->instruction_count; i++) {
        if (ir->instructions[i].op == IR_LABEL) {
            labels[label_count].label = ir->instructions[i].label;
            labels[label_count].instr_index = i;
            label_count++;
        }
    }

    int pc = 0;
    while (pc >= 0 && pc < ir->instruction_count) {
        IRInstruction *ins = &ir->instructions[pc];
        double a = 0.0, b = 0.0, r = 0.0;

        switch (ins->op) {
            case IR_ASSIGN:
                exec_set_value(ins->result, exec_get_value(ins->operand1, slots, MAX_IR_NODES), slots, MAX_IR_NODES);
                pc++;
                break;

            case IR_BINOP:
                a = exec_get_value(ins->operand1, slots, MAX_IR_NODES);
                b = exec_get_value(ins->operand2, slots, MAX_IR_NODES);
                switch (ins->op_type) {
                    case TOK_PLUS: r = a + b; break;
                    case TOK_MINUS: r = a - b; break;
                    case TOK_MUL: r = a * b; break;
                    case TOK_DIV: r = (b != 0.0) ? (a / b) : 0.0; break;
                    case TOK_MOD: r = (b != 0.0) ? (double)((long)a % (long)b) : 0.0; break;
                    case TOK_EQ: r = (a == b) ? 1.0 : 0.0; break;
                    case TOK_NEQ: r = (a != b) ? 1.0 : 0.0; break;
                    case TOK_LT: r = (a < b) ? 1.0 : 0.0; break;
                    case TOK_LE: r = (a <= b) ? 1.0 : 0.0; break;
                    case TOK_GT: r = (a > b) ? 1.0 : 0.0; break;
                    case TOK_GE: r = (a >= b) ? 1.0 : 0.0; break;
                    case TOK_AND: r = ((a != 0.0) && (b != 0.0)) ? 1.0 : 0.0; break;
                    case TOK_OR: r = ((a != 0.0) || (b != 0.0)) ? 1.0 : 0.0; break;
                    default: r = 0.0; break;
                }
                exec_set_value(ins->result, r, slots, MAX_IR_NODES);
                pc++;
                break;

            case IR_UNOP:
                a = exec_get_value(ins->operand1, slots, MAX_IR_NODES);
                if (ins->op_type == TOK_MINUS) r = -a;
                else if (ins->op_type == TOK_NOT) r = (a == 0.0) ? 1.0 : 0.0;
                else r = a;
                exec_set_value(ins->result, r, slots, MAX_IR_NODES);
                pc++;
                break;

            case IR_IF_GOTO:
                a = exec_get_value(ins->operand1, slots, MAX_IR_NODES);
                if (a != 0.0) {
                    int target = exec_label_target(labels, label_count, ins->label);
                    pc = (target >= 0) ? target : (pc + 1);
                } else {
                    pc++;
                }
                break;

            case IR_GOTO: {
                int target = exec_label_target(labels, label_count, ins->label);
                pc = (target >= 0) ? target : (pc + 1);
                break;
            }

            case IR_LABEL:
                pc++;
                break;

            case IR_PRINT:
                if (!suppress_output) {
                    double val = exec_get_value(ins->result, slots, MAX_IR_NODES);
                    printf("%g\n", val);
                }
                pc++;
                break;

            case IR_RETURN:
                return (int)exec_get_value(ins->result, slots, MAX_IR_NODES);

            default:
                pc++;
                break;
        }
    }

    return 0;
}

static long benchmark_ir(IRCode *ir, int runs) {
    long start = get_timestamp_ms();
    for (int i = 0; i < runs; i++) {
        (void)execute_ir(ir, 1);
    }
    return get_timestamp_ms() - start;
}

/* ====================== COMPILATION PIPELINE ====================== */

static int compile(const char *source_file) {
    printf("\n========== MODULAR OPTIMIZING COMPILER ==========\n");
    printf("Source: %s\n", source_file);
    printf("===================================================\n\n");
    
    /* READ SOURCE FILE */
    printf("[1/5] Reading source file...\n");
    char *source = read_file(source_file);
    if (!source) return 1;
    
    /* LEXICAL ANALYSIS */
    printf("[2/5] Lexical Analysis...\n");
    long lex_start = get_timestamp_ms();
    
    lexer_init(source);
    
    Token tokens[MAX_TOKENS];
    int token_count = 0;
    Token tok;
    
    do {
        tok = lexer_next_token();
        if (token_count < MAX_TOKENS) {
            tokens[token_count++] = tok;
        }
    } while (tok.type != TOK_EOF && token_count < MAX_TOKENS);
    
    stats.lex_time_ms = get_timestamp_ms() - lex_start;
    stats.total_tokens = token_count;
    
    printf("  ✓ Lexical analysis complete: %d tokens in %ld ms\n", 
           token_count, stats.lex_time_ms);
    
    /* PARSING */
    printf("[3/5] Parsing & Semantic Analysis...\n");
    long parse_start = get_timestamp_ms();
    
    parser_init(tokens, token_count);
    ASTNode *ast = parser_parse_program();
    
    stats.parse_errors = parser_get_error_count();
    stats.parse_time_ms = get_timestamp_ms() - parse_start;
    
    if (stats.parse_errors > 0) {
        printf("  ✗ Parser errors: %d\n", stats.parse_errors);
        return 1;
    }
    
    printf("  ✓ Parsing complete in %ld ms\n", stats.parse_time_ms);
    printf("  ✓ AST constructed successfully\n");
    
    /* SYMBOL TABLE & SEMANTIC CHECKING */
    stats.semantic_errors = symtab_get_error_count();
    if (stats.semantic_errors == 0) {
        printf("  ✓ Semantic analysis passed\n");
    } else {
        printf("  ✗ Semantic errors: %d\n", stats.semantic_errors);
    }
    
    /* IR BEFORE OPTIMIZATION */
    printf("[4/6] IR Generation (Before Optimization)...\n");
    long ir_start = get_timestamp_ms();
    IRCode *ir_before_opt = ir_generate(ast);
    stats.ir_time_ms = get_timestamp_ms() - ir_start;
    printf("  ✓ Pre-optimization IR generated in %ld ms\n", stats.ir_time_ms);

    /* OPTIMIZATION */
    printf("[5/6] Code Optimization...\n");
    long opt_start = get_timestamp_ms();
    
    ASTNode *optimized_ast = optimizer_optimize(ast);
    
    stats.opt_time_ms = get_timestamp_ms() - opt_start;
    printf("  ✓ Optimization complete in %ld ms\n", stats.opt_time_ms);
    
    /* IR AFTER OPTIMIZATION */
    printf("[6/6] IR Generation (After Optimization)...\n");
    ir_start = get_timestamp_ms();
    IRCode *ir_after_opt = ir_generate(optimized_ast);
    stats.ir_time_ms += (get_timestamp_ms() - ir_start);
    printf("  ✓ Post-optimization IR generated\n");
    
    /* ====================== OUTPUT ====================== */
    
    printf("\n========== COMPILATION OUTPUT ==========\n\n");
    
    /* AST Visualization */
    printf("ABSTRACT SYNTAX TREE:\n");
    printf("====================\n");
    if (optimized_ast) {
        ast_print(optimized_ast, 0);
    }
    
    /* Symbol Table */
    symtab_print();
    
    /* Optimization Statistics */
    optimizer_print_stats();
    
    /* Intermediate Representation */
    printf("\nIR BEFORE OPTIMIZATION:\n");
    ir_print(ir_before_opt);
    printf("\nIR AFTER OPTIMIZATION:\n");
    ir_print(ir_after_opt);

    /* Execution time comparison */
    {
        const int benchmark_runs = 1000;
        long before_ms = benchmark_ir(ir_before_opt, benchmark_runs);
        long after_ms = benchmark_ir(ir_after_opt, benchmark_runs);
        double speedup = (after_ms > 0) ? ((double)before_ms / (double)after_ms) : 0.0;

        printf("\n========== EXECUTION TIME COMPARISON ==========\n");
        printf("Benchmark runs:             %d\n", benchmark_runs);
        printf("Before optimization:        %ld ms\n", before_ms);
        printf("After optimization:         %ld ms\n", after_ms);
        if (after_ms > 0) {
            printf("Speedup (before/after):     %.2fx\n", speedup);
        } else {
            printf("Speedup (before/after):     n/a (after time is 0 ms)\n");
        }
        printf("===============================================\n");
    }
    
    /* Compilation Summary */
    printf("\n========== COMPILATION SUMMARY ==========\n");
    printf("Total Tokens:          %d\n", stats.total_tokens);
    printf("Parser Errors:         %d\n", stats.parse_errors);
    printf("Semantic Errors:       %d\n", stats.semantic_errors);
    printf("\nCompilation Times:\n");
    printf("  Lexical Analysis:    %ld ms\n", stats.lex_time_ms);
    printf("  Parsing:             %ld ms\n", stats.parse_time_ms);
    printf("  Optimization:        %ld ms\n", stats.opt_time_ms);
    printf("  IR Generation:       %ld ms\n", stats.ir_time_ms);
    printf("  Total:               %ld ms\n", 
           stats.lex_time_ms + stats.parse_time_ms + stats.opt_time_ms + stats.ir_time_ms);
    printf("=========================================\n");
    
    /* Cleanup */
    free(source);
    ir_free(ir_before_opt);
    ir_free(ir_after_opt);
    
    return (stats.parse_errors + stats.semantic_errors) > 0 ? 1 : 0;
}

/* ====================== MAIN ====================== */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s program.c\n", argv[0]);
        printf("\nSupported syntax:\n");
        printf("  main() { ... }   OR   int main() { ... }\n");
        printf("  int, float, char variable declarations\n");
        printf("  Arithmetic: +, -, *, /, %%\n");
        printf("  Comparison: ==, !=, <, <=, >, >=\n");
        printf("  Logical: &&, ||\n");
        printf("  Control: if, else, while, for\n");
        printf("  I/O: print()\n");
        return 1;
    }
    
    int result = compile(argv[1]);
    
    return result;
}
