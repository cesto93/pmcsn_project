#include "csv.h"

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

int readSeed(const char *input_path, unsigned long ** seeds) 
{
	FILE *file = fopen(input_path, "r");
	if (file == NULL) {
		error_msg("error in fopen");
		return -1;
	}
	int i;
	unsigned long temp;
	for (i = 0; fscanf(file, "%lu", &temp) == 1; i++) {
		*seeds = realloc(*seeds, (i + 1) * sizeof(unsigned long));
		if (*seeds == NULL) {
			error_msg("error in realloc");
			free(*seeds);
			return 0;
		}
		(*seeds)[i] = temp;
	}
	return i;
}

int readCSV_param(const char *input_path, struct sim_param ** param) 
{
	FILE *file = fopen(input_path, "r");
	if (file == NULL) {
		error_msg("error in fopen");
		return -1;
	}
	int i;
	struct sim_param temp;
	jump_csv_line(file);
	for (i = 0; fscanf(file, "%[^,],%lf,%d,%d,%d\n", temp.label, &(temp.seconds), &(temp.m), &(temp.lambda), &(temp.mu)) == 5; i++) {
		*param = realloc(*param, (i + 1) * sizeof(struct sim_param));
		if (*param == NULL) {
			error_msg("error in realloc");
			free(*param);
			return -1;
		}
		(*param)[i] = temp;
	}
	return i;
}

int readCSV_param_prio(const char *input_path, struct sim_param_prio ** param) 
{
	FILE *file = fopen(input_path, "r");
	if (file == NULL) {
		error_msg("error in fopen");
		return -1;
	}
	int i;
	struct sim_param_prio temp;
	jump_csv_line(file);
	for (i = 0; fscanf(file, "%[^,],%lf,%d,%d,%d, %lf\n", temp.label, &(temp.seconds), &(temp.m), &(temp.lambda), &(temp.mu), 
			   &(temp.p1)) == 6;
	     i++) {
		*param = realloc(*param, (i + 1) * sizeof(struct sim_param_prio));
		if (*param == NULL) {
			error_msg("error in realloc");
			free(*param);
			return 0;
		}
		(*param)[i] = temp;
	}
	return i;
}

int readCSV_res(const char *input_path, struct sim_data * result) 
{
	int i = 0;
	double temp_ts, temp_tq, temp_util;
	
	FILE *file = fopen(input_path, "r");
	if (file == NULL) {
		error_msg("error in fopen");
		return -1;
	}
	
	jump_csv_line(file);
		
	for (i = 0; fscanf(file, "%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%lf,%lf,%lf\n", &(temp_ts), &(temp_tq), &(temp_util)) == 3;
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

int readCSV_res_prio(const char *input_path, struct sim_prio_data * result) 
{
	int i;
	double temp_ts, temp_ts1, temp_ts2;
	double temp_tq, temp_tq1, temp_tq2;
	double temp_util, temp_util1, temp_util2;
	double temp_service_time;
	
	
	FILE *file = fopen(input_path, "r");
	if (file == NULL) {
		error_msg("error in fopen");
		return -1;
	}
	jump_csv_line(file);
		
	for (i = 0; fscanf(file, "%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
			   &(temp_ts), &(temp_ts1), &(temp_ts2),	&(temp_tq), &(temp_tq1), &(temp_tq2),
			   &(temp_util), &(temp_util1), &(temp_util2),	&(temp_service_time)) == 10; i++) 
			   {
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
