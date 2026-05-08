/* Module 2: Postfix Expression Parser
 * CS-346 Compiler Construction - Lab Project
 * 
 * Postfix (Reverse Polish) Notation Calculator
 * Example: 4 8 +  (equivalent to 4 + 8)
 * Uses stack-based evaluation
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Stack for postfix evaluation */
#define STACK_SIZE 100

double stack[STACK_SIZE];
int stack_top = -1;

void push(double val) {
    if (stack_top < STACK_SIZE - 1) {
        stack[++stack_top] = val;
        printf("  [Stack] Push: %.2f | Stack: ", val);
        for (int i = 0; i <= stack_top; i++) {
            printf("%.2f ", stack[i]);
        }
        printf("\n");
    } else {
        fprintf(stderr, "ERROR: Stack overflow\n");
    }
}

double pop() {
    if (stack_top >= 0) {
        double val = stack[stack_top--];
        printf("  [Stack] Pop: %.2f | Stack: ", val);
        for (int i = 0; i <= stack_top; i++) {
            printf("%.2f ", stack[i]);
        }
        if (stack_top < 0) printf("(empty)");
        printf("\n");
        return val;
    } else {
        fprintf(stderr, "ERROR: Stack underflow\n");
        return 0.0;
    }
}

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

/* Grammar rules for postfix */
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
        stack_top = -1;  /* Reset stack for next expression */
    }
    ;

expr:
    NUMBER          { $$ = $1; push($1); }
    | expr expr PLUS    { 
        double b = pop(); 
        double a = pop(); 
        $$ = a + b; 
        push($$);
        printf("  [Op] %.2f + %.2f = %.2f\n", a, b, $$);
    }
    | expr expr MINUS   { 
        double b = pop(); 
        double a = pop(); 
        $$ = a - b; 
        push($$);
        printf("  [Op] %.2f - %.2f = %.2f\n", a, b, $$);
    }
    | expr expr MUL     { 
        double b = pop(); 
        double a = pop(); 
        $$ = a * b; 
        push($$);
        printf("  [Op] %.2f * %.2f = %.2f\n", a, b, $$);
    }
    | expr expr DIV     { 
        double b = pop(); 
        double a = pop(); 
        if (b != 0) {
            $$ = a / b; 
            push($$);
            printf("  [Op] %.2f / %.2f = %.2f\n", a, b, $$);
        } else {
            fprintf(stderr, "ERROR: Division by zero\n");
            $$ = 0;
            push($$);
        }
    }
    | expr expr EXP     { 
        double b = pop(); 
        double a = pop(); 
        $$ = pow(a, b); 
        push($$);
        printf("  [Op] %.2f ^ %.2f = %.2f\n", a, b, $$);
    }
    | expr LOG          {
        double a = pop();
        $$ = log(a);
        push($$);
        printf("  [Op] log(%.2f) = %.2f\n", a, $$);
    }
    | expr EXP_FUNC     {
        double a = pop();
        $$ = exp(a);
        push($$);
        printf("  [Op] exp(%.2f) = %.2f\n", a, $$);
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}

int main(void) {
    printf("=================================================\n");
    printf("Module 2: Postfix Expression Parser\n");
    printf("CS-346 Compiler Construction - Lab Project\n");
    printf("=================================================\n");
    printf("\nPostfix notation: operands come before operators\n");
    printf("Example: '4 8 +' means '4 + 8'\n");
    printf("Supported: +, -, *, /, ^ (exponent), log, exp\n");
    printf("Enter expressions (one per line), Ctrl+D to exit:\n\n");
    
    return yyparse();
}
