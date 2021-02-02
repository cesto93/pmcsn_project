/* ----------------------------------------------------------------------- 
 * Name            : rngs.h  (header file for the library file rngs.c) 
 * Author          : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * ----------------------------------------------------------------------- 
 */

#ifndef RNGS_H
#define RNGS_H

double Random(void);
void PlantSeeds(unsigned long x);
void GetSeed(unsigned long *x);
void PutSeed(unsigned long x);
void SelectStream(unsigned int index);
void TestRandom(void);
double Exponential(double m);

#endif
