# Module 8: Annotated LLVM IR Comparison

## Overview

This document explains the key LLVM IR instructions and compares unoptimized vs optimized (`-O3`) output.

## Key LLVM IR Instructions Explained

### 1. `alloca` - Stack Allocation
```llvm
%a = alloca i32, align 4
```
- **Purpose**: Allocates space on the stack for a local variable
- **Parameters**: `i32` = 32-bit integer type, `align 4` = 4-byte alignment
- **Optimized**: Usually eliminated by "mem2reg" pass (stored in registers instead)

### 2. `load` - Memory Read
```llvm
%0 = load i32, i32* %a, align 4
```
- **Purpose**: Reads a value from memory into a register
- **Parameters**: `i32` = type to load, `i32* %a` = pointer to memory location
- **Optimized**: Eliminated when variables are kept in registers

### 3. `store` - Memory Write
```llvm
store i32 10, i32* %a, align 4
```
- **Purpose**: Writes a value to memory
- **Parameters**: `i32 10` = value to store, `i32* %a` = destination pointer
- **Optimized**: Dead stores (unused writes) are eliminated

### 4. `add` - Addition
```llvm
%add = add nsw i32 %0, %1
```
- **Purpose**: Adds two integer values
- **Parameters**: `nsw` = no signed wrap (undefined behavior on overflow), `i32` = 32-bit integers
- **Optimized**: Constant folding: `add i32 10, 20` → `i32 30`

### 5. `call` - Function Call
```llvm
%call = call i32 @printf(...)
```
- **Purpose**: Calls a function
- **Parameters**: Return type, function name, arguments
- **Optimized**: May be inlined for small functions

### 6. `ret` - Return
```llvm
ret i32 %3
```
- **Purpose**: Returns from a function with a value
- **Parameters**: Return type and value
- **Optimized**: Direct return of constants if result is known

### 7. Branch Instructions
```llvm
br label %entry          ; Unconditional branch
br i1 %cond, label %t, label %f  ; Conditional branch
```
- **Purpose**: Control flow (jumps)
- **Optimized**: Dead branches eliminated

## Comparison: Unoptimized vs -O3 Optimized

### Example Program (test1.c)
```c
main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    print(c);
    return c;
}
```

### Unoptimized IR (`-O0`)
```llvm
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4      ; Stack allocation for return
  %a = alloca i32, align 4           ; Stack allocation
  %b = alloca i32, align 4           ; Stack allocation
  %c = alloca i32, align 4           ; Stack allocation
  store i32 0, i32* %retval, align 4
  store i32 10, i32* %a, align 4     ; a = 10
  store i32 20, i32* %b, align 4     ; b = 20
  %0 = load i32, i32* %a, align 4    ; Load a
  %1 = load i32, i32* %b, align 4    ; Load b
  %add = add nsw i32 %0, %1          ; a + b (at runtime)
  store i32 %add, i32* %c, align 4   ; c = result
  %2 = load i32, i32* %c, align 4    ; Load for print
  %call = call i32 @printf(...)
  %3 = load i32, i32* %c, align 4    ; Load for return
  ret i32 %3
}
```
**Characteristics**:
- All variables stored on stack
- Explicit load/store operations
- Addition happens at runtime
- Multiple memory accesses

### Optimized IR (`-O3`)
```llvm
define dso_local i32 @main() local_unnamed_addr #0 {
entry:
  %call = tail call i32 @printf(..., i32 noundef 30)
  ret i32 30
}
```
**Characteristics**:
- All stack allocations eliminated (registers used)
- `10 + 20` computed at compile time (constant folding)
- Direct `ret i32 30` (no variable lookup)
- `tail call` optimization for printf

## Optimizations Applied by `-O3`

1. **Mem2Reg**: Converts stack allocations to registers
   - Eliminates `alloca`, `load`, `store` for local variables
   
2. **Constant Folding**: 
   - `10 + 20` → `30` at compile time
   
3. **Dead Code Elimination**:
   - Unused stores removed
   - Unreachable code eliminated
   
4. **Tail Call Optimization**:
   - `call` → `tail call` when safe
   
5. **Attribute Refinement**:
   - `optnone` removed, `nounwind` added
   - `frame-pointer="none"` (no frame pointer needed)

## Test File: test5_loop_opt.c

### Source
```c
main() {
    int i;
    int sum;
    sum = 0;
    for (i = 0; i < 100; i = i + 1) {
        sum = sum + i;
    }
    print(sum);
    return 0;
}
```

### Optimized IR Note
With `-O3`, LLVM recognizes this as an arithmetic series:
- Unoptimized: 100 iterations at runtime
- Optimized: LLVM computes `sum = 99*100/2 = 4950` at compile time!
- Or uses vectorization/SIMD if profitable

## How to Generate

```bash
# Unoptimized IR
clang -S -emit-llvm test1.c -o test1_unopt.ll

# Optimized IR (-O3)
clang -S -emit-llvm -O3 test1.c -o test1_opt.ll

# Assemble to bitcode
llvm-as test1_unopt.ll -o test1_unopt.bc

# Disassemble back to text
llvm-dis test1_unopt.bc -o test1_unopt_dis.ll

# Compile IR to executable
clang test1_unopt.ll -o test1 -lm
```

## References

- [LLVM Language Reference Manual](https://llvm.org/docs/LangRef.html)
- [LLVM Optimization Levels](https://llvm.org/docs/HowToUseAttributes.html)
- Clang User Manual: `man clang` or `clang --help`
