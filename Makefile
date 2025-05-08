CFLAGS = -Wall -Wextra -Werror -c

all: build clean

build : s21_grep.o list.o
	gcc -o ~/s21_grep s21_grep.o list.o
	sh tools/build.sh s21_grep
 
s21_grep.o : s21_grep.c
	gcc $(CFLAGS) s21_grep.c

list.o : tools/list.c
	gcc $(CFLAGS) tools/list.c

clean: 
	rm *.o
