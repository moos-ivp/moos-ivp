#ifndef __DifferentialDriveIntegrator_h__
#define __DifferentialDriveIntegrator_h__

#include "dtime.h"
#define MAX_DV_DT_HISTORY_TIME 0.2

#include <list>
using namespace std;

class DifferentialDriveIntegrator {
public:
	DifferentialDriveIntegrator() {
		al = 1;
		wd = 0.5;
		c2m = 1;
		ZeroPosition();
	}
	
	DifferentialDriveIntegrator(double axle_length, double wheel_diameter, double input_conv_to_m) {
		al = axle_length;
		wd = wheel_diameter;
		c2m = input_conv_to_m;
		ZeroPosition();
	}

	void SetAxleLength(double l) { al = l; }
	void SetWheelDiameter(double w) { wd = w; }
	void SetInputConversionFactor(double icf) { c2m = icf; }

	void VelsToRPS(double tv, double rv, double &lw, double &rw);

	double X() { return x; }
	double Y() { return y; }
	double T() { return theta; }
	double V() { return lvel; }
	double TV() { return tvel; }
	
	void ZeroPosition() {
		x = y = theta = lvel = tvel = 0;
		lhistory.clear();
		thistory.clear();
	}

	void StepSystem(double readingtime, double lmovement, double rmovement);
	
private:
	list<pair<double,double> > lhistory;
	list<pair<double,double> > thistory;

	void CleanupVelocities();
	void UpdateVelocities();
	
	double x, y, theta, lvel, tvel;
	double al, wd, c2m;
};

#endif
