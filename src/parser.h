#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

/* ====================== PARSER INTERFACE ====================== */

/* Initialize parser with token stream */
void parser_init(Token *token_stream, int token_count);

/* Parse entire program */
ASTNode *parser_parse_program(void);

/* Parse statement */
ASTNode *parser_parse_statement(void);

/* Parse expression with precedence climbing */
ASTNode *parser_parse_expression(void);

/* Parse primary expression */
ASTNode *parser_parse_primary(void);

/* Parse variable declaration */
ASTNode *parser_parse_declaration(DataType type);

/* Get current parsing position */
int parser_get_position(void);

/* Get error count */
int parser_get_error_count(void);

#endif /* PARSER_H */
