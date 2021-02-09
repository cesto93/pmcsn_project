#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "rngs.h"
#include "utils/list.h"
#include "utils/utils.h"
#include "utils/csv.h"
#include "msq.h"
#include "transient.h"
#include "steady.h"

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
	if (tot.index > 0) {
		res->avg_ts = tot.node_area / tot.index;
		res->avg_tq = tot.queue_area / tot.index;
		res->service_time = tot.service_area / tot.index;
		res->interarr_time = last / tot.index;
	} else {
		res->avg_ts = 0;
		res->avg_tq = 0;
		res->service_time = 0;
		res->interarr_time = 0;
	}
	
	if (current > 0.0) {
		res->util = tot.service_area / (current * servers);
		res->avg_nnode = tot.node_area / current;
		res->avg_nqueue = tot.queue_area / current;
	} else {
		res->util = 0;
		res->avg_nnode = 0;
		res->avg_nqueue = 0;		
	}
	res->njobs = tot.index;
}

void set_sim_result_prio(struct sim_result_prio *res, struct class_info tot, struct class_info c1, struct class_info c2, 
			 double current, int servers)
{
	if (tot.index != 0) {
		res->avg_ts = tot.node_area / tot.index;
		res->avg_tq = tot.queue_area / tot.index;
		res->service_time = tot.service_area / tot.index;
	} else { // avoid nan
		res->avg_ts = 0;
		res->avg_tq = 0;
		res->service_time = 0;
	}
	
	if (c1.index != 0) {
		res->avg_ts1 = c1.node_area / c1.index;
		res->avg_tq1 = c1.queue_area / c1.index;
	} else {
		res->avg_ts1 = 0;
		res->avg_tq1 = 0;
	}
	
	if (c2.index != 0) {
		res->avg_ts2 = c2.node_area / c2.index;
		res->avg_tq2 = c2.queue_area / c2.index;
	} else {
		res->avg_ts2 = 0;
		res->avg_tq2 = 0;
	}
	
	
	if (current != 0.0) {
		res->util = tot.service_area / (current * servers);
		res->util1 = c1.service_area / (current * servers);
		res->util2 = c2.service_area / (current * servers);
		res->avg_nnode = tot.node_area / current;
		res->avg_nqueue = tot.queue_area / current;
	} else { 	// avoid nan
		res->util = 0;
		res->util1 = 0;
		res->util2 = 0;
		res->avg_nnode = 0;
		res->avg_nqueue = 0;		
	}
	res->njobs = tot.index;
}		

void write_csv_steady(const char *output_path, const char *input_path, unsigned long *seeds, int nseeds)
{
	struct sim_param *param = NULL;
	int nparam = readCSV_param(input_path, &param);
	char path[45];
	for (int i = 0; i < nparam; i++) {
		sprintf(path, output_path, param[i].label);
		FILE * res_file = fopen(path, "w");	
		printCSV_header(res_file);
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
		// initialize residual_list and tot with a full run
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
		
		// initialize residual_list and tot,c1,c2 with a full run
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
	
	/*write_csv_steady("./output/steady/standard/%s.csv", "./input/steady/http_param.csv", seeds, nseeds);
	write_csv_steady("./output/steady/standard/%s.csv", "./input/steady/multi_param.csv", seeds, nseeds);
	
	write_csv_prio_steady("./output/steady/priority/%s.csv", "./input/steady/http_prio_param.csv", seeds, nseeds);
	write_csv_prio_steady("./output/steady/priority/%s.csv", "./input/steady/multi_prio_param.csv", seeds, nseeds);
	
	write_csv_transient("./output/transient/standard/%s.csv", "./input/transient/http_param.csv", seeds, nseeds);
	write_csv_transient("./output/transient/standard/%s.csv", "./input/transient/multi_param.csv", seeds, nseeds);*/
	
	write_csv_prio_transient("./output/transient/priority/%s.csv", "./input/transient/http_prio_param.csv", seeds, nseeds);
	write_csv_prio_transient("./output/transient/priority/%s.csv", "./input/transient/multi_prio_param.csv", seeds, nseeds);

	return EXIT_SUCCESS;
}

