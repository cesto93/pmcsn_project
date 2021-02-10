#include <stdlib.h>
#include <assert.h>

#include "steady.h"
#include "rngs.h"
#include "utils/utils.h"

/* steadystate simulation start at 0.0 */
struct sim_result simul(const unsigned int servers, double lambda, double mu, unsigned long seed, long seconds)
{	
	double current = 0.0; // current time
	double arrival = current + GetInterarrival(lambda); // next arrival time
	double completion = seconds; // next completion time
	double last = 0.0; // last arrival time
	
	struct class_info tot = {0,0,0,0.0,0.0,0.0};
	 
	struct node_t *residual_list = NULL;

	PlantSeeds(seed);

	for (double next = arrival; next < seconds; next = min(arrival, completion)) {
		if (tot.nservice > 0) {	/* update integrals  */
			update_class_area(tot, next, current);
		}
		assert(tot.nservice <= servers);
		assert((tot.nqueue == 0 && tot.nservice == 0) || tot.nservice != 0);
		current = next;	/* advance the clock */

		if (current == arrival) { /* process an arrival */
			if (tot.nservice < servers) { // we populate a server with a job
				tot.nservice++;
				add_ordered(&residual_list, current + GetService(mu));
				completion = residual_list->val;
			} else { // queue
				tot.nqueue++;
			}
			arrival = current + GetInterarrival(lambda);
			if (arrival > seconds)
				last = current;
		} else { /* process a completion */
			tot.index++;
			tot.nservice--;
			remove_next(&residual_list);
			if (tot.nqueue > 0) { // queue was not empty
				add_ordered(&residual_list, current + GetService(mu));
				tot.nservice++;
				tot.nqueue--;
			}
			completion = tot.nservice > 0 ? residual_list->val : seconds;
		}
	}
	struct sim_result res;
	set_sim_result(&res, tot, current, last, servers);
	return res;
}

/* steadystate simulation start at 0.0 */
struct sim_result_prio simul_prio(const unsigned int servers, double lambda, double mu, unsigned long seed, double p1, long seconds)
{	
	double current = 0.0; // current time
	double arrival = current + GetInterarrival(lambda); // next arrival time
	double completion = seconds; // next completion time
	 
	struct node_t *residual_list = NULL;
	
	struct class_info c1 = {0,0,0,0.0,0.0,0.0};
	struct class_info c2 = {0,0,0,0.0,0.0,0.0};
	struct class_info tot = {0,0,0,0.0,0.0,0.0};

	PlantSeeds(seed);

	for (double next = arrival; next < seconds; next = min(arrival, completion)) {
		if (tot.nservice > 0) {	/* update integrals  */
			update_class_area(tot, next, current);
			update_class_area(c1, next, current);
			update_class_area(c2, next, current);
		}
		assert(tot.nservice <= servers);
		assert((tot.nqueue == 0 && tot.nservice == 0) || tot.nservice != 0);
		assert(tot.nservice == c1.nservice + c2.nservice);
		assert(tot.nqueue == c1.nqueue + c2.nqueue);
		current = next;	/* advance the clock */

		if (current == arrival) { /* process an arrival */
			if (tot.nservice < servers) { // we populate a server with a job
				tot.nservice++;
				if (GetClass(p1)) {
					c1.nservice++;
					add_ordered_prio(&residual_list, current + GetService(mu), PREMIUM);
				} else {
					c2.nservice++;
					add_ordered_prio(&residual_list, current + GetService(mu), STANDARD);
				}
				completion = residual_list->val;
			} else { // queue
				tot.nqueue++;	
				if (GetClass(p1))
					c1.nqueue++;
				else
					c2.nqueue++;
			}
			arrival = current + GetInterarrival(lambda);
		} else { /* process a completion */
			tot.index++;
			tot.nservice--;
			if (residual_list->priority == PREMIUM) {
				c1.index++;
				c1.nservice--;
			} else {
				c2.index++;
				c2.nservice--;
			}
			remove_next(&residual_list);
			if (tot.nqueue > 0) { //queue was not empty
				tot.nqueue--;
				tot.nservice++;
				if (c1.nqueue > 0) {
					c1.nqueue--;
					c1.nservice++;
					add_ordered_prio(&residual_list, current + GetService(mu), PREMIUM);
				} else {
					c2.nqueue--;
					c2.nservice++;
					add_ordered_prio(&residual_list, current + GetService(mu), STANDARD);
				}
			}
			completion = tot.nservice > 0 ? residual_list->val : seconds;
		}
	}

	struct sim_result_prio res = {
		.avg_ts = tot.node_area / tot.index, .avg_ts1 = c1.node_area / c1.index, .avg_ts2 = c2.node_area / c2.index,
		.avg_tq = tot.queue_area / tot.index, .avg_tq1 = c1.queue_area / c1.index, .avg_tq2 = c2.queue_area / c2.index, 
		.util = tot.service_area / (current * servers), .util1 = c1.service_area / (current * servers), 
		.util2 = c2.service_area / (current * servers),
		.service_time = tot.service_area / tot.index, .avg_nnode = tot.node_area / current, .avg_nqueue = tot.queue_area / current,
		.njobs = tot.index
	};
	return res;
}
