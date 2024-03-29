/**
 * Clasp Lexical Analizer Implementation
 * Authored 12/2023-present
 * 
 * This program is part of the Clasp Source Libraries
 * 
 * Copyright (c) 2024, Frederick Ziola
 *                      frederick.ziola@gmail.com
 * 
 * SPDX-License-Identifier: GPL-3.0
 * 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <clasp/lexer.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <cvector/cvector.h>

static char lexer_read(ClaspLexer *l);

void new_lexer(ClaspLexer *lexer, StreamReadFn fn, void *args) {
    lexer->stream = fn;
    lexer->current  = NULL;
    lexer->next     = NULL;
    lexer->previous = NULL;
    lexer->_stream_args = args;
    (void) lexer_read(lexer);

    lexer->lineno = 0;
    lexer->col_idx = 0;

    lexer->lines = NULL;
    lexer->current_line = NULL;

    (void) lexer_next(lexer);

    return;
}

ClaspToken *lexer_next(ClaspLexer *lexer) {
    if (lexer->current == NULL) {  // initialize
        lexer->current = lexer_scan(lexer);
        lexer->next    = lexer_scan(lexer);
        
    } else {
        //if (lexer->previous) free(lexer->previous->data);
        //free(lexer->previous);
        lexer->previous = lexer->current;
        lexer->current  = lexer->next;
        lexer->next     = lexer_scan(lexer);
    }
    //if (lexer->previous) token_print(lexer->previous);
    return lexer->previous;
}

static bool is_identifier(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static ClaspToken *new_token(ClaspLexer *l, char *data, ClaspTokenType type) {
    ClaspToken *out = malloc(sizeof(ClaspToken));
    out->data = data;
    out->type = type;

    char *line = malloc(cvector_size(l->current_line) + 1);
    memcpy(line, l->current_line, cvector_size(l->current_line));
    line[cvector_size(l->current_line)] = '\0';
    out->line = line;
    out->where = l->col_idx;
    out->lineno = l->lineno;
    return out;
}
static ClaspToken *new_token_const(ClaspLexer *l, const char *const data, ClaspTokenType type) {
    char *buf = malloc(strlen(data) + 1);
    strcpy(buf, data);
    return new_token(l, buf, type);
}

static char lexer_read(ClaspLexer *l) {
    cvector_push_back(l->current_line, l->cCurrent);
    l->col_idx++;
    l->cCurrent = l->stream(l->_stream_args);
}

ClaspToken *lexer_scan(ClaspLexer *lexer) {
    char current = lexer->cCurrent;
    if (current == EOF) return new_token_const(lexer, "\xff", TOKEN_EOF);
    while (isspace(current)) {
        if (current == '\n') {
            cvector_push_back(lexer->lines, lexer->current_line);
            lexer->current_line = NULL;
            lexer->lineno++;
            lexer->col_idx = 0;
        }

        current = lexer_read(lexer);
    }
    if (current == EOF) return new_token_const(lexer, "\xff", TOKEN_EOF);

        // Identifiers
    if (is_identifier(current)) {
        char *identifier = malloc(128);
        unsigned int length = 1;
        identifier[0] = current;

        current = lexer_read(lexer);
        while ((is_identifier(current) || isdigit(current))) {
            if (length < 128) identifier[length] = current;
            ++length;
            current = lexer_read(lexer);
        }
        char *final = malloc(length + 1);
        memcpy(final, identifier, length);
        final[length] = '\0';
        free(identifier);
        lexer->cCurrent = current;

            // Keywords
        if (!strcmp(final, "return")) return new_token(lexer, final, TOKEN_KW_RETURN);
        if (!strcmp(final, "if"    )) return new_token(lexer, final, TOKEN_KW_IF    );
        if (!strcmp(final, "while" )) return new_token(lexer, final, TOKEN_KW_WHILE );
        if (!strcmp(final, "for"   )) return new_token(lexer, final, TOKEN_KW_FOR   );
        if (!strcmp(final, "fn"    )) return new_token(lexer, final, TOKEN_KW_FN    );
        if (!strcmp(final, "var"   )) return new_token(lexer, final, TOKEN_KW_VAR   );
        if (!strcmp(final, "let"   )) return new_token(lexer, final, TOKEN_KW_LET   );
        if (!strcmp(final, "const" )) return new_token(lexer, final, TOKEN_KW_CONST );
        return new_token(lexer, final, TOKEN_ID);
    }
        // Number literals
    if (isdigit(current) || current == '.') {
        char *num = malloc(128);
        unsigned int length = 1;
        num[0] = current;

        int decimal_count = (current == '.');

        while (decimal_count < 2 && (isdigit(current = lexer_read(lexer)) || current == '.') && length < 128)  {
            decimal_count += (current == '.');
            num[length++] = current;
        }

        char *final = malloc(length + 1);
        memcpy(final, num, length);
        final[length] = '\0';
        free(num);
        lexer->cCurrent = current;
        return new_token(lexer, final, TOKEN_NUMBER);
    }

    if (current == '+') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "+=", TOKEN_PLUS_EQ);
        }
        if (lexer->cCurrent == '+') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "++", TOKEN_PLUS_PLUS);
        }
        return new_token_const(lexer, "+", TOKEN_PLUS);
    }
    if (current == '-') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "-=", TOKEN_MINUS_EQ);
        }
        if (lexer->cCurrent == '>') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "->", TOKEN_RIGHT_POINT);
        }
        if (lexer->cCurrent == '-') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "--", TOKEN_MINUS_MINUS);
        }
        return new_token_const(lexer, "-", TOKEN_MINUS);
    }
    if (current == '*') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "*=", TOKEN_ASTERIX_EQ);
        }
        return new_token_const(lexer, "*", TOKEN_ASTERIX);
    }
    if (current == '/') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "/=", TOKEN_SLASH_EQ);
        }
        return new_token_const(lexer, "/", TOKEN_SLASH);
    }
    if (current == '%') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "%=", TOKEN_PERC_EQ);
        }
        return new_token_const(lexer, "%", TOKEN_PERC);
    }
    if (current == '^') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "^=", TOKEN_CARAT_EQ);
        }
        return new_token_const(lexer, "^", TOKEN_CARAT);
    }
    if (current == '=') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "==", TOKEN_EQ_EQ);
        }
        return new_token_const(lexer, "=", TOKEN_EQ);
    }
    if (current == '!') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "!=", TOKEN_BANG_EQ);
        }
        return new_token_const(lexer, "!", TOKEN_BANG);
    }
    if (current == '~') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "~=", TOKEN_TILDE_EQ);
        }
        return new_token_const(lexer, "~", TOKEN_TILDE);
    }
    if (current == '<') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "<=", TOKEN_LESS_EQ);
        }
        if (lexer->cCurrent == '-') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, "<-", TOKEN_LEFT_POINT);
        }
        return new_token_const(lexer, "<", TOKEN_LESS);
    }
    if (current == '>') {
        lexer->cCurrent = lexer_read(lexer);
        if (lexer->cCurrent == '=') {
            lexer->cCurrent = lexer_read(lexer);
            return new_token_const(lexer, ">=", TOKEN_GREATER_EQ);
        }
        return new_token_const(lexer, ">", TOKEN_GREATER);
    }

    if (current == '(') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, "(", TOKEN_LEFT_PAREN);
    }
    if (current == ')') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, ")", TOKEN_RIGHT_PAREN);
    }

    if (current == '[') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, "[", TOKEN_LEFT_SQUARE);
    }
    if (current == ']') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, "]", TOKEN_RIGHT_SQUARE);
    }

    if (current == '{') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, "{", TOKEN_LEFT_CURLY);
    }
    if (current == '}') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, "}", TOKEN_RIGHT_CURLY);
    }

    if (current == ',') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, ",", TOKEN_COMMA);
    }
    if (current == ';') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, ";", TOKEN_SEMICOLON);
    }
    if (current == ':') {
        lexer->cCurrent = lexer_read(lexer);
        return new_token_const(lexer, ":", TOKEN_COLON);
    }

    fprintf(stderr, "Syntax error on character '%c': \"Unexpected character '%c' (0x%2x).\"\n", current, current, current & 0xff);

    return new_token_const(lexer, "", TOKEN_UNKNOWN);
}
int lexer_has(ClaspLexer *l, ClaspTokenType t) {
    return l->current->type == t;
}

// oh how i wish there was a better way to do this
#define CASE(typ) case (typ): return (#typ);
const char *tktyp_str(ClaspTokenType typ) {
    switch (typ) {
        CASE(TOKEN_ID)
        CASE(TOKEN_NUMBER)
        CASE(TOKEN_KW_RETURN)
        CASE(TOKEN_KW_IF)
        CASE(TOKEN_KW_WHILE)
        CASE(TOKEN_KW_FOR)
        CASE(TOKEN_KW_FN)
        CASE(TOKEN_KW_VAR)
        CASE(TOKEN_KW_LET)
        CASE(TOKEN_KW_CONST)
        CASE(TOKEN_PLUS)
        CASE(TOKEN_MINUS)
        CASE(TOKEN_ASTERIX)
        CASE(TOKEN_SLASH)
        CASE(TOKEN_PERC)
        CASE(TOKEN_CARAT)
        CASE(TOKEN_EQ_EQ)
        CASE(TOKEN_PLUS_PLUS)
        CASE(TOKEN_MINUS_MINUS)
        CASE(TOKEN_BANG)
        CASE(TOKEN_BANG_EQ)
        CASE(TOKEN_TILDE)
        CASE(TOKEN_TILDE_EQ)
        CASE(TOKEN_LESS)
        CASE(TOKEN_LESS_EQ)
        CASE(TOKEN_GREATER)
        CASE(TOKEN_GREATER_EQ)
        CASE(TOKEN_EQ)
        CASE(TOKEN_PLUS_EQ)
        CASE(TOKEN_MINUS_EQ)
        CASE(TOKEN_ASTERIX_EQ)
        CASE(TOKEN_SLASH_EQ)
        CASE(TOKEN_PERC_EQ)
        CASE(TOKEN_CARAT_EQ)
        CASE(TOKEN_LEFT_PAREN)
        CASE(TOKEN_RIGHT_PAREN)
        CASE(TOKEN_LEFT_SQUARE)
        CASE(TOKEN_RIGHT_SQUARE)
        CASE(TOKEN_LEFT_CURLY)
        CASE(TOKEN_RIGHT_CURLY)
        CASE(TOKEN_COLON)
        CASE(TOKEN_RIGHT_POINT)
        CASE(TOKEN_LEFT_POINT)
        CASE(TOKEN_COMMA)
        CASE(TOKEN_SEMICOLON)
        CASE(TOKEN_EOF)
        CASE(TOKEN_UNKNOWN)
        default: return "unknown";
    }
}
#undef CASE

void token_print(ClaspToken *token) {
    printf("Token(%s) { %s }\n", tktyp_str(token->type), token->data);
}