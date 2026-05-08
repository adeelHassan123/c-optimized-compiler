# Module 3: Extended Grammar

## Overview
This module extends the basic expression parser to support more advanced mathematical operations and data types.

## Features
- **Exponentiation Operator (`^`):** Implements right-associative exponentiation.
- **Mathematical Functions:** Support for `log()` and `exp()` functions.
- **Floating-Point Arithmetic:** Full support for `double` precision arithmetic throughout the pipeline.
- **Precedence Handling:** Integrates new operators into the precedence hierarchy (e.g., `^` has higher precedence than `*`).

## Implementation
These features are integrated into the primary lexer and parser in the `src/` directory and demonstrated in the `module2_parser` standalone tools.
