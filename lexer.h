#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

typedef struct {
    const char* input;
    int pos;
    int start;
    int state;
    char buffer[256];
} Lexer;

typedef enum {
    STATE_START,
    STATE_IN_ID,
    STATE_IN_INT,
    STATE_IN_FLOAT,
    STATE_IN_EQ,
    STATE_IN_NEQ,
    STATE_IN_LT,
    STATE_IN_GT,
    STATE_IN_PLUS,
    STATE_IN_MINUS,
    STATE_IN_MULT,
    STATE_IN_DIV,
    STATE_IN_MOD,
    STATE_IN_BIT_AND,
    STATE_IN_BIT_OR,
    STATE_IN_BIT_XOR,
    STATE_IN_BIT_NOT,
    STATE_IN_AND,
    STATE_IN_OR,
    STATE_IN_NOT,
    STATE_IN_ASSIGN,
    STATE_IN_INC,
    STATE_IN_DEC,
    STATE_IN_LPAREN,
    STATE_IN_RPAREN,
    STATE_IN_LBRACE,
    STATE_IN_RBRACE,
    STATE_IN_LBRACKET,
    STATE_IN_RBRACKET,
    STATE_IN_SEMICOLON,
    STATE_IN_COMMA,
    STATE_IN_DOT,
    STATE_IN_COLON,
    STATE_IN_STRING,
    STATE_IN_CHAR,
    STATE_IN_COMMENT_SINGLE,
    STATE_IN_COMMENT_MULTI,
    STATE_IN_PREPROC
} LexerState;

Lexer* create_lexer(const char* input);
Token* next_token(Lexer* lex);

#endif
