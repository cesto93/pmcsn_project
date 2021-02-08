#ifndef UTILS_H
#define UTILS_H

#include "msq.h"
#include <stdio.h>

void error_msg(char *msg);					

#define min(a,b)		\
       ({ typeof (a) _a = (a);	\
           typeof (b) _b = (b);	\
         _a < _b ? _a : _b; })

#define _min(a,b) a < b ? a : b        
         
#endif
