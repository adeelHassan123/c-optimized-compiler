%{
#include "types.h"
#include "ast.h"
#include <stdio.h>

extern int yylex();
extern int line_num;
void yyerror(const char *s);

ASTNode *root;
%}

%union {
    long int_val;
    double float_val;
    char *name;
    struct ASTNode *node;
}

%token <int_val> INT_LIT
%token <float_val> FLOAT_LIT
%token <name> ID
%token INT FLOAT CHAR VOID
%token IF ELSE WHILE FOR RETURN PRINT
%token PLUS MINUS MUL DIV MOD EXP ASSIGN
%token EQ NEQ LT GT LTE GTE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA

%type <node> program stmt_list stmt expr decl assign print_stmt return_stmt

%left PLUS MINUS
%left MUL DIV MOD
%right EXP

%%

program:
    ID LPAREN RPAREN LBRACE stmt_list RBRACE { 
        printf("  [REDUCE] program -> ID(%s) () { stmt_list }\n", $1);
        root = ast_new_program(1); 
        root->data.program.nodes = $5->data.block.statements; 
        root->data.program.count = $5->data.block.count; 
    }
    ;

stmt_list:
    stmt { 
        printf("  [REDUCE] stmt_list -> stmt\n");
        $$ = ast_alloc_node(NODE_BLOCK, 1); 
        $$->data.block.statements = malloc(sizeof(ASTNode*)*100); 
        $$->data.block.count = 0; 
        ast_add_node($$, $1); 
    }
    | stmt_list stmt { 
        printf("  [REDUCE] stmt_list -> stmt_list stmt\n");
        ast_add_node($1, $2); 
        $$ = $1; 
    }
    ;

stmt:
    decl SEMICOLON { printf("  [REDUCE] stmt -> decl ;\n"); $$ = $1; }
    | assign SEMICOLON { printf("  [REDUCE] stmt -> assign ;\n"); $$ = $1; }
    | print_stmt SEMICOLON { printf("  [REDUCE] stmt -> print_stmt ;\n"); $$ = $1; }
    | return_stmt SEMICOLON { printf("  [REDUCE] stmt -> return_stmt ;\n"); $$ = $1; }
    ;

decl:
    INT ID { 
        printf("  [REDUCE] decl -> int ID(%s)\n", $2);
        $$ = ast_new_decl(TYPE_INT, $2, NULL, 1); 
    }
    | INT ID ASSIGN expr { 
        printf("  [REDUCE] decl -> int ID(%s) = expr\n", $2);
        $$ = ast_new_decl(TYPE_INT, $2, $4, 1); 
    }
    ;

assign:
    ID ASSIGN expr { 
        printf("  [REDUCE] assign -> ID(%s) = expr\n", $1);
        $$ = ast_new_assign($1, $3, 1); 
    }
    ;

print_stmt:
    PRINT LPAREN expr RPAREN { 
        printf("  [REDUCE] print_stmt -> print(expr)\n");
        $$ = ast_alloc_node(NODE_PRINT, @1.first_line); 
        $$->data.print.value = $3; 
    }
    ;

return_stmt:
    RETURN expr { 
        printf("  [REDUCE] return_stmt -> return expr\n");
        $$ = ast_alloc_node(NODE_RETURN, @1.first_line); 
        $$->data.ret.value = $2; 
    }
    ;

expr:
    INT_LIT { 
        printf("  [REDUCE] expr -> INT_LIT(%ld)\n", $1);
        $$ = ast_new_int($1, 1); 
    }
    | ID { 
        printf("  [REDUCE] expr -> ID(%s)\n", $1);
        $$ = ast_new_var($1, 1); 
    }
    | expr PLUS expr { 
        printf("  [REDUCE] expr -> expr + expr\n");
        $$ = ast_new_binop(TOK_PLUS, $1, $3, 1); 
    }
    | expr MINUS expr { 
        printf("  [REDUCE] expr -> expr - expr\n");
        $$ = ast_new_binop(TOK_MINUS, $1, $3, 1); 
    }
    | expr MUL expr { 
        printf("  [REDUCE] expr -> expr * expr\n");
        $$ = ast_new_binop(TOK_MUL, $1, $3, 1); 
    }
    | expr DIV expr { 
        printf("  [REDUCE] expr -> expr / expr\n");
        $$ = ast_new_binop(TOK_DIV, $1, $3, 1); 
    }
    | LPAREN expr RPAREN { 
        printf("  [REDUCE] expr -> ( expr )\n");
        $$ = $2; 
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "PARSER ERROR: %s at line %d\n", s, line_num);
}
