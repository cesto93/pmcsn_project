#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "rngs.h"
#include "list.h"
#include "utils.h"
#include "msq.h"

#define START	0.0	// initial time
#define STOP	30000.0	// terminal (close the door) time

typedef struct sim_result (*func_sim)(const unsigned int servers, double lambda, double mu, const unsigned long seed);

struct class_info {
	long nqueue; // number in queue
	long nservice; // number in sevice
	long index; /* used to count departed jobs */
	double node_area; /* time integrated number in the node  */
	double queue_area; /* time integrated number in the queue */
	double service_area; /* time integrated number in service   */
};

#define update_class_area(c_info, next, current) do {					\
	c_info.node_area += (next - current) * (c_info.nservice + c_info.nqueue);	\
	c_info.queue_area += (next - current) * (c_info.nqueue);			\
	c_info.service_area += (next - current) * (c_info.nservice);			\
	} while(0)

static double GetInterarrival(const double lambda)
{
	SelectStream(0);
	return Exponential(1.0 / lambda);
}

static double GetService(const double mu)
{
	SelectStream(1);
	return Exponential(1.0 / mu);
}

static int GetClass(const double p)
{
	SelectStream(2);
	return Bernoulli(p);
}

struct sim_result simulation(const unsigned int servers, double lambda, double mu, unsigned long seed)
{	
	double current = START; // current time
	double arrival = current + GetInterarrival(lambda); // next arrival time
	double completion = STOP; // next completion time
	double last = START; // last arrival time
	
	struct class_info tot = {0,0,0,0.0,0.0,0.0};
	 
	struct node_t *residual_list = NULL;

	PlantSeeds(seed);

	for (double next = arrival; next < STOP; next = min(arrival, completion)) {
		if (tot.nservice > 0) {	/* update integrals  */
			update_class_area(tot, next, current);
		}
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
			if (arrival > STOP)
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
			completion = tot.nservice > 0 ? residual_list->val : STOP;
		}
	}

	struct sim_result res = {
		.avg_ts = tot.node_area / tot.index, .avg_tq = tot.queue_area / tot.index, .util = tot.service_area / (current * servers),
		.service_time = tot.service_area / tot.index, .avg_nnode = tot.node_area / current, .avg_nqueue = tot.queue_area / current,
		.njobs = tot.index, .interarr_time = last / tot.index, 
	};
	return res;
}

struct sim_result_prio simulation_prio(const unsigned int servers, double lambda, double mu, unsigned long seed, double p1)
{	
	double current = START; // current time
	double arrival = current + GetInterarrival(lambda); // next arrival time
	double completion = STOP; // next completion time
	 
	struct node_t *residual_list = NULL;
	
	struct class_info c1 = {0,0,0,0.0,0.0,0.0};
	struct class_info c2 = {0,0,0,0.0,0.0,0.0};
	struct class_info tot = {0,0,0,0.0,0.0,0.0};

	PlantSeeds(seed);

	for (double next = arrival; next < STOP; next = min(arrival, completion)) {
		if (tot.nservice > 0) {	/* update integrals  */
			update_class_area(tot, next, current);
			update_class_area(c1, next, current);
			update_class_area(c2, next, current);
		}
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
			completion = tot.nservice > 0 ? residual_list->val : STOP;
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

void write_output_csv(const char *output_path, struct sim_param *param, int nparam, unsigned long *seeds, int nseeds)
{
	char path[35];
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");	
		fprintf(res_file, "m,lambda,mu,seed,E(ts),E(tq),util\n");
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			struct sim_result res = simulation(param[i].m, param[i].lambda, param[i].mu, param[i].seed);
			printCSV(res_file, param[i], res);
		}
		fclose(res_file);
	}
	printf("done %s\n", path);
}

void write_output_csv_prio(const char *output_path, struct sim_param_prio *param, int nparam, unsigned long *seeds, int nseeds)
{
	char path[35];
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");	
		printCSV_prio_header(res_file);
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			struct sim_result_prio res = simulation_prio(param[i].m, param[i].lambda, param[i].mu, param[i].seed, param[i].p1);
			printCSV_prio(res_file, param[i], res);
		}
		fclose(res_file);
	}
	printf("done %s\n", path);
}

void printRes(struct sim_result res) 
{
	printf("for %ld jobs\n", res.njobs);
	printf("average interarrival time = %6.6f\n", res.interarr_time);
	printf("average ts \t = %6.6f\n", res.avg_ts);
	printf("average tq \t = %6.6f\n", res.avg_tq);
	printf("average service time \t = %6.6f\n", res.service_time);
	printf("average # in the node \t = %6.6f\n", res.avg_nnode);
	printf("average # in the queue \t = %6.6f\n", res.avg_nqueue);
	printf("utilization \t = %6.6f\n", res.util);
}

int main()
{
	struct sim_param *param = NULL;
	struct sim_param_prio *param_prio = NULL;
	unsigned long *seeds = NULL;
	int nparam;
	
	FILE * http_param = fopen("./input/http_param.csv", "r");
	FILE * multi_param = fopen("./input/multi_param.csv", "r");
	FILE * http_param_prio = fopen("./input/http_prio_param.csv", "r");
	FILE * multi_param_prio = fopen("./input/multi_prio_param.csv", "r");
	FILE * seeds_file = fopen("./input/seed.csv", "r");
	
	if (http_param == NULL || multi_param == NULL || http_param_prio == NULL || multi_param_prio == NULL || seeds_file == NULL) {
		error_msg("error in fopen");
		return 0;
	}	
	
	const int nseeds = readSeed(seeds_file, &seeds);
	
	nparam = readCSV_param(http_param, &param);
	write_output_csv("./output/base/%s.csv", param, nparam, seeds, nseeds);
	nparam = readCSV_param(multi_param, &param);
	write_output_csv("./output/base/%s.csv", param, nparam, seeds, nseeds);
	free(param);
	
	nparam = readCSV_param_prio(http_param_prio, &param_prio);
	write_output_csv_prio("./output/priority/%s.csv", param_prio, nparam, seeds, nseeds);
	nparam = readCSV_param_prio(multi_param_prio, &param_prio);
	write_output_csv_prio("./output/priority/%s.csv", param_prio, nparam, seeds, nseeds);
	free(param_prio);

	return EXIT_SUCCESS;
}

