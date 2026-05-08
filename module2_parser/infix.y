/* Module 2: Infix Expression Parser
 * CS-346 Compiler Construction - Lab Project
 * 
 * Infix Notation Calculator (standard arithmetic)
 * Example: 4 + 8
 * Requires precedence and associativity rules
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Enable debug mode */
#define YYDEBUG 1

int yylex(void);
void yyerror(const char *s);
%}

%union {
    double dval;
}

%token <dval> NUMBER
%token PLUS MINUS MUL DIV EXP
%token LPAREN RPAREN
%token LOG EXP_FUNC
%token NEWLINE

%type <dval> expr

/* Operator precedence and associativity */
/* Lower precedence = evaluated first (bottom-up) */

%left PLUS MINUS        /* Left associative, lowest precedence */
%left MUL DIV           /* Left associative, higher precedence */
%right EXP              /* Right associative (exponentiation) */
%left LOG EXP_FUNC      /* Function calls */

%start input

/* Grammar rules for infix - standard arithmetic notation */
%%

input:
    /* empty */
    | input line
    ;

line:
    NEWLINE
    | expr NEWLINE { 
        printf("\n========================================\n");
        printf("Result: %.2f\n", $1); 
        printf("========================================\n\n");
    }
    ;

expr:
    NUMBER          { $$ = $1; }
    | expr PLUS expr { 
        $$ = $1 + $3; 
        printf("  [Op] %.2f + %.2f = %.2f\n", $1, $3, $$);
    }
    | expr MINUS expr { 
        $$ = $1 - $3; 
        printf("  [Op] %.2f - %.2f = %.2f\n", $1, $3, $$);
    }
    | expr MUL expr { 
        $$ = $1 * $3; 
        printf("  [Op] %.2f * %.2f = %.2f\n", $1, $3, $$);
    }
    | expr DIV expr { 
        if ($3 != 0) {
            $$ = $1 / $3; 
            printf("  [Op] %.2f / %.2f = %.2f\n", $1, $3, $$);
        } else {
            fprintf(stderr, "ERROR: Division by zero\n");
            $$ = 0;
        }
    }
    | expr EXP expr { 
        $$ = pow($1, $3); 
        printf("  [Op] %.2f ^ %.2f = %.2f\n", $1, $3, $$);
    }
    | LOG LPAREN expr RPAREN {
        $$ = log($3);
        printf("  [Op] log(%.2f) = %.2f\n", $3, $$);
    }
    | EXP_FUNC LPAREN expr RPAREN {
        $$ = exp($3);
        printf("  [Op] exp(%.2f) = %.2f\n", $3, $$);
    }
    | MINUS expr %prec EXP {  /* Unary minus */
        $$ = -$2;
        printf("  [Op] -%.2f = %.2f\n", $2, $$);
    }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}

int main(int argc, char **argv) {
    printf("=================================================\n");
    printf("Module 2: Infix Expression Parser\n");
    printf("CS-346 Compiler Construction - Lab Project\n");
    printf("=================================================\n");
    printf("\nInfix notation: standard arithmetic notation\n");
    printf("Example: '4 + 8'\n");
    printf("Precedence: ^ > *,/ > +,-\n");
    printf("Supported: +, -, *, /, ^ (exponent), log(), exp()\n\n");
    
    /* Enable debug output if -d flag is given */
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        yydebug = 1;
        printf("Debug mode enabled\n\n");
    }
    
    printf("Enter expressions (one per line), Ctrl+D to exit:\n\n");
    
    return yyparse();
}
