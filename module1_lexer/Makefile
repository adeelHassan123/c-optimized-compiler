# CS-346 Compiler Construction: Integrated Multi-Module Makefile
# Supports both Linux and Windows (with win_flex/gcc)

CC = gcc
CFLAGS = -Wall

# Detect OS and set appropriate tools
ifeq ($(OS),Windows_NT)
    # Windows detected
    FLEX = win_flex
    BISON = win_bison
    RM = del /Q
    EXE = .exe
    # Use && for sequential execution in Windows CMD
    SEP = &&
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        FLEX = flex
        BISON = bison
        RM = rm -f
        EXE =
        SEP = ;
    else
        # Default to Linux tools
        FLEX = flex
        BISON = bison
        RM = rm -f
        EXE =
        SEP = ;
    endif
endif

# Target Executables for each module
M1 = module1_lexer$(EXE)
M2 = module2_parser$(EXE)
M3 = module3_semantic$(EXE)
M4 = module4_symbol_table$(EXE)
M5 = module5_ir_gen$(EXE)
M6 = module6_optimizer$(EXE)
M7 = module7_codegen$(EXE)
M8 = module8_executor$(EXE)

ALL_TARGETS = $(M1) $(M2) $(M3) $(M4) $(M5) $(M6) $(M7) $(M8)

.PHONY: all clean test pipeline

# Build all modules
all: $(ALL_TARGETS)

# Module 1: Lexer (Flex generated)
$(M1): lex.yy.c
ifeq ($(UNAME_S),Linux)
	$(CC) $(CFLAGS) -o $(M1) lex.yy.c -lfl
else
	$(CC) $(CFLAGS) -o $(M1) lex.yy.c
endif

lex.yy.c: lexer.l
	$(FLEX) lexer.l

# Modules 2-8: C source based
$(M2): module2_parser.c
	$(CC) $(CFLAGS) -o $(M2) module2_parser.c

$(M3): module3_semantic.c
	$(CC) $(CFLAGS) -o $(M3) module3_semantic.c

$(M4): module4_symbol_table.c
	$(CC) $(CFLAGS) -o $(M4) module4_symbol_table.c

$(M5): module5_ir_gen.c
	$(CC) $(CFLAGS) -o $(M5) module5_ir_gen.c

$(M6): module6_optimizer.c
	$(CC) $(CFLAGS) -o $(M6) module6_optimizer.c

$(M7): module7_codegen.c
	$(CC) $(CFLAGS) -o $(M7) module7_codegen.c

$(M8): module8_executor.c
	$(CC) $(CFLAGS) -o $(M8) module8_executor.c

clean:
	$(RM) $(ALL_TARGETS) lex.yy.c

# Run Module 1 with sample input
test: $(M1)
	./$(M1) sample_input.txt

# Run the complete compiler pipeline (Module 1 to 8)
pipeline: all
	@echo Starting Full Compiler Pipeline...
	@echo ----------------------------------------------------
	./$(M1) sample_input.txt
	./$(M2)
	./$(M3)
	./$(M4)
	./$(M5)
	./$(M6)
	./$(M7)
	./$(M8)
	@echo ----------------------------------------------------
	@echo Pipeline Completed Successfully!
