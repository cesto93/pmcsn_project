#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

struct sim_result_exported {
	int m;
	int lambda;
	int mu;
	double *avg_ts; //EXPORTED
	double *avg_tq; //EXPORTED
	double *util; //EXPORTED
};

int readCSV_res(FILE *file, struct sim_result_exported * result) 
{
	int i;
	double temp_ts, temp_tq, temp_util;
	if (fscanf(file, "%*[^\n]\n") == -1) 
		error_msg("error in readCSV");
		
	for (i = 0; fscanf(file, "%d,%d,%d,%*[^,],%lf,%lf,%lf\n", 
				&(result->m), &(result->lambda), &(result->mu), &(temp_ts), &(temp_tq), &(temp_util)) == 6;
				 i++) {
		result->avg_ts = realloc(result->avg_ts, (i + 1) * sizeof(double));
		result->avg_tq = realloc(result->avg_tq, (i + 1) * sizeof(double));
		result->util = realloc(result->util, (i + 1) * sizeof(double));
		if (result->avg_ts == NULL || result->avg_tq == NULL || result->util == NULL) {
			error_msg("error in realloc");
			free(result->avg_ts);
			free(result->avg_tq);
			free(result->avg_ts);
			return 0;
		}
		result->avg_ts[i] = temp_ts;
		result->avg_tq[i] = temp_tq;
		result->util[i] = temp_util;
	}
	return i;
}

void stat(double val[], int size, double *mean, double *var) 
{
	int n = 0;
	*mean = 0.0;
	double sum = 0.0;
	for (int i = 0; i < size; i++) {
		n++;
		double diff = val[i] - *mean;
		sum  += diff * diff * (n - 1.0) / n;
		*mean += diff / n;
	}
	*var = sum / n;
}

void print_stat(const char *path)
{
	double ts_mean, ts_var, tq_mean, tq_var, util_mean, util_var;
	struct sim_result_exported sample;
	sample.avg_tq = NULL;
	sample.avg_ts = NULL;
	sample.util = NULL;
	FILE * sample_file = fopen(path, "r");
	int n = readCSV_res(sample_file, &sample);
	
	stat(sample.avg_ts, n, &ts_mean, &ts_var);
	stat(sample.avg_tq, n, &tq_mean, &tq_var);
	stat(sample.util, n, &util_mean, &util_var);
	
	printf("util: mean %.9lf var %.9lf\n", util_mean, util_var);
	printf("ts: mean %.9lf var %.9lf\n", ts_mean, ts_var);
	printf("tq: mean %.9lf var %.9lf\n", tq_mean, tq_var);
}

int main() 
{
	print_stat("http08_12.csv");
}
