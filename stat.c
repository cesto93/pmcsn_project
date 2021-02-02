#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

const double student_20[] = {1.72472,	2.08596,	2.52798,	2.84534,	3.55181,	3.84952};
const double student_10[] = {1.812,		2.228,		2.764,		3.169,		4.144,		4.587};

#define confidenza_90 0
#define confidenza_95 1
#define confidenza_98 2
#define confidenza_99 3
#define confidenza_998 4
#define confidenza_999 5

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

double confidence_delta(int confidence, double sample_stdev) {
	const int n = 20;
	return student_20[confidence] * sample_stdev / (n - 1);
}

void print_stat(const char *path)
{
	double ts_mean, ts_var, tq_mean, tq_var, util_mean, util_var;
	double ts_delta, tq_delta, util_delta;
	
	struct sim_result_exported sample;
	sample.avg_tq = NULL;
	sample.avg_ts = NULL;
	sample.util = NULL;
	FILE * sample_file = fopen(path, "r");
	
	if (sample_file == NULL)
		error_msg("error in fopen\n");
	
	int n = readCSV_res(sample_file, &sample);
	
	stat(sample.avg_ts, n, &ts_mean, &ts_var);
	stat(sample.avg_tq, n, &tq_mean, &tq_var);
	stat(sample.util, n, &util_mean, &util_var);
	ts_delta = confidence_delta(confidenza_98, sqrt(ts_var));
	tq_delta = confidence_delta(confidenza_98, sqrt(tq_var));
	util_delta = confidence_delta(confidenza_98, sqrt(util_var));
	
	printf("%s\n", path);
	
	printf("util:\t %.9lf, %.9lf\n", util_mean - util_delta, util_mean + util_delta);
	printf("tq:\t %.9lf, %.9lf\n", tq_mean - tq_delta, tq_mean + tq_delta);
	printf("ts:\t %.9lf, %.9lf\n", ts_mean - ts_delta, ts_mean + ts_delta);

}

int main() 
{
	print_stat("./output/http08_12.csv");
	print_stat("./output/http12_15.csv");
	print_stat("./output/http15_20.csv");
	print_stat("./output/http20_24.csv");
	print_stat("./output/http24_08.csv");
}
