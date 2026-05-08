#!/bin/bash
# Module 8: LLVM IR Generation Script
# Generates .ll files for all test programs

echo "=================================="
echo "Module 8: LLVM IR Generator"
echo "=================================="
echo ""

# Check if clang is installed
if ! command -v clang &> /dev/null; then
    echo "ERROR: clang not found. Please install clang."
    echo "  Ubuntu/Debian: sudo apt-get install clang"
    echo "  Fedora: sudo dnf install clang"
    exit 1
fi

# Create output directory
mkdir -p llvm_output

echo "Clang version:"
clang --version
echo ""

# Generate LLVM IR for each test program
TEST_DIR="../tests"
OUTPUT_DIR="llvm_output"

echo "Generating unoptimized LLVM IR (.ll files)..."
for test_file in $TEST_DIR/test*.c; do
    if [ -f "$test_file" ]; then
        basename=$(basename "$test_file" .c)
        echo "  Processing: $basename.c"
        
        # Generate unoptimized IR
        clang -S -emit-llvm "$test_file" -o "$OUTPUT_DIR/${basename}_unopt.ll" 2>/dev/null || echo "    Skipped (may have unsupported constructs)"
        
        # Generate optimized IR with -O3
        clang -S -emit-llvm -O3 "$test_file" -o "$OUTPUT_DIR/${basename}_opt.ll" 2>/dev/null || echo "    Skipped optimized version"
    fi
done

echo ""
echo "=================================="
echo "LLVM IR Generation Complete!"
echo "=================================="
echo ""
echo "Output files in: $OUTPUT_DIR/"
ls -la $OUTPUT_DIR/
