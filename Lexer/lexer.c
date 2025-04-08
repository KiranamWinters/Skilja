/*
====================================================
        AUTHOR :  Kiranam Dewangan
        DATE   :  March 18, 2025
====================================================
*/

#include "lexer.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../file_handler/file_handler.h"

const char* TOKEN_LOOKUP[] = {
        "TOKEN_EOF",
        "INVALID_TOKEN",
        "TOKEN_STRING",
        "TOKEN_COMMENT",
        "TOKEN_PREPROCESSOR",
        "TOKEN_UNKNOWN",
        "TOKEN_SYMBOL",
        "TOKEN_SEMICOLON",
        "TOKEN_NUMBER",
        "TOKEN_BRACKET",
        "TOKEN_SQUARE_BRACKET",
        "TOKEN_BLOCK_BRACKET",
        "TOKEN_OPERATOR",
        "TOKEN_LITERAL",
        "TOKEN_KEYWORD",
        "TOKEN_PLUS",
        "TOKEN_MINUS",
        "TOKEN_STAR",
        "TOKEN_LESS_THAN",
        "TOKEN_GREATER_THAN",
        "TOKEN_NOT",
        "TOKEN_EQUALS",
        "TOKEN_MODULUS",
        "TOKEN_AMPERSAND",
        "TOKEN_SLASH",
        "TOKEN_EXTENDED",
        "TOKEN_DEBUG"
};

//This does not remove the whitespace, it merely ignores them
void skip_whitespace(Lexer* l){
        while(isspace(l->content[l->pointer]) && l->pointer < l->content_len){
                l->pointer += 1;
        }
}

Lexer init_lexer(const char *str, size_t str_len){
      Lexer l = {0};
      l.content = str;
      l.content_len = str_len;
      return l;
}

// Returns 0 if character is a number or a special symbol
bool check_symbol_first_valid(char c){
        return isalpha(c) || c =='_';
}

// Return a positive value if the character is a alphanumeric character or '_'
bool check_symbol(char a){
        return isalnum(a) || a == '_';
}

char LITERAL_SYMBOLS[] = {'(',')','{','}','[',']',';',':','.',',','\'',',',';'};
#define LITERAL_SYMBOLS_SIZE sizeof(LITERAL_SYMBOLS)/sizeof(LITERAL_SYMBOLS[0])

char OPERATOR_LIST[] = {'=','+','-','*','<','>','!','%','&','/'};
#define OPERATOR_LIST_SIZE sizeof(OPERATOR_LIST)/sizeof(OPERATOR_LIST[0])

unsigned int OPERATOR_TOKENS[] = {
        TOKEN_EQUALS,           // =
        TOKEN_PLUS,             // +
        TOKEN_MINUS,            // -
        TOKEN_STAR,             // *
        TOKEN_LESS_THAN,        // <
        TOKEN_GREATER_THAN,     // >
        TOKEN_NOT,              // !
        TOKEN_MODULUS,          // %
        TOKEN_AMPERSAND,        // &
        TOKEN_SLASH             // /
};

const char* KEYWORD_LIST[] = {
        "if","else","loop","loop_condition",
        "case","default",
        "typedef","struct",
        "true", "false",
        "mem8","mem16","mem32","mem64","mem128",
        
};

#define KEYWORD_COUNT sizeof(KEYWORD_LIST)/sizeof(KEYWORD_LIST[0])

void check_keyword(Token *t){
        for (int i = 0; i < KEYWORD_COUNT; i++){
                if(!strncmp(t->str, KEYWORD_LIST[i], strlen(KEYWORD_LIST[i]))){
                       t->type = TOKEN_KEYWORD;
                       return;
                }
        }
}

//CHECKS FOR SYMBOLS SUCH AS [//, ++ , --, &&, ||]
void check_repeated_symbols(Lexer *l,Token *token){
        Token t = peek_next(l);
        if(token->type == TOKEN_SLASH){
                if(t.type == TOKEN_SLASH){
                        token->type = TOKEN_COMMENT;
                        while(l->pointer < l->content_len && l->content[l->pointer] != '\n'){
                                l->pointer++;
                        }
                        token->size = &l->content[l->pointer] - token->str;
                }
                return;
        }
        
        if(token->type == t.type){
                token->type =    TOKEN_EXTENDED;
                token->size += 1;
                l->pointer      += 1;
                return;
        }

}

Token lexer_next(Lexer *l){
        skip_whitespace(l);

        Token token = { 
                .str = &l->content[l->pointer],
        };

        //END OF SOURCE FILE CHECK
        if(l->pointer >= l->content_len){return token;};


        //CHECKING FOR LITERALS
        for(int i = 0; i < LITERAL_SYMBOLS_SIZE;i++){
                if(l->content[l->pointer] == LITERAL_SYMBOLS[i]){
                        token.type = TOKEN_LITERAL;
                        token.size++;
                        l->pointer++;
                        return token;
                }
        }

        //CHECKING FOR OPERATORS
        for(int i = 0; i < OPERATOR_LIST_SIZE; i++){
                if(l->content[l->pointer] == OPERATOR_LIST[i]){
                        token.size += 1;
                        l->pointer++;
                        token.type = OPERATOR_TOKENS[i];
                        check_repeated_symbols(l,&token);
                        return token;
                }
        }
        
        //NUMBERICAL CHARACTER CHECKER
        if(isdigit(l->content[l->pointer])){
                token.type = TOKEN_NUMBER;
                while(isdigit(l->content[l->pointer]) && l->pointer < l->content_len){
                        l->pointer++;
                }
                token.size = &l->content[l->pointer] - token.str;
                return token;
        }

        switch (l->content[l->pointer]) {
                //PREPROCESSOR TOKENIZER
                case '#':{
                                token.type = TOKEN_PREPROCESSOR;
                                while(l->pointer < l->content_len && l->content[l->pointer] != '\n'){
                                        l->pointer += 1;
                        }
                                if(l->pointer < l->content_len){
                                        l->pointer += 1;
                                }
                                token.size = &l->content[l->pointer] - token.str;
                                return token;
                        }       break;

                //PREPROCESSOR TOKENIZER
                case '@':{
                                token.type = TOKEN_PREPROCESSOR;
                                while(l->pointer < l->content_len && l->content[l->pointer] != '\n'){
                                        l->pointer += 1;
                        }
                                if(l->pointer < l->content_len){
                                        l->pointer += 1;
                                }
                                token.size = &l->content[l->pointer] - token.str;
                                return token;
                        }       break;

                //STRING TOKENIZER
                case '"':{
                                token.type = TOKEN_STRING;
                                l->pointer++;
                                while(l->pointer < l->content_len && l->content[l->pointer] != '"'){
                                        l->pointer++;
                                }
                                token.size = &l->content[l->pointer] - token.str;
                                token.size++;
                                l->pointer++;
                                return token;
                         }
                default:
                //SYMBOL CHECKER
                        //printf("%d\n", check_symbol_first_valid(l->content[l->pointer]));
                        if(check_symbol_first_valid(l->content[l->pointer])){
                                token.type = TOKEN_SYMBOL;
                                while(l->pointer < l->content_len && check_symbol(l->content[l->pointer])){
                                        l->pointer += 1;
                                }
                                token.size = &l->content[l->pointer] - token.str;
                                check_keyword(&token);
                                return token;
                        }       break;

        }

        token.type = TOKEN_UNKNOWN;
        l->pointer += 1;
        token.size += 1;
        return  token;
}

//THIS PROCESS FETCHES NEXT TOKEN WITHOUT INCREMENTING THE CURRENT TOKEN POSITION
Token peek_next(Lexer *l){
        int temp = l->pointer;
        Token t = lexer_next(l);
        l->pointer = temp;
        return t;
}

//RETURN A STRING BASED ON GIVEN TOKEN
const char* token_as_str(int token){
        return TOKEN_LOOKUP[token];
}

void print_token(Token t){
        printf("%s : %.*s\n",token_as_str(t.type),(int)t.size,t.str);
}

int main2(){
        const char* src = load_file_into_memory("lexer.c");
        Lexer l = init_lexer(src, strlen(src));
        Token t = lexer_next(&l);
        while(t.type != TOKEN_EOF){
                print_token(t);
                t = lexer_next(&l);
        }
        return 0;
}

