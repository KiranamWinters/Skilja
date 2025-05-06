#ifndef LEXER_H_
#define LEXER_H_

#include <stddef.h>

typedef enum {
        TOKEN_EOF = 0,
        INVALID_TOKEN,
        TOKEN_STRING,
        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_STAR,
        TOKEN_LESS_THAN,
        TOKEN_GREATER_THAN,
        TOKEN_SLASH,
        TOKEN_NOT,
        TOKEN_EQUALS,
        TOKEN_MODULUS,
        TOKEN_COMMENT,
        TOKEN_PREPROCESSOR,
        TOKEN_UNKNOWN,
        TOKEN_SYMBOL,
        TOKEN_SEMICOLON,
        TOKEN_INTEGER,
        TOKEN_BRACKET,
        TOKEN_SQUARE_BRACKET,
        TOKEN_BLOCK_BRACKET,
        TOKEN_OPERATOR,
        TOKEN_LITERAL,
        TOKEN_KEYWORD,
        TOKEN_AMPERSAND,
        TOKEN_EXTENDED,
        TOKEN_DEBUG,
        TOKEN_VARIABLE_TYPE
}       TOKEN_TYPE;



typedef struct {
        const char* str;
        size_t size;
        size_t type;
}       Token;

typedef struct {
        const char* content;
        size_t pointer;
        size_t content_len;
        size_t line_no;
}       Lexer;

Token peek_next(Lexer *l);

const char* token_as_str(int );
Lexer init_lexer(const char *str, size_t str_len);
Token lexer_next(Lexer *l);
void print_token(Token t);


#endif // !LEXER_H_

