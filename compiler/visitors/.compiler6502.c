#include "../visitor.h"

#include <stdio.h>
#include <stdlib.h>

void visitor_literal_ulint(unsigned long int val) {
    if (val >= 256) {
        fprintf(stderr, "Compile error: constants larger than 255 are not yet supported\n");
        exit(-1);
    }
    printf("lda #%d  ; Integer constant\npha\n\n", val);
}
void visitor_op_unary(TokenTyp operator_type) {
    switch (operator_type) {
        case TOKEN_MINUS: printf("pla  ; Unary negation\nxor #$ff\nclc\nadc #1\npha\n\n", DECD_TOKEN_TYP(operator_type)); break;
        default: {
            fprintf(stderr, "Compile error: Unknown unary operator\n");
            exit(-1);
        }
    }
}
void visitor_op_binary(TokenTyp operator_type) {
    printf("pla  ; Binary operator\nsta $00\npla\n");
    switch (operator_type) {
        case TOKEN_PLUS:  printf("clc\nadc $00\n"); break;
        case TOKEN_MINUS: printf("clc\nsbc $00\n"); break;
        default: {
            fprintf(stderr, "Compile error: unknown binary operator\n");
            exit(-1);
        }
    }
    printf("pha\n\n");
}