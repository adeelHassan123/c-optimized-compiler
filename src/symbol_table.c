#include "symbol_table.h"

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

void symtab_init(void) {
    sym_count = 0;
    current_scope = 0;
    error_count = 0;
    scope_count = 1;
    scopes[0].start = 0;
    scopes[0].end = 0;
}

void symtab_enter_scope(void) {
    if (scope_count >= 100) return;
    current_scope = scope_count;
    scopes[current_scope].start = sym_count;
    scopes[current_scope].end = sym_count;
    scope_count++;
}

void symtab_exit_scope(void) {
    if (current_scope > 0) current_scope--;
}

static int find_symbol_index(const char *name) {
    for (int i = sym_count - 1; i >= 0; i--) {
        if (strcmp(symtab[i].name, name) == 0) return i;
    }
    return -1;
}

static int find_symbol_in_scope(const char *name, int scope_level) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(symtab[i].name, name) == 0 && symtab[i].scope_level == scope_level) return i;
    }
    return -1;
}

Symbol *symtab_lookup(const char *name) {
    int idx = find_symbol_index(name);
    return (idx >= 0) ? &symtab[idx] : NULL;
}

int symtab_exists_in_scope(const char *name) {
    return find_symbol_in_scope(name, current_scope) >= 0;
}

int symtab_add(const char *name, DataType type, int line) {
    if (sym_count >= MAX_SYMBOLS) { 
        printf("  [SEMANTIC ERROR] Symbol table full at line %d\n", line);
        error_count++; 
        return -1; 
    }
    if (symtab_exists_in_scope(name)) { 
        printf("  [SEMANTIC ERROR] Duplicate symbol '%s' at line %d\n", name, line);
        error_count++; 
        return -1; 
    }
    
    printf("  [SEMANTIC] Adding symbol '%s' (Type: %s, Scope: %d) at line %d\n", 
           name, type_to_string(type), current_scope, line);
    
    Symbol *s = &symtab[sym_count];
    strncpy(s->name, name, IDENTIFIER_MAX - 1);
    s->type = type;
    s->scope_level = current_scope;
    s->initialized = 0;
    s->used = 0;
    s->is_constant = 0;
    s->declared_line = line;
    return sym_count++;
}

void symtab_set_initialized(const char *name) {
    Symbol *s = symtab_lookup(name);
    if (s) s->initialized = 1;
}

void symtab_set_used(const char *name) {
    Symbol *s = symtab_lookup(name);
    if (s) s->used = 1;
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
    if (s) s->is_constant = 0;
}

void symtab_print(void) {
    printf("\n============ SYMBOL TABLE ============\n");
    printf("%-20s %-10s %-6s %-10s %-8s %-8s\n", "NAME", "TYPE", "SCOPE", "INIT", "USED", "CONST");
    for (int i = 0; i < sym_count; i++) {
        printf("%-20s %-10s %-6d %-10s %-8s %-8s\n",
               symtab[i].name, type_to_string(symtab[i].type), symtab[i].scope_level,
               symtab[i].initialized ? "yes" : "no", symtab[i].used ? "yes" : "no",
               symtab[i].is_constant ? "yes" : "no");
    }
}

int symtab_get_error_count(void) { return error_count; }
