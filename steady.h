#ifndef STEADY_H
#define STEADY_H

#include "utils/list.h"
#include "msq.h"

struct sim_result simul(const unsigned int servers, double lambda, double mu, unsigned long seed, long seconds);
struct sim_result_prio simul_prio(const unsigned int servers, double lambda, double mu, unsigned long seed, double p1, long seconds);				  

#endif
