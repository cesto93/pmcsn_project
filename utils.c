#include <stdlib.h>
#include <stdio.h>
#include "utils.h"	

void error_msg(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
