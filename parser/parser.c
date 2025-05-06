#include "../Lexer/lexer.h"
#include "../file_handler/file_handler.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include <stdbool.h>

//Precedence table
#define MAX_PRECEDENCES 32
int PRECEDENCES[MAX_PRECEDENCES]                = {0};
prefixParseFuncs prefixParseFns[MAX_FUNCTIONS]  = {NULL};
infixParseFuncs infixParseFns[MAX_FUNCTIONS]    = {NULL};

void set_precedences(){
        PRECEDENCES[TOKEN_EQUALS]        = EQUALS;
        PRECEDENCES[TOKEN_NOT]           = EQUALS;
        PRECEDENCES[TOKEN_LESS_THAN]     = LESS_GREATER;
        PRECEDENCES[TOKEN_GREATER_THAN]  = LESS_GREATER;
        PRECEDENCES[TOKEN_PLUS]          = SUM;
        PRECEDENCES[TOKEN_MINUS]         = SUM;
        PRECEDENCES[TOKEN_STAR]          = PRODUCT;
        PRECEDENCES[TOKEN_SLASH]         = PRODUCT;
}

int next_precedence(Parser *p){
        
        return PRECEDENCES[p->next_token.type];
}

int current_precedence(Parser *p){
        return PRECEDENCES[p->current_token.type];
}

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

VarDeclStatement init_default_VarDeclStatement(){
        VarDeclStatement variableStatement;
        memset(&variableStatement, 0, sizeof(VarDeclStatement));
        return variableStatement;
}

#define STMT_VARIBALE_DECLARATION 1
/*VarDeclStatement init_VarDeclStatement(const char* stmt_name, size_t stmt_size,Token token,int val){
        VarDeclStatement vStmt = {
                .name_size = stmt_size,
                .name = stmt_name,
                .value = val,
                .token = token,
        };
        return vStmt;
}
*/

Token generate_empty_token(){
        Token t = {0};
        return t;
}

#define ERROR_EXPRESSION_INCOMPLETE 2

Expression* init_empty_expression(EXPR_TYPE type){
        Expression *e = malloc(sizeof(Expression));
        memset(e, 0, sizeof(Expression));
        switch (type) {
                case EXPR_INTEGER:{
                        e->type = EXPR_INTEGER; 
                        return e;
                }
                default:{
                        return e;
                }
        }
        return NULL;
}

Expression *parse_IntegerLiteral(Parser *p){
        Expression *e = init_empty_expression(EXPR_INTEGER);
        e->token = p->current_token;
        e->int_value = token_to_num(p->current_token);
        printf("NUMBER: %ld\n",e->int_value);
        return e;
}

//void print_IntegerLiteral(IntegerLiteral i){
//        printf("String : %.*s\nvalue: %lu\n",(int)i.token.size,i.token.str,i.value);
//}

void register_prefix_fn(TOKEN_TYPE type, prefixParseFuncs parseFn){
        prefixParseFns[type] = parseFn;
}

void register_infix_fn(TOKEN_TYPE type, infixParseFuncs parseFn){
        infixParseFns[type] = parseFn;
}
Expression *parse_infix_expression(Parser *p, Expression* left);
Expression *parse_expression(Parser *p, int precedence);
Expression *parse_prefix_fn(Parser *p){
        Expression *prfxExprsn = init_empty_expression(EXPR_PREFIX);
        prfxExprsn->token = p->current_token;
        prfxExprsn->prefix.prfx_operator = prfxExprsn->token.str[0];
        printf("Operator : %c\n", prfxExprsn->prefix.prfx_operator);
        parser_next(p);
        prfxExprsn->prefix.right = parse_expression(p,PREFIX);
        return prfxExprsn;
}

Expression* parse_infix_expression(Parser* p,Expression* left){
        Expression *e = init_empty_expression(EXPR_INFIX);
        e->token = p->current_token;
        e->infix.left = left;
        e->infix.infx_operator = e->token.str[0];
        int precedence = PRECEDENCES[p->current_token.type];
        //printf("%d\n",next_precedence(p)); 
        parser_next(p);
        e->infix.right = parse_expression(p,precedence);
        return e;
}

void register_parsing_fns(){
        register_prefix_fn(TOKEN_MINUS,parse_prefix_fn);
        register_prefix_fn(TOKEN_NOT, parse_prefix_fn);
        register_prefix_fn(TOKEN_INTEGER, parse_IntegerLiteral);

        register_infix_fn(TOKEN_PLUS, parse_infix_expression);
        register_infix_fn(TOKEN_MINUS, parse_infix_expression);
        register_infix_fn(TOKEN_STAR, parse_infix_expression);
        register_infix_fn(TOKEN_SLASH, parse_infix_expression);
        register_infix_fn(TOKEN_NOT, parse_infix_expression);
        register_infix_fn(TOKEN_EQUALS, parse_infix_expression);
        register_infix_fn(TOKEN_LESS_THAN, parse_infix_expression);
        register_infix_fn(TOKEN_GREATER_THAN, parse_infix_expression);

}

expr *init_expr(){
        expr *e = malloc(sizeof(expr));
        memset(e, 0, sizeof(expr));
        return e;
}

int parse_expr(Parser *p){
        int value  = token_to_num(p->current_token);
       while(p->current_token.type != TOKEN_SEMICOLON){
                if(p->next_token.type == TOKEN_KEYWORD){
                        printf("Error: Previous Expression not completed\n");
                        exit(ERROR_EXPRESSION_INCOMPLETE);
                }
                parser_next(p);
                switch(p->current_token.type){
                        case TOKEN_LESS_THAN:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value = (value < token_to_num(p->current_token));
                        }       break;
                        case TOKEN_GREATER_THAN:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value = (value > token_to_num(p->current_token));
                        }       break;
                        case TOKEN_PLUS:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value += token_to_num(p->current_token);
                        }       break;
                        case TOKEN_MINUS:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value -= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_SLASH:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value /= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_STAR:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value *= token_to_num(p->current_token);
                        }       break;
                        case TOKEN_MODULUS:{
                                parser_expected_next(p, TOKEN_INTEGER);
                                value %= token_to_num(p->current_token);
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
       return value;
}


Expression *parse_expression(Parser *p, int precedence){
        // We want to handle two prefix tokens
        // 1. "-"
        // 2. "!"
        prefixParseFuncs prefix = NULL;
        prefix = prefixParseFns[p->current_token.type];
        print_token(p->current_token);
        if(prefix == NULL){
                printf("ERROR : No parse function related to this Token : %*.s\n",(int)p->current_token.size,p->current_token.str);
                return NULL;
        }
        Expression *left_exprsn = prefix(p);
        while(p->next_token.type!=TOKEN_SEMICOLON && precedence < next_precedence(p)){
                //printf("HELLOOOOOOO\n");
                printf("CURR: %d | NEXT: %d\n",precedence,next_precedence(p));
                infixParseFuncs infix = infixParseFns[p->next_token.type];
                if(!infix){return left_exprsn;}
                parser_next(p);
                left_exprsn = infix(p,left_exprsn);
        }
        return left_exprsn;
}

Statement * init_default_statement(size_t);
Statement* parseVarDeclStatement(Parser *p){
        VarDeclStatement stmt = init_default_VarDeclStatement();
        Statement *s = init_default_statement(STMT_VARIBALE_DECLARATION);
        s->stmt = p->current_token.str;
        stmt.token = p->current_token;
        //Checking and advancing the parser if the next token is a symbol
        //in this case the symbol will act as a Identifier
        parser_expected_next(p, TOKEN_SYMBOL);
        //printf("String :%.*s",(int)p->current_token.size,p->current_token.str);
        stmt.name = p->current_token.str;
        stmt.name_size = p->current_token.size;
        
        //Checking for equals
        parser_expected_next(p,TOKEN_EQUALS);
        parser_next(p);
        stmt.value = parse_expr(p);
        size_t stmt_size = p->current_token.str - s->stmt;
        s->Statement_type = STMT_VARIBALE_DECLARATION;
        s->variableStatement = stmt;
        s->next = NULL;
        s->stmt_size = stmt_size+1;

        //printf("EXPRSN:  %ld\n",s.stmt_size);
        printf("EXPRSN:  %.*s\n",(int)s->stmt_size,s->stmt);
        return s;
}

void print_statement(Statement *stmt){
        switch (stmt->Statement_type) {
                case STMT_VARIBALE_DECLARATION:
                       return;
                default:
                       printf("Unknown Statement\n");
                       return;
        }
}

Statement* parse_statement(Parser *p){
        Statement *s = NULL;
        switch (p->current_token.type) {
                case TOKEN_VARIABLE_TYPE:
                        //printf("VARIABLE:%.*s\n",(int)p->next_token.size,p->next_token.str);
                        return parseVarDeclStatement(p);
                default:
                        printf("NOT A STATEMENT:%.*s\n",(int)p->current_token.size,p->current_token.str );
                        return s;
        }
        return s;
}

Statement* init_default_statement(size_t STMT_TYPE){
        Statement *stmt = malloc(sizeof(Statement));
        if(stmt == NULL){printf("Failed to initialize struct:Statement\n");return NULL;}
        memset(stmt, 0, sizeof(Statement));
        //NOTE: THIS IS TEMP ! FIX LATER
        stmt->Statement_type = STMT_TYPE;
        return stmt;
}

void append_statement(Statement* rootStmt, Statement *stmt){
        Statement* temp = rootStmt;
        while(temp->next != NULL){ 
                temp = temp->next;
        }
        temp->next = stmt;
}

Statement* parse_program(Parser *p){
        //(TODO): Implement init_statement
        Statement *rootNodeStatement = init_default_statement(STMT_VARIBALE_DECLARATION);
        Statement *stmt = init_default_statement(STMT_VARIBALE_DECLARATION);
        while(p->current_token.type != TOKEN_EOF){
                stmt = parse_statement(p);
                if(stmt->Statement_type != 0){
                        append_statement(rootNodeStatement, stmt);
                }
                parser_next(p);
        };
        return rootNodeStatement;
}

void print_statement_tree(Statement *rootStmt){
        rootStmt = rootStmt->next;
        while(rootStmt != NULL){
                printf("statement: %.*s\n",(int)rootStmt->stmt_size, rootStmt->stmt);
                if(rootStmt->Statement_type == STMT_VARIBALE_DECLARATION){
                        printf("statement_type: %s\n","STMT_VARIBALE_DECLARATION");
                }
                printf("value: %d\n",   rootStmt->variableStatement.value);
                rootStmt = rootStmt->next;
        }
}

void generate_code(Statement* prgm){
        if(prgm->Statement_type == 0){
                printf("Statement not recognize yet");
                printf("stmt: %s\n",prgm->stmt);
                if(prgm->next!= NULL){
                        prgm = prgm->next;
                }else{
                        printf("Statement not recognized");
                        return;
                }
        }
        FILE* file = fopen("output.asm", "w");
        if(!file){printf("file not created\n");return;}
        fprintf(file, "section .data\n");
        fprintf(file, "%.*s dd %d\n\n",(int)prgm->variableStatement.name_size,prgm->variableStatement.name,prgm->variableStatement.value);

        fprintf(file, "section .text\n");
        fprintf(file, "global _start\n");
        fprintf(file, "_start:\n");
        fprintf(file, "    mov rax, 60\n");
        fprintf(file, "    xor rdi, rdi\n");
        fprintf(file, "    syscall\n");
        fclose(file);
}
//char* create_string_from_stmt(Statement *stmt){
//        char *str = malloc(sizeof(char) * stmt->stmt_size + 5);
//        int s = sizeof(char) * stmt->stmt_size;
//        memcpy(&str[0], stmt->variableStatement.token.str, stmt->variableStatement.token.size);
//        memcpy(&str[stmt->variableStatement.token.size], stmt->variableStatement.name,stmt->variableStatement.name_size);
//        char *t1 = " = ";
//        memcpy (&str[stmt->variableStatement.token.size + stmt->variableStatement.name_size],t1, 3);
        //Expression
//       memcpy(&str[stmt->variableStatement.token.size+ stmt->variableStatement.name_size +3],
//                        stmt->variableStatement.exprsn->token.str,stmt->variableStatement.exprsn->token.size+1);
//        int i = stmt->variableStatement.token.size + stmt->variableStatement.name_size + 3 + stmt->variableStatement.exprsn->token.size;
//        str[i] = ';';
//      <F2>  str[i+1] = '\0';
 //       printf("Allocated buffer:%d | string: %lu\n",s, strlen(str));
 //       return str;
//}


int main(int argc, char *argv[]){
        register_parsing_fns();
        set_precedences();
        const char *src = load_file_into_memory(argv[1]);
        Lexer l = init_lexer(src, strlen(src));
        Parser p = init_parser(&l);
        Statement *prgm = parse_program(&p);

        generate_code(prgm);
        //printf("NAME : %.*s\n",(int)prgm->variableStatement.name_size,prgm->variableStatement.name); 
        //print_statement_tree(prgm);
}
