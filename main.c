#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "utils.h"

int main() {
    const char* filename = "text.loom";
    char* input = read_file(filename);
    if (!input) return 1;

    Lexer* lexer = create_lexer(input);
    Token* tok;
    while ((tok = next_token(lexer))->type != TOK_END) {
        printf("Token: %d, Value: %s\n", tok->type, tok->value);
    }

    free(input);
    free(lexer);
    return 0;
}