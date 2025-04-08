CC := clang

SRC := $(wildcard *.c)

OBJECT := $(wildcard *.o)

lexer : lexer.o
	$(CC) $(OBJECT) -o lexer

lexer.o : lexer.c
	$(CC) -c $(SRC) ../file_handler/file_handler.c  
