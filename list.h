/*
 *	Botarleanu Robert-Mihai 331CB
 *	Implementation of a simple linked list, the values it holds
 *	are generic.
 *
 */

#ifndef LIST_HEADER_GUARD
#define LIST_HEADER_GUARD

/* memory management */
#include <stdlib.h>
#include <string.h>

/* debugging */
#include <stdio.h>

/* the simple-linked list struct
 * It contains:
 *	- the information is a generic type
 *	- the next cell
 */
typedef struct list {
	void *i;
	struct list *next;
} list;

/* add an element ot the list, create a new list if it does not exist */
void list_add(list **l, int pos, void *el);

/* adds to the end of a list */
void list_append(list **l, void *el);

/* remove an element of the list, from the given position */
void list_remove(list **l, int pos);

/*
 * get an element value from the list at the specified position
 * @return: returns NULL if the element was not found
 */
void *list_get(list *l, int pos);

/* destroy the list, deallocating all used memory */
void list_destroy(list **l);

/* finds the size of the given list */
unsigned int list_size(list *l);

#endif