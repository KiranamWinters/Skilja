#ifndef PARSER_H_
#define PARSER_H_

#include <stdint.h>
#include <string.h>
#include "../Lexer/lexer.h"

typedef enum {
        LOWEST, 
        EQUALS,         // = or !
        LESS_GREATER,   // < or >
        SUM,            // + 
        PRODUCT,        // *
        PREFIX,         // - or !
}       OPERATOR_PRECEDENCE;

typedef enum{
        STMT_INVALID = 0,
        STMT_VARIBALE_DECLARATION, 
        STMT_IF_STATEMENT
}       STMT_TYPE;
typedef enum{
        EXPR_INTEGER,
        EXPR_PREFIX,
        EXPR_INFIX,
}       EXPR_TYPE;

typedef struct Expression {
        Token token;
        EXPR_TYPE type; 
        union{
               long int_value;
               struct {
                        char prfx_operator;
                        struct Expression *right;
               } prefix;
               struct {
                        char infx_operator;
                        struct Expression *right;
                        struct Expression *left;
               } infix;
        }; 
} Expression ;  

typedef struct expr{
        char op;
        int left;
        struct expr *right;
} expr;

typedef struct {
        const char* name;
        Token token;
        size_t name_size;
        int value;
}       VarDeclStatement;

typedef struct{
        Lexer *lexer;
        Token current_token;
        Token next_token;
}       Parser;

#define MAX_FUNCTIONS 128
typedef Expression* (*prefixParseFuncs) (Parser *p);
typedef Expression* (*infixParseFuncs)  (Parser *p, Expression* left);

struct Statement{
        VarDeclStatement                variableStatement;
        //ConditionalStatement          ifStatement / switch(TODO);
        //LoopStatement                 for-each / while / for(decl;cond;increment)
        //FunctionCall(TODO)
        struct Statement *next;
        int Statement_type;
        const char* stmt;
        size_t stmt_size;
};
typedef struct Statement Statement;

void parser_next(Parser *p);
#endif
