#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================== PARSER STATE ====================== */

static Token *tokens;
static int token_count;
static int token_pos;
static int error_count;

/* ====================== UTILITY FUNCTIONS ====================== */

static Token current_token(void) {
    if (token_pos >= token_count) {
        Token eof;
        eof.type = TOK_EOF;
        eof.line = 0;
        strcpy(eof.lexeme, "EOF");
        return eof;
    }
    return tokens[token_pos];
}

static Token peek_next_token(void) {
    if (token_pos + 1 >= token_count) {
        Token eof;
        eof.type = TOK_EOF;
        eof.line = 0;
        strcpy(eof.lexeme, "EOF");
        return eof;
    }
    return tokens[token_pos + 1];
}

static void advance(void) {
    if (token_pos < token_count) token_pos++;
}

static int check(TokenType type) {
    return current_token().type == type;
}

static int match(TokenType type) {
    if (check(type)) {
        advance();
        return 1;
    }
    return 0;
}

static int expect(TokenType type, const char *message) {
    if (!check(type)) {
        fprintf(stderr, "ERROR: Expected %s but got %s at line %d\n",
                message, token_type_name(current_token().type), current_token().line);
        error_count++;
        return 0;
    }
    advance();
    return 1;
}

/* ====================== FORWARD DECLARATIONS ====================== */

static ASTNode *parse_expression(void);
static ASTNode *parse_statement(void);
static ASTNode *parse_block(void);
static ASTNode *parse_assignment_expression(void);

/* ====================== PRECEDENCE CLIMBING ====================== */

static int get_precedence(TokenType op) {
    switch (op) {
        case TOK_OR: return 1;
        case TOK_AND: return 2;
        case TOK_EQ: case TOK_NEQ: return 3;
        case TOK_LT: case TOK_LE: case TOK_GT: case TOK_GE: return 4;
        case TOK_PLUS: case TOK_MINUS: return 5;
        case TOK_MUL: case TOK_DIV: case TOK_MOD: return 6;
        default: return 0;
    }
}

static int is_binary_op(TokenType op) {
    return get_precedence(op) > 0;
}

/* ====================== PRIMARY EXPRESSIONS ====================== */

static ASTNode *parse_primary(void) {
    Token tok = current_token();
    
    /* Literals */
    if (tok.type == TOK_INT_LIT) {
        ASTNode *node = ast_alloc_node(NODE_INT, tok.line);
        node->data.int_val = tok.int_val;
        advance();
        return node;
    }
    
    if (tok.type == TOK_FLOAT_LIT) {
        ASTNode *node = ast_alloc_node(NODE_FLOAT, tok.line);
        node->data.float_val = tok.float_val;
        advance();
        return node;
    }
    
    if (tok.type == TOK_CHAR_LIT) {
        ASTNode *node = ast_alloc_node(NODE_CHAR, tok.line);
        node->data.char_val = tok.char_val;
        advance();
        return node;
    }
    
    if (tok.type == TOK_STRING_LIT) {
        ASTNode *node = ast_alloc_node(NODE_STRING, tok.line);
        strncpy(node->data.var.name, tok.lexeme, IDENTIFIER_MAX - 1);
        advance();
        return node;
    }
    
    /* Variable reference */
    if (tok.type == TOK_IDENT) {
        ASTNode *node = ast_alloc_node(NODE_VAR, tok.line);
        strncpy(node->data.var.name, tok.lexeme, IDENTIFIER_MAX - 1);
        symtab_set_used(tok.lexeme);
        advance();
        return node;
    }
    
    /* Parenthesized expression */
    if (tok.type == TOK_LPAREN) {
        advance();
        ASTNode *expr = parse_expression();
        expect(TOK_RPAREN, ")");
        return expr;
    }
    
    /* Unary operators */
    if (tok.type == TOK_NOT || tok.type == TOK_MINUS) {
        int op = tok.type;
        advance();
        ASTNode *node = ast_alloc_node(NODE_UNOP, tok.line);
        node->data.unop.op = op;
        node->data.unop.operand = parse_primary();
        return node;
    }
    
    fprintf(stderr, "ERROR: Unexpected token %s at line %d\n",
            token_type_name(tok.type), tok.line);
    error_count++;
    /* Recovery: consume one token to avoid infinite loops on bad input */
    advance();
    return NULL;
}

/* ====================== EXPRESSION PARSING ====================== */

static ASTNode *parse_expression_with_precedence(ASTNode *left, int min_prec) {
    while (is_binary_op(current_token().type)) {
        int prec = get_precedence(current_token().type);
        if (prec < min_prec) break;
        
        int op = current_token().type;
        int op_line = current_token().line;
        advance();
        
        ASTNode *right = parse_primary();
        
        while (is_binary_op(current_token().type) &&
               get_precedence(current_token().type) > prec) {
            right = parse_expression_with_precedence(right, prec + 1);
        }
        
        ASTNode *node = ast_alloc_node(NODE_BINOP, op_line);
        node->data.binop.op = op;
        node->data.binop.left = left;
        node->data.binop.right = right;
        left = node;
    }
    
    return left;
}

static ASTNode *parse_expression(void) {
    ASTNode *left = parse_primary();
    return parse_expression_with_precedence(left, 1);
}

static ASTNode *parse_assignment_expression(void) {
    ASTNode *expr = parse_expression();
    if (expr && expr->type == NODE_VAR && match(TOK_ASSIGN)) {
        ASTNode *value = parse_assignment_expression();
        ASTNode *node = ast_alloc_node(NODE_ASSIGN, expr->line);
        strncpy(node->data.assign.varname, expr->data.var.name, IDENTIFIER_MAX - 1);
        node->data.assign.varname[IDENTIFIER_MAX - 1] = '\0';
        node->data.assign.value = value;
        symtab_set_initialized(expr->data.var.name);
        return node;
    }
    return expr;
}

/* ====================== STATEMENT PARSING ====================== */

static ASTNode *parse_declaration(DataType type) {
    Token name_tok = current_token();
    if (!expect(TOK_IDENT, "identifier")) {
        return NULL;
    }
    
    ASTNode *node = ast_alloc_node(NODE_DECL, name_tok.line);
    node->data.decl.dtype = type;
    strncpy(node->data.decl.name, name_tok.lexeme, IDENTIFIER_MAX - 1);
    node->data.decl.init_value = NULL;
    
    symtab_add(name_tok.lexeme, type, name_tok.line);
    
    if (match(TOK_ASSIGN)) {
        node->data.decl.init_value = parse_expression();
        symtab_set_initialized(name_tok.lexeme);
    }
    
    expect(TOK_SEMI, ";");
    return node;
}

static ASTNode *parse_if_statement(void) {
    int line = current_token().line;
    advance();  /* skip 'if' */
    
    expect(TOK_LPAREN, "(");
    ASTNode *condition = parse_expression();
    expect(TOK_RPAREN, ")");
    
    ASTNode *then_branch = parse_statement();
    ASTNode *else_branch = NULL;
    
    if (match(TOK_ELSE)) {
        else_branch = parse_statement();
    }
    
    ASTNode *node = ast_alloc_node(NODE_IF, line);
    node->data.if_node.condition = condition;
    node->data.if_node.then_branch = then_branch;
    node->data.if_node.else_branch = else_branch;
    
    return node;
}

static ASTNode *parse_while_statement(void) {
    int line = current_token().line;
    advance();  /* skip 'while' */
    
    expect(TOK_LPAREN, "(");
    ASTNode *condition = parse_expression();
    expect(TOK_RPAREN, ")");
    
    symtab_enter_scope();
    ASTNode *body = parse_statement();
    symtab_exit_scope();
    
    ASTNode *node = ast_alloc_node(NODE_WHILE, line);
    node->data.while_node.condition = condition;
    node->data.while_node.body = body;
    
    return node;
}

static ASTNode *parse_for_statement(void) {
    int line = current_token().line;
    advance();  /* skip 'for' */
    
    expect(TOK_LPAREN, "(");
    
    ASTNode *init = NULL;
    if (!check(TOK_SEMI)) {
        init = parse_assignment_expression();
    }
    expect(TOK_SEMI, ";");
    
    ASTNode *condition = NULL;
    if (!check(TOK_SEMI)) {
        condition = parse_expression();
    }
    expect(TOK_SEMI, ";");
    
    ASTNode *update = NULL;
    if (!check(TOK_RPAREN)) {
        update = parse_assignment_expression();
    }
    expect(TOK_RPAREN, ")");
    
    symtab_enter_scope();
    ASTNode *body = parse_statement();
    symtab_exit_scope();
    
    ASTNode *node = ast_alloc_node(NODE_FOR, line);
    node->data.for_node.init = init;
    node->data.for_node.condition = condition;
    node->data.for_node.update = update;
    node->data.for_node.body = body;
    
    return node;
}

static ASTNode *parse_print_statement(void) {
    int line = current_token().line;
    advance();  /* skip 'print' */
    
    expect(TOK_LPAREN, "(");
    ASTNode *arg = parse_expression();
    expect(TOK_RPAREN, ")");
    expect(TOK_SEMI, ";");
    
    ASTNode *node = ast_alloc_node(NODE_PRINT, line);
    node->data.print.arg = arg;
    
    return node;
}

static ASTNode *parse_return_statement(void) {
    int line = current_token().line;
    advance();  /* skip 'return' */
    
    ASTNode *value = NULL;
    if (!check(TOK_SEMI)) {
        value = parse_expression();
    }
    expect(TOK_SEMI, ";");
    
    ASTNode *node = ast_alloc_node(NODE_RETURN, line);
    node->data.ret.value = value;
    
    return node;
}

static ASTNode *parse_assignment_or_expr_statement(void) {
    ASTNode *expr = parse_assignment_expression();
    expect(TOK_SEMI, ";");
    return expr;
}

static ASTNode *parse_statement(void) {
    Token tok = current_token();
    
    /* Type declarations */
    if (tok.type == TOK_INT) {
        advance();
        return parse_declaration(TYPE_INT);
    }
    if (tok.type == TOK_FLOAT) {
        advance();
        return parse_declaration(TYPE_FLOAT);
    }
    if (tok.type == TOK_CHAR) {
        advance();
        return parse_declaration(TYPE_CHAR);
    }
    
    /* Control flow */
    if (tok.type == TOK_IF) return parse_if_statement();
    if (tok.type == TOK_WHILE) return parse_while_statement();
    if (tok.type == TOK_FOR) return parse_for_statement();
    
    /* I/O */
    if (tok.type == TOK_PRINT) return parse_print_statement();
    
    /* Return */
    if (tok.type == TOK_RETURN) return parse_return_statement();
    
    /* Block */
    if (tok.type == TOK_LBRACE) return parse_block();
    
    /* Expression statement */
    return parse_assignment_or_expr_statement();
}

/* ====================== BLOCK PARSING ====================== */

static ASTNode *parse_block(void) {
    int line = current_token().line;
    expect(TOK_LBRACE, "{");
    
    symtab_enter_scope();
    
    ASTNode **stmts = malloc(1000 * sizeof(ASTNode*));
    int stmt_count = 0;
    
    while (!check(TOK_RBRACE) && !check(TOK_EOF)) {
        int start_pos = token_pos;
        ASTNode *stmt = parse_statement();
        if (stmt) {
            stmts[stmt_count++] = stmt;
        }
        /* Recovery: if no tokens were consumed, force progress */
        if (token_pos == start_pos) {
            Token t = current_token();
            fprintf(stderr, "ERROR: Recovery skip token %s at line %d\n",
                    token_type_name(t.type), t.line);
            error_count++;
            advance();
        }
    }
    
    symtab_exit_scope();
    
    expect(TOK_RBRACE, "}");
    
    ASTNode *node = ast_alloc_node(NODE_BLOCK, line);
    node->data.block.stmts = stmts;
    node->data.block.stmt_count = stmt_count;
    
    return node;
}

/* ====================== PROGRAM PARSING ====================== */

static ASTNode *parse_main(void) {
    /* Accept both:
     *   main() { ... }
     *   int main() { ... }
     */
    if (check(TOK_INT)) {
        advance();
    }
    expect(TOK_MAIN, "main");
    expect(TOK_LPAREN, "(");
    expect(TOK_RPAREN, ")");
    
    ASTNode *body = parse_block();
    
    return body;
}

ASTNode *parser_parse_program(void) {
    symtab_init();
    symtab_enter_scope();
    
    ASTNode *main = parse_main();
    
    ASTNode *program = ast_alloc_node(NODE_PROGRAM, 1);
    ASTNode **stmts = malloc(sizeof(ASTNode*));
    stmts[0] = main;
    program->data.block.stmts = stmts;
    program->data.block.stmt_count = 1;
    
    return program;
}

/* ====================== PARSER INITIALIZATION ====================== */

void parser_init(Token *token_stream, int t_count) {
    tokens = token_stream;
    token_count = t_count;
    token_pos = 0;
    error_count = 0;
}

/* ====================== STATUS FUNCTIONS ====================== */

int parser_get_position(void) {
    return token_pos;
}

int parser_get_error_count(void) {
    return error_count;
}
// Commit Marker// Commit Marker