#ifndef __CPWMController_h__
#define __CPWMController_h__

#include <pthread.h>
#include "CSerialPort.h"
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <vector>

typedef enum {
	MODE_DISABLED = 0,
	MODE_DIGITAL,
	MODE_PWM,
} pwm_mode_t;

class CPWMController {
public:
	CPWMController(int npwm, int nana);
	~CPWMController();

	void SetPort(std::string port);

	void StartCommThread(void);
	void StopCommThread(void);
	
	void ConfigureServo(int servo, pwm_mode_t mde) {
		if(servo > (n_servos+1) || servo < 1) {
			throw std::runtime_error("Servo out of range");
		} else if(mde < MODE_DISABLED || mde > MODE_PWM) {
			throw std::runtime_error("Mode out of range");
		}
		info[servo-1].first = mde;
	}
	
	void SetPower(int servo, uint8_t pwr) {
		if(pwr < 1) {
			throw std::runtime_error("Power out of range");
		} else if(servo > (n_servos+1) || servo < 1) {
			throw std::runtime_error("Servo out of range");
		} else if (info[servo-1].first != MODE_PWM) {
			throw std::runtime_error("SetPower not valid for this mode");
		}
		//printf("setting servo %i to %i\n", servo, pwr);
		info[servo-1].second = pwr;
	}

	void SetDigital(int servo, bool p) {
		if(servo > (n_servos+1) || servo < 1) {
			throw std::runtime_error("Servo out of range");
		} else if(info[servo-1].first != MODE_DIGITAL) {
			throw std::runtime_error("SetDigital not valid for this mode");
		}
		//printf("setting do %i to %s\n", servo, p? "true":"false");
		info[servo-1].second = (p == true)? 1 : 0;
	}

	void SetCallback(void (*cb)(void *, int, double), void *uo) {
		userobj = uo;
		usercb = cb;
	}

private:
	CSerialPort *port;
	std::string portn;

	pthread_t thr;
	static void *trampoline(void *p) {
		((CPWMController *)p)->CommThreadProc();
		return NULL;
	}
	void CommThreadProc();
	bool running;

	int n_servos;
	int n_analog;

	std::vector<pair<pwm_mode_t, uint8_t> > info;
	
	void (*usercb)(void *, int, double);
	void *userobj;
};

#endif /* __CFDCtl_h__ */
