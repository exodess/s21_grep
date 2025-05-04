#include <stdio.h>
#include <string.h>
#include <ctype.h>

void flag_i(char * str)
{
	while(str != strchr(str, '\0')) {
		*str = tolower(*str);
		str++;
	}
}

int match_template(char * str, char * pattern, int * modes)
{
	if(MODEFL(i))
	return (strstr(str, pattern) != NULL);
}


