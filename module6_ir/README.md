# Module 6: Intermediate Representation (TAC)

## Overview
This module translates the high-level Abstract Syntax Tree (AST) into a lower-level Intermediate Representation (IR) called Three-Address Code (TAC).

## Features
- **TAC Emission:** Generates instructions of the form `result = operand1 op operand2`.
- **Temporary Allocation:** Automatically manages `t0`, `t1`, `t2`... temporary variables for complex expressions.
- **Instruction Types:**
  - `IR_ASSIGN`: Assignment
  - `IR_BINOP`: Binary operations
  - `IR_PRINT`: Output statements
  - `IR_RETURN`: Function returns

## Role in Pipeline
The IR serves as a bridge between the front-end (Lexer/Parser) and the back-end (Code Generator/Optimizer).
