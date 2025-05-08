#ifndef LIST_H
#define LIST_H

typedef struct list {
	char * name;
	struct list * next;
} LIST;

void init(LIST **);
void push(LIST ** root, char * name);
char * pop(LIST ** root);
void destroy(LIST **);

#endif
