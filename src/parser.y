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

%type <node> program stmt_list stmt expr decl assign print_stmt return_stmt if_stmt while_stmt for_stmt block

%left EQ NEQ
%left LT GT LTE GTE
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
    | stmt_list {
        printf("  [REDUCE] program -> stmt_list\n");
        root = ast_new_program(1);
        root->data.program.nodes = $1->data.block.statements;
        root->data.program.count = $1->data.block.count;
    }
    ;

stmt_list:
    stmt { 
        printf("  [REDUCE] stmt_list -> stmt\n");
        $$ = ast_new_block(1); 
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
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | for_stmt { $$ = $1; }
    | block { $$ = $1; }
    ;

block:
    LBRACE stmt_list RBRACE { 
        printf("  [REDUCE] block -> { stmt_list }\n");
        $$ = $2; 
    }
    ;

if_stmt:
    IF LPAREN expr RPAREN stmt { 
        printf("  [REDUCE] if_stmt -> if (expr) stmt\n");
        $$ = ast_new_if($3, $5, NULL, 1); 
    }
    | IF LPAREN expr RPAREN stmt ELSE stmt { 
        printf("  [REDUCE] if_stmt -> if (expr) stmt else stmt\n");
        $$ = ast_new_if($3, $5, $7, 1); 
    }
    ;

while_stmt:
    WHILE LPAREN expr RPAREN stmt { 
        printf("  [REDUCE] while_stmt -> while (expr) stmt\n");
        $$ = ast_new_while($3, $5, 1); 
    }
    ;

for_stmt:
    FOR LPAREN assign SEMICOLON expr SEMICOLON assign RPAREN stmt { 
        printf("  [REDUCE] for_stmt -> for (assign; expr; assign) stmt\n");
        $$ = ast_new_for($3, $5, $7, $9, 1); 
    }
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
    | FLOAT ID { 
        printf("  [REDUCE] decl -> float ID(%s)\n", $2);
        $$ = ast_new_decl(TYPE_FLOAT, $2, NULL, 1); 
    }
    | FLOAT ID ASSIGN expr { 
        printf("  [REDUCE] decl -> float ID(%s) = expr\n", $2);
        $$ = ast_new_decl(TYPE_FLOAT, $2, $4, 1); 
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
        $$ = ast_new_print($3, 1); 
    }
    ;

return_stmt:
    RETURN expr { 
        printf("  [REDUCE] return_stmt -> return expr\n");
        $$ = ast_new_return($2, 1); 
    }
    ;

expr:
    INT_LIT { 
        printf("  [REDUCE] expr -> INT_LIT(%ld)\n", $1);
        $$ = ast_new_int($1, 1); 
    }
    | FLOAT_LIT { 
        printf("  [REDUCE] expr -> FLOAT_LIT(%.2f)\n", $1);
        $$ = ast_new_float($1, 1); 
    }
    | ID { 
        printf("  [REDUCE] expr -> ID(%s)\n", $1);
        $$ = ast_new_var($1, 1); 
    }
    | ID LPAREN expr RPAREN {
        printf("  [REDUCE] expr -> ID(%s)(expr)\n", $1);
        $$ = ast_new_call($1, $3, 1);
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
    | expr EXP expr { 
        printf("  [REDUCE] expr -> expr ^ expr\n");
        $$ = ast_new_binop(TOK_EXP, $1, $3, 1); 
    }
    | expr EQ expr { 
        printf("  [REDUCE] expr -> expr == expr\n");
        $$ = ast_new_binop(TOK_EQ, $1, $3, 1); 
    }
    | expr NEQ expr { 
        printf("  [REDUCE] expr -> expr != expr\n");
        $$ = ast_new_binop(TOK_NEQ, $1, $3, 1); 
    }
    | expr LT expr { 
        printf("  [REDUCE] expr -> expr < expr\n");
        $$ = ast_new_binop(TOK_LT, $1, $3, 1); 
    }
    | expr GT expr { 
        printf("  [REDUCE] expr -> expr > expr\n");
        $$ = ast_new_binop(TOK_GT, $1, $3, 1); 
    }
    | expr LTE expr { 
        printf("  [REDUCE] expr -> expr <= expr\n");
        $$ = ast_new_binop(TOK_LTE, $1, $3, 1); 
    }
    | expr GTE expr { 
        printf("  [REDUCE] expr -> expr >= expr\n");
        $$ = ast_new_binop(TOK_GTE, $1, $3, 1); 
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
