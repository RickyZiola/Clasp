#include "lexer.h"
#include "../dynabuf/dynabuf.h"

// Index of the lexer into the input string
static unsigned int idx = 0;

static Token  current;
static Token prev;

// The file descriptor of the current file being lexed
short fd;
unsigned int lineno = 1;

static char *str;

Token token_current() {
    return current;
}
Token token_next() {
    prev = current;
    Token out = current;
    
    Token next = _parse_token();
    current = next;

    return out;
}
Token token_peek() {
    unsigned int currentIdx = idx;
    Token out = _parse_token();
    idx = currentIdx;
    return out;
}
Token token_previous() {
    return prev;
}

Token synchronize(bool string) {
    char current;
    while (idx < strlen(str)) {
        current = char_next();
        if (current == '\"') string = !string;
        if (!string && (current == ';' || current == '{' || current == '}' || current == '\n'))  // Synchronization chars, these should always mean the start of a new token. TODO: get a better list of sync chars to catch more errors the first time
            return _parse_token();  // This should start on the character after the sync char
    } return (Token) { NULL, 0, fd, lineno, TOKEN_EOF };
}

Token lex_err(bool string, const char *errf, ...) {
    va_list args;
    va_start(args, errf);

    //fprintf(stderr, "Lexical error at line %d of file \"%s\": \"", lineno, readDynamicBuf(char *, loaded_files, fd)); // TODO: sort out the loaded_files buffer
    fprintf(stderr, "Lexical error at line %d of file TODO: \"", lineno);
    vfprintf(stderr, errf, args);
    fprintf(stderr, "\"\n");
    hadErr = true;
    --idx;
    return synchronize(string);
}

bool is_whitespace(char chr) {
    return (chr == ' ') || (chr == '\t');  // TODO add all the whitespace characters
}

bool isnumeric(char chr) {
    return (chr >= '0') && (chr <= '9');
}

Token _parse_token() {  // TODO
    while(1) {
        char next = char_next();
        if (idx > strlen(str)) return (Token) { NULL, 0, fd, lineno, TOKEN_EOF };

            // Keywords
            // 'var', 'let', 'const', 'while', 'if', 'fn',
            //                                       'for'
            // TODO

        // Identifiers
        if (next == '_' || (next >= 'a' && next <= 'z') || (next > 'A' && next < 'Z')) {
            DynamicBuffer(char) *out = newDynamicBuf(char);
            initDynamicBuf(char, out);

            short startIdx = idx;
            char current = next;
            writeDynamicBuf(char, out, idx - startIdx, current);

            current = char_next();


            while (current == '_' || (current >= 'a' && current <= 'z') || (current > 'A' && current < 'Z') || (current > '0' && current < '9')) {
                writeDynamicBuf(char, out, idx - startIdx, current);
                current = char_next();
                if (idx >= strlen(str)) break;
            }
            current = char_next();
            writeDynamicBuf(char, out, idx - startIdx, current);

            char *final = malloc(idx - startIdx);
            memcpy(final, out->data, idx - startIdx);
            freeDynamicBuf(char, out);

            --idx;
            --idx;
            return (Token) { final, idx - startIdx + 1, fd, lineno, TOKEN_IDENTIFIER };
        }
        switch (next) {

            // Newlines
            case '\r': {
                if (char_next() != '\n') return lex_err(false, "Unexpected character \\r without following \\n");
                ++lineno;
                return (Token) { "\r\n", 2, fd, lineno, TOKEN_NEWLINE };
            } break;
            case '\n': {
                ++lineno;
                return (Token) {   "\n", 1, fd, lineno, TOKEN_NEWLINE };
            } break;

            // Punctuation
            case ';': {
                return (Token) { ";", 1, fd, lineno, TOKEN_SEMICOLON };
            } break;
            case ':': {
                return (Token) { ":", 1, fd, lineno, TOKEN_COLON };
            } break;

            case '(': {
                return (Token) { "(", 1, fd, lineno, TOKEN_LEFT_PAREN };
            } break;
            case ')': {
                return (Token) { ")", 1, fd, lineno, TOKEN_RIGHT_PAREN };
            } break;

            case '{': {
                return (Token) { "{", 1, fd, lineno, TOKEN_LEFT_CURLY };
            } break;
            case '}': {
                return (Token) { "}", 1, fd, lineno, TOKEN_RIGHT_CURLY };
            } break;

            // Operators. This is where things get fun...
            // TODO: should be able to compress this significantly with some smarter logic
            case '+': {   // '+', '+=' or '++'
                if (char_current() == '=') { char_next(); return (Token) { "+=", 2, fd, lineno, TOKEN_PLUS_EQ   }; };
                if (char_current() == '+') { char_next(); return (Token) { "++", 2, fd, lineno, TOKEN_PLUS_PLUS }; };
                return (Token) { "+", 1, fd, lineno, TOKEN_PLUS };
            } break;

            case '-': {   // '-', '-=' or '--'
                if (char_current() == '=') { char_next(); return (Token) { "-=", 2, fd, lineno, TOKEN_MINUS_EQ    }; };
                if (char_current() == '-') { char_next(); return (Token) { "--", 2, fd, lineno, TOKEN_MINUS_MINUS }; };
                return (Token) { "-", 1, fd, lineno, TOKEN_MINUS };
            } break;

            case '*': {   // '*' or '*='
                if (char_current() == '=') { char_next(); return (Token) { "*=", 2, fd, lineno, TOKEN_ASTERIX_EQ    }; };
                return (Token) { "*", 1, fd, lineno, TOKEN_ASTERIX };
            } break;

            case '/': {   // '/' or '/='
                if (char_current() == '=') { char_next(); return (Token) { "/=", 2, fd, lineno, TOKEN_SLASH_EQ    }; };
                return (Token) { "/", 1, fd, lineno, TOKEN_SLASH };
            } break;

            case '=': {   // '=' or '=='
                if (char_current() == '=') { char_next(); return (Token) { "==", 2, fd, lineno, TOKEN_EQ_EQ }; };
                return (Token) { "=", 1, fd, lineno, TOKEN_EQ };
            } break;

            case '>': {   // '>' or '>='
                if (char_current() == '=') { char_next(); return (Token) { ">=", 2, fd, lineno, TOKEN_GREATER_EQ }; };
                return (Token) { ">", 1, fd, lineno, TOKEN_GREATER };
            } break;

            case '<': {   // '<' or '<='
                if (char_current() == '=') { char_next(); return (Token) { "<=", 2, fd, lineno, TOKEN_LESS_EQ }; };
                return (Token) { "<", 1, fd, lineno, TOKEN_LESS };
            } break;

            // Number literals
            case  '0':
            case  '1':
            case  '2':
            case  '3':
            case  '4':
            case  '5':
            case  '6':
            case  '7':
            case  '8':
            case  '9': {
                DynamicBuffer(char) *number = newDynamicBuf(char);
                initDynamicBuf(char, number);
                char current = next;
                short startIdx = idx;

                while (isnumeric(current)) {
                    writeDynamicBuf(char, number, idx - startIdx, current);
                    current = char_next();
                } 
                if (current == '.') {
                    writeDynamicBuf(char, number, idx - startIdx, '.');
                    current = char_next();
                    while (isnumeric(current)) {
                        writeDynamicBuf(char, number, idx - startIdx, current);
                        current = char_next();
                    }
                }
                char *final = malloc(idx - startIdx);
                memcpy(final, number->data, idx - startIdx);
                freeDynamicBuf(char, number);

                --idx;
                return (Token) { final, idx - startIdx + 1, fd, lineno, TOKEN_NUM_LITERAL };
                
            } break;

            case '.': {
                DynamicBuffer(char) *number = newDynamicBuf(char);
                initDynamicBuf(char, number);
                short startIdx = idx;
                char current = next;

                writeDynamicBuf(char, number, idx - startIdx, current);
                current = char_next();
                while (isnumeric(current)) {
                    writeDynamicBuf(char, number, idx - startIdx, current);
                    current = char_next();
                }

                char *final = malloc(idx - startIdx);
                memcpy(final, number->data, idx - startIdx);
                freeDynamicBuf(char, number);

                --idx;
                return (Token) { final, idx - startIdx + 1, fd, lineno, TOKEN_NUM_LITERAL };
            } break;

            // String literals
            case '\"': {   // TODO: escape codes
                DynamicBuffer(char) *out = newDynamicBuf(char);
                initDynamicBuf(char, out);

                short startIdx = idx + 1;
                char current = char_next();

                while (current != '\"') {
                    writeDynamicBuf(char, out, idx - startIdx, current);
                    current = char_next();
                    if (idx >= strlen(str)) return lex_err(true, "Unexpected EOF whilst parsing string literal");
                }
                current = char_next();

                char *final = malloc(idx - startIdx);
                memcpy(final, out->data, idx - startIdx);
                freeDynamicBuf(char, out);

                --idx;
                return (Token) { final, idx - startIdx, fd, lineno, TOKEN_STR_LITERAL };

            } break;

            // Character literals
            case '\'': {   // TODO: escape codes
                char *chr = malloc(1);
                chr[0] = char_next();
                if (char_next() != '\'') return lex_err(false, "Expected single quote after character literal %c", chr[0]);
                return (Token) { chr, 1, fd, lineno, TOKEN_CHR_LITERAL };
            } break;

            default: {
                if (is_whitespace(next)) continue;
                return lex_err(false, "Unexpected character \'%c\' matches no lexical rule", next);
            }
        };
    }
}

char char_next() {
    char out = str[idx];
    if (idx <= strlen(str)) ++idx;
    return out;
}

char char_current() {
    return str[idx];
}

char char_peek() {
    unsigned int newIdx = idx;
    if (newIdx <= strlen(str)) newIdx++;
    return str[newIdx];
}

void load_string(char *string) {
    str = string;
    current = _parse_token();
}