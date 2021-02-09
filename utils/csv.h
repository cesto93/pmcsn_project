#ifndef CSV_H
#define CSV_H

#include "../msq.h"
#include "../stat.h"

int readSeed(const char *input_path, unsigned long ** seeds);
int readCSV_param(const char *input_path, struct sim_param ** param);
int readCSV_param_prio(const char *input_path, struct sim_param_prio ** param);

int readCSV_res(const char *input_path, struct sim_data * result);
int readCSV_res_prio(const char *input_path, struct sim_prio_data * result);

#define printCSV_header(file) fprintf(file, "sec,m,lambda,mu,seed,E(ts),E(tq),util\n");

#define printCSV(file, param, res) fprintf(file, "%lf,%d,%d,%d,%lu,%.9f,%.9f,%.9f\n",		\
			param.seconds, param.m, param.lambda, param.mu, param.seed, res.avg_ts, res.avg_tq, res.util);

#define printCSV_prio_header(file) fprintf(file, "sec,m,lambda,mu,seed,E(ts),E(ts1),E(ts2),E(tq),E(tq1),E(tq2),util,util1,util2,E(s)\n")
			
#define printCSV_prio(file, param, res) fprintf(file, "%lf,%d,%d,%d,%lu,%.9f,%.9f,%.9f, %.9f,%.9f,%.9f, %.9f,%.9f,%.9f,%.9f\n",		\
			param.seconds, param.m, param.lambda, param.mu, param.seed, (res).avg_ts, (res).avg_ts1, (res).avg_ts2,		\
			(res).avg_tq, (res).avg_tq1, (res).avg_tq2, (res).util, (res).util1, (res).util2, (res).service_time);

#define jump_csv_line(file)			\
	do {					\
	if (fscanf(file, "%*[^\n]\n") == -1)	\
		error_msg("error in readCSV");	\
	} while (0) 

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

#define printCSV_mstat_header(file) fprintf(file, "label,E(ts),E(tq),util\n")

#define printCSV_mstat(file, name, stat) fprintf(file, "%s,%.9lf,%.9lf,%.9lf\n", name,			\
		stat.avg_ts[0],	stat.avg_tq[0], stat.util[0])

#define printCSV_mstat_prio_header(file) fprintf(file, "label,E(ts),E(ts1),E(ts2),E(tq),E(tq1),E(tq2),util,util1,util2,sd2\n")
		
#define printCSV_mstat_prio(file, name, stat) fprintf(file, "%s,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,%.9lf,"				\
		"%.9lf,%.9lf,%.9lf,%.9lf\n",											\
		name, stat.avg_ts[0], stat.avg_ts1[0], stat.avg_ts2[0],	stat.avg_tq[0], stat.avg_tq1[0], stat.avg_tq2[0], 	\
		stat.util[0],stat.util1[0],stat.util2[0], stat.sd2[0])

#endif
