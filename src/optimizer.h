#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ast.h"

/* ====================== OPTIMIZATION STATISTICS ====================== */

typedef struct {
    int constant_folds;           /* Constant folding */
    int constant_propagations;    /* Constant propagation */
    int dead_code_eliminations;   /* Dead code elimination */
    int unreachable_eliminations; /* Unreachable code */
    int algebraic_simplifications;/* Algebraic simplification */
    int strength_reductions;      /* Strength reduction (e.g. x*2 -> x+x) */
    int copy_propagations;        /* Copy propagation */
    int cse_eliminations;         /* Common subexpression elimination */
    int loop_invariants_moved;    /* Loop invariant code motion */
    int induction_vars_eliminated;/* Induction variable elimination */
    int loop_strength_reductions; /* Strength reduction in loops */
} OptimizationStats;

/* ====================== OPTIMIZER INTERFACE ====================== */

/* Optimize AST and return optimized version */
ASTNode *optimizer_optimize(ASTNode *ast);

/* Code optimization passes */
ASTNode *optimizer_constant_folding(ASTNode *node);
ASTNode *optimizer_constant_propagation(ASTNode *node);
ASTNode *optimizer_dead_code_elimination(ASTNode *node);
ASTNode *optimizer_unreachable_code_elimination(ASTNode *node);
ASTNode *optimizer_algebraic_simplification(ASTNode *node);
ASTNode *optimizer_strength_reduction(ASTNode *node);
ASTNode *optimizer_copy_propagation(ASTNode *node);
ASTNode *optimizer_common_subexpression_elimination(ASTNode *node);

/* Loop optimization passes */
ASTNode *optimizer_loop_invariant_motion(ASTNode *node);
ASTNode *optimizer_induction_elimination(ASTNode *node);
ASTNode *optimizer_loop_strength_reduction(ASTNode *node);
ASTNode *optimizer_loop_unrolling(ASTNode *node, int unroll_factor);

/* Get optimization statistics */
OptimizationStats optimizer_get_stats(void);

/* Print optimization report */
void optimizer_print_stats(void);

#endif /* OPTIMIZER_H */
