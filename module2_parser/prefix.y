/* Module 2: Prefix Expression Parser
 * CS-346 Compiler Construction - Lab Project
 * 
 * Prefix (Polish) Notation Calculator
 * Example: + 4 8  (equivalent to 4 + 8)
 * Operators come before operands
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

/* Grammar rules for prefix - operators come before operands */
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
    | PLUS expr expr { 
        $$ = $2 + $3; 
        printf("  [Op] %.2f + %.2f = %.2f\n", $2, $3, $$);
    }
    | MINUS expr expr { 
        $$ = $2 - $3; 
        printf("  [Op] %.2f - %.2f = %.2f\n", $2, $3, $$);
    }
    | MUL expr expr { 
        $$ = $2 * $3; 
        printf("  [Op] %.2f * %.2f = %.2f\n", $2, $3, $$);
    }
    | DIV expr expr { 
        if ($3 != 0) {
            $$ = $2 / $3; 
            printf("  [Op] %.2f / %.2f = %.2f\n", $2, $3, $$);
        } else {
            fprintf(stderr, "ERROR: Division by zero\n");
            $$ = 0;
        }
    }
    | EXP expr expr { 
        $$ = pow($2, $3); 
        printf("  [Op] %.2f ^ %.2f = %.2f\n", $2, $3, $$);
    }
    | LOG expr      {
        $$ = log($2);
        printf("  [Op] log(%.2f) = %.2f\n", $2, $$);
    }
    | EXP_FUNC expr {
        $$ = exp($2);
        printf("  [Op] exp(%.2f) = %.2f\n", $2, $$);
    }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}

int main(void) {
    printf("=================================================\n");
    printf("Module 2: Prefix Expression Parser\n");
    printf("CS-346 Compiler Construction - Lab Project\n");
    printf("=================================================\n");
    printf("\nPrefix notation: operators come before operands\n");
    printf("Example: '+ 4 8' means '4 + 8'\n");
    printf("Example: '* + 4 8 2' means '(4 + 8) * 2'\n");
    printf("Supported: +, -, *, /, ^ (exponent), log, exp\n");
    printf("Enter expressions (one per line), Ctrl+D to exit:\n\n");
    
    return yyparse();
}
