/*
*	Botarleanu Robert-Mihai 331CB
*	Implementation of the simple linked data structure.
*/

# include "list.h"

/* create a cell with the given data */
list *make_cell(void *el)
{
	list *cell;

	cell = (list *) calloc(1, sizeof(list));
	if (cell == NULL)
		return NULL;
	cell->i = el;
	cell->next = NULL;
	return cell;
}

/* destroys a cell and it's information */
void destroy_cell(list **cell)
{
	free(*cell);
}

/* add an element ot the list, create a new list if it does not exist */
void list_add(list **l, int pos, void *el)
{
	int i;
	list *it;
	list *aux;
	list *cell;

	/* make the cell */
	cell = make_cell(el);
	/* new list */
	if (l == NULL) {
		*l = cell;
		return;
	}
	/* add to the beginning */
	if (pos == 0) {
		cell->next = *l;
		*l = cell;
		return;
	}
	/* add to a non-first position */
	for (i = 0, it = *l; i < pos - 1 && it != NULL; ++i)
		it = it->next;
	/* put the cell between the cell and it's successor */
	aux = it->next;
	it->next = cell;
	cell->next = aux;
}

/* finds the size of the given list */
unsigned int list_size(list *l)
{
	list *aux;
	unsigned int size;

	aux = l;
	size = 0;

	while (aux != NULL) {
		++size;
		aux = aux->next;
	}
	return size;
}

/* adds to the end of a list */
void list_append(list **l, void *el)
{
	list_add(l, list_size(*l), el);
}

/* remove an element of the list, from the given position */
void list_remove(list **l, int pos)
{
	list *cell;
	list *aux;

	cell = *l;
	aux = *l;

	if (pos == 0)
		*l = (*l)->next;
	else {
		while (--pos && aux != NULL)
			aux = aux->next;
		/* redirect pointers to decouple the cell from the list */
		cell = aux->next;
		aux->next = cell->next;
	}
	destroy_cell(&cell);
}

/* get an element value from the list at the specified position
 * @return: returns NULL if the element was not found
 */
void *list_get(list *l, int pos)
{
	++pos;
	while (--pos && l != NULL)
		l = l->next;
	if (l == NULL)
		return NULL;
	return l->i;
}

/* destroy the list, deallocating all used memory */
void list_destroy(list **l)
{
	while (*l != NULL) {
		list *aux = *l;
		*l = (*l)->next;
		destroy_cell(&aux);
	}
}