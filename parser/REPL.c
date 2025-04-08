#include "REPL.h"
#include <stdio.h>
#include "../Lexer/lexer.h"
#include <string.h>

#define MAX_STATEMENT_SIZE 200


int main4(){
        char buffer[MAX_STATEMENT_SIZE];
        while(1){
                printf(">> ");
                fgets(buffer,sizeof(buffer), stdin);
                if(strcmp(buffer, "exit\n") == 0){
                        printf("EXIT REACHED\n");
                        return 0;
                }
                Lexer l = init_lexer(buffer,strlen(buffer));
                Token t = lexer_next(&l);
                while(t.type != TOKEN_EOF){
                        print_token(t);
                        t = lexer_next(&l);
                }
        }
        return 0;
}
