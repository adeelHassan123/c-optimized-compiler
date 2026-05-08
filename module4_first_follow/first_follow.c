/* Module 4: FIRST & FOLLOW Sets / LL(1) Parsing Table
 * CS-346 Compiler Construction - Lab Project
 * 
 * Computes FIRST and FOLLOW sets for a given grammar
 * and constructs the LL(1) parsing table.
 * 
 * Target Grammar:
 * E  -> T E'
 * E' -> + T E' | - T E' | ε
 * T  -> F T'
 * T' -> * F T' | / F T' | ε
 * F  -> ( E ) | id | num
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOLS 50
#define MAX_PRODUCTIONS 50
#define MAX_SYMBOL_LEN 20
#define MAX_FIRST 20
#define MAX_FOLLOW 20

/* Grammar representation */
typedef struct {
    char lhs[MAX_SYMBOL_LEN];
    char rhs[MAX_SYMBOLS][MAX_SYMBOL_LEN];
    int rhs_count;
} Production;

/* Symbol table */
typedef struct {
    char name[MAX_SYMBOL_LEN];
    int is_terminal;
    int is_epsilon;  /* Represents ε */
    int is_start;
} Symbol;

/* FIRST and FOLLOW sets */
typedef struct {
    char symbols[MAX_FIRST][MAX_SYMBOL_LEN];
    int count;
} Set;

/* Global data */
Production grammar[MAX_PRODUCTIONS];
int production_count = 0;

Symbol symbols[MAX_SYMBOLS];
int symbol_count = 0;

Set first_sets[MAX_SYMBOLS];
Set follow_sets[MAX_SYMBOLS];

/* LL(1) Parsing Table */
typedef struct {
    char non_terminal[MAX_SYMBOL_LEN];
    char terminal[MAX_SYMBOL_LEN];
    int production_idx;  /* -1 if empty */
} ParseTableEntry;

ParseTableEntry parse_table[MAX_SYMBOLS][MAX_SYMBOLS];
int nt_count = 0;  /* Non-terminal count */
int t_count = 0;   /* Terminal count */

/* Function prototypes */
void init_grammar();
void add_production(const char *lhs, const char *rhs_list);
int find_symbol(const char *name);
int is_terminal(const char *symbol);
int is_epsilon(const char *symbol);
void compute_first();
void compute_follow();
void compute_first_of_string(char *str[], int count, Set *result);
void build_parse_table();
void print_first_sets();
void print_follow_sets();
void print_parse_table();
void print_grammar();

/* Initialize the target grammar */
void init_grammar() {
    printf("Initializing Grammar...\n");
    printf("=======================\n");
    
    /* E -> T E' */
    add_production("E", "T E'");
    
    /* E' -> + T E' | - T E' | ε */
    add_production("E'", "+ T E'");
    add_production("E'", "- T E'");
    add_production("E'", "ε");
    
    /* T -> F T' */
    add_production("T", "F T'");
    
    /* T' -> * F T' | / F T' | ε */
    add_production("T'", "* F T'");
    add_production("T'", "/ F T'");
    add_production("T'", "ε");
    
    /* F -> ( E ) | id | num */
    add_production("F", "( E )");
    add_production("F", "id");
    add_production("F", "num");
    
    printf("Grammar initialized with %d productions.\n\n", production_count);
}

void add_production(const char *lhs, const char *rhs_list) {
    Production *p = &grammar[production_count++];
    strncpy(p->lhs, lhs, MAX_SYMBOL_LEN - 1);
    p->lhs[MAX_SYMBOL_LEN - 1] = '\0';
    
    /* Parse RHS symbols */
    char temp[256];
    strncpy(temp, rhs_list, 255);
    temp[255] = '\0';
    
    char *token = strtok(temp, " ");
    p->rhs_count = 0;
    while (token != NULL) {
        strncpy(p->rhs[p->rhs_count], token, MAX_SYMBOL_LEN - 1);
        p->rhs[p->rhs_count][MAX_SYMBOL_LEN - 1] = '\0';
        p->rhs_count++;
        token = strtok(NULL, " ");
    }
}

void print_grammar() {
    printf("Grammar Productions:\n");
    printf("====================\n");
    for (int i = 0; i < production_count; i++) {
        Production *p = &grammar[i];
        printf("%2d: %s -> ", i, p->lhs);
        for (int j = 0; j < p->rhs_count; j++) {
            printf("%s ", p->rhs[j]);
        }
        printf("\n");
    }
    printf("\n");
}

int find_symbol(const char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbols[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int is_terminal(const char *symbol) {
    /* Terminals: lowercase, operators, parentheses, $ */
    if (strcmp(symbol, "$") == 0 ||
        strcmp(symbol, "+") == 0 ||
        strcmp(symbol, "-") == 0 ||
        strcmp(symbol, "*") == 0 ||
        strcmp(symbol, "/") == 0 ||
        strcmp(symbol, "(") == 0 ||
        strcmp(symbol, ")") == 0 ||
        strcmp(symbol, "id") == 0 ||
        strcmp(symbol, "num") == 0) {
        return 1;
    }
    return 0;
}

int is_epsilon(const char *symbol) {
    return strcmp(symbol, "ε") == 0 || strcmp(symbol, "epsilon") == 0;
}

/* Add symbol to a set (if not already present) */
int add_to_set(Set *set, const char *symbol) {
    for (int i = 0; i < set->count; i++) {
        if (strcmp(set->symbols[i], symbol) == 0) {
            return 0;  /* Already exists */
        }
    }
    strncpy(set->symbols[set->count], symbol, MAX_SYMBOL_LEN - 1);
    set->symbols[set->count][MAX_SYMBOL_LEN - 1] = '\0';
    set->count++;
    return 1;
}

/* Compute FIRST sets */
void compute_first() {
    printf("Computing FIRST sets...\n");
    printf("======================\n");
    
    /* Initialize FIRST sets for terminals */
    /* Terminals have themselves in their FIRST set */
    
    /* Iterate until no changes */
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < production_count; i++) {
            Production *p = &grammar[i];
            int lhs_idx = find_symbol(p->lhs);
            if (lhs_idx == -1) {
                /* Add to symbols */
                strncpy(symbols[symbol_count].name, p->lhs, MAX_SYMBOL_LEN - 1);
                symbols[symbol_count].name[MAX_SYMBOL_LEN - 1] = '\0';
                symbols[symbol_count].is_terminal = 0;
                lhs_idx = symbol_count++;
            }
            
            Set *lhs_first = &first_sets[lhs_idx];
            
            /* For production A -> α */
            if (p->rhs_count == 1 && is_epsilon(p->rhs[0])) {
                /* A -> ε, add ε to FIRST(A) */
                if (add_to_set(lhs_first, "ε")) {
                    changed = 1;
                }
            } else {
                /* For each symbol in RHS */
                int all_epsilon = 1;
                for (int j = 0; j < p->rhs_count && all_epsilon; j++) {
                    const char *symbol = p->rhs[j];
                    
                    if (is_terminal(symbol)) {
                        /* Terminal: add to FIRST(A) */
                        if (add_to_set(lhs_first, symbol)) {
                            changed = 1;
                        }
                        all_epsilon = 0;
                    } else {
                        /* Non-terminal */
                        int sym_idx = find_symbol(symbol);
                        if (sym_idx == -1) {
                            strncpy(symbols[symbol_count].name, symbol, MAX_SYMBOL_LEN - 1);
                            symbols[symbol_count].name[MAX_SYMBOL_LEN - 1] = '\0';
                            symbols[symbol_count].is_terminal = 0;
                            sym_idx = symbol_count++;
                        }
                        
                        /* Add FIRST(symbol) - {ε} to FIRST(A) */
                        Set *sym_first = &first_sets[sym_idx];
                        for (int k = 0; k < sym_first->count; k++) {
                            if (!is_epsilon(sym_first->symbols[k])) {
                                if (add_to_set(lhs_first, sym_first->symbols[k])) {
                                    changed = 1;
                                }
                            }
                        }
                        
                        /* Check if symbol can derive ε */
                        int has_epsilon = 0;
                        for (int k = 0; k < sym_first->count; k++) {
                            if (is_epsilon(sym_first->symbols[k])) {
                                has_epsilon = 1;
                                break;
                            }
                        }
                        
                        if (!has_epsilon) {
                            all_epsilon = 0;
                        }
                    }
                }
                
                /* If all symbols can derive ε, add ε to FIRST(A) */
                if (all_epsilon) {
                    if (add_to_set(lhs_first, "ε")) {
                        changed = 1;
                    }
                }
            }
        }
    }
    
    printf("FIRST sets computed.\n\n");
}

/* Compute FOLLOW sets */
void compute_follow() {
    printf("Computing FOLLOW sets...\n");
    printf("=========================\n");
    
    /* Add $ to FOLLOW(S) where S is the start symbol */
    int e_idx = find_symbol("E");
    if (e_idx >= 0) {
        add_to_set(&follow_sets[e_idx], "$");
    }
    
    /* Iterate until no changes */
    int changed = 1;
    while (changed) {
        changed = 0;
        
        for (int i = 0; i < production_count; i++) {
            Production *p = &grammar[i];
            int lhs_idx = find_symbol(p->lhs);
            
            /* For production A -> αBβ */
            for (int j = 0; j < p->rhs_count; j++) {
                const char *b = p->rhs[j];
                
                if (is_terminal(b) || is_epsilon(b)) {
                    continue;
                }
                
                int b_idx = find_symbol(b);
                if (b_idx == -1) continue;
                
                Set *b_follow = &follow_sets[b_idx];
                
                /* If B is followed by β */
                if (j + 1 < p->rhs_count) {
                    /* Compute FIRST(β) */
                    Set first_beta;
                    first_beta.count = 0;
                    
                    int all_epsilon = 1;
                    for (int k = j + 1; k < p->rhs_count && all_epsilon; k++) {
                        const char *symbol = p->rhs[k];
                        
                        if (is_terminal(symbol)) {
                            add_to_set(&first_beta, symbol);
                            all_epsilon = 0;
                        } else {
                            int sym_idx = find_symbol(symbol);
                            if (sym_idx >= 0) {
                                Set *sym_first = &first_sets[sym_idx];
                                for (int m = 0; m < sym_first->count; m++) {
                                    if (!is_epsilon(sym_first->symbols[m])) {
                                        add_to_set(&first_beta, sym_first->symbols[m]);
                                    }
                                }
                                
                                int has_epsilon = 0;
                                for (int m = 0; m < sym_first->count; m++) {
                                    if (is_epsilon(sym_first->symbols[m])) {
                                        has_epsilon = 1;
                                        break;
                                    }
                                }
                                if (!has_epsilon) {
                                    all_epsilon = 0;
                                }
                            }
                        }
                    }
                    
                    /* Add FIRST(β) - {ε} to FOLLOW(B) */
                    for (int k = 0; k < first_beta.count; k++) {
                        if (add_to_set(b_follow, first_beta.symbols[k])) {
                            changed = 1;
                        }
                    }
                    
                    /* If β can derive ε, add FOLLOW(A) to FOLLOW(B) */
                    if (all_epsilon && lhs_idx >= 0) {
                        Set *lhs_follow = &follow_sets[lhs_idx];
                        for (int k = 0; k < lhs_follow->count; k++) {
                            if (add_to_set(b_follow, lhs_follow->symbols[k])) {
                                changed = 1;
                            }
                        }
                    }
                } else {
                    /* B is at end: add FOLLOW(A) to FOLLOW(B) */
                    if (lhs_idx >= 0) {
                        Set *lhs_follow = &follow_sets[lhs_idx];
                        for (int k = 0; k < lhs_follow->count; k++) {
                            if (add_to_set(b_follow, lhs_follow->symbols[k])) {
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    printf("FOLLOW sets computed.\n\n");
}

void print_first_sets() {
    printf("FIRST Sets:\n");
    printf("===========\n");
    printf("%-10s Nullable?  FIRST\n", "Non-Term");
    printf("------------------------------------------\n");
    
    for (int i = 0; i < symbol_count; i++) {
        if (!symbols[i].is_terminal) {
            printf("%-10s ", symbols[i].name);
            
            Set *first = &first_sets[i];
            int nullable = 0;
            
            /* Check if nullable */
            for (int j = 0; j < first->count; j++) {
                if (is_epsilon(first->symbols[j])) {
                    nullable = 1;
                    break;
                }
            }
            
            printf("%-8s  { ", nullable ? "Yes" : "No");
            
            int first_printed = 0;
            for (int j = 0; j < first->count; j++) {
                if (!is_epsilon(first->symbols[j])) {
                    if (first_printed) printf(", ");
                    printf("%s", first->symbols[j]);
                    first_printed = 1;
                }
            }
            printf(" }\n");
        }
    }
    printf("\n");
}

void print_follow_sets() {
    printf("FOLLOW Sets:\n");
    printf("============\n");
    printf("%-10s FOLLOW\n", "Non-Term");
    printf("------------------------------------------\n");
    
    for (int i = 0; i < symbol_count; i++) {
        if (!symbols[i].is_terminal) {
            printf("%-10s { ", symbols[i].name);
            
            Set *follow = &follow_sets[i];
            for (int j = 0; j < follow->count; j++) {
                if (j > 0) printf(", ");
                printf("%s", follow->symbols[j]);
            }
            printf(" }\n");
        }
    }
    printf("\n");
}

/* Build LL(1) Parsing Table */
void build_parse_table() {
    printf("Building LL(1) Parsing Table...\n");
    printf("=================================\n");
    
    /* Initialize table to -1 (empty) */
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            parse_table[i][j].production_idx = -1;
        }
    }
    
    /* Collect terminals and non-terminals */
    char terminals[MAX_SYMBOLS][MAX_SYMBOL_LEN];
    t_count = 0;
    
    /* For each production A -> α */
    for (int i = 0; i < production_count; i++) {
        Production *p = &grammar[i];
        int a_idx = find_symbol(p->lhs);
        
        /* For each terminal a in FIRST(α) */
        Set first_alpha;
        first_alpha.count = 0;
        
        int all_epsilon = 1;
        for (int j = 0; j < p->rhs_count && all_epsilon; j++) {
            const char *symbol = p->rhs[j];
            
            if (is_epsilon(symbol)) {
                continue;
            } else if (is_terminal(symbol)) {
                add_to_set(&first_alpha, symbol);
                all_epsilon = 0;
            } else {
                int sym_idx = find_symbol(symbol);
                if (sym_idx >= 0) {
                    Set *sym_first = &first_sets[sym_idx];
                    for (int k = 0; k < sym_first->count; k++) {
                        if (!is_epsilon(sym_first->symbols[k])) {
                            add_to_set(&first_alpha, sym_first->symbols[k]);
                        }
                    }
                    
                    int has_epsilon = 0;
                    for (int k = 0; k < sym_first->count; k++) {
                        if (is_epsilon(sym_first->symbols[k])) {
                            has_epsilon = 1;
                            break;
                        }
                    }
                    if (!has_epsilon) {
                        all_epsilon = 0;
                    }
                }
            }
        }
        
        /* Add M[A, a] = A -> α for each a in FIRST(α) */
        for (int j = 0; j < first_alpha.count; j++) {
            /* Find or add terminal */
            int t_idx = -1;
            for (int k = 0; k < t_count; k++) {
                if (strcmp(terminals[k], first_alpha.symbols[j]) == 0) {
                    t_idx = k;
                    break;
                }
            }
            if (t_idx == -1) {
                strncpy(terminals[t_count], first_alpha.symbols[j], MAX_SYMBOL_LEN - 1);
                terminals[t_count][MAX_SYMBOL_LEN - 1] = '\0';
                t_idx = t_count++;
            }
            
            parse_table[a_idx][t_idx].production_idx = i;
            strncpy(parse_table[a_idx][t_idx].non_terminal, p->lhs, MAX_SYMBOL_LEN - 1);
            strncpy(parse_table[a_idx][t_idx].terminal, first_alpha.symbols[j], MAX_SYMBOL_LEN - 1);
        }
        
        /* If ε is in FIRST(α), add M[A, b] for each b in FOLLOW(A) */
        if (all_epsilon) {
            Set *a_follow = &follow_sets[a_idx];
            for (int j = 0; j < a_follow->count; j++) {
                /* Find or add terminal */
                int t_idx = -1;
                for (int k = 0; k < t_count; k++) {
                    if (strcmp(terminals[k], a_follow->symbols[j]) == 0) {
                        t_idx = k;
                        break;
                    }
                }
                if (t_idx == -1) {
                    strncpy(terminals[t_count], a_follow->symbols[j], MAX_SYMBOL_LEN - 1);
                    terminals[t_count][MAX_SYMBOL_LEN - 1] = '\0';
                    t_idx = t_count++;
                }
                
                parse_table[a_idx][t_idx].production_idx = i;
                strncpy(parse_table[a_idx][t_idx].non_terminal, p->lhs, MAX_SYMBOL_LEN - 1);
                strncpy(parse_table[a_idx][t_idx].terminal, a_follow->symbols[j], MAX_SYMBOL_LEN - 1);
            }
        }
    }
    
    printf("LL(1) Parsing Table built.\n\n");
    
    /* Print table */
    printf("LL(1) Parsing Table:\n");
    printf("====================\n\n");
    
    /* Header row */
    printf("%-8s |", "");
    for (int j = 0; j < t_count; j++) {
        printf(" %-10s |", terminals[j]);
    }
    printf("\n");
    
    /* Separator */
    printf("---------");
    for (int j = 0; j < t_count; j++) {
        printf("-------------");
    }
    printf("\n");
    
    /* Table rows */
    for (int i = 0; i < symbol_count; i++) {
        if (!symbols[i].is_terminal) {
            printf("%-8s |", symbols[i].name);
            
            for (int j = 0; j < t_count; j++) {
                if (parse_table[i][j].production_idx >= 0) {
                    printf(" %-10d |", parse_table[i][j].production_idx);
                } else {
                    printf(" %-10s |", "");
                }
            }
            printf("\n");
        }
    }
    
    printf("\nProduction numbers reference:\n");
    for (int i = 0; i < production_count; i++) {
        printf("%2d: ", i);
        Production *p = &grammar[i];
        printf("%s -> ", p->lhs);
        for (int j = 0; j < p->rhs_count; j++) {
            printf("%s ", p->rhs[j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    printf("=================================================\n");
    printf("Module 4: FIRST & FOLLOW Sets / LL(1) Table\n");
    printf("CS-346 Compiler Construction - Lab Project\n");
    printf("=================================================\n\n");
    
    /* Initialize data */
    memset(symbols, 0, sizeof(symbols));
    memset(first_sets, 0, sizeof(first_sets));
    memset(follow_sets, 0, sizeof(follow_sets));
    
    /* Setup grammar */
    init_grammar();
    print_grammar();
    
    /* Compute FIRST sets */
    compute_first();
    print_first_sets();
    
    /* Compute FOLLOW sets */
    compute_follow();
    print_follow_sets();
    
    /* Build LL(1) Parsing Table */
    build_parse_table();
    
    printf("=================================================\n");
    printf("Summary:\n");
    printf("  - Grammar has %d productions\n", production_count);
    printf("  - FIRST sets computed for all non-terminals\n");
    printf("  - FOLLOW sets computed for all non-terminals\n");
    printf("  - LL(1) parsing table constructed\n");
    printf("=================================================\n");
    
    return 0;
}
