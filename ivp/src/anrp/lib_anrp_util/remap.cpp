#include "remap.h"

double remap(double in, double inlow, double inhigh, double outlow, double outhigh)
{
	bool reverse = false;
	if(inlow > inhigh) {
		reverse = true;
		double tmp = inlow;
		inlow = inhigh;
		inhigh = tmp;
	}
	
	if(in < inlow) in = inlow;
	if(in > inhigh) in = inhigh;

	double id, od;

	id = inhigh - inlow;
	od = outhigh - outlow;

	in -= inlow;

	return outlow + (reverse? 1.0-(in/id) : (in/id)) * od;
}

#ifdef TESTING
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	fprintf(stderr, "remap(%lf,%lf,%lf,%lf,%lf) = %lf\n",
			atof(argv[1]),
			atof(argv[2]),
			atof(argv[3]),
			atof(argv[4]),
			atof(argv[5]),
			remap(atof(argv[1]), atof(argv[2]), atof(argv[3]),
			      atof(argv[4]), atof(argv[5])));
}

#endif

