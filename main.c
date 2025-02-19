#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    /*-------------------------------------
    * Mots-clés standards
    *-------------------------------------*/
    TOK_IF,          // if
    TOK_ELSE,        // else
    TOK_WHILE,       // while
    TOK_FOR,         // for
    TOK_DO,          // do
    TOK_RETURN,      // return
    TOK_BREAK,       // break
    TOK_CONTINUE,    // continue
    TOK_SWITCH,      // switch
    TOK_CASE,        // case
    TOK_DEFAULT,     // default
    TOK_TYPEDEF,     // typedef
    TOK_STRUCT,      // struct
    TOK_ENUM,        // enum
    TOK_UNION,       // union
    TOK_CONST,       // const
    TOK_VOLATILE,    // volatile
    TOK_SIZEOF,      // sizeof

    /*-------------------------------------
    * Types de données
    *-------------------------------------*/
    TOK_KW_INT,      // int
    TOK_KW_FLOAT,    // float
    TOK_KW_CHAR,     // char
    TOK_KW_VOID,     // void
    TOK_KW_SHORT,    // short
    TOK_KW_LONG,     // long
    TOK_KW_DOUBLE,   // double
    TOK_KW_SIGNED,   // signed
    TOK_KW_UNSIGNED, // unsigned

    /*-------------------------------------
    * Identificateurs et littéraux
    *-------------------------------------*/
    TOK_ID,          // identificateur
    TOK_INT_LIT,     // littéral entier (42)
    TOK_FLOAT_LIT,   // littéral flottant (3.14)
    TOK_CHAR_LIT,    // caractère ('a')
    TOK_STRING_LIT,  // chaîne ("hello")

    /*-------------------------------------
    * Opérateurs
    *-------------------------------------*/
    // Arithmétiques
    TOK_PLUS,        // +
    TOK_MINUS,       // -
    TOK_MULT,        // *
    TOK_DIV,         // /
    TOK_MOD,         // %
    TOK_INC,         // ++
    TOK_DEC,         // --

    // Comparaison
    TOK_EQ,          // ==
    TOK_NEQ,         // !=
    TOK_LT,          // <
    TOK_GT,          // >
    TOK_LTE,         // <=
    TOK_GTE,         // >=

    // Logiques
    TOK_LOGICAL_AND, // &&
    TOK_LOGICAL_OR,  // ||
    TOK_LOGICAL_NOT, // !

    // Bit à bit
    TOK_BIT_AND,     // &
    TOK_BIT_OR,      // |
    TOK_BIT_XOR,     // ^
    TOK_BIT_NOT,     // ~
    TOK_SHL,         // <<
    TOK_SHR,         // >>

    // Affectation
    TOK_ASSIGN,      // =
    TOK_ADD_ASSIGN,  // +=
    TOK_SUB_ASSIGN,  // -=
    TOK_MUL_ASSIGN,  // *=
    TOK_DIV_ASSIGN,  // /=
    TOK_MOD_ASSIGN,  // %=
    TOK_AND_ASSIGN,  // &=
    TOK_OR_ASSIGN,   // |=
    TOK_XOR_ASSIGN,  // ^=
    TOK_SHL_ASSIGN,  // <<=
    TOK_SHR_ASSIGN,  // >>=

    /*-------------------------------------
    * Délimiteurs et ponctuation
    *-------------------------------------*/
    TOK_LPAREN,      // (
    TOK_RPAREN,      // )
    TOK_LBRACE,      // {
    TOK_RBRACE,      // }
    TOK_LBRACKET,    // [
    TOK_RBRACKET,    // ]
    TOK_COMMA,       // ,
    TOK_SEMICOLON,   // ;
    TOK_COLON,       // :
    TOK_QUESTION,    // ?
    TOK_DOT,         // .
    TOK_ARROW,       // ->

    /*-------------------------------------
    * Préprocesseur
    *-------------------------------------*/
    TOK_PREPROC,     // #
    TOK_INCLUDE,     // #include
    TOK_DEFINE,      // #define
    TOK_IFDEF,       // #ifdef

    /*-------------------------------------
    * Contrôle de flux
    *-------------------------------------*/
    TOK_END,         // fin de fichier
    TOK_UNKNOWN      // token inconnu
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
} Token;

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
    STATE_IN_XOR,
    STATE_IN_NOT,
    STATE_IN_SHL,
    STATE_IN_SHR,
    STATE_IN_STRING,
    STATE_IN_CHAR,
    STATE_IN_PREPROC
} LexerState;

/* Liste des mots-clés avec leurs tokens associés */
static const struct {
    const char* keyword;
    TokenType token;
} keywords[] = {
    {"if", TOK_IF}, {"else", TOK_ELSE}, {"while", TOK_WHILE},
    {"for", TOK_FOR}, {"do", TOK_DO}, {"return", TOK_RETURN},
    {"break", TOK_BREAK}, {"continue", TOK_CONTINUE},
    {"switch", TOK_SWITCH}, {"case", TOK_CASE}, {"default", TOK_DEFAULT},
    {"typedef", TOK_TYPEDEF}, {"struct", TOK_STRUCT}, {"enum", TOK_ENUM},
    {"union", TOK_UNION}, {"const", TOK_CONST}, {"volatile", TOK_VOLATILE},
    {"sizeof", TOK_SIZEOF}, {"int", TOK_KW_INT}, {"float", TOK_KW_FLOAT},
    {"char", TOK_KW_CHAR}, {"void", TOK_KW_VOID}, {"short", TOK_KW_SHORT},
    {"long", TOK_KW_LONG}, {"double", TOK_KW_DOUBLE}, {"signed", TOK_KW_SIGNED},
    {"unsigned", TOK_KW_UNSIGNED}
};

int is_keyword(const char* str) {
    size_t num_keywords = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) return keywords[i].token;
    }
    return TOK_ID; // Return TOK_ID if not a keyword
}

Token* create_token(TokenType type, const char* value) {
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

void reset_lexer(Lexer* lex) {
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
                    case '^':
                        tok = create_token(TOK_BIT_XOR, "^");
                        lex->pos++;
                        return tok;
                    case '~':
                        tok = create_token(TOK_BIT_NOT, "~");
                        lex->pos++;
                        return tok;
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
                    case '[':
                        tok = create_token(TOK_LBRACKET, "[");
                        lex->pos++;
                        return tok;
                    case ']':
                        tok = create_token(TOK_RBRACKET, "]");
                        lex->pos++;
                        return tok;
                    case ':':
                        tok = create_token(TOK_COLON, ":");
                        lex->pos++;
                        return tok;
                    case '?':
                        tok = create_token(TOK_QUESTION, "?");
                        lex->pos++;
                        return tok;
                    case '.':
                        tok = create_token(TOK_DOT, ".");
                        lex->pos++;
                        return tok;
                    default:
                        tok = create_token(TOK_UNKNOWN, &c);
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
                    lex->pos--;
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
                } else if (c == '<') {
                    tok = create_token(TOK_SHL, "<<");
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
                } else if (c == '>') {
                    tok = create_token(TOK_SHR, ">>");
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
                } else if (c == '=') {
                    tok = create_token(TOK_ADD_ASSIGN, "+=");
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
                } else if (c == '=') {
                    tok = create_token(TOK_SUB_ASSIGN, "-=");
                    lex->pos++;
                } else if (c == '>') {
                    tok = create_token(TOK_ARROW, "->");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_MINUS, "-");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_BIT_AND:
                if (c == '&') {
                    tok = create_token(TOK_LOGICAL_AND, "&&");
                    lex->pos++;
                } else if (c == '=') {
                    tok = create_token(TOK_AND_ASSIGN, "&=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_BIT_AND, "&");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_BIT_OR:
                if (c == '|') {
                    tok = create_token(TOK_LOGICAL_OR, "||");
                    lex->pos++;
                } else if (c == '=') {
                    tok = create_token(TOK_OR_ASSIGN, "|=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_BIT_OR, "|");
                    lex->pos--;
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_STRING:
                if (c == '"') {
                    tok = create_token(TOK_STRING_LIT, lex->buffer);
                    lex->pos++;
                    lex->state = STATE_START;
                } else if (c == '\\') {
                    lex->pos++;
                    c = lex->input[lex->pos];
                    strncat(lex->buffer, &c, 1); // Handle escape sequences
                    lex->pos++;
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
                } else if (c == '\\') {
                    lex->pos++;
                    c = lex->input[lex->pos];
                    strncat(lex->buffer, &c, 1); // Handle escape sequences
                    lex->pos++;
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

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length + 1);
    if (!buffer) {
        perror("Could not allocate memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0'; // Null-terminate the string
    fclose(file);
    return buffer;
}

int main() {
    const char* filename = "text.loom"; // Change this to your file name
    char* input = read_file(filename);
    if (!input) {
        return 1; // Error reading file
    }

    Lexer* lexer = create_lexer(input);
    Token* tok;

    while((tok = next_token(lexer))->type != TOK_END) {
        printf("Token: %d, Value: %s\n", tok->type, tok->value);
        free(tok);
    }

    free(input);
    free(lexer);
    return 0;
}