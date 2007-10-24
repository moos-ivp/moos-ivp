#ifndef __SimulatedSurfaceObject_h__
#define __SimulatedSurfaceObject_h__

#include <string>
#include <MOOSThread.h>

using namespace std;

class SimulatedSurfaceObject {
public:
	SimulatedSurfaceObject();
	~SimulatedSurfaceObject();

	void Run(void (*)(void *, SimulatedSurfaceObject *), void *);

	void CyclesPerSecond(double d) { cps = d; }
	void X(double d) { x = d; }
	void Y(double d) { y = d; }
	void Yaw(double d) { yaw = d; }

	double X(void) { return x; }
	double Y(void) { return y; }
	double Yaw(void) { return yaw; }
	
	void RudderChangeSpeed(double d) { rcs = d; }
	void ThrustSpeed(double d) { ts = d; }
	void SetCurrentDirection(double d) { cd = d; }
	void SetCurrentStrength(double d) { cs = d; }
	void SetSlowdownPercent(double d) { sp = d; }
	void SetVerbose(bool b) { verbose = b; }

	void SetThrust(double pow) { cmdthrust = pow; }
	void SetRudder(double rud) { cmdrudder = rud; }

	void StepSystem();
	
private:
	double cps;
	
	double x, y, yaw;

	double rcs, ts, cd, cs, sp;
	double cmdthrust, cmdrudder;
	double rudder;
	double speed;
	bool verbose;

	CMOOSThread thr;
	void (*ufn)(void *, SimulatedSurfaceObject *);
	bool running;
	void *up;
	void RunThread();

        // The 'bool' return value is just to satisfy CMOOSThread's signature
        // requirements.
	static bool trampoline(void *p) {
		((SimulatedSurfaceObject *)p)->RunThread();
		return true;
	}
};

#endif
