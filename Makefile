CFLAGS = -Wall -Wextra -Werror -c
GDB = -Wall -Wextra -Werror -g 

all: build clean

build : s21_grep.o list.o
	gcc -o ~/s21_grep s21_grep.o list.o
	sh tools/build.sh s21_grep

debug : s21_grep.c tools/clist.c
	gcc $(GDB) s21_grep.c tools/list.c -o gdb_grep
	gdb ./gdb_grep
 
s21_grep.o : s21_grep.c
	gcc $(CFLAGS) s21_grep.c

list.o : tools/list.c
	gcc $(CFLAGS) tools/list.c

clean: 
	rm *.o
