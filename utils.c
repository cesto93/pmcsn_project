#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

void error_msg(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

int readSeed(FILE *file, unsigned long ** seeds) 
{
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

int readCSV_param(FILE *file, struct sim_param ** param) 
{
	int i;
	struct sim_param temp;
	if (fscanf(file, "%*[^\n]\n") == -1) 
		error_msg("error in readCSV");
	for (i = 0; fscanf(file, "%[^,],%d,%d,%d\n", temp.label, &(temp.m), &(temp.lambda), &(temp.mu)) == 4; i++) {
		*param = realloc(*param, (i + 1) * sizeof(struct sim_param));
		if (*param == NULL) {
			error_msg("error in realloc");
			free(*param);
			return 0;
		}
		(*param)[i] = temp;
	}
	return i;
}
