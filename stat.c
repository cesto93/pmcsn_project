#include "stat.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "csv.h"

const double student_20[] = {1.72472, 2.08596, 2.52798, 2.84534, 3.55181, 3.84952};
const double student_19[] = {1.729, 2.093, 2.539, 2.861, 3.579, 3.883};
const double student_10[] = {1.812, 2.228, 2.764, 3.169, 4.144, 4.587};

#define confidenza_90 0
#define confidenza_95 1
#define confidenza_98 2
#define confidenza_99 3
#define confidenza_998 4
#define confidenza_999 5	

void mean_and_var(double val[], int size, double *mean, double *var) 
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
	if (var != NULL)
		*var = sum / (n-1);
}

void confidence_interval(double val[], int size, double *res) {
	double mean, var;
	mean_and_var(val, size, &mean, &var);
	const int n = 20;
	double delta = student_19[confidenza_98] * sqrt(var) / sqrt(n);
	res[0] = mean - delta;
	res[1] = mean + delta;
}

void calculate_stat(const char *path, struct sim_data *stat)
{	
	struct sim_data sample = {NULL, NULL, NULL};
	stat->avg_ts = malloc(sizeof(double)*2);
	stat->avg_tq = malloc(sizeof(double)*2);
	stat->util = malloc(sizeof(double)*2);
	
	int n = readCSV_res(path, &sample);
	confidence_interval(sample.avg_ts, n, stat->avg_ts);
	confidence_interval(sample.avg_tq, n, stat->avg_tq);
	confidence_interval(sample.util, n, stat->util);
	
	free(sample.avg_ts);
	free(sample.avg_tq);
	free(sample.util);
}

void calculate_prio_stat(const char *path, struct sim_prio_data *stat)
{	
	struct sim_prio_data sample = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
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
	
	int n = readCSV_res_prio(path, &sample);
	
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
}

void print_stat(struct sim_data stat, const char *label, int n)
{
	if (n == 2) {
		printf("%s\n", label);
		printf("util:\t %.9lf, %.9lf\n", stat.util[0], stat.util[1]);
		printf("tq:\t %.9lf, %.9lf\n", stat.avg_tq[0], stat.avg_tq[1]);
		printf("ts:\t %.9lf, %.9lf\n", stat.avg_ts[0], stat.avg_ts[1]);
	} else if (n == 1) {
		printf("%s\n", label);
		printf("util:\t %.9lf\n", stat.util[0]);
		printf("tq:\t %.9lf\n", stat.avg_tq[0]);
		printf("ts:\t %.9lf\n", stat.avg_ts[0]);
	}
}

void print_priostat(struct sim_prio_data stat, const char *label, int n)
{	
	if (n == 2) {
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
	} else if (n == 1) {
		printf("%s\n", label);
		printf("util:\t %.9lf\n", stat.util[0]);
		printf("util1:\t %.9lf\n", stat.util1[0]);
		printf("util2:\t %.9lf\n", stat.util2[0]);
		printf("tq:\t %.9lf\n", stat.avg_tq[0]);
		printf("tq1:\t %.9lf\n", stat.avg_tq1[0]);
		printf("tq2:\t %.9lf\n", stat.avg_tq2[0]);
		printf("ts:\t %.9lf\n", stat.avg_ts[0]);
		printf("ts1:\t %.9lff\n", stat.avg_ts1[0]);
		printf("ts2:\t %.9lf\n", stat.avg_ts2[0]);
		printf("sd2:\t %.9lf\n", stat.sd2[0]);
	}
}

void print_stat_file(const char *output_path, const char *input_dir, const char * label[], int nlabels)
{
	struct sim_data stat = {NULL, NULL, NULL};
	
	char input_path[50];
	FILE *file = fopen(output_path, "w");
	if (file == NULL) {
		error_msg("error in fopen");
		return;
	}
	
	printCSV_stat_header(file);
	for (int i = 0; i < nlabels; i++) {
		sprintf(input_path, input_dir, label[i]);
		calculate_stat(input_path, &stat);
		printCSV_stat(file, label[i], stat);
		//print_stat(stat, label[i], 2);
	}
	fclose(file);
}

void print_prio_stat_file(const char *output_path, const char *input_dir, const char * label[], int nlabels)
{
	struct sim_prio_data stat = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	char input_path[50];
	FILE *file = fopen(output_path, "w");
	if (file == NULL) {
		error_msg("error in fopen");
		return;
	}
	
	printCSV_stat_prio_header(file);
	for (int i = 0; i < nlabels; i++) {
		sprintf(input_path, input_dir, label[i]);
		calculate_prio_stat(input_path, &stat);
		printCSV_stat_prio(file, label[i], stat);
		//print_priostat(stat, label[i], 2);
	}
	fclose(file);
}

void print_mstat_file(const char *output_path, const char *input_dir, char * label[], int nlabels)
{
	struct sim_data stat = {NULL, NULL, NULL};
	struct sim_data sample = {NULL, NULL, NULL};
	char input_path[50];
	
	
	FILE *file = fopen(output_path, "w");
	if (file == NULL) {
		error_msg("error in fopen");
		return;
	}
	
	stat.avg_ts = alloca(sizeof(double));
	stat.avg_tq = alloca(sizeof(double));
	stat.util = alloca(sizeof(double));
	
	printCSV_mstat_header(file);
	for (int i = 0; i < nlabels; i++) {
		sprintf(input_path, input_dir, label[i]);
		int n = readCSV_res(input_path, &sample);
		mean_and_var(sample.avg_ts, n, stat.avg_ts, NULL);
		mean_and_var(sample.avg_tq, n, stat.avg_tq, NULL);
		mean_and_var(sample.util, n, stat.util, NULL);
		printCSV_mstat(file, label[i], stat);
		//print_stat(stat, label[i], 1);
	}
	fclose(file);
}

void print_prio_mstat_file(const char *output_path, const char *input_dir, char * label[], int nlabels)
{
	struct sim_prio_data stat = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	struct sim_prio_data sample = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	char input_path[50];
	
	FILE *file = fopen(output_path, "w");
	if (file == NULL) {
		error_msg("error in fopen");
		return;
	}
	
	stat.avg_ts = alloca(sizeof(double));
	stat.avg_ts1 = alloca(sizeof(double));
	stat.avg_ts2 = alloca(sizeof(double));
	stat.avg_tq = alloca(sizeof(double));
	stat.avg_tq1 = alloca(sizeof(double));
	stat.avg_tq2 = alloca(sizeof(double));
	stat.util = alloca(sizeof(double));
	stat.util1 = alloca(sizeof(double));
	stat.util2 = alloca(sizeof(double));
	stat.sd2 = alloca(sizeof(double));
	
	printCSV_mstat_prio_header(file);
	for (int i = 0; i < nlabels; i++) {
		sprintf(input_path, input_dir, label[i]);
		int n = readCSV_res_prio(input_path, &sample);
		mean_and_var(sample.avg_ts, n, stat.avg_ts, NULL);
		mean_and_var(sample.avg_ts1, n, stat.avg_ts1, NULL);
		mean_and_var(sample.avg_ts2, n, stat.avg_ts2, NULL);
		mean_and_var(sample.avg_tq, n, stat.avg_tq, NULL);
		mean_and_var(sample.avg_tq1, n, stat.avg_tq1, NULL);
		mean_and_var(sample.avg_tq2, n, stat.avg_tq2, NULL);
		mean_and_var(sample.util, n, stat.util, NULL);
		mean_and_var(sample.util1, n, stat.util1, NULL);
		mean_and_var(sample.util2, n, stat.util2, NULL);
		mean_and_var(sample.sd2, n, stat.sd2, NULL);
		
		printCSV_mstat_prio(file, label[i], stat);
		//print_priostat(stat, label[i], 1);
	}
	fclose(file);
}

int main() 
{	
	const char* label_http[] = {"http08_12", "http12_15", "http15_20", "http20_24", "http24_08"};
	const char* label_multi[] = {"multi08_12", "multi12_15", "multi15_20", "multi20_24", "multi24_08"};
	
	char* label_http_t[5 * 9];
	char* label_multi_t[5 * 9];
	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			label_http_t[i * 9 + j] = alloca(50);
			snprintf(label_http_t[i * 9 + j], 50, "%s_%d", label_http[i], j + 1);
			label_multi_t[i * 9 + j] = alloca(50);
			snprintf(label_multi_t[i * 9 + j], 50, "%s_%d", label_multi[i], j + 1);
		}
	}			      

	puts("\nsteady standard\n");
	print_stat_file("./output/steady/stat/http.csv", "./output/steady/standard/%s.csv", label_http, 5);
	print_stat_file("./output/steady/stat/multi.csv", "./output/steady/standard/%s.csv", label_multi, 5);
	
	puts("\nsteady priority\n\n");
	print_prio_stat_file("./output/steady/stat/http_prio.csv", "./output/steady/priority/%s.csv", label_http, 5);
	print_prio_stat_file("./output/steady/stat/multi_prio.csv", "./output/steady/priority/%s.csv", label_multi, 5);
	
	puts("\ntransient standard\n");
	print_mstat_file("./output/transient/stat/http.csv", "./output/transient/standard/%s.csv", label_http_t, 45);
	print_mstat_file("./output/transient/stat/multi.csv", "./output/transient/standard/%s.csv", label_multi_t, 45);
	
	puts("\ntransient priority\n");
	print_prio_mstat_file("./output/transient/stat/http_prio.csv", "./output/transient/priority/%s.csv", label_http_t, 45);
	print_prio_mstat_file("./output/transient/stat/multi_prio.csv", "./output/transient/priority/%s.csv", label_multi_t, 45);

}
