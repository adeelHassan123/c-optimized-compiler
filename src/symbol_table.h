#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "types.h"

/* ====================== SYMBOL STRUCTURE ====================== */

typedef struct {
    char name[IDENTIFIER_MAX];
    DataType type;
    int scope_level;
    int initialized;
    int used;
    int is_constant;  /* For constant propagation */
    int declared_line;
    
    /* Value storage (for constant folding) */
    union {
        long int_val;
        double float_val;
        char char_val;
        char string_val[STRING_MAX];
    } value;
} Symbol;

/* ====================== SYMBOL TABLE INTERFACE ====================== */

/* Initialize symbol table */
void symtab_init(void);

/* Enter new scope */
void symtab_enter_scope(void);

/* Exit current scope */
void symtab_exit_scope(void);

/* Add symbol to current scope */
int symtab_add(const char *name, DataType type, int line);

/* Find symbol in symbol table */
Symbol *symtab_lookup(const char *name);

/* Check if symbol exists in current scope only */
int symtab_exists_in_scope(const char *name);

/* Mark symbol as initialized */
void symtab_set_initialized(const char *name);

/* Mark symbol as used */
void symtab_set_used(const char *name);

/* Set constant value (for optimization) */
void symtab_set_constant_value(const char *name, long int_val, double float_val);

/* Clear constant flag when value becomes non-constant */
void symtab_clear_constant_value(const char *name);

/* Print symbol table */
void symtab_print(void);

/* Get error count */
int symtab_get_error_count(void);

#endif /* SYMBOL_TABLE_H */
