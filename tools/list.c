#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void init(LIST ** root)
{
	LIST * temp = malloc(sizeof(LIST));

	temp -> next = NULL;
	temp -> name = NULL;
	
	*root = temp;
}

void push(LIST ** root, char * name)
{

	LIST* temp = (LIST *) malloc(sizeof(LIST));
	temp -> name = malloc((strlen(name) + 1) * sizeof(LIST));
	strcpy(temp -> name, name);

	
	temp -> next = *root;
	*root = temp;


}

char * pop(LIST ** root)
{
	char * temp = (*root) -> name;
	LIST * temp_node = *root;
	*root = (*root) -> next;
	free(temp_node);
	
	return temp;
}

void printList(LIST * root)
{
	LIST * temp = root;

	while(temp -> name != NULL){
		printf("%s\n", temp -> name);
		temp = temp -> next;
	}
}

void destroy(LIST ** root)
{
	LIST * temp = *root;
	while(temp != NULL) {
		*root = (*root) -> next; 
		free(temp);
		temp = *root;
	}
}
