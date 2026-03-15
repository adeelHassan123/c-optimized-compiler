#ifndef LEXER_H
#define LEXER_H

#include "types.h"

/* ====================== TOKEN STRUCTURE ====================== */

typedef struct {
    TokenType type;
    char lexeme[IDENTIFIER_MAX];
    int line;
    long int_val;
    double float_val;
    char char_val;
} Token;

/* ====================== LEXER INTERFACE ====================== */

/* Initialize lexer with source code */
void lexer_init(const char *source);

/* Get next token from input */
Token lexer_next_token(void);

/* Get current token without advancing */
Token lexer_peek_token(void);

/* Skip to next token */
void lexer_advance(void);

/* Get token type name for debugging */
const char *token_type_name(TokenType t);

/* Print token information */
void token_print(Token t);

#endif /* LEXER_H */
// Commit Marker