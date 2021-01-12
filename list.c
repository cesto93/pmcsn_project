#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "utils.h"

struct node_t *create_node(double val)
{
	struct node_t *n = malloc(sizeof(struct node_t));
	if (n == NULL)
		error_msg("errore malloc");
	n->val = val;
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

void add_ordered(struct node_t **pnext, double val)
{

	struct node_t *p;
	struct node_t *new = create_node(val);

	for (p = *pnext; p != NULL; pnext = &(p->next), p = p->next) {
		if (p->val > new->val) {
			/* inserisci new dopo pnext, ossia prima di p */
			add_next(new, pnext);
			return;
		}
	}

	/* l'elemento va inserito in fondo alla lista */
	add_next(new, pnext);
}
