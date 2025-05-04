#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

#include "../../common/list.h"
#include "s21_grep_flags.h"
#include "../../common/colors.h" // окрашивание ввода

#define _GNU_SOURCE
#define WITHOUT_COLOR
#define MODEFL(ch) modes[strchr(FLAGS, *#ch) - FLAGS]
#define PRINT 1
#define TEST 0
#define ERR_WITHOUT_ARG -2 // если у ключей -f или -e нет аргумента
#define ERR_PATTERN_FILE_NF -3 // not found 

int processing_input(int, char *[], struct option [COUNT_FLAGS],
					int mode[COUNT_FLAGS], LIST **, LIST **);
void processing_modes(int [COUNT_FLAGS], int);
long find_newline(FILE *, long);
void printNameFile(char * name, int flag);
int countFiles(LIST *);
int readln(char **, FILE *, int *);
int match_template(char *, char *, int *, int);
void printLineColors(char *, int, int);

int main(int argc, char * argv[])
{
	LIST * list_file, * list_patterns;
	FILE * fp = NULL;
	struct option long_options[COUNT_FLAGS + 1];
	int len = 1;
	char * buffer = malloc(len * sizeof(char));
	*buffer = ' ';
	
	int modes[COUNT_FLAGS + 1] = {0};
	// 				| <--	 			COUNT_FLAGS			 	 	 -->|
	// 				| 					--------				    	|
	// int modes = 	| e | i | v | c | l | n | h | s | o | f |  isPrint  |
	// последний флаг показывает, нужно ли выводить на экран найденные строки 
	
	init(&list_file);
	init(&list_patterns);
	
	if(processing_input(argc, argv, long_options, modes, &list_patterns, &list_file)){
		
		processing_modes(modes, countFiles(list_file));
		
		if(list_file -> name == NULL) push(&list_file, "-");
		
		while(list_file -> name != NULL) {
		
			if(strcmp(list_file -> name, "-") == 0) fp = stdin;
			
			if((fp == stdin) || (fp = fopen(list_file -> name, "r")) != NULL) {
				int rez = 1, numberln = 1, countlns = 0;
				
				while(rez){
					rez = readln(&buffer, fp, &len);
					
					/*if((*buffer != '\0') && match_template(buffer, patterns, modes, TEST) ^ MODEFL(v)){ // нужная строка
						if(MODEFL(P) && !MODEFL(h)) printNameFile(list_file -> name, 1);
						if(MODEFL(P) && MODEFL(n)) printf("%d:", numberln);
						if(MODEFL(P)) match_template(buffer, patterns, modes, PRINT);
						countlns++;
					}*/
					numberln += (*buffer != '\0');
				}

				if(!MODEFL(P)) { // та информация, которая выводится в конце
					if(MODEFL(l) && countlns) printNameFile(list_file -> name, 0);
					else if(!MODEFL(h) && countlns) printNameFile(list_file -> name, 1);
					if(MODEFL(c)) printf("%d\n", countlns);					
				}

				if(fp != stdin) fclose(fp);
			}
			else if(!MODEFL(s)) fprintf(stderr, "grep: %s: нет такого файла\n", list_file -> name);

			pop(&list_file);

		}
	}
	
	else printf("Использование: grep [ПАРАМЕТР]… ШАБЛОНЫ [ФАЙЛ]…\n");

	free(buffer);
	destroy(&list_file);
	destroy(&list_patterns);
	return 0;
}

int match_template(char * str, char * pattern, int * modes, int PFLAG)
{
	int rez = 0;
	char * temp = NULL;
	
	if(MODEFL(i)) {
		char * cpstr = malloc((strlen(str) + 1) * sizeof(*str));
		
		while(str[rez] != '\0') {
			cpstr[rez] = tolower(str[rez]);
			rez++;
		}
		rez = 0;
		while(pattern[rez] != '\0') {
			pattern[rez] = tolower(pattern[rez]);
			rez++;
		}
		temp = strstr(cpstr, pattern);
		rez = (temp != NULL) ? (temp - cpstr) : -1;
		
		free(cpstr);
	}
	else {
		temp = strstr(str, pattern);
		rez = (temp != NULL) ? (temp - str) : -1;
	}
	if(PFLAG == PRINT) {
		if(MODEFL(o)) { 
			strncpy(str, str + rez, strlen(pattern));
			*(str + strlen(pattern)) = '\n';
			*(str + strlen(pattern) + 1) = '\0';
			rez = 0;
		}
		#ifdef WITHOUT_COLOR
			printf("%s", str);
		#else
			printLineColors(str, rez, rez + (rez != -1) * strlen(pattern));
		#endif
	}
	return (rez != -1) && (*str != '\0');
}

void printLineColors(char * str, int start_drow_point, int stop_drow_point)
{
	int i = 0;

	while(str[i] != '\0') {
		if(i == start_drow_point) printf("%s%s", HARD_LIGHT, RED);
		else if(i == stop_drow_point) printf("%s", ESC);
		
		printf("%c", str[i]);
		i++;
	}
}

int readln(char ** buffer, FILE * fp, int * len)
{
	// если считан EOF, вернется 0, иначе 1
	char ch = ' ';
	int j = 0;
	while((ch = fgetc(fp)) != '\n' && ch != EOF) {
		if((j + 1) == *len) {
			char * temp = malloc((*len + 20)*sizeof(char));
			strncpy(temp, *buffer, j);
			free(*buffer);
			*buffer = temp;
			*len += 20;
		}
		(*buffer)[j] = ch;
		j++;
	}
	if(ch == '\n') {
		(*buffer)[j] = '\n';
		(*buffer)[j + 1] = '\0';	
	}
	else (*buffer)[j] = '\0';
	
	return (ch != EOF);
}

int processing_input(int argc, char * argv[],
	struct option long_options[COUNT_FLAGS], int modes [COUNT_FLAGS],  
	LIST ** list_patt, LIST ** list_files)
{
	
	int rez;
	int err = 0;
	int opt_index;
	for(int i = 0; i < COUNT_FLAGS; i++) {
		long_options[i].name = GNU_FLAGS[i];
		long_options[i].has_arg = no_argument;
		long_options[i].flag = &modes[i];
		long_options[i].val = 1;
	}
	
	long_options[COUNT_FLAGS - 1].has_arg = required_argument;
	long_options[0].has_arg = required_argument;
	
	long_options[COUNT_FLAGS].name = NULL;
	long_options[COUNT_FLAGS].has_arg = 0;
	long_options[COUNT_FLAGS].flag = NULL;
	long_options[COUNT_FLAGS].val = 0;

	opterr = 0; // не выводим ошибки
	while((rez = getopt_long(argc, argv, FLAGS_OPT, long_options, &opt_index)) != -1) {
		if(strchr(FLAGS, rez) != NULL && rez != 'P') { // короткое название
			
			for(int i = 0; i < COUNT_FLAGS; i++)
				modes[i] += (modes[i]) ? 0 : (rez == FLAGS[i]);

			
			if(rez == 'e' || opt_index == 'e'){
					if(optarg == NULL) err = ERR_WITHOUT_ARG;
					else push(list_patt, optarg);
			}
				/*case 'f': {
					if(optarg == NULL) err = ERR_WITHOUT_ARG;
					else {
						FILE * fp = fopen(optarg, "r");
						if(fp == NULL) err = ERR_PATTERN_FILE_NF;
					}
				}*/
			
		}
		else printf("Неправильный флагс - %c!", rez);
	}

	printList(*list_patt);
	printList(*list_files);
	
	if(err == 0){
		printf("It's OK!");
	}
	else rez = err;
	printf("%d", err);
	return rez;
}

void processing_modes(int modes[COUNT_FLAGS], int count_file)
{
	if(MODEFL(h) == 0) MODEFL(h) = (count_file > 1) ? 0 : 1;	
	
	if(MODEFL(l)) {
		MODEFL(h) = 0;
		MODEFL(c) = 0;
	}

/*	if(MODEFL(v)) {
		MODEFL(c) = (MODEFL(c)) ? 1 : ;
		MODEFL(n) = (MODEFL(n)) ? -1 : 0;		
	}
*/
	MODEFL(P) = !MODEFL(l) && !MODEFL(c);
	MODEFL(P) = MODEFL(P) ^ (MODEFL(v) && MODEFL(o));
	MODEFL(h) = (MODEFL(v) && MODEFL(o)) ? 1 : MODEFL(h);
	MODEFL(c) = (MODEFL(l)) ? 0 : MODEFL(c);
}


int countFiles(LIST * root)
{
	int j = 0;
	while(root -> name != NULL) {
		root = root -> next;
		j++;
	}
	return j;
}

void printNameFile(char * name, int flag)
{	
	if(flag)
		#ifdef WITHOUT_COLOR
			printf("%s:", name);
		#else
			printf("%s%s:%s", PURPLE, name, ESC); 
		#endif
	else 
		#ifdef WITHOUT_COLOR
			printf("%s\n", name);
		#else
			printf("%s%s%s\n", PURPLE, name, ESC);
		#endif
}
