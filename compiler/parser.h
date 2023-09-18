#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "visitor.h"

static bool panicMode   = false;
static bool hadParseErr = false;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} OperatorPrecedence;

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;

    OperatorPrecedence precedence;
} ParseRule;

void expression();
void number();
void unary();
void binary();
void grouping();

static ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_CURLY]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_CURLY]   = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_ASTERIX]       = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQ]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQ]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQ_EQ]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQ]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQ]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STR_LITERAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUM_LITERAL]   = {number,   NULL,   PREC_NONE},
  // [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  // [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

void parse_err(Token tok, const char *errf, ...);

void consume(TokenTyp typ, const char *errf, ...);
void parser_synchronize(); // TODO

void advance();

void parse_precedence(OperatorPrecedence precedence);

static ParseRule* get_rule(TokenTyp typ) {
  return &rules[typ];
}

#endif