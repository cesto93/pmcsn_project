#ifndef TRANSIENT_H
#define TRANSIENT_H

struct sim_result transient_simul(unsigned int servers, double lambda, double mu, unsigned long *seed, long seconds, 
				  struct class_info *tot, struct node_t **list);
struct sim_result_prio transient_simul_prio(unsigned int servers, double lambda, double mu, unsigned long *seed, double p1, 
					    long seconds, struct class_info *c1, struct class_info *c2, struct class_info *tot,
					    struct node_t **list);				  

#endif
