/* (C) 2005 Luke Macpherson */

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "microuptime.h"

/* returns uptime in microseconds */
uint64_t
microuptime(){
	struct timeval tv;
	uint64_t mt;

	assert(gettimeofday(&tv,NULL)==0);

	mt  = tv.tv_sec;
	mt *= US_PER_S;
	mt += tv.tv_usec;

	return mt;
}
