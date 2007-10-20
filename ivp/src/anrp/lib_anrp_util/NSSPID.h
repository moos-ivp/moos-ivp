#ifndef __NSSPID_H__
#define __NSSPID_H__

#include <dtime.h>
#include <math.h>
#include "DoubleLinearInterp.h"

class NSSPID {
public:
	NSSPID() {
		max_history = 2.0;
		setpoint = 0.0;
		setting = 0.0;
		error = 0.0;
		deriv_period = 0.5;
		deriv_step = 0.1;
		fsrange = 1;
	}
	
	~NSSPID() {}

	void SetSetpoint(double p) { setpoint = p; }
	void SetFullScaleRange(double v) { fsrange = v; }
	void SetMaxHistory(double t) { max_history = t; }
	void SetGains(double p, double i, double d) {
		pgain = p; igain = i; dgain = d;
	}
	
	double Update(double time, double val)
	{
		CleanBuffers();
		
		error = setpoint - val;

		// ...
		//while(error > M_PI) error -= M_PI*2;
		//while(error < -M_PI) error += M_PI*2;
		
		history[time] = val;
		setpoints[time] = setpoint;
		errors[time] = error;
		
		/* derivative */
		double dval = 0.0;
		DoubleInterpBuffer derivs;
		for(double t = time; t > time - deriv_period; t -= deriv_step) {
			derivs[t - deriv_step/2.0] = (errors(t-deriv_step) - errors(t)) / deriv_step;
		}

		dval = derivs[derivs.MaxKey()];
		dval *= 2/fsrange;

		/* sum previous error */
		double ierror = 0.0;
		if(history.size()) {
			double tv = 0.0;
			for(DoubleInterpBufferIterator it = history.begin(); 
					it != history.end(); it++) {
				ierror += history[it->first] - setpoints[it->first];
				tv += 1;
			}

			ierror /= tv;
			ierror *= 2/fsrange;
		}

		setting = (pgain * error) + (igain * ierror) - (dgain * dval);

		double osetting = setting;
		setting *= fsrange/2;
		if(setting > 1.0) setting = 1.0;
		if(setting < -1.0) setting = -1.0;

		fprintf(stderr, "time = %f, setpoint = %f, pos = %f, error = %f, ierror = %f, dval = %f, setting = %f, osetting = %f\n", time, setpoint, val, error, ierror, dval, setting, osetting);

		settings[time] = setting;
		return setting;
	}
	
private:
	
	double setpoint;
	double setting;
	double max_history;
	double deriv_period;
	double deriv_step;
	double error;
	double fsrange;

	double pgain, igain, dgain;
	
	DoubleInterpBuffer history;
	std::map<double,double> settings;
	std::map<double,double> setpoints;
	DoubleInterpBuffer errors;

	void CleanBuffers()
	{
		double now = dtime();
		history.EraseOld(now - max_history);
		while(settings.size() && settings.begin()->first + max_history < now) settings.erase(settings.begin());
		while(setpoints.size() && setpoints.begin()->first + max_history < now) setpoints.erase(setpoints.begin());
		errors.EraseOld(now - max_history);
	}
};

#endif /* __NSSPID_H__ */

