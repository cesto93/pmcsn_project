#include <stdlib.h>

#include "rngs.h"
#include "utils.h"
#include "list.h"
#include "msq.h"

struct sim_result transient_simul(unsigned int servers, double lambda, double mu, unsigned long *seed, long seconds, 
				    struct class_info *tot, struct node_t **residual_list)
{	
	double current = 0.0; // current time
	double arrival = current + GetInterarrival(lambda); // next arrival time
	double completion = seconds; // next completion time
	double last = 0.0; // last arrival time
	
	PlantSeeds(*seed);
	
	tot->service_area = 0;
	tot->queue_area = 0;
	tot->node_area = 0;
	tot->index = 0;
	
	while (tot->nservice < servers && tot->nqueue != 0) {
		tot->nservice++;
		tot->nqueue--;
		add_ordered(residual_list, current + GetService(mu));
	}
	
	if (tot->nservice > 0)
		completion = (*residual_list)->val;	
	
	for (double next = arrival; next < seconds; next = min(arrival, completion)) {
		
		if (tot->nservice > 0) {	/* update integrals  */
			update_class_area(*tot, next, current);
		}
		
		current = next;	/* advance the clock */

		if (current == arrival) { /* process an arrival */
			if (tot->nservice < servers) { // we populate a server with a job
				tot->nservice++;
				add_ordered(residual_list, current + GetService(mu));
				completion = (*residual_list)->val;
			} else { // queue
				tot->nqueue++;
			}
			arrival = current + GetInterarrival(lambda);
			if (arrival > seconds)
				last = current;
		} else { /* process a completion */
			tot->index++;
			tot->nservice--;
			remove_next(residual_list);
			if (tot->nqueue > 0 && tot->nservice < servers) { // queue was not empty
				add_ordered(residual_list, current + GetService(mu));
				tot->nservice++;
				tot->nqueue--;
			}
			completion = tot->nservice > 0 ? (*residual_list)->val : seconds;
		}
	}
	
	for (struct node_t *temp = *residual_list; temp !=NULL; temp = temp->next)
		temp->val -= current;

	struct sim_result res;
	set_sim_result(&res, *tot, current, last, servers);
	return res;
}

struct sim_result_prio transient_simul_prio(unsigned int servers, double lambda, double mu, unsigned long *seed, double p1, 
					    long seconds, struct class_info *c1, struct class_info *c2, struct class_info *tot,
					    struct node_t **residual_list)
{	
	double current = 0.0; // current time
	double arrival = current + GetInterarrival(lambda); // next arrival time
	double completion = seconds; // next completion time

	PlantSeeds(*seed);
	tot->service_area = 0;
	tot->queue_area = 0;
	tot->node_area = 0;
	tot->index = 0;
	
	while (tot->nservice < servers && tot->nqueue != 0) {
		tot->nservice++;
		tot->nqueue--;
		if (c1->nqueue > 0) {
			c1->nqueue--;
			c1->nservice++;
			add_ordered_prio(residual_list, current + GetService(mu), PREMIUM);
		} else {
			c2->nqueue--;
			c2->nservice++;
			add_ordered_prio(residual_list, current + GetService(mu), STANDARD);
		}
	}
	
	if (tot->nservice > 0)
		completion = (*residual_list)->val;

	for (double next = arrival; next < seconds; next = min(arrival, completion)) {
		if (tot->nservice > 0) {	/* update integrals  */
			update_class_area(*tot, next, current);
			update_class_area(*c1, next, current);
			update_class_area(*c2, next, current);
		}
		current = next;	/* advance the clock */

		if (current == arrival) { /* process an arrival */
			if (tot->nservice < servers) { // we populate a server with a job
				tot->nservice++;
				if (GetClass(p1)) {
					c1->nservice++;
					add_ordered_prio(residual_list, current + GetService(mu), PREMIUM);
				} else {
					c2->nservice++;
					add_ordered_prio(residual_list, current + GetService(mu), STANDARD);
				}
				completion = (*residual_list)->val;
			} else { // queue
				tot->nqueue++;	
				if (GetClass(p1))
					c1->nqueue++;
				else
					c2->nqueue++;
			}
			arrival = current + GetInterarrival(lambda);
		} else { /* process a completion */
			tot->index++;
			tot->nservice--;
			if ((*residual_list)->priority == PREMIUM) {
				c1->index++;
				c1->nservice--;
			} else {
				c2->index++;
				c2->nservice--;
			}
			remove_next(residual_list);
			if (tot->nqueue > 0 && tot->nservice < servers) { // queue not empty there are empty servers
				tot->nqueue--;
				tot->nservice++;
				if (c1->nqueue > 0) {
					c1->nqueue--;
					c1->nservice++;
					add_ordered_prio(residual_list, current + GetService(mu), PREMIUM);
				} else {
					c2->nqueue--;
					c2->nservice++;
					add_ordered_prio(residual_list, current + GetService(mu), STANDARD);
				}
			}
			completion = tot->nservice > 0 ? (*residual_list)->val : seconds;
		}
	}
	
	for (struct node_t *temp = *residual_list; temp !=NULL; temp = temp->next)
		temp->val -= current;

	struct sim_result_prio res = {
		.avg_ts = tot->node_area / tot->index, .avg_ts1 = c1->node_area / c1->index, .avg_ts2 = c2->node_area / c2->index,
		.avg_tq = tot->queue_area / tot->index, .avg_tq1 = c1->queue_area / c1->index, .avg_tq2 = c2->queue_area / c2->index, 
		.util = tot->service_area / (current * servers), .util1 = c1->service_area / (current * servers), 
		.util2 = c2->service_area / (current * servers),
		.service_time = tot->service_area / tot->index, .avg_nnode = tot->node_area / current, .avg_nqueue = tot->queue_area / current,
		.njobs = tot->index
	};
	return res;
}
