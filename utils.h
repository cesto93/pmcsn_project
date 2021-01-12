#ifndef UTILS_H
#define UTILS_H

static void error_msg(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

#endif
