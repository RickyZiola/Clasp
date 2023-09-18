#ifndef LEXER_H
#define LEXER_H

#include <stdio.h> 
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// TODO:
// Keywords
// Concatenation operator (//)?
// Exponent operator (**)?
// Boolean logic (&, |, &&, ||, ^)

// Enum to represent types of tokens
typedef enum {
    TOKEN_EOF, TOKEN_NEWLINE,      // Special delimeter tokens

    TOKEN_SEMICOLON, TOKEN_LEFT_PAREN,    // Punctuation, TODO
    TOKEN_RIGHT_PAREN, TOKEN_LEFT_CURLY,
    TOKEN_RIGHT_CURLY, TOKEN_COLON,

    TOKEN_PLUS, TOKEN_MINUS,    // Operators, TODO
    TOKEN_ASTERIX, TOKEN_SLASH,
    TOKEN_EQ_EQ, TOKEN_BANG_EQ,
    TOKEN_LESS_EQ, TOKEN_GREATER_EQ,
    TOKEN_LESS, TOKEN_GREATER,
    TOKEN_BANG, TOKEN_PLUS_PLUS,
    TOKEN_MINUS_MINUS, TOKEN_PLUS_EQ,
    TOKEN_MINUS_EQ, TOKEN_ASTERIX_EQ,
    TOKEN_SLASH_EQ, TOKEN_EQ,

    TOKEN_NUM_LITERAL, TOKEN_STR_LITERAL,  // Literals and identifiers
    TOKEN_CHR_LITERAL, TOKEN_IDENTIFIER,
    
    TOKEN_KEYWORD,                        // Well, keywords...

    TOKEN_MISC               // Should (hopefully) never show up
} TokenTyp;


// Stringify a TokenTyp
#define DECD_TOKEN_TYP(typ) ({                                  \
    char *out;                                                  \
    switch (typ) {                                              \
        case TOKEN_EOF:         out = "TOKEN_EOF";         break; \
        case TOKEN_NEWLINE:     out = "TOKEN_NEWLINE";     break; \
        case TOKEN_SEMICOLON:   out = "TOKEN_SEMICOLON";   break; \
        case TOKEN_LEFT_PAREN:  out = "TOKEN_LEFT_PAREN";  break; \
        case TOKEN_RIGHT_PAREN: out = "TOKEN_RIGHT_PAREN"; break; \
        case TOKEN_LEFT_CURLY:  out = "TOKEN_LEFT_CURLY";  break; \
        case TOKEN_RIGHT_CURLY: out = "TOKEN_RIGHT_CURLY"; break; \
        case TOKEN_COLON:       out = "TOKEN_COLON";       break; \
        case TOKEN_PLUS:        out = "TOKEN_PLUS";        break; \
        case TOKEN_MINUS:       out = "TOKEN_MINUS";       break; \
        case TOKEN_ASTERIX:     out = "TOKEN_ASTERIX";     break; \
        case TOKEN_SLASH:       out = "TOKEN_SLASH";       break; \
        case TOKEN_EQ_EQ:       out = "TOKEN_EQ_EQ";       break; \
        case TOKEN_BANG_EQ:     out = "TOKEN_BANG_EQ";     break; \
        case TOKEN_LESS_EQ:     out = "TOKEN_LESS_EQ";     break; \
        case TOKEN_GREATER_EQ:  out = "TOKEN_GREATER_EQ";  break; \
        case TOKEN_LESS:        out = "TOKEN_LESS";        break; \
        case TOKEN_GREATER:     out = "TOKEN_GREATER";     break; \
        case TOKEN_BANG:        out = "TOKEN_BANG";        break; \
        case TOKEN_PLUS_PLUS:   out = "TOKEN_PLUS_PLUS";   break; \
        case TOKEN_MINUS_MINUS: out = "TOKEN_MINUS_MINUS"; break; \
        case TOKEN_PLUS_EQ:     out = "TOKEN_PLUS_EQ";     break; \
        case TOKEN_MINUS_EQ:    out = "TOKEN_MINUS_EQ";    break; \
        case TOKEN_ASTERIX_EQ:  out = "TOKEN_ASTERIX_EQ";  break; \
        case TOKEN_SLASH_EQ:    out = "TOKEN_SLASH_EQ";    break; \
        case TOKEN_EQ:          out = "TOKEN_EQ";          break; \
        case TOKEN_NUM_LITERAL: out = "TOKEN_NUM_LITERAL"; break; \
        case TOKEN_STR_LITERAL: out = "TOKEN_STR_LITERAL"; break; \
        case TOKEN_CHR_LITERAL: out = "TOKEN_CHR_LITERAL"; break; \
        case TOKEN_IDENTIFIER:  out = "TOKEN_IDENTIFIER";  break; \
        case TOKEN_KEYWORD:     out = "TOKEN_KEYWORD";     break; \
        \
        case TOKEN_MISC:        out = "TOKEN_MISC";        break; \
        default:                out = "TOKEN_UNKNOWN";     \
    }; out; })


// Struct to represnt a token / lexeme
typedef struct {
    char *str;     // THIS IS NOT AN NTS/ZTS! This is a pointer acting with length below as a string.
    short length;  // Hope we don't have a 65537-character token lmao

    short fd;          // File descriptor (index into files[] array where the token is located), used for errors
    unsigned int line; // Line # of this token in files[fd]

    TokenTyp typ;      // Type of this token
} Token;

// These functions give access to the lexer's stream of tokens:
//   \/ current
//  |var| |example| |:| |int| |=| |42| |;| // Token stream
//   var   example   :   int   =   42   ;  // Character stream (expanded)
//     idx ^

// current() would return 'var',
// peek() would return 'example',
// next() would return 'var', 
// then current() would reurn 'example',
// previous() would return 'var'
Token token_next();
Token token_current();
Token token_peek();
Token token_previous();

static bool hadErr = false;

Token lex_err(bool string, const char *errf, ...);
static Token synchronize(bool string);

// Parse the next token from the input string
static Token _parse_token();

static char char_next();
static char char_current();
static char char_peek();

// Load a string into `str`
void load_string(char *string);

#endif