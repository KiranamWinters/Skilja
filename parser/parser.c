#include "../Lexer/lexer.h"
#include "../file_handler/file_handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

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

VarDeclStatement parseVarDeclStatement(Parser *p){
        VarDeclStatement stmt = {0};
        parser_next(p);
        stmt.name_size = p->current_token.size;
        stmt.name = p->current_token.str;
        if(p->next_token.type != TOKEN_EQUALS){
                printf("ERROR: Expected TOKEN '=' found '%.*s']\n ", (int)p->next_token.size,p->next_token.str);
        }
        parser_next(p); 
        if(p->next_token.type == TOKEN_NUMBER){
                char temp[p->next_token.size];
                strncpy(temp,p->next_token.str,p->next_token.size);
                stmt.value = atoi(temp);
        }
        return stmt;
}

void print_statement(VarDeclStatement stmt){
        printf("Name : %.*s\nValue : %d\n",(int)stmt.name_size,stmt.name,stmt.value);
}

int main(){
        const char *src = "mem8b EightByteVariable = 63;";
        Lexer l = init_lexer(src, strlen(src));
        Parser p = init_parser(&l);
        if(p.current_token.type == TOKEN_KEYWORD){
                VarDeclStatement stmt = parseVarDeclStatement(&p);
                print_statement(stmt);
        }
        return 0;
}
