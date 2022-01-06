#ifndef DLIST_H
#define DLIST_H

#include <stdlib.h>

typedef struct ListElmt_ {
	void                *data;
	struct ListElmt_    *prev;
	struct ListElmt_    *next;
} ListElmt;

typedef struct List_ {
	int size;

	ListElmt *head;
	ListElmt *tail;
	ListElmt *cur;
} List;

// export

void list_init(List *list);

void list_destroy(List *list);

int list_ins_next(List *list, ListElmt *element, const void *data);

int list_ins_prev(List *list, ListElmt *element, const void *data);

int list_remove(List *list, ListElmt *element, void **data);

#define list_size(list) ((list)->size)

#define list_head(list) ((list)->head)

#define list_tail(list) ((list)->tail)

#define list_is_head(element) ((element)->prev == NULL ? 1 : 0)

#define list_is_tail(element) ((element)->next == NULL ? 1 : 0)

#define list_data(element) ((element)->data)

#define list_next(element) ((element)->next)

#define list_prev(element) ((element)->prev)

#endif