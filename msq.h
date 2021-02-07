#ifndef MSQ_H
#define MSQ_H

struct sim_param {
	double seconds;
	int m;
	int lambda;
	int mu;
	unsigned long seed;
	char label[10];
};

struct sim_param_prio {
	double seconds;
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

struct class_info {
	long nqueue; // number in queue
	long nservice; // number in sevice
	long index; /* used to count departed jobs */
	double node_area; /* time integrated number in the node  */
	double queue_area; /* time integrated number in the queue */
	double service_area; /* time integrated number in service   */
};

#define empty_class_info(cl_info)		\
	do {					\
		(cl_info).nqueue = 0;		\
		(cl_info).nservice = 0;		\
		(cl_info).index = 0;		\
		(cl_info).node_area = 0;	\
		(cl_info).queue_area = 0;	\
		(cl_info).service_area = 0;	\
	} while (0)

#define update_class_area(c_info, next, current) do {					\
	(c_info).node_area += (next - current) * ((c_info).nservice + (c_info).nqueue);	\
	(c_info).queue_area += (next - current) * ((c_info).nqueue);			\
	(c_info).service_area += (next - current) * ((c_info).nservice);			\
	} while(0)

void set_sim_result(struct sim_result *res, struct class_info tot, double current, double last, int servers);	

double GetInterarrival(const double lambda);
double GetService(const double mu);
int GetClass(const double p);

#endif
