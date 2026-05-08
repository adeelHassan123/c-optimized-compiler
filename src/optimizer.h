#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "types.h"

typedef struct {
    int constant_folds;
    int constant_propagations;
    int dead_code_eliminations;
} OptimizationStats;

ASTNode *optimizer_optimize(ASTNode *ast);
OptimizationStats optimizer_get_stats(void);
void optimizer_print_stats(void);

#endif
