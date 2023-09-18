#include "parser.h"

void parse_err(Token tok, const char *errf, ...) {
    hadErr    = true;
    panicMode = true;
    va_list args;
    va_start(args, errf);

    fprintf(stderr, "Parse error at %d of file TODO: \"", tok.line);
    vfprintf(stderr, errf, args);
    fprintf(stderr, "\"\n");
    parser_synchronize();
}

void parser_synchronize() {
    panicMode = false;
    return; // TODO
}

void consume(TokenTyp typ, const char *errf, ...) {
    va_list args;
    va_start(args, errf);

    if (token_current().typ != typ) parse_err(token_current(), errf, args);
    advance();
}

void advance() {
    token_next();
}

void parse_precedence(OperatorPrecedence precedence) {
    advance();
    ParseFn prefixRule = get_rule(token_previous().typ)->prefix;
    if (prefixRule == NULL) {
            parse_err(token_current(), "Expected expression.");
        return;
    }
    prefixRule();


    while (precedence <= get_rule(token_current().typ)->precedence) {
      advance();
      ParseFn infixRule = get_rule(token_previous().typ)->infix;
      infixRule();
    }
}


void expression() {
    parse_precedence(PREC_ASSIGNMENT);
}

void number() {
    Token num = token_previous(); // TODO: floating-points
    char *str = malloc(num.length + 1);
    memcpy(str, num.str, num.length);
    str[num.length] = '\0';
    if (strchr(str, '.') !=  NULL) parse_err(num, "Floating-points are not yet supported");
    unsigned long int value = strtol(str, NULL, 10);
    visitor_literal_ulint(value);
}
void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression");
}
void unary() {
    TokenTyp typ = token_previous().typ;
    parse_precedence(PREC_UNARY); // Operand, assume the result is on the stack.
    visitor_op_unary(typ);
}
void binary() {
    TokenTyp typ = token_previous().typ;
    ParseRule *rule = get_rule(typ);
    parse_precedence((OperatorPrecedence) (rule->precedence + 1)); // Operand, assume the result is on the stack

    visitor_op_binary(typ);
}