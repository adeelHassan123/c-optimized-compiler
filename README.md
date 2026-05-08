# Modular Optimizing Compiler (CS-346 Project)

A complete, high-transparency compiler pipeline implementing all eight modules of the CS-346 Compiler Construction course. This project integrates tool-based (Flex/Bison) and hand-written components into a unified, diagnostic-rich compiler.

## 🚀 Unified Pipeline (M1 - M8)

The core of this project is a **Unified Compiler Pipeline** that executes every phase of compilation sequentially with extreme detail.

### 🛠️ Execution Flow
1.  **Module 1: Lexical Analysis** - Token stream generation with type and line tracking.
2.  **Module 2: Syntax Analysis** - LALR(1) parsing with real-time rule reduction logging.
3.  **Module 3 & 4: Grammar Analysis** - Display of FIRST/FOLLOW sets and LL(1) constraints.
4.  **Module 5: Semantic Analysis** - Symbol table management with scope and type checking.
5.  **Module 7: Code Optimization** - Step-by-step reporting of Constant Folding and transformations.
6.  **Module 6: Intermediate Representation** - Three-Address Code (TAC) emission.
7.  **Module 8: LLVM IR Generation** - Production of target-independent LLVM assembly.

---

## 📦 Detailed Module Specifications

### ✅ Module 1: Lexical Analysis (Flex)
- **Location:** `module1_lexer/` and `src/lexer.l`
- **Features:** Supports Keywords (`int`, `float`, `while`, etc.), Identifiers, Integer/Float literals, Operators (`+`, `-`, `*`, `/`, `^`, `=`, `==`, etc.), and Comments.
- **Diagnostic:** Prints every token found with its category and line number.

### ✅ Module 2: Syntax Analysis (Bison)
- **Location:** `module2_parser/` and `src/parser.y`
- **Features:** Implements LALR(1) parsing. Support for Infix, Postfix, and Prefix notations in standalone modules. The unified compiler uses a comprehensive Infix grammar.
- **Diagnostic:** Logs every grammar rule reduction in real-time.

### ✅ Module 3: Extended Grammar
- **Location:** `module3_extended/` and integrated in `src/`
- **Features:** Adds support for exponentiation (`^`), and mathematical functions `log()` and `exp()`. Handles floating-point precision.

### ✅ Module 4: FIRST & FOLLOW Sets
- **Location:** `module4_first_follow/` and integrated in `src/compiler.c`
- **Features:** Computes and displays the FIRST and FOLLOW sets for the target grammar to ensure LL(1) compatibility and parser correctness.

### ✅ Module 5: Symbol Table & Semantic Analysis
- **Location:** `module5_symbol_table/` and `src/symbol_table.c`
- **Features:** Implements a scoped symbol table. Performs type checking, duplicate declaration detection, and tracks initialization/usage of variables.

### ✅ Module 6: Intermediate Representation (TAC)
- **Location:** `module6_ir/` and `src/ir.c`
- **Features:** Converts the Abstract Syntax Tree (AST) into Three-Address Code (TAC). Handles temporary variable allocation and instruction emission.

### ✅ Module 7: Code Optimization
- **Location:** `module7_optimization/` and `src/optimizer.c`
- **Features:** Performs multiple optimization passes including **Constant Folding**, Algebraic Simplification, and Dead Code Elimination.
- **Diagnostic:** Reports specific optimizations made (e.g., "5 + 3 -> 8").

### ✅ Module 8: LLVM IR Generation
- **Location:** `module8_llvm/` and `src/compiler.c`
- **Features:** Maps TAC/AST nodes to target-independent LLVM IR. Generates standard `.ll` files compatible with Clang/LLVM.

---

## ⚡ Quick Start (WSL/Linux)

### Build and Run the Full Pipeline
```bash
wsl make run
```

### Run All Module Tests
```bash
wsl make test-all
```

---

## 🛠️ Build Requirements
- **Compiler:** `gcc`
- **Tools:** `flex`, `bison`
- **Environment:** Linux or WSL (Ubuntu recommended)

---

**Note**: This project demonstrates a comprehensive understanding of compiler design principles, from lexical scanning to target code generation.
