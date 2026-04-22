#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================== SYMBOL TABLE STATE ====================== */

static Symbol symtab[MAX_SYMBOLS];
static int sym_count = 0;
static int current_scope = 0;
static int error_count = 0;

typedef struct {
    int start;
    int end;
} ScopeInfo;

static ScopeInfo scopes[100];
static int scope_count = 0;

/* ====================== TYPE NAME CONVERSION ====================== */

static const char *type_to_string(DataType t) {
    switch (t) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "string";
        case TYPE_VOID: return "void";
        default: return "unknown";
    }
}

/* ====================== INITIALIZATION ====================== */

void symtab_init(void) {
    sym_count = 0;
    current_scope = 0;
    error_count = 0;
    scope_count = 0;
    
    scopes[0].start = 0;
    scopes[0].end = 0;
    scope_count = 1;
}

/* ====================== SCOPE MANAGEMENT ====================== */

void symtab_enter_scope(void) {
    if (scope_count >= 100) {
        fprintf(stderr, "ERROR: Maximum scope depth exceeded\n");
        return;
    }
    
    current_scope = scope_count;
    scopes[current_scope].start = sym_count;
    scopes[current_scope].end = sym_count;
    scope_count++;
}

void symtab_exit_scope(void) {
    if (current_scope > 0) {
        current_scope--;
    }
}

/* ====================== SYMBOL LOOKUP ====================== */

static int find_symbol_index(const char *name) {
    /* Search from current scope backwards to global scope */
    for (int i = sym_count - 1; i >= 0; i--) {
        if (strcmp(symtab[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static int find_symbol_in_scope(const char *name, int scope_level) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(symtab[i].name, name) == 0 && symtab[i].scope_level == scope_level) {
            return i;
        }
    }
    return -1;
}

Symbol *symtab_lookup(const char *name) {
    int idx = find_symbol_index(name);
    if (idx >= 0) {
        return &symtab[idx];
    }
    return NULL;
}

int symtab_exists_in_scope(const char *name) {
    return find_symbol_in_scope(name, current_scope) >= 0;
}

/* ====================== SYMBOL ADDITION ====================== */

int symtab_add(const char *name, DataType type, int line) {
    if (sym_count >= MAX_SYMBOLS) {
        fprintf(stderr, "ERROR: Symbol table full\n");
        error_count++;
        return -1;
    }
    
    /* Check for duplicate in current scope */
    if (symtab_exists_in_scope(name)) {
        fprintf(stderr, "ERROR: Duplicate symbol '%s' at line %d\n", name, line);
        error_count++;
        return -1;
    }
    
    Symbol *s = &symtab[sym_count];
    strncpy(s->name, name, IDENTIFIER_MAX - 1);
    s->name[IDENTIFIER_MAX - 1] = '\0';
    s->type = type;
    s->scope_level = current_scope;
    s->initialized = 0;
    s->used = 0;
    s->is_constant = 0;
    s->declared_line = line;
    memset(&s->value, 0, sizeof(s->value));
    
    return sym_count++;
}

/* ====================== SYMBOL ATTRIBUTES ====================== */

void symtab_set_initialized(const char *name) {
    Symbol *s = symtab_lookup(name);
    if (s) {
        s->initialized = 1;
    }
}

void symtab_set_used(const char *name) {
    Symbol *s = symtab_lookup(name);
    if (s) {
        s->used = 1;
    }
}

void symtab_set_constant_value(const char *name, long int_val, double float_val) {
    Symbol *s = symtab_lookup(name);
    if (s) {
        s->is_constant = 1;
        s->value.int_val = int_val;
        s->value.float_val = float_val;
    }
}

void symtab_clear_constant_value(const char *name) {
    Symbol *s = symtab_lookup(name);
    if (s) {
        s->is_constant = 0;
    }
}

/* ====================== SYMBOL TABLE PRINTING ====================== */

void symtab_print(void) {
    printf("\n============ SYMBOL TABLE ============\n");
    printf("%-20s %-10s %-6s %-10s %-8s %-8s\n", 
           "NAME", "TYPE", "SCOPE", "INITIALIZED", "USED", "CONST");
    printf("======================================\n");
    
    for (int i = 0; i < sym_count; i++) {
        printf("%-20s %-10s %-6d %-10s %-8s %-8s\n",
               symtab[i].name,
               type_to_string(symtab[i].type),
               symtab[i].scope_level,
               symtab[i].initialized ? "yes" : "no",
               symtab[i].used ? "yes" : "no",
               symtab[i].is_constant ? "yes" : "no");
    }
    printf("=====================================\n");
    
    /* Print unused variable warnings */
    int unused_count = 0;
    for (int i = 0; i < sym_count; i++) {
        if (!symtab[i].used) {
            if (unused_count == 0) {
                printf("\nWARNING: Unused variables:\n");
            }
            printf("  - %s (declared at line %d)\n", symtab[i].name, symtab[i].declared_line);
            unused_count++;
        }
    }
}

/* ====================== ERROR MANAGEMENT ====================== */

int symtab_get_error_count(void) {
    return error_count;
}
