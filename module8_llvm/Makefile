# Module 8: LLVM IR Generator Makefile

CC = gcc
CFLAGS = -Wall -g

TARGET = llvm_generator
IR_CONVERTER = ir_to_llvm

.PHONY: all clean run test converter

all: $(TARGET) $(IR_CONVERTER)

$(TARGET): llvm_ir.c llvm_generator.c
	$(CC) $(CFLAGS) -o $(TARGET) llvm_ir.c llvm_generator.c

$(IR_CONVERTER): ir_to_llvm.c
	$(CC) $(CFLAGS) -o $(IR_CONVERTER) ir_to_llvm.c

converter: $(IR_CONVERTER)

run: $(TARGET)
	./$(TARGET)

# Generate .ll file and compile with clang
test: $(TARGET)
	./$(TARGET)
	@echo ""
	@echo "Testing with Clang..."
	@if command -v clang >/dev/null 2>&1; then \
		echo "Generating unoptimized IR..."; \
		clang -S -emit-llvm output.ll -o test_unopt.ll 2>/dev/null || echo "Note: clang not available or error occurred"; \
		echo "Generating optimized IR (-O3)..."; \
		clang -S -emit-llvm -O3 output.ll -o test_opt.ll 2>/dev/null || echo "Note: clang not available or error occurred"; \
	else \
		echo "Clang not found. Install clang to test IR generation."; \
		echo "  Linux: sudo apt-get install clang"; \
		echo "  Windows: Download from https://releases.llvm.org/"; \
	fi

# Generate LLVM IR for all test programs
generate-all: $(IR_CONVERTER)
	@echo "Generating LLVM IR for test programs..."
	@mkdir -p llvm_output
	@for f in ../tests/test*.c; do \
		basename=$$(basename $$f .c); \
		echo "Processing $$basename..."; \
		done

clean:
	rm -f $(TARGET) $(IR_CONVERTER) output.ll test_unopt.ll test_opt.ll
