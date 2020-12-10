#include <stdio.h>
#include <math.h>
#include "rngs.h"		/* the multi-stream generator */
#include "list.h"		//ordered list

#define START         0.0	/* initial time                   */
#define STOP      20000.0	/* terminal (close the door) time */
#define INFINITE   (100.0 * STOP)	/* must be much larger than STOP  */

double Exponential(double m)
{
	return (-m * log(1.0 - Random()));
}

double Min(double a, double c)
{
	if (a < c)
		return a;
	else
		return c;
}

double GetArrival(double lambda)
{
	static double arrival = START;

	SelectStream(0);
	arrival += Exponential(1.0 / lambda);
	return (arrival);
}

double GetService(double mu)
{
	SelectStream(1);
	return Exponential(1.0 / mu);
}

void simulation(int servers, double lambda, double mu)
{
	struct {
		double node;	/* time integrated number in the node  */
		double queue;	/* time integrated number in the queue */
		double service;	/* time integrated number in service   */
	} area = { 0.0, 0.0, 0.0 };

	long index = 0;		/* used to count departed jobs         */
	long nnode = 0;		/* number in the node                  */
	long nservice = 0;	/* number in sevice                  */

	double arrival;		/* next arrival time                   */
	double completion;	/* next completion time                */
	double current;		/* current time                        */
	double next;		/* next (most imminent) event time     */
	double last;		/* last arrival time                   */

	struct node_t *residual_list = NULL;

	PlantSeeds(123456789);
	current = START;	/* set the clock                         */
	last = START;
	arrival = GetArrival(lambda);	/* schedule the first arrival */
	completion = INFINITE;	/* the first event can't be a completion */
	
	while (arrival < STOP) {
		//printf("arrival %f\n", arrival);
		//printf("completion %f\n", completion);
		next = Min(arrival, completion);	/* next event time   */
		if (nnode > 0) {	/* update integrals  */
			area.node += (next - current) * nnode;
			area.queue += (next - current) * (nnode - nservice);
			area.service += (next - current) * nservice;
		}
		current = next;	/* advance the clock */

		if (current == arrival) {	/* process an arrival */
			nnode++;
			if (nservice < servers) {	//we populate a server with a job
				nservice++;
				add_ordered(&residual_list, current + GetService(mu));
				completion = residual_list->val;
				//printf("val %f\n", residual_list->val);
			}
			arrival = GetArrival(lambda);
			if (arrival > STOP) {
				last = current;
				arrival = INFINITE;
			}
		} else {	/* process a completion */
			index++;
			nnode--;
			remove_next(&residual_list);
			if (nnode >= servers) {
				add_ordered(&residual_list, current + GetService(mu));
				completion = residual_list->val;
			} else {
				nservice--;
			}

			if (nnode > 0)
				completion = residual_list->val;
			else
				completion = INFINITE;
		}
	}

	printf("\nfor %ld jobs\n", index);
	printf(" average interarrival time = %6.6f\n", last / index);
	printf(" average wait ............ = %6.6f\n", area.node / index);
	printf(" average delay ........... = %6.6f\n", area.queue / index);
	printf(" average service time .... = %6.6f\n", area.service / index);
	printf(" average # in the node ... = %6.6f\n", area.node / current);
	printf(" average # in the queue .. = %6.6f\n", area.queue / current);
	printf(" utilization ............. = %6.6f\n", area.service / (current * servers));
}

int main(void)
{
	simulation(2, 100.0, 200.0);
	return 0;
}
