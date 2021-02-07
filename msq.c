#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "rngs.h"
#include "list.h"
#include "utils.h"
#include "msq.h"
#include "transient.h"

#define STEADY_STOP	30000.0	// terminal (close the door) time

double GetInterarrival(const double lambda)
{
	SelectStream(0);
	return Exponential(1.0 / lambda);
}

double GetService(const double mu)
{
	SelectStream(1);
	return Exponential(1.0 / mu);
}

int GetClass(const double p)
{
	SelectStream(2);
	return Bernoulli(p);
}

void set_sim_result(struct sim_result *res, struct class_info tot, double current, double last, int servers)
{
	res->avg_ts = tot.node_area / tot.index;
	res->avg_tq = tot.queue_area / tot.index, 
	res->util = tot.service_area / (current * servers);
	res->service_time = tot.service_area / tot.index;
	res->avg_nnode = tot.node_area / current;
	res->avg_nqueue = tot.queue_area / current;
	res->njobs = tot.index;
	res->interarr_time = last / tot.index;
}		

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

void write_csv_steady(const char *output_path, const char *input_path, unsigned long *seeds, int nseeds)
{
	struct sim_param *param = NULL;
	int nparam = readCSV_param(input_path, &param);
	char path[45];
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");	
		fprintf(res_file, "m,lambda,mu,seed,E(ts),E(tq),util\n");
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			struct sim_result res = simul(param[i].m, param[i].lambda, param[i].mu, param[i].seed, STEADY_STOP);
			printCSV(res_file, param[i], res);
		}
		fclose(res_file);
	}
	printf("done %s\n", path);
}

void write_csv_prio_steady(const char *output_path, const char *input_path, unsigned long *seeds, int nseeds)
{
	struct sim_param_prio *param = NULL;
	int nparam = readCSV_param_prio(input_path, &param);
	char path[45];
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");	
		printCSV_prio_header(res_file);
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			struct sim_result_prio res = simul_prio(param[i].m, param[i].lambda, param[i].mu, param[i].seed, param[i].p1,
								STEADY_STOP);
			printCSV_prio(res_file, param[i], res);
		}
		fclose(res_file);
	}
	printf("done %s\n", path);
}

void write_csv_transient(const char *output_path, const char *input_path, unsigned long *seeds, int nseeds)
{
	struct sim_param *param = NULL;
	int nparam = readCSV_param(input_path, &param);
	
	char path[50];
	struct class_info tot = {0,0,0,0.0,0.0,0.0};
	struct sim_result res[nparam][nseeds];
	struct node_t *residual_list = NULL; 
	
	
	for (int i = 0; i < nseeds; i++) {
		unsigned long temp_seed = seeds[i];
		int last =  nparam - 1;
		transient_simul(param[last].m, param[last].lambda, param[last].mu, &temp_seed, param[last].seconds, &tot, &residual_list);
		for (int j = 0; j < nparam; j++) {
			res[j][i] = transient_simul(param[j].m, param[j].lambda, param[j].mu, &temp_seed, param[j].seconds, &tot,
						    &residual_list);
		}
		free_list(&residual_list);
		empty_class_info(tot);
	}
	
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");
		fprintf(res_file, "m,lambda,mu,seed,E(ts),E(tq),util\n");
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			printCSV(res_file, param[i], res[i][j]);
		}
		fclose(res_file);	
	}	
	printf("done %s\n", path);
}

void write_csv_prio_transient(const char *output_path, const char *input_path, unsigned long *seeds, int nseeds)
{
	struct sim_param_prio *param = NULL;
	int nparam = readCSV_param_prio(input_path, &param);
	
	char path[50];
	struct class_info c1 = {0,0,0,0.0,0.0,0.0};
	struct class_info c2 = {0,0,0,0.0,0.0,0.0};
	struct class_info tot = {0,0,0,0.0,0.0,0.0};
	struct sim_result_prio res[nparam][nseeds];
	struct node_t *residual_list = NULL;
	
	for (int i = 0; i < nseeds; i++) {
		unsigned long temp_seed = seeds[i];
		const int last = nparam - 1;
		transient_simul_prio(param[last].m, param[last].lambda, param[last].mu, &temp_seed, param[last].p1, param[last].seconds, 
				     &c1, &c2, &tot, &residual_list);
		for (int j = 0; j < nparam; j++) {
			res[j][i] = transient_simul_prio(param[j].m, param[j].lambda, param[j].mu, &temp_seed, param[j].p1, 
							 param[j].seconds, &c1, &c2, &tot, &residual_list);	
		}
		free_list(&residual_list);
		empty_class_info(c1);
		empty_class_info(c2);
		empty_class_info(tot);
	}
	
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");
		printCSV_prio_header(res_file);
		for (int j = 0; j < nseeds; j++) {
			param[i].seed = seeds[j];
			printCSV_prio(res_file, param[i], res[i][j]);
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
	unsigned long *seeds = NULL;		
	const int nseeds = readSeed("./input/seed.csv", &seeds);
	
	/*write_csv_steady("./output/steady/standard/%s.csv", "./input/http_param.csv", seeds, nseeds);
	write_csv_steady("./output/steady/standard/%s.csv", "./input/multi_param.csv", seeds, nseeds);
	
	write_csv_prio_steady("./output/steady/priority/%s.csv", "./input/http_prio_param.csv", seeds, nseeds);
	write_csv_prio_steady("./output/steady/priority/%s.csv", "./input/multi_prio_param.csv", seeds, nseeds);*/
	
	write_csv_transient("./output/transient/standard/%s.csv", "./input/transient/http_param.csv", seeds, nseeds);
	write_csv_transient("./output/transient/standard/%s.csv", "./input/transient/multi_param.csv", seeds, nseeds);
	
	write_csv_prio_transient("./output/transient/priority/%s.csv", "./input/transient/http_prio_param.csv", seeds, nseeds);
	write_csv_prio_transient("./output/transient/priority/%s.csv", "./input/transient/multi_prio_param.csv", seeds, nseeds);

	return EXIT_SUCCESS;
}

