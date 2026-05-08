#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "types.h"

#define MAX_SYMBOLS 256
#define IDENTIFIER_MAX 64
#define STRING_MAX 256

typedef struct {
    char name[IDENTIFIER_MAX];
    DataType type;
    int scope_level;
    int initialized;
    int used;
    int is_constant;
    int declared_line;
    
    union {
        long int_val;
        double float_val;
        char char_val;
        char string_val[STRING_MAX];
    } value;
} Symbol;

void symtab_init(void);
void symtab_enter_scope(void);
void symtab_exit_scope(void);
int symtab_add(const char *name, DataType type, int line);
Symbol *symtab_lookup(const char *name);
int symtab_exists_in_scope(const char *name);
void symtab_set_initialized(const char *name);
void symtab_set_used(const char *name);
void symtab_set_constant_value(const char *name, long int_val, double float_val);
void symtab_clear_constant_value(const char *name);
void symtab_print(void);
int symtab_get_error_count(void);

#endif
