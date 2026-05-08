# Module 7: Code Optimization

## Overview
This module implements various optimization techniques to improve the efficiency of the generated code.

## Techniques Implemented
- **Constant Folding:** Evaluates constant expressions at compile-time (e.g., `5 + 3` becomes `8`).
- **Algebraic Simplification:** Simplifies expressions like `x * 1` or `x + 0`.
- **Copy Propagation:** Replaces uses of a variable with its assigned value if it's a constant.
- **Dead Code Elimination:** Removes code that has no effect on the program's output.

## Diagnostic Reporting
The optimizer logs specific transformations:
`[OPTIMIZE] Constant Folding: 5 + 3 -> 8 (Line: 6)`

## Usage
Run as a pass over the AST before IR generation to ensure the final code is as efficient as possible.
