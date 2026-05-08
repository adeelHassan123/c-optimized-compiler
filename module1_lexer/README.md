# Module 1: Lexical Analysis (Flex)

## Overview
This module implements a Lexical Analyzer (Scanner) using Flex. It breaks down the source code into a stream of meaningful tokens.

## Features
- **Keywords:** `int`, `float`, `char`, `void`, `if`, `else`, `while`, `for`, `return`, `print`, `log`, `exp`.
- **Identifiers:** `[a-zA-Z_][a-zA-Z0-9_]*`
- **Literals:** Supports both Integer and Floating-point constants.
- **Operators:** `+`, `-`, `*`, `/`, `%`, `^`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`.
- **Delimiters:** `(`, `)`, `{`, `}`, `;`, `,`.
- **Comments:** Supports single-line (`//`) and multi-line (`/* ... */`) comments.

## Diagnostic Output
The scanner prints a detailed report for every token:
```
[TOKEN] KEYWORD_INT     | Lexeme: 'int'        | Line: 3
[TOKEN] IDENTIFIER      | Lexeme: 'x'          | Line: 3
```

## Build & Run
```bash
make
./lexer sample_input.txt
```
