CC = gcc
CFLAGS = -Wall -Wextra -g -O2
SRCDIR = src
BINDIR = bin
TESTDIR = tests

# Targets
SOURCES = $(SRCDIR)/lexer.c $(SRCDIR)/parser.c $(SRCDIR)/ast.c \
          $(SRCDIR)/symbol_table.c $(SRCDIR)/optimizer.c $(SRCDIR)/ir.c \
		  $(SRCDIR)/compiler.c

HEADERS = $(SRCDIR)/types.h $(SRCDIR)/lexer.h $(SRCDIR)/parser.h \
          $(SRCDIR)/ast.h $(SRCDIR)/symbol_table.h $(SRCDIR)/optimizer.h \
		  $(SRCDIR)/ir.h

OBJECTS = $(SOURCES:.c=.o)
TARGET = $(BINDIR)/compiler

# Default target
all: $(BINDIR) $(TARGET)

# Create bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Compile
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilation successful!"

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BINDIR) $(OBJECTS)
	@echo "Clean complete"

# Test
test: $(TARGET)
	@if [ -f $(TESTDIR)/test1.c ]; then \
		$(TARGET) $(TESTDIR)/test1.c; \
	else \
		echo "No test files found"; \
	fi

# Help
help:
	@echo "Available targets:"
	@echo "  make       - Build the compiler"
	@echo "  make clean - Remove build artifacts"
	@echo "  make test  - Run compiler on test files"
	@echo "  make help  - Show this help message"

.PHONY: all clean test help
// Commit Marker