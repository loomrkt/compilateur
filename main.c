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
    char value[32];
} Token;

typedef struct {
    const char* input;
    int pos;
    int start;
    int state;
    char buffer[32];
} Lexer;

typedef enum {
    STATE_START,
    STATE_IN_ID,
    STATE_IN_INT,
    STATE_IN_FLOAT,
    STATE_IN_EQ,
    STATE_IN_NEQ,
    STATE_IN_LT,
    STATE_IN_GT
} LexerState;

int is_keyword(const char* str) {
    static const char* keywords[] = {
        "if", "else", "while", "for", "do", "return", "break", "continue",
        "switch", "case", "default", "typedef", "struct", "enum", "union",
        "const", "volatile", "sizeof", "int", "float", "char", "void", "short",
        "long", "double", "signed", "unsigned"
    };
    
    size_t num_keywords = sizeof(keywords) / sizeof(keywords[0]);

    for (size_t i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

Token* create_token(TokenType type, const char* value) {
    Token* tok = malloc(sizeof(Token));
    tok->type = type;
    strncpy(tok->value, value, 31);
    tok->value[31] = '\0';
    return tok;
}

Lexer* create_lexer(const char* input) {
    Lexer* lex = malloc(sizeof(Lexer));
    lex->input = input;
    lex->pos = 0;
    lex->start = 0;
    lex->state = STATE_START;
    return lex;
}

void reset_lexer(Lexer* lex) {
    memset(lex->buffer, 0, 32);
    lex->start = lex->pos;
}

Token* next_token(Lexer* lex) {
    Token* tok = NULL;
    int float_point = 0;
    
    while(1) {
        char c = lex->input[lex->pos];
        
        switch(lex->state) {
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
                }
                else if (isdigit(c)) {
                    lex->state = STATE_IN_INT;
                    lex->buffer[0] = c;
                    lex->buffer[1] = '\0';
                    lex->pos++;
                }
                else switch(c) {
                    case '%':
                    tok = create_token(TOK_MOD, "%");
                    lex->pos++;
                    break;
                    case '&':
                    tok = create_token(TOK_BIT_AND, "&");
                    lex->pos++;
                    break;
                    case '|':
                    tok = create_token(TOK_BIT_OR, "|");
                    lex->pos++;
                    break;
                    case '^':
                    tok = create_token(TOK_BIT_XOR, "^");
                    lex->pos++;
                    break;
                    case '~':
                    tok = create_token(TOK_BIT_NOT, "~");
                    lex->pos++;
                    break;
                    case '[':
                    tok = create_token(TOK_LBRACKET, "[");
                    lex->pos++;
                    break;
                    case ']':
                    tok = create_token(TOK_RBRACKET, "]");
                    lex->pos++;
                    break;
                    case ',':
                    tok = create_token(TOK_COMMA, ",");
                    lex->pos++;
                    break;
                    case ':':
                    tok = create_token(TOK_COLON, ":");
                    lex->pos++;
                    break;
                    case '?':
                    tok = create_token(TOK_QUESTION, "?");
                    lex->pos++;
                    break;
                    case '.':
                    tok = create_token(TOK_DOT, ".");
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
                    case '<':
                        lex->state = STATE_IN_LT;
                        lex->pos++;
                        break;
                    case '>':
                        lex->state = STATE_IN_GT;
                        lex->pos++;
                        break;
                    case '+':
                        tok = create_token(TOK_PLUS, "+");
                        lex->pos++;
                        break;
                    case '-':
                        tok = create_token(TOK_MINUS, "-");
                        lex->pos++;
                        break;
                    case '*':
                        tok = create_token(TOK_MULT, "*");
                        lex->pos++;
                        break;
                    case '/':
                        tok = create_token(TOK_DIV, "/");
                        lex->pos++;
                        break;
                    case '(':
                        tok = create_token(TOK_LPAREN, "(");
                        lex->pos++;
                        break;
                    case ')':
                        tok = create_token(TOK_RPAREN, ")");
                        lex->pos++;
                        break;
                    case '{':
                        tok = create_token(TOK_LBRACE, "{");
                        lex->pos++;
                        break;
                    case '}':
                        tok = create_token(TOK_RBRACE, "}");
                        lex->pos++;
                        break;
                    case ';':
                        tok = create_token(TOK_SEMICOLON, ";");
                        lex->pos++;
                        break;
                    default:
                        tok = create_token(TOK_UNKNOWN, &c);
                        lex->pos++;
                }
                break;

            case STATE_IN_ID:
                if (isalnum(c)) {
                    strncat(lex->buffer, &c, 1);
                    lex->pos++;
                } else {
                    // Dans le case STATE_IN_ID :
                    if (is_keyword(lex->buffer)) {
                        if (strcmp(lex->buffer, "if") == 0) tok = create_token(TOK_IF, lex->buffer);
                        else if (strcmp(lex->buffer, "else") == 0) tok = create_token(TOK_ELSE, lex->buffer);
                        else if (strcmp(lex->buffer, "while") == 0) tok = create_token(TOK_WHILE, lex->buffer);
                        else if (strcmp(lex->buffer, "for") == 0) tok = create_token(TOK_FOR, lex->buffer);
                        else if (strcmp(lex->buffer, "do") == 0) tok = create_token(TOK_DO, lex->buffer);
                        else if (strcmp(lex->buffer, "return") == 0) tok = create_token(TOK_RETURN, lex->buffer);
                        else if (strcmp(lex->buffer, "break") == 0) tok = create_token(TOK_BREAK, lex->buffer);
                        else if (strcmp(lex->buffer, "continue") == 0) tok = create_token(TOK_CONTINUE, lex->buffer);
                        else if (strcmp(lex->buffer, "switch") == 0) tok = create_token(TOK_SWITCH, lex->buffer);
                        else if (strcmp(lex->buffer, "case") == 0) tok = create_token(TOK_CASE, lex->buffer);
                        else if (strcmp(lex->buffer, "default") == 0) tok = create_token(TOK_DEFAULT, lex->buffer);
                        else if (strcmp(lex->buffer, "typedef") == 0) tok = create_token(TOK_TYPEDEF, lex->buffer);
                        else if (strcmp(lex->buffer, "struct") == 0) tok = create_token(TOK_STRUCT, lex->buffer);
                        else if (strcmp(lex->buffer, "enum") == 0) tok = create_token(TOK_ENUM, lex->buffer);
                        else if (strcmp(lex->buffer, "union") == 0) tok = create_token(TOK_UNION, lex->buffer);
                        else if (strcmp(lex->buffer, "const") == 0) tok = create_token(TOK_CONST, lex->buffer);
                        else if (strcmp(lex->buffer, "volatile") == 0) tok = create_token(TOK_VOLATILE, lex->buffer);
                        else if (strcmp(lex->buffer, "sizeof") == 0) tok = create_token(TOK_SIZEOF, lex->buffer);
                        else if (strcmp(lex->buffer, "int") == 0) tok = create_token(TOK_KW_INT, lex->buffer);
                        else if (strcmp(lex->buffer, "float") == 0) tok = create_token(TOK_KW_FLOAT, lex->buffer);
                        else if (strcmp(lex->buffer, "char") == 0) tok = create_token(TOK_KW_CHAR, lex->buffer);
                        else if (strcmp(lex->buffer, "void") == 0) tok = create_token(TOK_KW_VOID, lex->buffer);
                        else if (strcmp(lex->buffer, "short") == 0) tok = create_token(TOK_KW_SHORT, lex->buffer);
                        else if (strcmp(lex->buffer, "long") == 0) tok = create_token(TOK_KW_LONG, lex->buffer);
                        else if (strcmp(lex->buffer, "double") == 0) tok = create_token(TOK_KW_DOUBLE, lex->buffer);
                        else if (strcmp(lex->buffer, "signed") == 0) tok = create_token(TOK_KW_SIGNED, lex->buffer);
                        else if (strcmp(lex->buffer, "unsigned") == 0) tok = create_token(TOK_KW_UNSIGNED, lex->buffer);
                    } else {
                        tok = create_token(TOK_ID, lex->buffer);
                    }
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
                    tok = create_token(TOK_UNKNOWN, "!");
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_LT:
                if (c == '=') {
                    tok = create_token(TOK_LTE, "<=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_LT, "<");
                }
                lex->state = STATE_START;
                break;

            case STATE_IN_GT:
                if (c == '=') {
                    tok = create_token(TOK_GTE, ">=");
                    lex->pos++;
                } else {
                    tok = create_token(TOK_GT, ">");
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