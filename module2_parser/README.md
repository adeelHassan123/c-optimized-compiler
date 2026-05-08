# Module 2: Syntax Analysis (Bison)

## Overview
This module implements Syntax Analyzers (Parsers) using Bison (Yacc). It verifies the grammatical structure of the token stream.

## Implementations
1. **Infix Parser:** Standard arithmetic notation (`4 + 8`).
2. **Postfix Parser:** Reverse Polish Notation (`4 8 +`).
3. **Prefix Parser:** Polish Notation (`+ 4 8`).

## Features
- **LALR(1) Parsing:** Efficient bottom-up parsing algorithm.
- **AST Generation:** Constructs an Abstract Syntax Tree (in the unified version).
- **Precedence & Associativity:** Handles operator priority (e.g., `*` before `+`).
- **Reduction Logging:** (Unified version) Logs every grammar rule applied during parsing.

## Build & Run
```bash
make
echo "4 8 +" | ./postfix
echo "+ 4 8" | ./prefix
echo "4 + 8" | ./infix
```
