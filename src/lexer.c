#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ====================== LEXER STATE ====================== */

static const char *source;
static int src_pos;
static int line_no;
static Token current_token;
static int token_fetched;

/* ====================== LEXER INITIALIZATION ====================== */

void lexer_init(const char *src) {
    source = src;
    src_pos = 0;
    line_no = 1;
    token_fetched = 0;
}

/* ====================== CHARACTER OPERATIONS ====================== */

static char peek_char(void) {
    if (src_pos >= (int)strlen(source)) return '\0';
    return source[src_pos];
}

static char peek_next_char(void) {
    if (src_pos + 1 >= (int)strlen(source)) return '\0';
    return source[src_pos + 1];
}

static char advance_char(void) {
    char c = source[src_pos++];
    if (c == '\n') line_no++;
    return c;
}

/* ====================== WHITESPACE AND COMMENTS ====================== */

static void skip_whitespace_and_comments(void) {
    while (1) {
        while (isspace((unsigned char)peek_char())) advance_char();
        
        if (peek_char() == '/' && peek_next_char() == '/') {
            advance_char(); advance_char();
            while (peek_char() && peek_char() != '\n') advance_char();
            continue;
        }
        
        if (peek_char() == '/' && peek_next_char() == '*') {
            advance_char(); advance_char();
            while (peek_char() && !(peek_char() == '*' && peek_next_char() == '/'))
                advance_char();
            if (peek_char() == '*') { advance_char(); advance_char(); }
            continue;
        }
        
        break;
    }
}

/* ====================== TOKEN NAME LOOKUP ====================== */

const char *token_type_name(TokenType t) {
    switch (t) {
        case TOK_INT: return "int";
        case TOK_FLOAT: return "float";
        case TOK_CHAR: return "char";
        case TOK_IF: return "if";
        case TOK_ELSE: return "else";
        case TOK_WHILE: return "while";
        case TOK_FOR: return "for";
        case TOK_RETURN: return "return";
        case TOK_PRINT: return "print";
        case TOK_MAIN: return "main";
        case TOK_IDENT: return "identifier";
        case TOK_INT_LIT: return "int_literal";
        case TOK_FLOAT_LIT: return "float_literal";
        case TOK_CHAR_LIT: return "char_literal";
        case TOK_STRING_LIT: return "string_literal";
        case TOK_ASSIGN: return "=";
        case TOK_SEMI: return ";";
        case TOK_COMMA: return ",";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_MUL: return "*";
        case TOK_DIV: return "/";
        case TOK_MOD: return "%";
        case TOK_GT: return ">";
        case TOK_LT: return "<";
        case TOK_EQ: return "==";
        case TOK_NEQ: return "!=";
        case TOK_GE: return ">=";
        case TOK_LE: return "<=";
        case TOK_AND: return "&&";
        case TOK_OR: return "||";
        case TOK_NOT: return "!";
        case TOK_EOF: return "EOF";
        default: return "unknown";
    }
}

/* ====================== SINGLE CHARACTER TOKENS ====================== */

static Token make_token(TokenType type, const char *lexeme) {
    Token t;
    t.type = type;
    strncpy(t.lexeme, lexeme, IDENTIFIER_MAX - 1);
    t.lexeme[IDENTIFIER_MAX - 1] = '\0';
    t.line = line_no;
    t.int_val = 0;
    t.float_val = 0.0;
    t.char_val = '\0';
    return t;
}

/* ====================== KEYWORD / IDENTIFIER SCANNING ====================== */

static Token scan_identifier(void) {
    int line = line_no;
    char buffer[IDENTIFIER_MAX];
    int len = 0;
    
    buffer[len++] = advance_char();
    while (isalnum((unsigned char)peek_char()) || peek_char() == '_') {
        if (len < IDENTIFIER_MAX - 1) buffer[len++] = advance_char();
    }
    buffer[len] = '\0';
    
    Token t = make_token(TOK_IDENT, buffer);
    t.line = line;
    
    if (strcmp(buffer, "int") == 0) t.type = TOK_INT;
    else if (strcmp(buffer, "float") == 0) t.type = TOK_FLOAT;
    else if (strcmp(buffer, "char") == 0) t.type = TOK_CHAR;
    else if (strcmp(buffer, "if") == 0) t.type = TOK_IF;
    else if (strcmp(buffer, "else") == 0) t.type = TOK_ELSE;
    else if (strcmp(buffer, "while") == 0) t.type = TOK_WHILE;
    else if (strcmp(buffer, "for") == 0) t.type = TOK_FOR;
    else if (strcmp(buffer, "return") == 0) t.type = TOK_RETURN;
    else if (strcmp(buffer, "print") == 0) t.type = TOK_PRINT;
    else if (strcmp(buffer, "main") == 0) t.type = TOK_MAIN;
    
    return t;
}

/* ====================== NUMBER SCANNING ====================== */

static Token scan_number(void) {
    int line = line_no;
    char buffer[IDENTIFIER_MAX];
    int len = 0;
    int is_float = 0;
    
    while (isdigit((unsigned char)peek_char())) {
        if (len < IDENTIFIER_MAX - 1) buffer[len++] = advance_char();
    }
    
    if (peek_char() == '.' && isdigit((unsigned char)peek_next_char())) {
        is_float = 1;
        if (len < IDENTIFIER_MAX - 1) buffer[len++] = advance_char();
        while (isdigit((unsigned char)peek_char())) {
            if (len < IDENTIFIER_MAX - 1) buffer[len++] = advance_char();
        }
    }
    
    buffer[len] = '\0';
    
    Token t;
    t.line = line;
    t.lexeme[0] = '\0';
    strncpy(t.lexeme, buffer, IDENTIFIER_MAX - 1);
    
    if (is_float) {
        t.type = TOK_FLOAT_LIT;
        t.float_val = atof(buffer);
        t.int_val = 0;
    } else {
        t.type = TOK_INT_LIT;
        t.int_val = atol(buffer);
        t.float_val = 0.0;
    }
    t.char_val = '\0';
    
    return t;
}

/* ====================== CHARACTER LITERAL SCANNING ====================== */

static Token scan_char_literal(void) {
    int line = line_no;
    advance_char();  /* consume ' */
    char c = advance_char();
    if (peek_char() == '\'') advance_char();  /* consume closing ' */
    
    Token t = make_token(TOK_CHAR_LIT, "");
    t.line = line;
    t.char_val = c;
    t.int_val = (int)c;
    
    return t;
}

/* ====================== STRING LITERAL SCANNING ====================== */

static Token scan_string_literal(void) {
    int line = line_no;
    advance_char();  /* consume " */
    char buffer[STRING_MAX];
    int len = 0;
    
    while (peek_char() && peek_char() != '"') {
        if (len < STRING_MAX - 1) buffer[len++] = advance_char();
    }
    buffer[len] = '\0';
    
    if (peek_char() == '"') advance_char();  /* consume closing " */
    
    Token t = make_token(TOK_STRING_LIT, buffer);
    t.line = line;
    
    return t;
}

/* ====================== MAIN TOKENIZATION ====================== */

static Token scan_single_token(void) {
    skip_whitespace_and_comments();
    
    int line = line_no;
    char c = peek_char();
    
    if (c == '\0') {
        return make_token(TOK_EOF, "EOF");
    }
    
    if (isalpha((unsigned char)c) || c == '_') {
        return scan_identifier();
    }
    
    if (isdigit((unsigned char)c)) {
        return scan_number();
    }
    
    if (c == '\'') {
        return scan_char_literal();
    }
    
    if (c == '"') {
        return scan_string_literal();
    }
    
    /* Single/double character operators */
    advance_char();
    
    switch (c) {
        case '+': return make_token(TOK_PLUS, "+");
        case '-': return make_token(TOK_MINUS, "-");
        case '*': return make_token(TOK_MUL, "*");
        case '/': return make_token(TOK_DIV, "/");
        case '%': return make_token(TOK_MOD, "%");
        case '(': return make_token(TOK_LPAREN, "(");
        case ')': return make_token(TOK_RPAREN, ")");
        case '{': return make_token(TOK_LBRACE, "{");
        case '}': return make_token(TOK_RBRACE, "}");
        case ';': return make_token(TOK_SEMI, ";");
        case ',': return make_token(TOK_COMMA, ",");
        
        case '=':
            if (peek_char() == '=') {
                advance_char();
                return make_token(TOK_EQ, "==");
            }
            return make_token(TOK_ASSIGN, "=");
        
        case '!':
            if (peek_char() == '=') {
                advance_char();
                return make_token(TOK_NEQ, "!=");
            }
            return make_token(TOK_NOT, "!");
        
        case '<':
            if (peek_char() == '=') {
                advance_char();
                return make_token(TOK_LE, "<=");
            }
            return make_token(TOK_LT, "<");
        
        case '>':
            if (peek_char() == '=') {
                advance_char();
                return make_token(TOK_GE, ">=");
            }
            return make_token(TOK_GT, ">");
        
        case '&':
            if (peek_char() == '&') {
                advance_char();
                return make_token(TOK_AND, "&&");
            }
            return make_token(TOK_UNKNOWN, "&");
        
        case '|':
            if (peek_char() == '|') {
                advance_char();
                return make_token(TOK_OR, "||");
            }
            return make_token(TOK_UNKNOWN, "|");
        
        default:
            return make_token(TOK_UNKNOWN, "");
    }
}

/* ====================== PUBLIC INTERFACE ====================== */

Token lexer_next_token(void) {
    current_token = scan_single_token();
    token_fetched = 1;
    return current_token;
}

Token lexer_peek_token(void) {
    if (!token_fetched) {
        current_token = scan_single_token();
        src_pos--;  /* Back up one character */
        if (source[src_pos] == '\n') line_no--;
    }
    return current_token;
}

void lexer_advance(void) {
    current_token = scan_single_token();
    token_fetched = 1;
}

void token_print(Token t) {
    printf("Token(%s, \"%s\", line %d)\n", token_type_name(t.type), t.lexeme, t.line);
}
// Commit Marker// Commit Marker