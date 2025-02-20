#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    /*-------------------------------------
    * Mots-clés standards
    *-------------------------------------*/
    TOK_IF,          // if
    TOK_ELSE,        // else
    TOK_WHILE,       // while
    TOK_FOR,         // for
    TOK_RETURN,      // return
    TOK_BREAK,       // break
    TOK_CONTINUE,    // continue
    TOK_SWITCH,      // switch
    TOK_CASE,        // case
    TOK_DEFAULT,     // default
    TOK_TYPEDEF,     // typedef
    TOK_STRUCT,      // struct
    TOK_ENUM,        // enum
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

    // Affectation
    TOK_ASSIGN,      // =
    TOK_ADD_ASSIGN,  // +=
    TOK_SUB_ASSIGN,  // -=

    /*-------------------------------------
    * Délimiteurs et ponctuation
    *-------------------------------------*/
    TOK_LPAREN,      // (
    TOK_RPAREN,      // )
    TOK_LBRACE,      // {
    TOK_RBRACE,      // }
    TOK_COMMA,       // ,
    TOK_SEMICOLON,   // ;

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
    TOK_UNKNOWN,      // token inconnu

    TOK_BIT_NOT,     // ~
    TOK_BIT_AND,     // &
    TOK_DOT,         // .
    TOK_ARROW,       // ->
    TOK_ARROW_STAR,  // ->*
    TOK_STAR,        // *
    TOK_BIT_OR,      // |
    TOK_BIT_XOR,     // ^
    TOK_LSHIFT,      // <<
    TOK_RSHIFT,      // >>
    TOK_COLON,       // :
    TOK_QUESTION,    // ?
    TOK_HASH,        // #
    TOK_LBRACKET,    // [
    TOK_RBRACKET,    // ]
    TOK_MUL_ASSIGN,  // *=
    TOK_DIV_ASSIGN,  // /=
    TOK_MOD_ASSIGN,  // %=
    TOK_LSHIFT_ASSIGN, // <<=
    TOK_RSHIFT_ASSIGN, // >>=
    TOK_AND_ASSIGN,  // &=
    TOK_OR_ASSIGN,   // |=
    TOK_XOR_ASSIGN,  // ^=
    TOK_OR,          // ||
    TOK_AND,         // &&
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
} Token;

#endif