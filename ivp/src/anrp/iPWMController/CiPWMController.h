// $Header: /home/cvsroot/project-marine-shell/src/iPWMController/CiPWMController.h,v 1.4 2007/08/03 19:58:50 anrp Exp $
// (c) 2004 
// CiPWMController.h: interface for the CiPWMController class.
////////////////////////////////////////////////

#ifndef __CiPWMController_h__
#define __CiPWMController_h__

#include "MOOSLib.h"
#include "CPWMController.h"

class servo_params_t {
public:
	servo_params_t() :
		servon(0), mode(MODE_DISABLED),
		enabled(false), dflt(0), desired(0), k(0.3), current(0), in_low(0),
		in_high(0), clamp_in_low(0), clamp_in_high(0),
		last_set(0), input_offset(0) {}

	int servon;
	pwm_mode_t mode;
	bool enabled;
	double dflt;
	double desired;
	double k;
	double current;
	double in_low;
	double in_high;
	double clamp_in_low;
	double clamp_in_high;
	double last_set;
	double input_offset;
};
	
typedef std::map<std::string, servo_params_t> servo_map;

class CiPWMController : public CMOOSApp
{
public:
	CiPWMController();
	virtual ~CiPWMController();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	CPWMController *pwm;
	static void cb(void *obj, int i, double v) {
		((CiPWMController *)obj)->callback(i, v);
	}
	void callback(int port, double volts);

	servo_map mapping;
};

#endif /* __CiPWMController_h__ */
