# Modular Optimizing Compiler

## Overview

This lab contains a modular educational compiler pipeline that reads a small C-like language, builds an AST, applies safe optimizations, and prints a **simple Intermediate Representation (IR / three-address code)**.  
For simplicity, the compiler output is **IR only** (it does not emit assembly).

## Architecture

### Module Structure

```
src/
├── types.h                    # Common type definitions
├── lexer.h / lexer.c          # Lexical Analysis (Tokenization)
├── parser.h / parser.c        # Syntax Analysis (Parsing)
├── ast.h / ast.c              # Abstract Syntax Tree Management
├── symbol_table.h / symbol_table.c   # Semantic Analysis & Symbols
├── optimizer.h / optimizer.c  # Code Optimization
├── ir.h / ir.c                # Intermediate Representation
└── compiler.c                 # Main Compiler Driver
```

### Compilation Pipeline

```
Source Code
    ↓
[LEXER] → Tokens
    ↓
[PARSER] → Abstract Syntax Tree (AST)
    ↓
[SYMBOL TABLE] → Validated AST
    ↓
[OPTIMIZER] → Optimized AST
    ↓
[IR GENERATOR] → Three-Address Code
    ↓
Intermediate Representation
```

## Modules

### 1. **Lexer** (`lexer.c/h`)
- Tokenizes source code
- Handles keywords, identifiers, literals, operators
- Supports single-line (`//`) and multi-line (`/* */`) comments
- Line number tracking for error reporting

**Key Functions:**
- `lexer_init()` - Initialize with source code
- `lexer_next_token()` - Get next token
- `token_type_name()` - Get token type name for debugging

### 2. **Parser** (`parser.c/h`)
- Builds Abstract Syntax Tree (AST)
- Implements recursive descent parsing
- Handles operator precedence with precedence climbing
- Performs basic semantic checks

**Key Functions:**
- `parser_init()` - Initialize with token stream
- `parser_parse_program()` - Parse entire program
- `parser_parse_expression()` - Parse expressions with correct precedence
- `parser_parse_statement()` - Parse individual statements

### 3. **AST** (`ast.c/h`)
- Represents program structure as tree
- Supports all node types: literals, variables, operators, statements
- Provides tree traversal and visualization

**Key Functions:**
- `ast_alloc_node()` - Allocate new AST node
- `ast_print()` - Visualize AST structure
- `ast_traverse()` - Traverse entire tree

### 4. **Symbol Table** (`symbol_table.c/h`)
- Manages variable declarations and scopes
- Tracks variable usage and initialization
- Supports constant values for optimization
- Produces warnings for unused variables

**Key Functions:**
- `symtab_add()` - Add symbol to current scope
- `symtab_lookup()` - Find symbol
- `symtab_set_initialized()` - Mark as initialized
- `symtab_print()` - Display symbol table

### 5. **Optimizer** (`optimizer.c/h`)
- Implements multiple optimization passes
- Tracks optimization statistics
- Modular design allows easy addition of new optimizations

**Implemented Optimizations (safe & actually applied):**

#### Code Optimization Techniques:
1. **Constant Folding** - Evaluate constant expressions at compile-time
   - `5 + 3` → `8`
   
2. **Constant Propagation** - Replace a variable use with its known constant value
   - `x = 10; y = x + 5;` → `y = 15` (when `x` is proven constant)
   
3. **Algebraic Simplification** - Simplify arithmetic expressions
   - `x + 0` → `x`
   - `x * 1` → `x`
   - `x * 0` → `0`
   
4. **Strength Reduction** - Replace expensive operations with cheaper ones
   - `x * 2` → `x + x`
   - `x / 2` → `x * 0.5`
   
5. **Dead Code Elimination** - Remove unused variable declarations
   
6. **Unreachable Code Elimination** - Remove code after return statements
   
7. **Copy / Constant Tracking** - Tracks assignments of constants for propagation

> Note: More advanced optimizations (e.g., **common subexpression elimination** and most **loop optimizations**) are listed in the lab handout, but are **not implemented as real transformations** in this codebase.

**Key Functions:**
- `optimizer_optimize()` - Run all optimization passes
- `optimizer_constant_folding()` - Fold constants
- `optimizer_algebraic_simplification()` - Simplify expressions
- `optimizer_print_stats()` - Display optimization results

### 6. **Intermediate Representation** (`ir.c/h`)
- Generates three-address code from AST
- Intermediate form between AST and final code
- Supports various IR operations: assignment, binary/unary ops, jumps, labels

**Key Functions:**
- `ir_generate()` - Generate IR from AST
- `ir_alloc_temp()` - Allocate temporary variable
- `ir_alloc_label()` - Allocate jump label
- `ir_print()` - Display IR code

### 7. **Main Compiler** (`compiler.c`)
- Orchestrates entire compilation process
- Handles file I/O
- Produces comprehensive output including:
  - AST visualization
  - Symbol table report
  - Optimization statistics
  - IR code (three-address code)
  - Compilation timing

## Building the Compiler

### Prerequisites
- GCC compiler
- Make utility
- Windows or Unix-like system

### Build Instructions

```bash
# Navigate to project directory
cd <project-directory>

# Build the compiler
make

# Clean build artifacts
make clean

# Run tests
make test
```

### Build Output
```
bin/compiler          # Compiled executable
```

## Using the Compiler

### Basic Usage
```bash
./bin/compiler tests/test1.c
```

### Output
The compiler produces:
1. **AST Structure** - Visual representation of program structure
2. **Symbol Table** - All declared variables with their properties
3. **Optimization Statistics** - Number of each optimization applied
4. **IR Code** - Three-address code representation
5. **Compilation Summary** - Timing and error statistics

## Language Syntax

### Supported Features

```c
/* Comments */
// Single-line comment
/* Multi-line comment */

/* Type declarations */
int variable;
float f_variable;
char c_variable;

/* Literals */
10              /* Integer */
3.14            /* Float */
'a'             /* Character */
"hello"         /* String */

/* Operators */
Arithmetic:     +, -, *, /, %
Comparison:     ==, !=, <, <=, >, >=
Logical:        &&, ||, !

/* Control Flow */
if (condition) { ... }
if (condition) { ... } else { ... }

while (condition) { ... }

for (init; condition; update) { ... }

/* I/O */
print(expression);

/* Return */
return expression;

/* Functions */
main() { ... }
```

### Example Programs

#### Test 1: Constant Folding
```c
main() {
    int x;
    x = 5 + 3;      // Optimized to x = 8
    print(x);
    return 0;
}
```

#### Test 2: Algebraic Simplification
```c
main() {
    int a;
    a = 100;
    int b = a + 0;  // Optimized to b = a
    int c = a * 1;  // Optimized to c = a
    print(b);
    print(c);
    return 0;
}
```

#### Test 3: Loop with Control Flow
```c
main() {
    int sum = 0;
    int i = 1;
    
    while (i <= 10) {
        sum = sum + i;
        i = i + 1;
    }
    
    if (sum > 50) {
        print(sum);
    }
    
    return 0;
}
```

## Optimization Example

### Input Code
```c
main() {
    int x;
    int y;
    
    x = 5 + 3;
    y = x + 0;
    z = y * 1;
    
    print(z);
    return 0;
}
```

### Optimizations Applied
```
Constant Folding:          5 + 3 → 8
Algebraic Simplification:  x + 0 → x
Algebraic Simplification:  y * 1 → y
Dead Code Elimination:     Remove unused variables
```

### Optimized IR
```
0: ASSIGN x = 8
1: PRINT x
2: RETURN 0
```

## Compilation Stages Output

### 1. Lexical Analysis
- **Input**: Source code (string)
- **Output**: Token stream
- **Tokens**: Keywords, identifiers, literals, operators

### 2. Parsing
- **Input**: Token stream
- **Output**: Abstract Syntax Tree (AST)
- **Error Handling**: Reports syntax errors with line numbers

### 3. Semantic Analysis
- **Input**: AST
- **Output**: Validated AST with symbol information
- **Checks**: Variable declarations, type consistency, scoping

### 4. Optimization
- **Input**: AST
- **Output**: Optimized AST
- **Passes**: Multiple optimization passes as described above

### 5. IR Generation
- **Input**: Optimized AST
- **Output**: Three-address code (IR)
- **Format**: Intermediate representation for further processing

## Error Handling

The compiler provides detailed error messages including:
- **Line numbers** for location information
- **Error types** (lexical, syntax, semantic)
- **Error descriptions** to help identify issues

## Code Quality Features

### Modularity
- Clean separation of concerns
- Independent modules with well-defined interfaces
- Easy to extend with new features

### Maintainability
- Clear function and variable names
- Comprehensive comments
- Consistent code style

### Efficiency
- Single-pass parsing
- Pool-based memory allocation
- Minimal memory overhead
- Compilation timing analysis

### Extensibility
- Framework for adding new optimizations
- Support for new language features
- Modular IR generation

## Testing

Test files are provided in `tests/` directory:
- `test1.c` - Constant folding
- `test2.c` - Algebraic simplification
- `test3.c` - Control flow and loops
- `test4.c` - Comprehensive optimization

Run tests:
```bash
make test
```

Or test individual files:
```bash
./bin/compiler tests/test1.c
./bin/compiler tests/test2.c
./bin/compiler tests/test3.c
./bin/compiler tests/test4.c
```

## Performance Considerations

### Optimization Impact
- Constant folding: Reduces computation at runtime
- Algebraic simplification: Simplifies expressions
- Strength reduction: Replaces expensive operations
- Dead code elimination: Reduces code size
- Unreachable code elimination: Reduces code size

### Compilation Speed
- Reported in compilation summary
- Breakdown by stage (lex, parse, optimize, IR)
- Minimal overhead from optimization

## Future Enhancements

Potential additions:
1. Function definitions and calls
2. Arrays and pointers
3. More complex data types (structs, unions)
4. Code generation to assembly or bytecode
5. Register allocation
6. More aggressive loop optimizations
7. Interprocedural optimizations
8. Link-time optimizations

## References

This compiler implements classical compiler techniques:
- Lexical analysis using hand-coded lexer
- Recursive descent parsing with precedence climbing
- AST-based compilation
- Symbol table management with scoping
- Multiple optimization passes
- Three-address code IR

## Author Notes

This modular compiler demonstrates:
1. **Clean Architecture** - Well-separated concerns
2. **Compiler Theory** - Standard compilation stages
3. **Optimization Techniques** - Practical optimizations
4. **Code Quality** - Maintainable, extensible code
5. **Software Engineering** - Proper module design

The modular structure makes it easy to:
- Understand each component in isolation
- Add new optimizations
- Extend language features
- Debug compilation issues
- Teach compiler concepts

---

**Modular Optimizing Compiler**
*A comprehensive example of well-structured compiler design*
