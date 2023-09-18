#include "../lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PRINT_BYTES(byteString, length) \
    do { \
        for (size_t i = 0; i < length; ++i) { \
            unsigned char c = (byteString)[i]; \
            if (c == '\n') { \
                putchar('\\'); \
                putchar('n'); \
            } else if (c == '\r') { \
                putchar('\\'); \
                putchar('r'); \
            } else if (c == '\t') { \
                putchar('\\'); \
                putchar('t'); \
            } else if (c == '\\') { \
                putchar('\\'); \
                putchar('\\'); \
            } else if (c == '\"') { \
                putchar('\\'); \
                putchar('\"'); \
            } else if (c >= 32 && c <= 126) { \
                putchar(c); \
            } else if (c >= '0' && c <= '9') { \
                putchar(c); \
            } else { \
                printf("\\x%02X", c); \
            } \
        } \
    } while (0)

void print_token(Token tok) {
    // Add %s before the '|' to print token type
    printf(" |", DECD_TOKEN_TYP(tok.typ)+6); // strip the TOKEN_
    PRINT_BYTES(tok.str, tok.length);
    printf("| ");
}

void print_tokens() {
    Token tok;
    while ((tok = token_next()).typ != TOKEN_EOF) {
        print_token(tok);
    }
    print_token(tok);  // always an EOF but...
    printf("\n");
}

int main(int argc, char *argv[]) {
    load_string("var test: int = 12.34;\r\nprint(\"wow\");\ntest = 21;");
    print_tokens();

    return 0;
}