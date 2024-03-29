/**
 * Clasp language Lexer test
 * Authored 12/2023
 * 
 * This program is part of the Clasp Test Suite
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

/**
 * Test status:
 *  Passes as of 12/18/2023
*/

#include <clasp/lexer.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    char *data;
    unsigned int idx;
} StringStream;

StringStream str;

char read_string() {
    if (str.idx == strlen(str.data)) return EOF;
    return str.data[str.idx++];
}

int main(int argc, char **argv) {
    str = (StringStream) { "5 * 2; 85/6;; 4; {8 + 3;}; -8 = 4 / 2; 5++; 8 + 6--; x = x + 1;\ny++; x = foo(a, b, x, 25); var test: int = 42; var test2: int; var test3 = 25.0; var err = ; test2 = 5;\n", 0 };
    ClaspLexer *l = malloc(sizeof(ClaspLexer));
    new_lexer(l, read_string, NULL);

    ClaspToken *current;

    current = lexer_next(l);
    unsigned int i = 0;
    while (current->type != TOKEN_EOF &&  current->type != TOKEN_UNKNOWN) {
        token_print(current);
        current = lexer_next(l);
        ++i;
    }

    token_print(current);
    
    return 0;
}