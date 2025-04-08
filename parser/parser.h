#ifndef PARSER_H_
#define PARSER_H_

#include <stdint.h>
#include <string.h>
#include "../Lexer/lexer.h"

typedef struct {
        const char* name;
        size_t name_size;
        int value;
}       VarDeclStatement;

typedef struct{
        Lexer *lexer;
        Token current_token;
        Token next_token;
}       Parser;


void parser_next(Parser *p);
#endif
