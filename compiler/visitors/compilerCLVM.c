#include "../visitor.h"
#include "compiler.h"
#include "../../dynabuf/dynabuf.h"
#include <stdio.h>
#include <stdlib.h>

static DynamicBuffer(byte) *codeSegment; // Up to 4GB of code
static uint32_t codeIdx = 0;

static DynamicBuffer(byte) *dataSegment; // Up to 4GB of data
static uint32_t dataIdx = 0;

void compiler_init() {
    codeSegment = newDynamicBuf(byte);
    initDynamicBuf(byte, codeSegment);

    dataSegment = newDynamicBuf(byte);
    initDynamicBuf(byte, dataSegment);
}

static void emit_byte(byte data) {
    writeDynamicBuf(byte, codeSegment, codeIdx, data); ++codeIdx;
}
static void emit_bytes(byte data1, byte data2) {
    emit_byte(data1);
    emit_byte(data2);
}
static int emit_constant_ulint(unsigned long int val) {  // ulints are 8 bytes
    int out = dataIdx;
    for (int i = 0; i < 8; ++i) {
        writeDynamicBuf(byte, dataSegment, dataIdx, (byte)(val & 0xff));
        ++dataIdx;
        val >>= 8;
    }
    return out;
}

void visitor_literal_ulint(unsigned long int val) {
    int idx = emit_constant_ulint(val);
    // TODO load the constant onto the stack
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