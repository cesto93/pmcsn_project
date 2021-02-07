#include <stdlib.h>
#include <stdio.h>
#include "utils.h"	

void error_msg(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

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
	jump_csv_line();
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
	jump_csv_line();
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
