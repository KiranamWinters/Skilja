CC := clang
BUILD := .

SRC := $(wildcard *.c ../Lexer/*.c ../file_handler/*.c)

INCLUDE := 

OBJECT := $(notdir $(SRC:.c=.o))

CFLAGS :=  -O2 -Wextra -Wall

parser : $(OBJECT)
	$(CC) $(OBJECT) -o parser

# $< = %.c (dependency)
# $@ = %.o (target)
#
%.o : %.c
	@echo "Compiling $< ----> $@"
	$(CC) $(CFLAGS) -c -g $(SRC) $<

clean:
	rm -f parser $(OBJECT)
