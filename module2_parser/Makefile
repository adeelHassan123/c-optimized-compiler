# Module 2: Bison Parser Makefile
# Builds Postfix, Prefix, and Infix calculators

CC = gcc

# Detect OS and set appropriate tools
ifeq ($(OS),Windows_NT)
    # Windows detected
    FLEX = win_flex
    BISON = win_bison
    RM = del /Q
    EXE = .exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        FLEX = flex
        BISON = bison
        RM = rm -f
        EXE =
    else
        # Default to Linux tools
        FLEX = flex
        BISON = bison
        RM = rm -f
        EXE =
    endif
endif

CFLAGS = -lm

TARGETS = postfix$(EXE) prefix$(EXE) infix$(EXE)

.PHONY: all clean test

all: $(TARGETS)

# Postfix calculator
postfix$(EXE): postfix.tab.c lex.yy.c
	$(CC) -o postfix$(EXE) postfix.tab.c lex.yy.c $(CFLAGS)

postfix.tab.c: postfix.y
	$(BISON) -d postfix.y

# Prefix calculator
prefix$(EXE): prefix.tab.c lex.yy.c
	$(CC) -o prefix$(EXE) prefix.tab.c lex.yy.c $(CFLAGS)

prefix.tab.c: prefix.y
	$(BISON) -d prefix.y

# Infix calculator
infix$(EXE): infix.tab.c lex.yy.c
	$(CC) -o infix$(EXE) infix.tab.c lex.yy.c $(CFLAGS)

infix.tab.c: infix.y
	$(BISON) -d infix.y

# Lexer
lex.yy.c: calc_lex.l
	$(FLEX) calc_lex.l

clean:
	$(RM) $(TARGETS) *.tab.c *.tab.h lex.yy.c

test: all
	@echo "Testing Postfix Calculator:"
	@echo "4 8 + 3 *" | .\postfix$(EXE)
	@echo ""
	@echo "Testing Prefix Calculator:"
	@echo "* + 4 8 3" | .\prefix$(EXE)
	@echo ""
	@echo "Testing Infix Calculator:"
	@echo "(4 + 8) * 3" | .\infix$(EXE)
