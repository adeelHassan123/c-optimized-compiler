# Lab 11: Modular Compiler - Build Guide

## Quick Start

### Prerequisites

You need a C compiler installed on your system

### Building

#### **Option 1: Windows Batch Script**

```batch
build.bat
```

#### **Option 2: Using Makefile (Linux/Mac)**

```bash
make
```

#### **Option 3: Manual Compilation**

```bash
# Create bin directory
mkdir bin

# Compile
gcc -Wall -Wextra -g -O2 -o bin/compiler src/*.c
```

### Running

```bash
# Test the compiler
./bin/compiler tests/test1.c
./bin/compiler tests/test2.c
./bin/compiler tests/test3.c
./bin/compiler tests/test4.c

# Or on Windows
bin\compiler.exe tests\test1.c
```

## Project Structure

```
Lab11/
├── src/                      # Source code
│   ├── types.h              # Type definitions
│   ├── lexer.h / lexer.c    # Tokenization
│   ├── parser.h / parser.c  # Parsing
│   ├── ast.h / ast.c        # AST management
│   ├── symbol_table.h / symbol_table.c  # Semantic analysis
│   ├── optimizer.h / optimizer.c        # Optimizations
│   ├── ir.h / ir.c          # IR generation
│   └── compiler.c           # Main driver
│
├── tests/                    # Test programs
│   ├── test1.c
│   ├── test2.c
│   ├── test3.c
│   └── test4.c
│
├── bin/                      # Compiled output (created during build)
│   └── compiler.exe
│
├── Makefile                  # Build instructions (Linux/Mac)
├── build.bat                 # Build script (Windows)
├── README.md                 # Full documentation
└── BUILD_GUIDE.md           # This file
```

## Compilation Output

After successful compilation, you'll get:

```
bin/compiler      (Linux/Mac)
bin/compiler.exe  (Windows)
```

## What's Included

### Modular Architecture

- ✓ **Lexer**: Tokenization with comment support
- ✓ **Parser**: Recursive descent with precedence climbing
- ✓ **AST**: Complete abstract syntax tree
- ✓ **Symbol Table**: Variable tracking with scopes
- ✓ **Optimizer**: safe AST optimizations (see below)
- ✓ **IR Generator**: **three-address code** IR generation

### Optimizations Implemented

- ✓ Constant Folding
- ✓ Constant Propagation
- ✓ Dead Code Elimination
- ✓ Unreachable Code Elimination
- ✓ Algebraic Simplification
- ✓ Strength Reduction
- ✓ Copy / Constant Tracking (enables propagation)

> Note: Some optimizations listed in the lab handout (e.g., common subexpression elimination and loop optimizations)
> are **not implemented as real transformations** in this project. The compiler focuses on correctness and IR output.

### Language Features

- ✓ Type declarations (int, float, char)
- ✓ Arithmetic operators (+, -, *, /, %)
- ✓ Comparison operators (==, !=, <, <=, >, >=)
- ✓ Logical operators (&&, ||, !)
- ✓ Control flow (if/else, while, for)
- ✓ I/O (print statement)
- ✓ Return statements
- ✓ Scoped variable declarations
- ✓ Comments (// and /* */)

## Compiler Output

When you run the compiler, it produces:

1. **Compilation Progress**
  - Shows each stage (Lexing, Parsing, Optimization, IR)
  - Reports timing for each stage
2. **Abstract Syntax Tree (AST)**
  - Visual representation of program structure
  - Shows optimization marks on optimized nodes
3. **Symbol Table**
  - All declared variables
  - Scope information
  - Initialization and usage status
  - Constants detected
  - Unused variable warnings
4. **Optimization Statistics**
  - Number of each optimization applied
  - Shows which optimizations benefited your code
5. **Intermediate Representation (IR)**
  - Three-address code
  - Instructions with operands and results
  - Labels and jumps for control flow
6. **Compilation Summary**
  - Token count
  - Error count (if any)
  - Timing breakdown (lex, parse, optimize, IR)

## Example Output

```
========== MODULAR OPTIMIZING COMPILER ==========
Source: tests/test1.c
===================================================

[1/5] Reading source file...
[2/5] Lexical Analysis...
  ✓ Lexical analysis complete: 45 tokens in 1 ms

[3/5] Parsing & Semantic Analysis...
  ✓ Parsing complete in 2 ms
  ✓ AST constructed successfully
  ✓ Semantic analysis passed

[4/5] Code Optimization...
  ✓ Optimization complete in 1 ms

[5/5] IR Generation...
  ✓ IR generation complete in 0 ms

========== COMPILATION OUTPUT ==========

ABSTRACT SYNTAX TREE:
====================
PROGRAM
  BLOCK
    DECL
      name: x
    ASSIGN
      var: x
      INT
        value: 8
    ...

============ SYMBOL TABLE ============
NAME                 TYPE       SCOPE  INITIALIZED  USED      CONST    
x                    int        0      yes          yes       yes      
y                    int        0      yes          yes       yes      

============ OPTIMIZATION STATISTICS ============
Constant Foldings:           2
Constant Propagations:       0
Dead Code Eliminations:      0
...

============ INTERMEDIATE REPRESENTATION ============
  0: ASSIGN t0 = 8
  1: ASSIGN x = t0
  2: ASSIGN t1 = 20
  3: ASSIGN y = t1
  4: PRINT x
  5: PRINT y
  6: RETURN 0
```

## Technical Details

### Compilation Pipeline

1. **Lexical Analysis** (lexer.c)
  - Converts source code to tokens
  - Handles all language elements
2. **Parsing** (parser.c)
  - Builds AST from tokens
  - Implements precedence climbing for expressions
  - Reports syntax errors
3. **Semantic Analysis** (symbol_table.c)
  - Validates variable usage
  - Tracks scope and initialization
  - Detects unused variables
4. **Optimization** (optimizer.c)
  - Multiple passes over AST
  - Applies transformations
  - Maintains semantics
5. **IR Generation** (ir.c)
  - Converts optimized AST to three-address code
  - Generates labels and jumps
  - Ready for code generation

## References

This compiler implements classical compiler theory:

- Recursive descent parsing
- AST-based compilation
- Multiple optimization passes
- Symbol table management
- Three-address code IR

---

// Commit Marker