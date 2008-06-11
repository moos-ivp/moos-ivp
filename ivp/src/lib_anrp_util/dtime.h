#ifndef __dtime_h__
#define __dtime_h__

#include <sys/time.h>
#include <time.h>

static inline double dtime(void) {
	struct timeval tv; struct timezone tz;
	gettimeofday(&tv, &tz);
	return ((double)(tv.tv_sec)) + ((double)(tv.tv_usec)) / 1000000.0;
}

#endif
