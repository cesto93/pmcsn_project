#ifndef STAT_H
#define STAT_H

struct sim_data {
	double *avg_ts;
	double *avg_tq;
	double *util;
};

struct sim_prio_data {
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

#endif
