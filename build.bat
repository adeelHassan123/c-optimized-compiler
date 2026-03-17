@echo off
REM Lab 11 Modular Compiler - Windows Build Script

echo ========================================
echo Lab 11: Modular Optimizing Compiler
echo ========================================
echo.

REM Create bin directory if it doesn't exist
if not exist "bin" (
    mkdir bin
    echo [*] Created bin directory
)

echo [*] Compiling with GCC...
echo.

REM Try to compile with gcc
gcc -Wall -Wextra -g -O2 -o bin\compiler.exe src\*.c

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo [SUCCESS] Compilation complete!
    echo ========================================
    echo.
    echo Usage:
    echo   bin\compiler.exe tests\test1.c
    echo   bin\compiler.exe tests\test2.c
    echo   bin\compiler.exe tests\test3.c
    echo   bin\compiler.exe tests\test4.c
    echo.
) else (
    echo.
    echo ========================================
    echo [ERROR] Compilation failed!
    echo ========================================
    echo.
    echo Make sure you have GCC installed and in your PATH.
    echo You can install it from:
    echo   - MinGW: https://www.mingw-w64.org/
    echo   - Cygwin: https://www.cygwin.com/
    echo   - MSYS2: https://www.msys2.org/
    echo.
)

pause
// Commit Marker