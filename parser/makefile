CC := clang
BUILD := .

SRC := $(wildcard *.c ../Lexer/*.c ../file_handler/*.c)

OBJECT := $(notdir $(SRC:.c=.o))

CFLAGS :=  -O2 -Werror -Wextra -Wall

parser : $(OBJECT)
	$(CC) $(OBJECT) -o parser

# $< = %.c (dependency)
# $@ = %.o (target)
#
%.o : %.c
	@echo "Compiling $< ----> $@"
	$(CC) $(CFLAGS) -c $(SRC) $<

clean:
	rm -f parser $(OBJECT)
