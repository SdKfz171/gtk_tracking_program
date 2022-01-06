#include <stdlib.h>
#include <string.h>

#include "list.h"

void list_init(List *list) {
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	list->cur = NULL;

	return;
}

void list_destroy(List *list) {
	void *data;

	while (list_size(list) > 0) {
		list_remove(list, list_tail(list), (void **)&data);
	}

	memset(list, 0, sizeof(List));

	return;
}

int list_insert_next(List *list, ListElmt *element, const void *data) {

	ListElmt *new_element;

	if (element == NULL && list_size(list) != 0)
		return -1;

	if ((new_element = (ListElmt *)malloc(sizeof(ListElmt))) == NULL)
		return -1;

	new_element->data = (void *)data;

	if (list_size(list) == 0) {
		list->head = new_element;
		list->head->prev = NULL;
		list->head->next = NULL;
		list->tail = new_element;
	}

	else {
		new_element->next = element->next;
		new_element->prev = element;

		if (element->next == NULL)
			list->tail = new_element;
		else
			element->next->prev = new_element;

		element->next = new_element;

	}

	list->size++;

	return 0;
}

//list_insert_prev(&list, list_head(&list), data)
int list_insert_prev(List *list, ListElmt *element, const void *data) {

	ListElmt * new_element;

	if (element == NULL && list_size(list) != 0)
		return -1;

	if ((new_element = (ListElmt *)malloc(sizeof(ListElmt))) == NULL)
		return -1;

	new_element->data = (void *)data;

	if (list_size(list) == 0) {
		list->head = new_element;
		list->head->prev = NULL;
		list->head->next = NULL;
		list->tail = new_element;
	}

	else {
		new_element->next = element;
		new_element->prev = element->prev;

		if (element->prev == NULL)
			list->head = new_element;
		else
			element->prev->next = new_element;

		element->prev = new_element;
	}

	list->size++;

	return 0;
}

int list_remove(List *list, ListElmt *element, void **data) {
	if (element == NULL || list_size(list) == 0)
		return -1;

	*data = element->data;

	if (element == list->head) {
		list->head = element->next;

		if (list->head == NULL)
			list->tail = NULL;
		else
			element->next->prev = NULL;
	}

	else {
		element->prev->next = element->next;

		if (element->next == NULL)
			list->tail = element->prev;
		else
			element->next->prev = element->prev;

	}

	free(element);

	list->size--;

	return 0;
}