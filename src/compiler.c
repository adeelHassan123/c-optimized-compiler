/*
 * Lab 11: Modular Optimizing Compiler
 * ====================================
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
    
    /* IR BEFORE OPTIMIZATION (Task 2 requirement) */
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
