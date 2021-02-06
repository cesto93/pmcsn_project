#ifndef LIST_H
#define LIST_H

struct node_t {
	double val;
	int priority;
	struct node_t *next;
};

void add_ordered_prio(struct node_t **pnext, double val, int priority);
void add_last_prio(struct node_t **pnext, double val, int priority);
void remove_next(struct node_t **pnext);

#define for_each(pos, head) for ((pos) = (head); (pos) != NULL; (pos) = (pos)->next)

#define add_last(pnext, val) add_last_prio(pnext, val, 0)
#define add_ordered(pnext, val) add_ordered_prio(pnext, val, 0)

#define PREMIUM 0
#define STANDARD 1

#endif
