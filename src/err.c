/**
 * Clasp error message implementations
 * Authored 12/18/2023-present
 * 
 * This program is part of the Clasp Source Libraries
 * 
 * Copyright (c) 2023, Frederick Ziola
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

#include <clasp/err.h>
#include <stdarg.h>
#include <stdio.h>

void general_err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    return;
}

// TODO: file/line numbers
void token_err(ClaspToken *tok, char *err) {
    fprintf(stderr, "Syntax error on token '%s': \"%s\"", tok->data, err);
}