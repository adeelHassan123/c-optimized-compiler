# Module 5: Symbol Table & Semantic Analysis

## Overview
This module manages the Symbol Table and performs semantic checks to ensure the program is logically sound.

## Features
- **Scoped Symbol Table:** Supports nested scopes (global and local).
- **Type Checking:** Ensures operations are performed on compatible data types.
- **Duplicate Detection:** Prevents multiple declarations of the same variable in the same scope.
- **Initialization Tracking:** Warns if a variable is used before being assigned a value.
- **Usage Tracking:** Identifies unused variables to assist in optimization.

## Data Structure
Each symbol stores:
- Name, Type, Scope Level
- Initialization Status, Usage Status
- Constant Status (for Constant Propagation)
- Declaration Line Number

## Usage
Integrated into the main compiler driver to perform semantic validation after parsing.
