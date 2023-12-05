#include <clasp/lexer.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void new_lexer(ClaspLexer *lexer, StreamReadFn fn) {
    lexer->stream = fn;
    lexer->current  = NULL;
    lexer->next     = NULL;
    lexer->previous = NULL;
    lexer->cCurrent = lexer->stream();
    (void) lexer_next(lexer);

    return;
}

ClaspToken *lexer_next(ClaspLexer *lexer) {
    if (lexer->current == NULL) {  // initialize
        lexer->current = lexer_scan(lexer);
        lexer->next    = lexer_scan(lexer);
        
    } else {
        if (lexer->previous) free(lexer->previous->data);
        free(lexer->previous);
        lexer->previous = lexer->current;
        lexer->current  = lexer->next;
        lexer->next     = lexer_scan(lexer);
    }
    return lexer->previous;
}

static bool is_identifier(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static ClaspToken *new_token(char *data, ClaspTokenType type) {
    ClaspToken *out = malloc(sizeof(ClaspToken));
    out->data = data;
    out->type = type;
    return out;
}
static ClaspToken *new_token_const(const char *const data, ClaspTokenType type) {
    char *buf = malloc(strlen(data) + 1);
    strcpy(buf, data);
    return new_token(buf, type);
}

ClaspToken *lexer_scan(ClaspLexer *lexer) {
    char current = lexer->cCurrent;
    if (current == EOF) return new_token("", TOKEN_EOF);
    while (isspace(current)) current = lexer->stream();

        // Identifiers
    if (is_identifier(current)) {
        char *identifier = malloc(128);
        unsigned int length = 1;
        identifier[0] = current;

        current = lexer->stream();
        while ((is_identifier(current) || isdigit(current))) {
            if (length < 128) identifier[length] = current;
            ++length;
            current = lexer->stream();
        }
        char *final = malloc(length + 1);
        memcpy(final, identifier, length);
        final[length] = '\0';
        free(identifier);
        lexer->cCurrent = current;

            // Keywords
        if (!strcmp(final, "if"   )) return new_token(final, TOKEN_KW_IF   );
        if (!strcmp(final, "while")) return new_token(final, TOKEN_KW_WHILE);
        if (!strcmp(final, "for"  )) return new_token(final, TOKEN_KW_FOR  );
        if (!strcmp(final, "fn"   )) return new_token(final, TOKEN_KW_FN   );
        return new_token(final, TOKEN_ID);
    }
        // Number literals
    if (isdigit(current) || current == '.') {
        char *num = malloc(128);
        unsigned int length = 1;
        num[0] = current;

        int decimal_count = (current == '.');

        while (decimal_count < 2 && (isdigit(current = lexer->stream()) || current == '.') && length < 128)  {
            decimal_count += (current == '.');
            num[length++] = current;
        }

        char *final = malloc(length + 1);
        memcpy(final, num, length);
        final[length] = '\0';
        free(num);
        lexer->cCurrent = current;
        return new_token(final, TOKEN_NUMBER);
    }

    if (current == '+') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("+=", TOKEN_PLUS_EQ);
        }
        return new_token_const("+", TOKEN_PLUS);
    }
    if (current == '-') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("-=", TOKEN_MINUS_EQ);
        }
        if (lexer->cCurrent == '>') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("->", TOKEN_RIGHT_POINT);
        }
        return new_token_const("-", TOKEN_MINUS);
    }
    if (current == '*') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("*=", TOKEN_ASTERIX_EQ);
        }
        return new_token_const("*", TOKEN_ASTERIX);
    }
    if (current == '/') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("/=", TOKEN_SLASH_EQ);
        }
        return new_token_const("/", TOKEN_SLASH);
    }
    if (current == '=') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("==", TOKEN_EQ_EQ);
        }
        return new_token_const("=", TOKEN_EQ);
    }
    if (current == '!') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("!=", TOKEN_BANG_EQ);
        }
        return new_token_const("!", TOKEN_BANG);
    }
    if (current == '<') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("<=", TOKEN_LESS_EQ);
        }
        if (lexer->cCurrent == '-') {
            lexer->cCurrent = lexer->stream();
            return new_token_const("<-", TOKEN_LEFT_POINT);
        }
        return new_token_const("<", TOKEN_LESS);
    }
    if (current == '>') {
        lexer->cCurrent = lexer->stream();
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer->stream();
            return new_token_const(">=", TOKEN_GREATER_EQ);
        }
        return new_token_const(">", TOKEN_GREATER);
    }

    if (current == '(') {
        lexer->cCurrent = lexer->stream();
        return new_token_const("(", TOKEN_LEFT_PAREN);
    }
    if (current == ')') {
        lexer->cCurrent = lexer->stream();
        return new_token_const(")", TOKEN_RIGHT_PAREN);
    }

    if (current == '[') {
        lexer->cCurrent = lexer->stream();
        return new_token_const("[", TOKEN_LEFT_SQUARE);
    }
    if (current == ']') {
        lexer->cCurrent = lexer->stream();
        return new_token_const("]", TOKEN_RIGHT_SQUARE);
    }

    if (current == '{') {
        lexer->cCurrent = lexer->stream();
        return new_token_const("{", TOKEN_LEFT_CURLY);
    }
    if (current == '}') {
        lexer->cCurrent = lexer->stream();
        return new_token_const("}", TOKEN_RIGHT_CURLY);
    }

    return new_token_const("", TOKEN_UNKNOWN);
}

#define CASE(typ) case (typ): return (#typ);
const char *tktyp_str(ClaspTokenType typ) {
    switch (typ) {
        CASE(TOKEN_ID)
        CASE(TOKEN_NUMBER)
        CASE(TOKEN_KW_IF)
        CASE(TOKEN_KW_WHILE)
        CASE(TOKEN_KW_FOR)
        CASE(TOKEN_KW_FN)
        CASE(TOKEN_PLUS)
        CASE(TOKEN_MINUS)
        CASE(TOKEN_ASTERIX)
        CASE(TOKEN_SLASH)
        CASE(TOKEN_EQ_EQ)
        CASE(TOKEN_BANG)
        CASE(TOKEN_BANG_EQ)
        CASE(TOKEN_LESS)
        CASE(TOKEN_LESS_EQ)
        CASE(TOKEN_GREATER)
        CASE(TOKEN_GREATER_EQ)
        CASE(TOKEN_EQ)
        CASE(TOKEN_PLUS_EQ)
        CASE(TOKEN_MINUS_EQ)
        CASE(TOKEN_ASTERIX_EQ)
        CASE(TOKEN_SLASH_EQ)
        CASE(TOKEN_LEFT_PAREN)
        CASE(TOKEN_RIGHT_PAREN)
        CASE(TOKEN_LEFT_SQUARE)
        CASE(TOKEN_RIGHT_SQUARE)
        CASE(TOKEN_LEFT_CURLY)
        CASE(TOKEN_RIGHT_CURLY)
        CASE(TOKEN_COLON)
        CASE(TOKEN_RIGHT_POINT)
        CASE(TOKEN_LEFT_POINT)
        CASE(TOKEN_EOF)
        CASE(TOKEN_UNKNOWN)
        default: return "unknown";
    }
}
#undef CASE

void token_print(ClaspToken *token) {
    printf("Token(%s) { %s }\n", tktyp_str(token->type), token->data);
}