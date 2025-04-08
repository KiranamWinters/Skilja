#include "../Lexer/lexer.h"
#include "../file_handler/file_handler.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include <stdbool.h>

Parser init_parser(Lexer *l){
        Parser p = {0};
        p.lexer = l;
        
        //Call parser_next twice so current_token and next_token are both set
        parser_next(&p);
        parser_next(&p);
        return p;
}

void parser_next(Parser *p){
        p->current_token = p->next_token;
        p->next_token = lexer_next(p->lexer);
}

//Nexts only if the next token is expected token
#define ERROR_TOKEN_MISMATCH 1
bool parser_expected_next(Parser *p, size_t expected_token_type){
        if(p->next_token.type != expected_token_type){
                printf("ERROR: Token does not match\nExpected Type: %s\nToken recieved: %s -> %s\n"
                                ,token_as_str(expected_token_type)
                                ,token_as_str(p->next_token.type)
                                ,p->next_token.str);
                exit(ERROR_TOKEN_MISMATCH);
                return false;
        }
        parser_next(p);
        return true;
}

int token_to_num(Token t){
        char temp[t.size+1];
        memcpy(temp,t.str,t.size);
        temp[t.size] = '\0';
        int result = atoi(temp);
        //printf("The result is :%d\n",result);
        return result;
}

#define ERROR_EXPRESSION_INCOMPLETE 2
VarDeclStatement parseVarDeclStatement(Parser *p){
        VarDeclStatement stmt = {0};
        parser_next(p);
        stmt.name_size = p->current_token.size;
        stmt.name = p->current_token.str;
         
        parser_expected_next(p,TOKEN_EQUALS);
        parser_expected_next(p,TOKEN_NUMBER);
        stmt.value = token_to_num(p->current_token);
        while(p->current_token.type != TOKEN_SEMICOLON){
                if(p->next_token.type == TOKEN_KEYWORD){
                        printf("Error: Previous Expression not completed\n");
                        exit(ERROR_EXPRESSION_INCOMPLETE);
                }
                parser_next(p);
                switch(p->current_token.type){
                        case TOKEN_PLUS:{
                                parser_expected_next(p, TOKEN_NUMBER);
                                stmt.value += token_to_num(p->current_token);
                        }       break;
                        case TOKEN_MINUS:{
                                parser_expected_next(p, TOKEN_NUMBER);
                                stmt.value -= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_SLASH:{
                                parser_expected_next(p, TOKEN_NUMBER);
                                stmt.value /= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_STAR:{
                                parser_expected_next(p, TOKEN_NUMBER);
                                stmt.value *= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_MODULUS:{
                                parser_expected_next(p, TOKEN_NUMBER);
                                stmt.value %= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_SEMICOLON:{
                                break;
                        }       break;
                        default:{
                                printf("ERROR: TOKEN MISMATCH\n");
                                print_token(p->current_token);
                                exit(ERROR_TOKEN_MISMATCH);
                        }       break;
                }
        }
        return stmt;
}

void print_statement(VarDeclStatement stmt){
        printf("Name : %.*s\nValue : %d\n",(int)stmt.name_size,stmt.name,stmt.value);
}

int main(){
        const char *src = "mem8b EightByteVariable= 8 / 2* 2+2;";
        Lexer l = init_lexer(src, strlen(src));
        Parser p = init_parser(&l);
        if(p.current_token.type == TOKEN_KEYWORD){
                VarDeclStatement stmt = parseVarDeclStatement(&p);
                print_statement(stmt);
        }
        return 0;
}
