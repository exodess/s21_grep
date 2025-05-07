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
#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define ERR_WITHOUT_ARG -2 		// если у ключей -f или -e нет аргумента
#define ERR_PATTERN_FILE_NF -3  // указанный файл не существует или не может быть открыт
#define ERR_BAD_FLAG -4 		// неправильно указан флаг
#define ERR_NOT_PATTERNS -5		// не указан шаблон 
#define ERR_FLAG_F "-f"
#define ERR_FLAG_E "-e"


typedef struct {
	char * flag; // с каким флагом связана ошибка
	int number; // номер ошибки
} err_t; 


void printFlags(int [COUNT_FLAGS + 1]);
void tolowerstr(char *); // переводит строку в нижний регистр
int countNodes(LIST *); // подсчитывает количество элементов в списке (файлы или шаблоны)
int processing_input(int, char *[], int mode[COUNT_FLAGS], LIST **, LIST **);
void processing_modes(int [COUNT_FLAGS], int);
void printNameFile(char * name, int flag);
int readln(char **, FILE *, int *);
void printLineColors(char *, int, int);

int main(int argc, char * argv[])
{
	LIST * list_file, * list_patterns;
	FILE * fp = NULL;
	int len = 1;
	char * buffer = malloc(len * sizeof(char));

	regmatch_t pmatch[1];
	int modes[COUNT_FLAGS + 1] = {0};
	// 				| <--	 			COUNT_FLAGS			 	 	 -->|
	// 				| 					--------				    	|
	// int modes = 	| e | i | v | c | l | n | h | s | o | f |  isPrint  |
	// последний флаг показывает, нужно ли выводить на экран найденные строки 
	
	init(&list_file);
	init(&list_patterns);

	// проверка на правильный ввод
	if(processing_input(argc, argv, modes, &list_patterns, &list_file) == 0){

		// обработка флагов (убрать лишние и решить зависимости)
		processing_modes(modes, countNodes(list_file));
		
		LIST * lp = list_patterns;
		int count_patt = countNodes(list_patterns);
		regex_t * arr_reg = malloc((count_patt) * sizeof(regex_t));

		// компиляция регулярных выражений из массива паттернов
		for(int i = 0; i < count_patt; i++) {
			if(MODEFL(i)) tolowerstr(lp -> name);
			if(regcomp(&(arr_reg[i]), lp -> name, REG_NEWLINE)) exit(EXIT_FAILURE);
			lp = lp -> next;
			
		}
		while(list_file -> name != NULL) {
			fp = (strcmp(list_file -> name, "-") == 0) ? stdin : fopen(list_file -> name, "r");
		
			if(fp != NULL) {
				
				int rez = 1, numberln = 1, countlns = 0;
				
				while(rez){ // считывание содержимого файла построчно
					rez = readln(&buffer, fp, &len); // считали очередную строчку из файла
					
					regoff_t global_off = 0; // смещение относительно начала строки (для -i)
					int flag_exit = (*buffer == '\0'); // флаг выхода для перехода к следующей строчке
					char * fline = buffer; // buffer должна всегда указывать на первую ячейку, которую выделила malloc()
					char * copy = NULL; // для строки buffer в нижнем регистре
					if(MODEFL(i)) { // уменьшаем регистр строки в соответствие с флагом -i
						// регистр шаблонов должен быть уменьшен до компиляции рег.выражений
						copy = malloc((strlen(buffer) + 1) * sizeof(char));
						strcpy(copy, buffer);
						tolowerstr(copy);
						fline = copy;
					}
					
					while(!flag_exit){
						// проходимся по строчке и ищем среди списка рег.выражений то, 
						// которое находится ближе всех к началу строки
						// выводим строчку до конечного символа найденного шаблона и сдвигаем строчку вперед до следующего символа
						// проходимся так до тех пор, пока строка не будет иметь нулевую длину
						// или не будет найдено ни одно совпадение с нашими шаблонами

						regoff_t plen = 0, min_off = strlen(fline);

						// поиск совпадений
						for(int i = 0; i < count_patt; i++) {
							if(regexec(&(arr_reg[i]), fline, ARRAY_SIZE(pmatch), pmatch, 0) == 0){
								if(min_off > pmatch[0].rm_so) {
									min_off = pmatch[0].rm_so;
									plen = pmatch[0].rm_eo - pmatch[0].rm_so;
								}
								
							}
							// иначе если не найдено ни одно совпадение
							else flag_exit = (i == (count_patt - 1)); 
						}
						
						if((fline == buffer) || (fline == copy)) { // если рассматриваемая подстрока находится в начале строки
							if((!flag_exit) ^ MODEFL(v))
							{
								if(MODEFL(P)){
									if(!MODEFL(h)) printf("%s:", list_file -> name);
									if(MODEFL(n))  printf("%d:", numberln);
									if(MODEFL(o)) printf("%s%.*s%s", ORANGE, plen, buffer + min_off + global_off, ORANGE);
									else printf("%.*s%s%.*s%s", min_off, buffer + global_off, ORANGE, plen, buffer + global_off + min_off, ESC);
								}
								countlns++;
																	
							}
						}
						else {
							// если рассматриваемая подстрока находится не в начале строки
							// значит, было найдено хотя бы одно совпадение с шаблоном
							if(!MODEFL(v) && MODEFL(P) ) {
								if(MODEFL(o) && !flag_exit) printf("%s%.*s%s", ORANGE, plen, buffer + global_off + min_off, ESC);
								else if(!MODEFL(o) && !flag_exit) printf("%.*s%s%.*s%s", min_off, buffer + global_off, ORANGE, plen, buffer + global_off + min_off, ESC);
								else if(!MODEFL(o)) printf("%.*s", min_off + plen, buffer + global_off);
								else printf("\n");
							}
						}
						  
						fline += (min_off + plen);
						global_off += min_off + plen;
					}	
					numberln += (*buffer != '\0');
					if(copy != NULL) free(copy);
				}

				if(!MODEFL(P)) { // та информация, которая выводится в конце
					if(MODEFL(l) && countlns) printNameFile(list_file -> name, 0);
					else if(!MODEFL(h) && countlns) printNameFile(list_file -> name, 1);
					if(MODEFL(c)) printf("%d\n", countlns);					
				}
				if(fp != stdin) fclose(fp);
			}
			
			else if(!MODEFL(s)) fprintf(stderr, "grep: %s: Нет такого файла или каталога\n", list_file -> name);

			pop(&list_file);

		}

		free(arr_reg);
	}
	
	else printf("Использование: grep [ПАРАМЕТР]… ШАБЛОНЫ [ФАЙЛ]…\n");
	
	free(buffer);
	destroy(&list_file);
	destroy(&list_patterns);
	
	return 0;
}


int processing_input(int argc, char * argv[], int modes [COUNT_FLAGS], LIST ** list_patt, LIST ** list_files)
{

	struct option long_options[COUNT_FLAGS + 1];
	int rez, opt_index;
	err_t err = {NULL, 0};
	
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
	opt_index = -1;
	while((rez = getopt_long(argc, argv, FLAGS_OPT, long_options, &opt_index)) != -1) {
	
		// printf("rez: %c[%d], opt_index = %c[%d], optarg = %s\n", rez, rez, opt_index, opt_index, optarg);

		rez = (rez == 0) ? FLAGS[opt_index] : rez;
		int flag = (rez != 0); // длинный или короткий параметр
		if(strchr(FLAGS, rez) != NULL && rez != 'P') { // короткое название
			for(int i = 0; i < COUNT_FLAGS; i++) modes[i] += (modes[i]) ? 0 : (rez == FLAGS[i]);

			if(rez == 'e') {
				if(optarg == NULL) { 
					if(flag) err.flag = GNU_FLAG_E;
					else err.flag = ERR_FLAG_E; 
					err.number = ERR_WITHOUT_ARG; 
				}
				else push(list_patt, optarg);
			}
			if(rez == 'f') {
				if(optarg == NULL) { 
						if(flag) err.flag = GNU_FLAG_F;
						else err.flag = ERR_FLAG_F;
						err.number = ERR_WITHOUT_ARG;
					}
				else {
					// параметр -f идет вместе с аргументом
					FILE * fp = fopen(optarg, "r");
					if(fp == NULL) { 
						err.number = ERR_PATTERN_FILE_NF; 
					}
					else {
						// файл существует и открывается
						int len = 1, rez = 1;
						char * buffer = malloc(len * sizeof(char));

						// чтение всех шаблонов из файла
						while(rez){
							rez = readln(&buffer, fp, &len);
							if(strchr(buffer, '\n') != NULL) *(strchr(buffer, '\n')) = '\0';
							push(list_patt, buffer);
						}
						fclose(fp);
						free(buffer);
					}
				}
						
			}

		}
		else err.number = ERR_BAD_FLAG;
		opt_index = -1;
	}
	if(err.number == 0){
		for(int i = (argc - 1); i > 0; i--) {
			if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "-f") == 0) 
				pop(list_files); // предыдущий аргумент относился к флагу -e или -f (которые требуют значение)

			else if(strcmp(argv[i], "-") == 0 || argv[i][0] != '-'){
				push(list_files, argv[i]);
			}
		}
		if(countNodes(*list_patt) == 0 && countNodes(*list_files) == 0) err.number = (err.number == 0) ? ERR_NOT_PATTERNS : err.number;
		else if(countNodes(*list_patt) == 0 && countNodes(*list_files) > 0) push(list_patt, pop(list_files));
		
		if(countNodes(*list_files) == 0) push(list_files, "-");
	}
	rez = err.number;
	
	if(err.number != 0) {
		if(err.number == ERR_WITHOUT_ARG) printf("Флаг \"%s\" должен иметь аргумент!\n", err.flag);
		else if(err.number == ERR_PATTERN_FILE_NF) printf("Файла не существует!\n");
		else if(err.number == ERR_BAD_FLAG) printf("Ошибочный флаг!\n");
	}
	return rez;
}


// -------------------------------------------------------------------------------------------------
// эти функции работают нормально


int readln(char ** buffer, FILE * fp, int * len)
{
	// если считан EOF, вернется 0, иначе 1
	char ch = ' ';
	int j = 0;
	while((ch = fgetc(fp)) != '\n' && ch != EOF) {
		if((j + 2) >= *len) {
			char * temp = malloc((*len + 20)*sizeof(char));
			strncpy(temp, *buffer, j);
			free(*buffer);
			*buffer = temp;
			*len += 20;
		}
		(*buffer)[j] = ch;
		j++;
	}
	if(ch == '\n'){
		(*buffer)[j] = '\n';
		(*buffer)[j + 1] = '\0';
	}
	else (*buffer)[j] = '\0';
	
	return (ch != EOF);
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


void processing_modes(int modes[COUNT_FLAGS], int count_file)
{
	MODEFL(P) = 1;
	if(MODEFL(h) == 0) MODEFL(h) = (count_file > 1) ? 0 : 1;	
	
	if(MODEFL(l)) {
		MODEFL(h) = 0;
		MODEFL(c) = 0;
	}
	
	MODEFL(P) = !MODEFL(l) && !MODEFL(c);
	MODEFL(P) = MODEFL(P) ^ (MODEFL(v) && MODEFL(o));
	MODEFL(h) = (MODEFL(v) && MODEFL(o)) ? 1 : MODEFL(h);
	MODEFL(c) = (MODEFL(l)) ? 0 : MODEFL(c);
}


int countNodes(LIST * root)
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


void tolowerstr(char * str)
{
	int j = 0;
	while(str[j] != '\0') {
		str[j] = tolower(str[j]);
		j++;
	}
}


void printFlags(int  modes[COUNT_FLAGS + 1])
{
	for(int i = 0; i < COUNT_FLAGS + 1; i++) printf("|%c|", FLAGS[i]);
	printf("\n");
	for(int i = 0; i < COUNT_FLAGS + 1; i++) printf("|%d|", modes[i]);
	printf("\n");
}
