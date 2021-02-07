#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

const double student_20[] = {1.72472, 2.08596, 2.52798, 2.84534, 3.55181, 3.84952};
const double student_19[] = {1.729, 2.093, 2.539, 2.861, 3.579, 3.883};
const double student_10[] = {1.812, 2.228, 2.764, 3.169, 4.144, 4.587};

#define confidenza_90 0
#define confidenza_95 1
#define confidenza_98 2
#define confidenza_99 3
#define confidenza_998 4
#define confidenza_999 5

struct sim_data {
	int m;
	int lambda;
	int mu;
	double *avg_ts;
	double *avg_tq;
	double *util;
};

struct sim_prio_data {
	int m;
	int lambda;
	int mu;
	double *avg_ts; 
	double *avg_ts1;
	double *avg_ts2;
	double *avg_tq;
	double *avg_tq1;
	double *avg_tq2;
	double *util;
	double *util1;
	double *util2;
	double *service_time;
	double *sd2;
};
#define printCSV_stat_header(file) fprintf(file, "label,E(ts)_l,E(ts)_r,E(tq)_l,E(tq)_r,util_l,util_r\n")

#define printCSV_stat(file, name, stat) fprintf(file, "%s,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf\n", name,			\
		stat.avg_ts[0], stat.avg_ts[1],	stat.avg_tq[0], stat.avg_tq[1], stat.util[0], stat.util[1])

#define printCSV_stat_prio_header(file) fprintf(file, "label,E(ts)_l,E(ts)_r,E(ts1)_l,E(ts1)_r,E(ts2)_l,E(ts2)_r,"	\
		"E(tq)_l,E(tq)_r,E(tq1)_l,E(tq1)_r,E(tq2)_l,E(tq2)_r,"							\
		"util_l,util_r,util1_l,util1_r,util2_l,util2_r,"							\
		"sd2_l,sd2_r\n")
#define printCSV_stat_prio(file, name, stat) fprintf(file, "%s,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,"			\
		"%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,"									\
		"%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,"									\
		"%.9lf,%.9lf\n", name, 											\
		stat.avg_ts[0], stat.avg_ts[1],	stat.avg_ts1[0], stat.avg_ts1[1], stat.avg_ts2[0], stat.avg_ts2[1],	\
		stat.avg_tq[0], stat.avg_tq[1], stat.avg_tq1[0], stat.avg_tq1[1], stat.avg_tq2[0], stat.avg_tq2[1],	\
		stat.util[0], stat.util[1],stat.util1[0], stat.util1[1],stat.util2[0], stat.util2[1],			\
		stat.sd2[0], stat.sd2[1])		

int readCSV_res(FILE *file, struct sim_data * result) 
{
	int i = 0;
	double temp_ts, temp_tq, temp_util;
	jump_csv_line();
		
	for (i = 0; fscanf(file, "%d,%d,%d,%*[^,],%lf,%lf,%lf\n", 
				&(result->m), &(result->lambda), &(result->mu), &(temp_ts), &(temp_tq), &(temp_util)) == 6;
				 i++) {
		result->avg_ts = realloc(result->avg_ts, (i + 1) * sizeof(double));
		result->avg_tq = realloc(result->avg_tq, (i + 1) * sizeof(double));
		result->util = realloc(result->util, (i + 1) * sizeof(double));
		if (result->avg_ts == NULL || result->avg_tq == NULL || result->util == NULL) {
			error_msg("error in realloc");
			return 0;
		}
		result->avg_ts[i] = temp_ts;
		result->avg_tq[i] = temp_tq;
		result->util[i] = temp_util;
	}
	return i;
}

int readCSV_res_prio(FILE *file, struct sim_prio_data * result) 
{
	int i;
	double temp_ts, temp_ts1, temp_ts2;
	double temp_tq, temp_tq1, temp_tq2;
	double temp_util, temp_util1, temp_util2;
	double temp_service_time;
	jump_csv_line();
		
	for (i = 0; fscanf(file, "%d,%d,%d,%*[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
				&(result->m), &(result->lambda), &(result->mu), 
				&(temp_ts), &(temp_ts1), &(temp_ts2),
				&(temp_tq), &(temp_tq1), &(temp_tq2),
				&(temp_util), &(temp_util1), &(temp_util2),
				&(temp_service_time)) == 13;
				i++) {
		result->avg_ts = realloc(result->avg_ts, (i + 1) * sizeof(double));
		result->avg_ts1 = realloc(result->avg_ts1, (i + 1) * sizeof(double));
		result->avg_ts2 = realloc(result->avg_ts2, (i + 1) * sizeof(double));
		result->avg_tq = realloc(result->avg_tq, (i + 1) * sizeof(double));
		result->avg_tq1 = realloc(result->avg_tq1, (i + 1) * sizeof(double));
		result->avg_tq2 = realloc(result->avg_tq2, (i + 1) * sizeof(double));
		result->util = realloc(result->util, (i + 1) * sizeof(double));
		result->util1 = realloc(result->util1, (i + 1) * sizeof(double));
		result->util2 = realloc(result->util2, (i + 1) * sizeof(double));
		result->service_time = realloc(result->service_time, (i + 1) * sizeof(double));
		result->sd2 = realloc(result->sd2, (i + 1) * sizeof(double));
		if (result->avg_ts == NULL || result->avg_tq == NULL || result->util == NULL) {
			error_msg("error in realloc");
			return 0;
		}
		result->avg_ts[i] = temp_ts;
		result->avg_ts1[i] = temp_ts1;
		result->avg_ts2[i] = temp_ts2;
		result->avg_tq[i] = temp_tq;
		result->avg_tq1[i] = temp_tq1;
		result->avg_tq2[i] = temp_tq2;
		result->util[i] = temp_util;
		result->util1[i] = temp_util1;
		result->util2[i] = temp_util2;
		result->service_time[i] = temp_service_time;
		result->sd2[i] = temp_ts2 / temp_service_time;
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
	*var = sum / (n-1);
}

double confidence_delta(double sample_stdev) {
	const int n = 20;
	return student_19[confidenza_98] * sample_stdev / sqrt(n);
}

void confidence_interval(double val[], int size, double *res) {
	double mean, var;
	stat(val, size, &mean, &var);
	double delta = confidence_delta(sqrt(var));
	res[0] = mean - delta;
	res[1] = mean + delta;
}

void calculate_stat(const char *path, struct sim_data *stat)
{	
	struct sim_data sample = {0,0,0, NULL, NULL, NULL};
	stat->avg_ts = malloc(sizeof(double)*2);
	stat->avg_tq = malloc(sizeof(double)*2);
	stat->util = malloc(sizeof(double)*2);
	
	FILE * sample_file = fopen(path, "r");
	if (sample_file == NULL)
		error_msg("error in fopen\n");
	
	int n = readCSV_res(sample_file, &sample);
	confidence_interval(sample.avg_ts, n, stat->avg_ts);
	confidence_interval(sample.avg_tq, n, stat->avg_tq);
	confidence_interval(sample.util, n, stat->util);
	
	free(sample.avg_ts);
	free(sample.avg_tq);
	free(sample.util);
	fclose(sample_file);
}

void print_stat(struct sim_data stat, const char *label)
{
	printf("%s\n", label);
	printf("util:\t %.9lf, %.9lf\n", stat.util[0], stat.util[1]);
	printf("tq:\t %.9lf, %.9lf\n", stat.avg_tq[0], stat.avg_tq[1]);
	printf("ts:\t %.9lf, %.9lf\n", stat.avg_ts[0], stat.avg_ts[1]);
}

void calculate_prio_stat(const char *path, struct sim_prio_data *stat)
{	
	struct sim_prio_data sample = {0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	stat->avg_ts = malloc(sizeof(double)*2);
	stat->avg_ts1 = malloc(sizeof(double)*2);
	stat->avg_ts2 = malloc(sizeof(double)*2);
	stat->avg_tq = malloc(sizeof(double)*2);
	stat->avg_tq1 = malloc(sizeof(double)*2);
	stat->avg_tq2 = malloc(sizeof(double)*2);
	stat->util = malloc(sizeof(double)*2);
	stat->util1 = malloc(sizeof(double)*2);
	stat->util2 = malloc(sizeof(double)*2);
	stat->sd2 = malloc(sizeof(double)*2);
	
	FILE * sample_file = fopen(path, "r");
	if (sample_file == NULL)
		error_msg("error in fopen\n");
	
	int n = readCSV_res_prio(sample_file, &sample);
	
	confidence_interval(sample.avg_ts, n, stat->avg_ts);
	confidence_interval(sample.avg_ts1, n, stat->avg_ts1);
	confidence_interval(sample.avg_ts2, n, stat->avg_ts2);
	confidence_interval(sample.avg_tq, n, stat->avg_tq);
	confidence_interval(sample.avg_tq1, n, stat->avg_tq1);
	confidence_interval(sample.avg_tq2, n, stat->avg_tq2);
	confidence_interval(sample.util, n, stat->util);
	confidence_interval(sample.util1, n, stat->util1);
	confidence_interval(sample.util2, n, stat->util2);
	confidence_interval(sample.sd2, n, stat->sd2);
	
	free(sample.avg_ts);
	free(sample.avg_ts1);
	free(sample.avg_ts2);
	free(sample.avg_tq);
	free(sample.avg_tq1);
	free(sample.avg_tq2);
	free(sample.util);
	free(sample.util1);
	free(sample.util2);
	free(sample.sd2);
	fclose(sample_file);
}

void print_priostat(struct sim_prio_data stat, const char *label)
{	
	printf("%s\n", label);
	printf("util:\t %.9lf, %.9lf\n", stat.util[0], stat.util[1]);
	printf("util1:\t %.9lf, %.9lf\n", stat.util1[0], stat.util1[1]);
	printf("util2:\t %.9lf, %.9lf\n", stat.util2[0], stat.util2[1]);
	printf("tq:\t %.9lf, %.9lf\n", stat.avg_tq[0], stat.avg_tq[1]);
	printf("tq1:\t %.9lf, %.9lf\n", stat.avg_tq1[0], stat.avg_tq1[1]);
	printf("tq2:\t %.9lf, %.9lf\n", stat.avg_tq2[0], stat.avg_tq2[1]);
	printf("ts:\t %.9lf, %.9lf\n", stat.avg_ts[0], stat.avg_ts[1]);
	printf("ts1:\t %.9lf, %.9lf\n", stat.avg_ts1[0], stat.avg_ts1[1]);
	printf("ts2:\t %.9lf, %.9lf\n", stat.avg_ts2[0], stat.avg_ts2[1]);
	printf("sd2:\t %.9lf, %.9lf\n", stat.sd2[0], stat.sd2[1]);
}

int main() 
{	
	FILE *stat_http_file = fopen("./output/stat/http.csv", "w");
	FILE *stat_multi_file = fopen("./output/stat/multi.csv", "w");
	FILE *stat_http_prio_file = fopen("./output/stat/http_prio.csv", "w");
	FILE *stat_multi_prio_file = fopen("./output/stat/multi_prio.csv", "w");
	
	const char* label_http[] = {"http08_12", "http12_15", "http15_20", "http20_24", "http24_08"};
	const char* label_multi[] = {"multi08_12", "multi12_15", "multi15_20", "multi20_24", "multi24_08"};
	char path[35];
	
	struct sim_data stat = {0,0,0, NULL, NULL, NULL};
	struct sim_prio_data prio_stat = {0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	
	printCSV_stat_header(stat_http_file);
	for (int i = 0; i < 5; i++) {
		sprintf(path, "./output/base/%s.csv", label_http[i]);
		calculate_stat(path, &stat);
		printCSV_stat(stat_http_file, label_http[i], stat);
		print_stat(stat, label_http[i]);
	}
	fclose(stat_http_file);
	
	printCSV_stat_header(stat_multi_file);
	for (int i = 0; i < 5; i++) {
		sprintf(path, "./output/base/%s.csv", label_multi[i]);
		calculate_stat(path, &stat);
		printCSV_stat(stat_multi_file, label_multi[i], stat);
		print_stat(stat, label_multi[i]);
	}
	fclose(stat_multi_file);
	
	printf("\npriority\n\n");
	printCSV_stat_prio_header(stat_http_prio_file);
	for (int i = 0; i < 5; i++) {
		sprintf(path, "./output/priority/%s.csv", label_http[i]);
		calculate_prio_stat(path, &prio_stat);
		printCSV_stat_prio(stat_http_prio_file, label_http[i], prio_stat);
		print_priostat(prio_stat, label_http[i]);
	}
	fclose(stat_http_prio_file);
	
	printCSV_stat_prio_header(stat_multi_prio_file);
	for (int i = 0; i < 5; i++) {
		sprintf(path, "./output/priority/%s.csv", label_multi[i]);
		printCSV_stat_prio(stat_multi_prio_file, label_multi[i], prio_stat);
		print_priostat(prio_stat, label_multi[i]);
	}
	fclose(stat_multi_prio_file);

}
