#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "rngs.h"
#include "list.h"
#include "utils.h"

#define START         0.0	// initial time
#define STOP      30000.0	// terminal (close the door) time

struct sim_result {
	long njobs;
	double interarr_time;
	double avg_ts; //EXPORTED
	double avg_tq; //EXPORTED
	double service_time;
	double avg_nnode;
	double avg_nqueue;
	double util; //EXPORTED
};

static double GetArrival(const double lambda, const double instant)
{
	SelectStream(0);
	return instant + Exponential(1.0 / lambda);
}

static double GetService(const double mu)
{
	SelectStream(1);
	return Exponential(1.0 / mu);
}

#define SIMULATION(param)	simulation(param.m, param.lambda, param.mu, param.seed)

struct sim_result simulation(const unsigned int servers, double lambda, double mu, unsigned long seed)
{
	double node_area = 0.0;	/* time integrated number in the node  */
	double queue_area = 0.0;	/* time integrated number in the queue */
	double service_area = 0.0;	/* time integrated number in service   */

	unsigned long index = 0;		/* used to count departed jobs */
	unsigned long nnode = 0, nservice = 0;		/* number in the node, number in sevice  */
	
	double current = START; // current time
	double next; // next (most imminent) event time
	double last = START; // last arrival time
	
	double arrival = GetArrival(lambda, current);		// next arrival time
	double completion = STOP;		// next completion time
	 
	struct node_t *residual_list = NULL;

	PlantSeeds(seed);

	for (next = arrival; next < STOP; next = min(arrival, completion)) {
		if (nnode > 0) {	/* update integrals  */
			node_area += (next - current) * nnode;
			queue_area += (next - current) * (nnode - nservice);
			service_area += (next - current) * nservice;
			//printf("nnode=%lu, nservice=%lu, servers=%u\n", nnode, nservice, servers);
			assert((nnode > nservice &&  nservice == servers) || (nnode == nservice && nnode <= servers));
		}
		current = next;	/* advance the clock */

		if (current == arrival) {	/* process an arrival */
			if (nservice < servers) {	// we populate a server with a job
				nservice++;
				add_ordered(&residual_list, current + GetService(mu));
				completion = residual_list->val;
			}
			nnode++;
			arrival = GetArrival(lambda, current);
			if (arrival > STOP)
				last = current;
		} else {	/* process a completion */
			index++;
			nnode--;
			struct node_t * removed = residual_list; //DEBUG
			remove_next(&residual_list);
			struct node_t *pos;
			if (residual_list != NULL && removed == residual_list) {
				for_each(pos, residual_list) {
					printf("val=%lf \t", pos->val);
				}
				printf("\n");
			}
			assert(residual_list == NULL || removed != residual_list);
			if (nnode >= servers) { //queue was not empty
				add_ordered(&residual_list, current + GetService(mu));
			} else { // queue was empty
				nservice--;
			}
			completion = nnode > 0 ? residual_list->val : STOP;
		}
	}

	struct sim_result res = {
		.njobs = index,	.interarr_time = last / index,	.avg_ts = node_area / index,	.avg_tq = queue_area / index,
		.service_time = service_area / index, .avg_nnode = node_area / current, .avg_nqueue = queue_area / current, 
		.util = service_area / (current * servers)
	};
	return res;
}

#define SIMULATION_SB_NP(param)	simulation_sb_np(param.m, param.lambda, param.mu, param.seed)

struct sim_result simulation_sb_np(const unsigned int servers, double lambda, double mu, const unsigned long seed)
{
	double node_area = 0.0;	/* time integrated number in the node  */
	double queue_area = 0.0;	/* time integrated number in the queue */
	double service_area = 0.0;	/* time integrated number in service   */

	unsigned long index = 0;		/* used to count departed jobs */
	unsigned long nnode = 0, nservice = 0;		/* number in the node, number in sevice  */
	
	double current = START; // current time
	double next; // next (most imminent) event time
	double last = START; // last arrival time
	
	double arrival = GetArrival(lambda, current);		// next arrival time
	double completion =  STOP;		// next completion time
	 
	struct node_t *running_jobs = NULL;
	struct node_t *small_jobs = NULL;
	struct node_t *big_jobs = NULL;
	const double medium_time = 1.0 / mu;
	
	PlantSeeds(seed);

	for (next = arrival; next < STOP; next = min(arrival, completion)) {
		if (nnode > 0) {	/* update integrals  */
			node_area += (next - current) * nnode;
			queue_area += (next - current) * (nnode - nservice);
			service_area += (next - current) * nservice;
			assert((nnode > nservice &&  nservice == servers) || (nnode == nservice && nnode <= servers));
		}
		current = next;	/* advance the clock */

		if (current == arrival) {	/* process an arrival */
			if (nservice < servers) {	// we populate a server with a job
				nservice++;
				add_ordered(&running_jobs, current + GetService(mu));
				completion = running_jobs->val;
			}
			else {
				double temp_job =  GetService(mu);
				if (temp_job < medium_time)
					add_last(&small_jobs, temp_job);
				else
					add_last(&big_jobs, temp_job);
			}
			nnode++;
			arrival = GetArrival(lambda, current);
			if (arrival > STOP)
				last = current;
		} else {	/* process a completion */
			index++;
			nnode--;
			struct node_t * removed = running_jobs; //DEBUG
			remove_next(&running_jobs);
			assert(running_jobs == NULL || removed != running_jobs);
			
			if (nnode >= servers) { // queue was not empty
				if (small_jobs != NULL) {
					add_ordered(&running_jobs, small_jobs->val + current);
					remove_next(&small_jobs);
				}
				else {
					add_ordered(&running_jobs, big_jobs->val + current);
					remove_next(&big_jobs);
				}
			} else { // queue was empty
				nservice--;
			}
			completion = nnode > 0 ? running_jobs->val : STOP;
		}
	}

	struct sim_result res = {
		.njobs = index,	.interarr_time = last / index,	.avg_ts = node_area / index,	.avg_tq = queue_area / index,
		.service_time = service_area / index, .avg_nnode = node_area / current, .avg_nqueue = queue_area / current, 
		.util = service_area / (current * servers)
	};
	return res;
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

void write_output_csv(const char * output_path, struct sim_param *param, int nparam, unsigned long *seeds, int nseeds)
{
	char path[35];
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");	
		fprintf(res_file, "m,lambda,mu,seed,E(ts),E(tq),util\n");
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			printCSV(res_file, param[i], SIMULATION_SB_NP(param[i]));
		}
		fclose(res_file);
	}
	printf("done %s\n", path);
}

int main()
{
	struct sim_param *param = NULL;
	unsigned long *seeds = NULL;
	int nparam;
	
	FILE * http_param = fopen("./input/http_param.csv", "r");
	FILE * multi_param = fopen("./input/multi_param.csv", "r");
	FILE * http_param_sbnp = fopen("./input/http_sbnp_param.csv", "r");
	FILE * multi_param_sbnp = fopen("./input/multi_sbnp_param.csv", "r");
	FILE * seeds_file = fopen("./input/seed.csv", "r");
	
	if (http_param == NULL || multi_param == NULL || http_param_sbnp == NULL || multi_param_sbnp == NULL || seeds_file == NULL) {
		error_msg("error in fopen");
		return 0;
	}	
	
	const int nseeds = readSeed(seeds_file, &seeds);
	nparam = readCSV_param(http_param, &param);
	write_output_csv("./output/base/%s.csv", param, nparam, seeds, nseeds);
	nparam = readCSV_param(multi_param, &param);
	write_output_csv("./output/base/%s.csv", param, nparam, seeds, nseeds);
	
	nparam = readCSV_param(http_param_sbnp, &param);
	write_output_csv("./output/migliorato/%s.csv", param, nparam, seeds, nseeds);
	nparam = readCSV_param(multi_param_sbnp, &param);
	write_output_csv("./output/migliorato/%s.csv", param, nparam, seeds, nseeds);

	return EXIT_SUCCESS;
}

