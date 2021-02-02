#ifndef LIST_H
#define LIST_H

struct node_t {
	double val;
	struct node_t *next;
};

void add_ordered(struct node_t **pnext, double val);
void add_last(struct node_t **pnext, double val);
void remove_next(struct node_t **pnext);

#define for_each(pos, head) for ((pos) = (head); (pos) != NULL; (pos) = (pos)->next)

#endif
