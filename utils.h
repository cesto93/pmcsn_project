#ifndef UTILS_H
#define UTILS_H

#include "msq.h"
#include <stdio.h>

void error_msg(char *msg);
int readSeed(const char *input_path, unsigned long ** seeds);
int readCSV_param(const char *input_path, struct sim_param ** param);
int readCSV_param_prio(const char *input_path, struct sim_param_prio ** param);

#define printCSV(file, param, res) fprintf(file, "%lf,%d,%d,%d,%lu,%.9f,%.9f,%.9f\n",		\
			param.seconds, param.m, param.lambda, param.mu, param.seed, res.avg_ts, res.avg_tq, res.util);

#define printCSV_prio_header(file) fprintf(file, "m,lambda,mu,seed,E(ts),E(ts1),E(ts2),E(tq),E(tq1),E(tq2),util,util1,util2,E(s)\n")			
#define printCSV_prio(file, param, res) fprintf(file, "%lf,%d,%d,%d,%lu,%.9f,%.9f,%.9f, %.9f,%.9f,%.9f, %.9f,%.9f,%.9f,%.9f\n",		\
			param.seconds, param.m, param.lambda, param.mu, param.seed, (res).avg_ts, (res).avg_ts1, (res).avg_ts2,		\
			(res).avg_tq, (res).avg_tq1, (res).avg_tq2, (res).util, (res).util1, (res).util2, (res).service_time);						

#define min(a,b)		\
       ({ typeof (a) _a = (a);	\
           typeof (b) _b = (b);	\
         _a < _b ? _a : _b; })

#define _min(a,b) a < b ? a : b   

#define jump_csv_line()				\
	do {					\
	if (fscanf(file, "%*[^\n]\n") == -1)	\
		error_msg("error in readCSV");	\
	} while (0)      
         
#endif
