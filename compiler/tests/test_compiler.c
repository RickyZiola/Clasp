#include "../lexer.h"
#include "../parser.h"
#include "../visitors/compiler.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
    compiler_init();

    load_string("32 - 28 + -5");
    expression();
    printf("\n");
    return 0;
}