#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "tokens.h"

/* Liste des mots-clés avec leurs tokens associés */
static const struct {
    const char* keyword;
    TokenType token;
} keywords[] = {
    {"if", TOK_IF}, {"else", TOK_ELSE}, {"while", TOK_WHILE},
    {"for", TOK_FOR}, {"return", TOK_RETURN},
    {"break", TOK_BREAK}, {"continue", TOK_CONTINUE},
    {"switch", TOK_SWITCH}, {"case", TOK_CASE}, {"default", TOK_DEFAULT},
    {"typedef", TOK_TYPEDEF}, {"struct", TOK_STRUCT}, {"enum", TOK_ENUM},
    {"const", TOK_CONST}, {"volatile", TOK_VOLATILE},
    {"sizeof", TOK_SIZEOF}, {"int", TOK_KW_INT}, {"float", TOK_KW_FLOAT},
    {"char", TOK_KW_CHAR}, {"void", TOK_KW_VOID}, {"short", TOK_KW_SHORT},
    {"long", TOK_KW_LONG}, {"double", TOK_KW_DOUBLE}, {"signed", TOK_KW_SIGNED},
    {"unsigned", TOK_KW_UNSIGNED}
};

static int is_keyword(const char* str) {
    size_t num_keywords = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) return keywords[i].token;
    }
    return TOK_ID; // Return TOK_ID if not a keyword
}

static Token* create_token(TokenType type, const char* value) {
    Token* tok = malloc(sizeof(Token));
    tok->type = type;
    strncpy(tok->value, value, 255);
    tok->value[255] = '\0';
    return tok;
}

 Lexer* create_lexer(const char* input) {
    Lexer* lex = malloc(sizeof(Lexer));
    lex->input = input;
    lex->pos = 0;
    lex->start = 0;
    lex->state = STATE_START;
    lex->buffer[0] = '\0';
    return lex;
}

static void reset_lexer(Lexer* lex) {
    memset(lex->buffer, 0, sizeof(lex->buffer));
    lex->start = lex->pos;
}

 Token* next_token(Lexer* lex) {
    Token* tok = NULL;
    
    while (1) {
        char c = lex->input[lex->pos];
        
        switch (lex->state) {
            case STATE_START:
                if (c == '\0') {
                    return create_token(TOK_END, "");
                }
                if (isspace(c)) {
                    lex->pos++;
                    break;
                }
                if (isalpha(c)) {
                    lex->state = STATE_IN_ID;
                    lex->buffer[0] = c;
                    lex->buffer[1] = '\0';
                    lex->pos++;
                } else if (isdigit(c)) {
                    lex->state = STATE_IN_INT;
                    lex->buffer[0] = c;
                    lex->buffer[1] = '\0';
                    lex->pos++;
                } else switch (c) {
                    case '+':
                        lex->state = STATE_IN_PLUS;
                        lex->pos++;
                        break;
                    case '-':
                        lex->state = STATE_IN_MINUS;
                        lex->pos++;
                        break;
                    case '*':
                        tok = create_token(TOK_MULT, "*");
                        lex->pos++;
                        return tok;
                    case '/':
                        tok = create_token(TOK_DIV, "/");
                        lex->pos++;
                        return tok;
                    case '%':
                        tok = create_token(TOK_MOD, "%");
                        lex->pos++;
                        return tok;
                    case '&':
                        lex->state = STATE_IN_BIT_AND;
                        lex->pos++;
                        break;
                    case '|':
                        lex->state = STATE_IN_BIT_OR;
                        lex->pos++;
                        break;
                    case '<':
                        lex->state = STATE_IN_LT;
                        lex->pos++;
                        break;
                    case '>':
                        lex->state = STATE_IN_GT;
                        lex->pos++;
                        break;
                    case '=':
                        lex->state = STATE_IN_EQ;
                        lex->pos++;
                        break;
                    case '!':
                        lex->state = STATE_IN_NEQ;
                        lex->pos++;
                        break;
                    case '"':
                        lex->state = STATE_IN_STRING;
                        lex->buffer[0] = '\0'; // Reset buffer
                        lex->pos++;
                        break;
                    case '\'':
                        lex->state = STATE_IN_CHAR;
                        lex->buffer[0] = '\0'; // Reset buffer
                        lex->pos++;
                        break;
                    case '#':
                        lex->state = STATE_IN_PREPROC;
                        lex->buffer[0] = '\0'; // Reset buffer
                        lex->pos++;
                        break;
                    case ';':
                        tok = create_token(TOK_SEMICOLON, ";");
                        lex->pos++;
                        return tok;
                    case ',':
                        tok = create_token(TOK_COMMA, ",");
                        lex->pos++;
                        return tok;
                    case '(':
                        tok = create_token(TOK_LPAREN, "(");
                        lex->pos++;
                        return tok;
                    case ')':
                        tok = create_token(TOK_RPAREN, ")");
                        lex->pos++;
                        return tok;
                    case '{':
                        tok = create_token(TOK_LBRACE, "{");
                        lex->pos++;
                        return tok;
                    case '}':
                        tok = create_token(TOK_RBRACE, "}");
                        lex->pos++;
                        return tok;
                    default:
                        lex->pos++;
                        return tok;
                }
                break;

            case STATE_IN_ID:
                if (isalnum(c)) {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                } else {
                    tok = create_token(is_keyword(lex->buffer), lex->buffer);
                    lex->state = STATE_START;
                }
                break;

            case STATE_IN_INT:
                if (isdigit(c)) {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                } else if (c == '.') {
                    strncat(lex->buffer, &c, 1);
                    lex->state = STATE_IN_FLOAT;
                    lex->pos++;
                } else {
                    tok = create_token(TOK_INT_LIT, lex->buffer);
                    lex->state = STATE_START;
                }
                break;

            case STATE_IN_FLOAT:
                if (isdigit(c)) {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                } else {
                    tok = create_token(TOK_FLOAT_LIT, lex->buffer);
                    lex->state = STATE_START;
                }
                break;

                case STATE_IN_EQ:
                if (c == '=') {
                    tok = create_token(TOK_EQ, "==");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_ASSIGN, "=");
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_NEQ:
                if (c == '=') {
                    tok = create_token(TOK_NEQ, "!=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_LOGICAL_NOT, "!");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_LT:
                if (c == '=') {
                    tok = create_token(TOK_LTE, "<=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_LT, "<");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_GT:
                if (c == '=') {
                    tok = create_token(TOK_GTE, ">=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_GT, ">");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_PLUS:
                if (c == '+') {
                    tok = create_token(TOK_INC, "++");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_PLUS, "+");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_MINUS:
                if (c == '-') {
                    tok = create_token(TOK_DEC, "--");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_MINUS, "-");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_STRING:
                if (c == '"') {
                    tok = create_token(TOK_STRING_LIT, lex->buffer);
                    lex->pos++;
                    lex->state = STATE_START;
                } else {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                }
                break;

            case STATE_IN_CHAR:
                if (c == '\'') {
                    tok = create_token(TOK_CHAR_LIT, lex->buffer);
                    lex->pos++;
                    lex->state = STATE_START;
                } else {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                }
                break;

            case STATE_IN_PREPROC:
                while (isalnum(c)) {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                    c = lex->input[lex->pos];
                }
                if (strcmp(lex->buffer, "include") == 0) {
                    tok = create_token(TOK_INCLUDE, "#include");
                } else if (strcmp(lex->buffer, "define") == 0) {
                    tok = create_token(TOK_DEFINE, "#define");
                } else if (strcmp(lex->buffer, "ifdef") == 0) {
                    tok = create_token(TOK_IFDEF, "#ifdef");
                } else {
                    tok = create_token(TOK_PREPROC, "#");
                }
                lex->state = STATE_START;
                break;
        }

        if (tok) {
            reset_lexer(lex);
            return tok;
        }
    }
}
