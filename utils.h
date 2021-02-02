#ifndef UTILS_H
#define UTILS_H

struct sim_param {
	int m;
	int lambda;
	int mu;
	unsigned long seed;
	char label[10];
};

void error_msg(char *msg);
int readSeed(FILE *file, unsigned long ** seeds);
int readCSV_param(FILE *file, struct sim_param ** param);

#define printCSV(file, param, res) fprintf(file, "%d,%d,%d,%lu,%.9f,%.9f,%.9f\n", \
			param.m, param.lambda, param.mu, param.seed, res.avg_ts, res.avg_tq, res.util);

#define MIN(a,b, res) 	\
	do {				\
		if (a < b)		\
			res = a;	\
		else			\
			res = b;	\
	} while(0)			

#define min(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a < _b ? _a : _b; })

#define _min(a,b) a < b ? a : b         
         
#endif
