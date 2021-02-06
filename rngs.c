/* -------------------------------------------------------------------------
 * This is an ANSI C library for multi-stream random number generation.  
 * The library supplies 256 streams of random numbers; use 
 * SelectStream(s) to switch between streams indexed s = 0,1,...,255.
 *
 * The streams must be initialized.  The recommended way to do this is by
 * using the function PlantSeeds(x) with the value of x used to initialize 
 * the default stream and all other streams initialized automatically with
 * values dependent on the value of x.  
 * The generator used in this library is a so-called 'Lehmer random number
 * generator' which returns a pseudo-random number uniformly distributed
 * 0.0 and 1.0.  The period is (m - 1) where m = 2,147,483,647 and the
 * smallest and largest possible values are (1 / m) and 1 - (1 / m)
 * respectively.  For more details see:
 * 
 *       "Random Number Generators: Good Ones Are Hard To Find"
 *                   Steve Park and Keith Miller
 *              Communications of the ACM, October 1988
 *
 * Name            : rngs.c  (Random Number Generation - Multiple Streams)
 * Authors         : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * ------------------------------------------------------------------------- 
 */

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "rngs.h"

#define MODULUS    2147483647	/* DON'T CHANGE THIS VALUE                  */
#define MULTIPLIER 48271	/* DON'T CHANGE THIS VALUE                  */
#define CHECK      399268537	/* DON'T CHANGE THIS VALUE                  */
#define STREAMS    256		/* # of streams, DON'T CHANGE THIS VALUE    */
#define A256       22925	/* jump multiplier, DON'T CHANGE THIS VALUE */
#define DEFAULT    123456789	/* initial seed, use 0 < DEFAULT < MODULUS  */

static unsigned long seed[STREAMS] = { DEFAULT };	/* current state of each stream   */

static int stream = 0;		/* stream index, 0 is the default */
static int initialized = 0;	/* test for stream initialization */


double Random() // returns a pseudo-random real number uniformly distributed between 0.0 and 1.0
{
	const long Q = MODULUS / MULTIPLIER;
	const long R = MODULUS % MULTIPLIER;
	long t;

	t = MULTIPLIER * (seed[stream] % Q) - R * (seed[stream] / Q);
	if (t > 0)
		seed[stream] = t;
	else
		seed[stream] = t + MODULUS;
	return ((double)seed[stream] / MODULUS);
}

/* Set the state of all streams by "planting" a sequence of seeds with all states dictated by the state of the default stream. 
 * The sequence of planted states is separated one from the next by 8,367,782 calls to Random() */
void PlantSeeds(unsigned long x)
{
	const long Q = MODULUS / A256;
	const long R = MODULUS % A256;
	long t = x % MODULUS;

	initialized = 1;
	seed[0] = t;                            /* set seed[0]                 */
	for (int j = 1; j < STREAMS; j++) {
		t = A256 * (seed[j - 1] % Q) - R * (seed[j - 1] / Q);
		if (t > 0)
			seed[j] = t;
		else
			seed[j] = t + MODULUS;
	}
}

void PutSeed(unsigned long x) // Use this function to set the state of the current random number
{
	x = x % MODULUS;	/* correct if x is too large  */
	seed[stream] = x;
}
                                      
void GetSeed(unsigned long *x) // get the state of the current random number generator stream.
{
	*x = seed[stream];
}
 
void SelectStream(unsigned int index) // set the current random number generator stream
{
	stream = index % STREAMS;
	/* protect against un-initialized streams */
	/*if ((initialized == 0) && (stream != 0))
		PlantSeeds(DEFAULT);*/
}

void TestRandom() // Use this (optional) function to test for a correct implementation
{
	unsigned long i;
	unsigned long x;
	double u;
	char ok = 0;

	SelectStream(0);	/* select the default stream */
	PutSeed(1);		/* and set the state to 1    */
	for (i = 0; i < 10000; i++)
		u = Random();
	GetSeed(&x);		/* get the new state value   */
	ok = (x == CHECK);	/* and check for correctness */

	(void)u;

	SelectStream(1);	/* select stream 1                 */
	PlantSeeds(1);		/* set the state of all streams    */
	GetSeed(&x);		/* get the state of stream 1       */
	ok = ok && (x == A256);	/* x should be the jump multiplier */
	if (ok)
		printf("\n The implementation of rngs.c is correct.\n\n");
	else
		printf
		    ("\n\a ERROR -- the implementation of rngs.c is not correct.\n\n");
}

/* =========================================================
 * Returns an exponentially distributed positive real number. 
 * NOTE: use m > 0.0
 * =========================================================
 */
double Exponential(double m)
{
	return (-m * log(1.0 - Random()));
}

/* ========================================================
 * Returns 1 with probability p or 0 with probability 1 - p. 
 * NOTE: use 0.0 < p < 1.0                                   
 * ========================================================
 */
long Bernoulli(double p) 
{
	return ((Random() < (1.0 - p)) ? 0 : 1);
}
