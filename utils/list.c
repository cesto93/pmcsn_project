#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "utils.h"

struct node_t *create_node_prio(double val, int priority)
{
	struct node_t *n = malloc(sizeof(struct node_t));
	if (n == NULL)
		error_msg("errore malloc");
	n->val = val;
	n->priority = priority;
	n->next = NULL;
	return n;
}

void add_next(struct node_t *new, struct node_t **pnext)
{
	new->next = *pnext;
	*pnext = new;
}

void remove_next(struct node_t **pnext)
{
	struct node_t *p = *pnext;
	p = p->next;
	free(*pnext);
	*pnext = p;
}

void free_list(struct node_t **pnext)
{
	while(*pnext != NULL)
		remove_next(pnext);
}

void add_last_prio(struct node_t **pnext, double val, int priority) 
{
	struct node_t *p;
	struct node_t *new = create_node_prio(val, priority);

	for (p = *pnext; p != NULL; pnext = &(p->next), p = p->next);

	add_next(new, pnext); // l'elemento va inserito in fondo alla lista
}

void add_ordered_prio(struct node_t **pnext, double val, int priority)
{
	struct node_t *p;
	struct node_t *new = create_node_prio(val, priority);

	for (p = *pnext; p != NULL; pnext = &(p->next), p = p->next) {
		if (p->val > new->val) {
			/* inserisci new dopo pnext, ossia prima di p */
			add_next(new, pnext);
			return;
		}
	}

	add_next(new, pnext); // l'elemento va inserito in fondo alla lista
}
