# Module 4: FIRST and FOLLOW Sets

## Overview
This module focuses on grammar analysis, specifically computing the FIRST and FOLLOW sets required for LL(1) parsing.

## Features
- **Set Computation:** Automatically calculates FIRST and FOLLOW sets for any given grammar.
- **LL(1) Table:** Generates the predictive parsing table.
- **Nullable Tracking:** Identifies non-terminals that can derive the empty string (ε).

## Target Grammar
The module analyzes a standard expression grammar:
```
E   -> T E'
E'  -> + T E' | - T E' | ε
T   -> F T'
T'  -> * F T' | / F T' | ε
F   -> ( E ) | id | num
```

## Build & Run
```bash
make run
```
