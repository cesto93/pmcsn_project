#include <stdio.h>
#include <math.h>
#include "rngs.h"
#include "list.h"

#define START         0.0	// initial time
#define STOP      30000.0	// terminal (close the door) time
#define INFINITE   (100.0 * STOP)	// must be much larger than STOP

#define INPUT "./param.csv"

#define MIN(a,b, res) 	\
	do {				\
		if (a < b)		\
			res = a;	\
		else			\
			res = b;	\
	} while(0)
	
struct sim_param {
	int m;
	double lambda;
	double mu;
	int seed;
};

struct sim_result {
	long njobs;
	double interarr_time;
	double avg_ts;
	double avg_tq;
	double service_time;
	double avg_nnode;
	double avg_nqueue;
	double util;
};

double Exponential(double m)
{
	return (-m * log(1.0 - Random()));
}

double GetArrival(double lambda, double instant)
{
	SelectStream(0);
	return instant + Exponential(1.0 / lambda);
}

double GetService(double mu)
{
	SelectStream(1);
	return Exponential(1.0 / mu);
}

#define SIMULATION(param)	simulation(param.m, param.lambda, param.mu, param.seed)

struct sim_result simulation(int servers, double lambda, double mu, int seed)
{
	double node = 0.0;	/* time integrated number in the node  */
	double queue = 0.0;	/* time integrated number in the queue */
	double service = 0.0;	/* time integrated number in service   */

	long index = 0;		/* used to count departed jobs         */
	long nnode = 0;		/* number in the node                  */
	long nservice = 0;	/* number in sevice                  */

	double arrival;		/* next arrival time                   */
	double completion;	/* next completion time                */
	double current;		/* current time                        */
	double next;		/* next (most imminent) event time     */
	double last;		/* last arrival time                   */

	struct node_t *residual_list = NULL;

	PlantSeeds(seed);
	current = START;	/* set the clock                         */
	last = START;
	arrival = GetArrival(lambda, current);	/* schedule the first arrival */
	completion = INFINITE;	/* the first event can't be a completion */
	
	while (arrival < STOP) {
		MIN(arrival, completion, next);	/* next event time   */
		if (nnode > 0) {	/* update integrals  */
			node += (next - current) * nnode;
			queue += (next - current) * (nnode - nservice);
			service += (next - current) * nservice;
		}
		current = next;	/* advance the clock */

		if (current == arrival) {	/* process an arrival */
			nnode++;
			if (nservice < servers) {	//we populate a server with a job
				nservice++;
				add_ordered(&residual_list, current + GetService(mu));
				completion = residual_list->val;
				//printf("val %f\n", residual_list->val);
			}
			arrival = GetArrival(lambda, current);
			if (arrival > STOP) {
				last = current;
				arrival = INFINITE;
			}
		} else {	/* process a completion */
			index++;
			nnode--;
			remove_next(&residual_list);
			if (nnode >= servers) {
				add_ordered(&residual_list, current + GetService(mu));
				completion = residual_list->val;
			} else {
				nservice--;
			}

			if (nnode > 0)
				completion = residual_list->val;
			else
				completion = INFINITE;
		}
	}
	struct sim_result res = {
		.njobs = index,
		.interarr_time = last / index,
		.avg_ts = node / index,
		.avg_tq = queue / index,
		.service_time = service / index,
		.avg_nnode = node / current,
		.avg_nqueue = queue / current,
		.util = service / (current * servers)
	};
	return res;
}

void printRes(struct sim_result res) 
{
	printf("\nfor %ld jobs\n", res.njobs);
	printf(" average interarrival time = %6.6f\n", res.interarr_time);
	printf(" average ts ............ = %6.6f\n", res.avg_ts);
	printf(" average tq ........... = %6.6f\n", res.avg_tq);
	printf(" average service time .... = %6.6f\n", res.service_time);
	printf(" average # in the node ... = %6.6f\n", res.avg_nnode);
	printf(" average # in the queue .. = %6.6f\n", res.avg_nqueue);
	printf(" utilization ............. = %6.6f\n", res.util);
}

void printCSV(struct sim_param param, struct sim_result res) 
{
	printf("%d \t %.6f \t %.6f \t %d \t %.6f \t %.6f \t %.6f\n", 
			param.m, param.lambda, param.mu, param.seed, res.avg_ts, res.avg_tq, res.util);
}

int readCSV(FILE *file, struct sim_param * param) 
{
	if (fscanf(file, "%d,%lf,%lf,%d", &(param->m), &(param->lambda), &(param->mu), &(param->seed)) == -1) {
		return 1;
	}
	return 0;
}

int main(void)
{
	struct sim_param param;
	FILE * file = fopen(INPUT, "r");
	
	if (file == NULL) {
		error_msg("errore in fopen");
		return 0;
	}
	if (fscanf(file, "%*[^\n]\n") == -1) 
		error_msg("errore in readCSV");
	
	printf("m \t lambda \t mu \t seed \t E(ts) \t E(tq) \t util\n");
	while (!readCSV(file, &param)) {
		printCSV(param, SIMULATION(param));
	}
	
	return 0;
}
