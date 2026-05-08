# Compiler Pipeline Makefile (WSL Optimized)
CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lm
SRCDIR = src
BINDIR = bin
TESTDIR = tests


# Main Compiler Components
SOURCES = $(SRCDIR)/lexer.yy.c $(SRCDIR)/parser.tab.c $(SRCDIR)/ast.c \
          $(SRCDIR)/symbol_table.c $(SRCDIR)/optimizer.c $(SRCDIR)/ir.c \
		  $(SRCDIR)/compiler.c

HEADERS = $(SRCDIR)/types.h $(SRCDIR)/ast.h $(SRCDIR)/symbol_table.h \
          $(SRCDIR)/optimizer.h $(SRCDIR)/ir.h $(SRCDIR)/parser.tab.h

TARGET = $(BINDIR)/compiler

# Default target
all: $(BINDIR) $(TARGET)

# Create bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Generate Lexer and Parser using WSL
$(SRCDIR)/lexer.yy.c: $(SRCDIR)/lexer.l $(SRCDIR)/parser.tab.h
	flex -o $(SRCDIR)/lexer.yy.c $(SRCDIR)/lexer.l

$(SRCDIR)/parser.tab.c $(SRCDIR)/parser.tab.h: $(SRCDIR)/parser.y
	bison -d -o $(SRCDIR)/parser.tab.c $(SRCDIR)/parser.y

# Compile the unified compiler
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@echo "==============================================="
	@echo "Unified Compiler Built Successfully!"
	@echo "==============================================="

# Main Pipeline Execution (Module 1 -> Module 8)
run: $(TARGET)
	@echo "Starting Compiler Pipeline for $(TESTDIR)/test1.c..."
	@echo ""
	$(TARGET) $(TESTDIR)/test1.c
	@echo ""
	@echo "Pipeline execution finished."

# Clean
clean:
	rm -rf $(BINDIR) $(SRCDIR)/lexer.yy.c $(SRCDIR)/parser.tab.c $(SRCDIR)/parser.tab.h
	@echo "Clean complete"

# Module Testing
test-all: run
	@echo "Running tests on all modules..."
	$(MAKE) -C module1_lexer test
	$(MAKE) -C module2_parser test
	$(MAKE) -C module4_first_follow run
	$(MAKE) -C module8_llvm test

.PHONY: all clean run test-all
