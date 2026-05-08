# Module 8: LLVM IR Generation

## Overview

This module generates LLVM Intermediate Representation (IR) that can be compiled using Clang. It demonstrates LLVM IR generation as required by CS-346 Compiler Construction Lab Project.

## Files

- `llvm_ir.h` - Header file defining LLVM IR structures
- `llvm_ir.c` - Core LLVM IR generation functions
- `llvm_generator.c` - Main driver program
- `Makefile` - Build instructions

## LLVM IR Instructions Supported

| Instruction | LLVM IR Example | Description |
|-------------|-----------------|-------------|
| ALLOCA | `%x = alloca i32, align 4` | Stack allocation |
| LOAD | `%t = load i32, i32* %x` | Load from memory |
| STORE | `store i32 %val, i32* %x` | Store to memory |
| ADD | `%res = add nsw i32 %a, %b` | Integer addition |
| SUB | `%res = sub nsw i32 %a, %b` | Integer subtraction |
| MUL | `%res = mul nsw i32 %a, %b` | Integer multiplication |
| SDIV | `%res = sdiv i32 %a, %b` | Integer division |
| SREM | `%res = srem i32 %a, %b` | Integer remainder |
| ICMP | `%res = icmp eq i32 %a, %b` | Integer comparison |
| BR | `br label %5` | Unconditional branch |
| COND_BR | `br i1 %cond, label %5, label %6` | Conditional branch |
| LABEL | `5:` | Label definition |
| RET | `ret i32 %val` | Return statement |

## Build Commands

```bash
# Build the LLVM generator
make

# Run and generate sample IR
make run

# Generate IR and test with Clang (if installed)
make test
```

## Manual Clang Commands

### Generate LLVM IR (unoptimized)
```bash
clang -S -emit-llvm input.c -o output.ll
```

### Generate LLVM IR (optimized with -O3)
```bash
clang -S -emit-llvm -O3 input.c -o output_optimized.ll
```

### Assemble IR to bitcode
```bash
llvm-as output.ll -o output.bc
```

### Disassemble bitcode to text
```bash
llvm-dis output.bc -o output_dis.ll
```

### Compile IR to executable
```bash
clang output.ll -o output
```

## Example Output

Generated IR looks like:

```llvm
; ModuleID = 'output.ll'
source_filename = "output.ll"

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define dso_local i32 @main() #0 {
entry:
  %x = alloca i32, align 4
  store i32 10, i32* %x, align 4
  %t0 = load i32, i32* %x, align 4
  %t1 = add nsw i32 %t0, 20
  ret i32 %t1
}
```

## Key LLVM IR Concepts

- **alloca**: Allocates stack space for a variable
- **load**: Reads value from memory location
- **store**: Writes value to memory location
- **add/sub/mul/sdiv**: Arithmetic operations
- **icmp**: Integer comparison (eq, ne, slt, sle, sgt, sge)
- **br**: Branch instructions (conditional and unconditional)
- **label**: Code location for branching
- **ret**: Function return

## Integration with Main Compiler

To use this module with the main compiler:

1. After parsing and optimization, call `llvm_generate_from_ast()`
2. Output the generated IR using `llvm_print_to_file()`
3. The generated `.ll` files can be compiled with Clang

## Requirements for Project

- [x] LLVM IR generation
- [ ] Verify LLVM installation (screenshot)
- [ ] Generate unoptimized .ll for test programs
- [ ] Generate -O3 optimized .ll for test programs
- [ ] Annotate key IR instructions
- [ ] Compare optimized vs non-optimized

## References

- [LLVM IR Reference](https://llvm.org/docs/LangRef.html)
- [Clang User Manual](https://clang.llvm.org/docs/UsersManual.html)
