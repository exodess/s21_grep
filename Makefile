CFLAGS = -Wall -Wextra -Werror -c
GDB = -Wall -Wextra -Werror -g 

all: build clean

build : s21_grep.o list.o
	gcc -o s21_grep s21_grep.o list.o

debug : s21_grep.c ../../common/list.c
	gcc $(GDB) s21_grep.c ../../common/list.c -o gdb_grep
 
s21_grep.o : s21_grep.c
	gcc $(CFLAGS) s21_grep.c

s21_grep_flags.o : s21_grep_flags.c
	gcc $(CFLAGS) s21_grep_flags.c

list.o : ../../common/list.c
	gcc $(CFLAGS) ../../common/list.c

clean: 
	rm *.o
