#ifndef S21_GREP_FLAGS_H
#define S21_GREP_FLAGS_H

#define GNU_FLAG_E "regexp"					// шаблон
#define GNU_FLAG_I "ignore-case" 			// игнорирование регистра
#define GNU_FLAG_V "invert-match"			// ищем несовпадения
#define GNU_FLAG_C "count"					// количество совпадений
#define GNU_FLAG_L "files-with_match"		// выводит только файл (h -> 0)
#define GNU_FLAG_N "line-number"			// выводить номер строки
#define GNU_FLAG_H "no-filename"			// не выводить имя файла
#define GNU_FLAG_S "no-messages"			// без вывода ошибок
#define GNU_FLAG_O "only-matching"			// v -> 0, вывод только строки
#define GNU_FLAG_F "file"					// файл, откуда нужно читать данные

#define FLAGS "eivclnhsofP"
#define FLAGS_OPT "e:ivclnhsof:"
#define COUNT_FLAGS 10

char * GNU_FLAGS[COUNT_FLAGS] = {
	GNU_FLAG_E,
	GNU_FLAG_I,
	GNU_FLAG_V,
	GNU_FLAG_C,
	GNU_FLAG_L,
	GNU_FLAG_N,
	GNU_FLAG_H,
	GNU_FLAG_S,
	GNU_FLAG_O,
	GNU_FLAG_F,
};


#endif
