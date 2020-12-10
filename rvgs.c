/*      Generator         Range (x)     Mean         Variance
 *      Exponential(m)    x > 0         m            m*m
 */

#include <math.h>
#include "rngs.h"
#include "rvgs.h"

//Returns an exponentially distributed positive real number  NOTE: use m > 0.0
double Exponential(double m) {
  return (-m * log(1.0 - Random()));
}
