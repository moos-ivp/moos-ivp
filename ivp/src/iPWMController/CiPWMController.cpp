// $Header: /home/cvsroot/project-marine-shell/src/iPWMController/CiPWMController.cpp,v 1.6 2007/08/03 19:59:34 anrp Exp $
// (c) 2004

// CiPWMController.cpp: implementation of the CiPWMController class.
////////////////////////////////////////////////////////

#include <iterator>
#include <stdint.h>
#include <inttypes.h>
#include "CiPWMController.h"
#include "tokenize.h"
#include "sutil.h"
#include "remap.h"

using namespace std;

static inline uint8_t mu8SCALE(double v, double il, double iu) {
	double rv = remap(v, il, iu, 1, 255);
	return (uint8_t)rv;
}

static inline double dCLAMP(const double in, const double low, const double high)
{
	if(in < low) return low;
	if(in > high) return high;
	return in;
}

CiPWMController::CiPWMController()
{
}

CiPWMController::~CiPWMController()
{
}

bool CiPWMController::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator it;
	
	for(it = NewMail.begin(); it != NewMail.end(); it++) {
		CMOOSMsg &msg = *it;
		
		if(mapping.find(msg.m_sKey) != mapping.end()) {
			servo_params_t &p = mapping[msg.m_sKey];
			p.last_set = MOOSTime();
			if(p.mode == MODE_PWM) {
				double iv = dCLAMP(msg.m_dfVal+p.input_offset, p.clamp_in_low, p.clamp_in_high);
				pwm->SetPower(p.servon, mu8SCALE(iv, p.in_low, p.in_high));
			} else if(p.mode == MODE_DIGITAL) {
				bool enabled = strtobool(msg.m_sVal);
				pwm->SetDigital(p.servon, enabled);
				fprintf(stdout, "setting %i to %s\n", p.servon, enabled? "true" : "false");
			}
		}
	}

	NewMail.clear();
	
	return true;
}

bool CiPWMController::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet

	string port;

	m_MissionReader.GetConfigurationParam("Port", port);
	
	pwm = new CPWMController(8, 5); // defaults for pontech
	pwm->SetPort(port);
	pwm->SetCallback(&cb, this);

	// read in config info here...
	for(int i=1; i<=8; i++) {
		char n[20];
		snprintf(n, 20, "Servo%i", i);
		string tmp;
		if(m_MissionReader.GetConfigurationParam(n, tmp)) {
			printf("got line %s\n", tmp.c_str());
			vector<string> vs = explode(tmp, ",");

			for(vector<string>::iterator it = vs.begin(); it != vs.end(); it++) *it = clean_spaces(*it);

			if(vs.size() == 0) continue;

			servo_params_t p;
			p.servon = i;
			
			if(strcasecmp(vs[0].c_str(), "digital")==0) {
				if(vs.size() < 3) {
					throw std::runtime_error(string("Invalid number of arguments for ") + n);
				}

				p.mode = MODE_DIGITAL;
				p.enabled = strtobool(vs[2]);

				mapping[vs[1]] = p;
			} else if(strcasecmp(vs[0].c_str(), "pwm")==0) {
				if(vs.size() < 6) {
					throw std::runtime_error(string("Invalid number of arguments for ") + n);
				}

				p.mode = MODE_PWM;
				p.in_low = stof(vs[2]);
				p.in_high = stof(vs[3]);
				p.clamp_in_low = stof(vs[4]);
				p.clamp_in_high = stof(vs[5]);
				
				if(vs.size() >= 7) {
					p.dflt = stof(vs[6]);
				} else {
					p.dflt = (p.in_low + p.in_high) / 2.0;
				}

				mapping[vs[1]] = p;
			}
		}
	}

	for(servo_map::iterator it = mapping.begin(); it != mapping.end(); it++) {
		m_Comms.Register(it->first, 0);
		servo_params_t &p = it->second;

		pwm->ConfigureServo(p.servon, p.mode);
		if(p.mode == MODE_DIGITAL) {
			pwm->SetDigital(p.servon, p.enabled);
		} else if(p.mode == MODE_PWM) {
			pwm->SetPower(p.servon, mu8SCALE(p.dflt, p.in_low, p.in_high));
		}
	}

	pwm->StartCommThread();
	
	return true;
}

bool CiPWMController::Iterate()
{
	// happens AppTick times per second
	
	for(servo_map::iterator it = mapping.begin(); it != mapping.end(); it++) {
		if(it->second.last_set + 5 < MOOSTime()) {
			printf("Timeout on %s\n", it->first.c_str());
			if(it->second.mode == MODE_PWM) {
				pwm->SetPower(it->second.servon, mu8SCALE(it->second.dflt, it->second.in_low, it->second.in_high));
			}
		}
	}
	
	return true;
}

void CiPWMController::callback(int input, double volts)
{
	char tmp[40];
	snprintf(tmp, 40, "ANALOG_IN_%i", input);
	m_Comms.Notify(tmp, volts);
	fprintf(stderr, "Read voltage on port %i @ %lf volts\n", input, volts);
}

bool CiPWMController::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

