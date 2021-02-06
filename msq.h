#ifndef MSQ_H
#define MSQ_H

struct sim_param {
	int m;
	int lambda;
	int mu;
	unsigned long seed;
	char label[10];
};

struct sim_param_prio {
	int m;
	int lambda;
	int mu;
	unsigned long seed;
	double p1;
	char label[10];
};

struct sim_result {
	double avg_ts; //EXPORTED
	double avg_tq; //EXPORTED
	double util; //EXPORTED
	long njobs;
	double interarr_time;
	double service_time;
	double avg_nnode;
	double avg_nqueue;
};

struct sim_result_prio {
	double avg_ts; //EXPORTED
	double avg_ts1; //EXPORTED
	double avg_ts2; //EXPORTED
	double avg_tq; //EXPORTED
	double avg_tq1; //EXPORTED
	double avg_tq2; //EXPORTED
	double util; //EXPORTED
	double util1; //EXPORTED
	double util2; //EXPORTED
	long njobs;
	double service_time;
	double avg_nnode;
	double avg_nqueue;

};

#endif
