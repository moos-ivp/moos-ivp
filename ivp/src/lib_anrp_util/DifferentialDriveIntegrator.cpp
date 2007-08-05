#include "DifferentialDriveIntegrator.h"
#include <math.h>

void DifferentialDriveIntegrator::StepSystem(double readingtime, double lmovement, double rmovement)
{
	double l_r, r_r;

	l_r = lmovement * c2m;
	r_r = rmovement * c2m;

	double a_d, l_d;

	a_d = (r_r - l_r) / al;
	l_d = (r_r + l_r) / 2.0;

	lhistory.push_front(make_pair(readingtime, l_d));
	thistory.push_front(make_pair(readingtime, a_d));

	x += l_d * cos(theta + (a_d / 2.0));
	y += l_d * sin(theta + (a_d / 2.0));
	theta += a_d;

	while(theta < -M_PI) theta += 2*M_PI;
	while(theta >  M_PI) theta -= 2*M_PI;

	CleanupVelocities();
	UpdateVelocities();
}

void DifferentialDriveIntegrator::VelsToRPS(double tv, double rv, double &lw, double &rw)
{
	double wc = wd * 2 * M_PI;

	rw = lw = tv / wc * 2;
	
	// pos rv = turn right (lw up, rw down)
	// neg rv = turn left  (lw down, rw up)
	double rvm = rv * (al * M_PI);

	lw += rvm;
	rw -= rvm;
}

void DifferentialDriveIntegrator::CleanupVelocities()
{
	double now = dtime();
	
	while(lhistory.size() > 0 && lhistory.back().first < now - MAX_DV_DT_HISTORY_TIME) {
		lhistory.pop_back();
	}

	while(thistory.size() > 0 && thistory.back().first < now - MAX_DV_DT_HISTORY_TIME) {
		thistory.pop_back();
	}
}

void DifferentialDriveIntegrator::UpdateVelocities()
{
	list<pair<double,double> >::iterator it;

	double total = 0; int cyc = 0;
	
	for(it = lhistory.begin(); it != lhistory.end(); it++) {
		total += it->second;
		cyc++;
	}

	lvel = total / (double)cyc;

	total = cyc = 0;
	
	for(it = thistory.begin(); it != thistory.end(); it++) {
		total += it->second;
		cyc++;
	}
}

