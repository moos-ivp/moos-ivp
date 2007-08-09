#include "SimulatedSurfaceObject.h"
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
SimulatedSurfaceObject::SimulatedSurfaceObject()
{
	cps = x = y = rcs = ts = cd = cs = sp = cmdthrust = cmdrudder = rudder =
		0.0;
	speed = 0.0;
	running = false;
	verbose = true;
}

SimulatedSurfaceObject::~SimulatedSurfaceObject()
{
	if(running) {
		running = false;
                thr.Stop();
	}
}

void SimulatedSurfaceObject::Run(void (*uf)(void *, SimulatedSurfaceObject *),
				 void *uptr)
{
	up = uptr;
	ufn = uf;
        thr.Initialise(&trampoline, this);
}

static inline double gethrtime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((double)tv.tv_sec) + ((double)tv.tv_usec)/1000000.0;
}

void SimulatedSurfaceObject::RunThread()
{
	running = true;
	double stime, lltime;

	while(running) {
		stime = gethrtime();
		StepSystem();
		lltime = gethrtime();
		int tts = ((int)(1000000.0 * ((1.0/cps) - (lltime - stime))));
		usleep(tts < 0? 0: tts);
	}
}

void SimulatedSurfaceObject::StepSystem()
{
	// move rudder
	double inr = cmdrudder * 180.0/100.0;
	if(inr != rudder) {
		double diff = (inr - rudder);
		if(diff > (1.0/cps) * rcs) diff = (1.0/cps) * rcs;
		if(diff < (1.0/cps) * -rcs) diff = (1.0/cps) * -rcs;
		rudder += diff;
	}

	// add current
	x += sin(cd * M_PI/180.0) * cs * (1.0/cps);
	y += cos(cd * M_PI/180.0) * cs * (1.0/cps);

	// figure out speed
	double cspd = ts * cmdthrust / 100.0;
	speed += (1.0/cps) * (sp/100.0) * (cspd - speed);
	
	double forwardspeed = speed * (90.0 - fabs(rudder))/90.0;
	double yawrot = (1.0/(5*cps)) * (cmdthrust / 100.0) * rudder;
		
	yaw += yawrot * (1.0/cps);
	
	// integrate
	x += sin(yaw) * forwardspeed * (1.0/cps);
	y += cos(yaw) * forwardspeed * (1.0/cps);

	// send update
	if(ufn) ufn(up, this);

	if(verbose) {
		fprintf(stderr, "x=%lf, y=%lf, yaw=%lf, th=%lf, rd=%lf\n",
			x, y, yaw, cmdthrust, rudder);
		fprintf(stderr, "fwspd=%lf, speed=%lf, yawrot=%lf, cspd=%lf\n",
			forwardspeed, speed, yawrot, cspd);
	}
}

