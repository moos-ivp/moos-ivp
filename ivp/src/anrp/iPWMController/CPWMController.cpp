// for PRI*
#define __STDC_FORMAT_MACROS

#include "CPWMController.h"
#include <fcntl.h>
#include <inttypes.h>

using namespace std;

CPWMController::CPWMController(int npwm, int nana)
{
	running = false;

	n_servos = npwm;
	n_analog = nana;
	info.resize(npwm);

	for(int i=0; i<n_servos; i++) {
		info[i].first = MODE_DISABLED;
		info[i].second = 0;
	}
	
	port = NULL;
	portn = "";

	userobj = NULL;
	usercb = NULL;
}

CPWMController::~CPWMController()
{
	StopCommThread();
}

void CPWMController::SetPort(string port)
{
	portn = port;
}

void CPWMController::CommThreadProc()
{
	// select board
	port->AppendWriteQueue("BD0\r");
	
	// set all outputs to digital and zero them
	for(int i=0; i<n_servos; i++) {
		char cmd[21];
		snprintf(cmd, 21, "SV%iM0\r", i+1);
		port->AppendWriteQueue(cmd);
	}

	port->FullQueueWrite();
	
	while(running) {
		for(int i=0; i<n_servos; i++) {
			char cmd[21];
			if(info[i].first == MODE_PWM) {
				snprintf(cmd, 21, "SV%iM%"PRIu8"\r", i+1, info[i].second);
				//printf("%s\n", cmd);
				port->AppendWriteQueue(cmd);
			} else if(info[i].first == MODE_DIGITAL) {
				snprintf(cmd, 21, "SV%iM0P%c%i\r", i+1, info[i].second == 0?
					 'C' : 'S', i+1);
				//printf("%s\n", cmd);
				port->AppendWriteQueue(cmd);
			}
		}

		port->FullQueueWrite();

		// read all analog inputs
		for(int i=0; i<n_analog; i++) {
			char tmp[30];
			snprintf(tmp, 30, "AD%i\r", i+1);
			port->AppendWriteQueue(tmp);
			port->FullQueueWrite();
			port->ReadUntilLen(1);
			int l = port->ReadBufSize();

			if(l > 0) {
				char *p = port->Read(l);
				int sv = atoi(p);
				double v = ((double)sv) / 255.0 * 5.0;
				
				if(usercb) {
					(*(usercb))(userobj, i, v);
				}

				free(p);
			} else {
				fprintf(stderr, "no board attached?\n");
			}
		}
	}

	// turn off all outputs
	for(int i=0; i<n_servos; i++) {
		char cmd[21];
		snprintf(cmd, 21, "SV%iM0\r", i+1);
		port->AppendWriteQueue(cmd);
	}

	port->FullQueueWrite();
}

void CPWMController::StartCommThread(void)
{
	if(running) return;

	port = new CSerialPort(portn);
	port->SetBaudRate(9600);
	
	running = true;
	pthread_create(&thr, NULL, trampoline, this);

	return;
}

void CPWMController::StopCommThread(void)
{
	if(running) {
		running = false;
		pthread_join(thr, NULL);
		delete port; port = NULL;
	}
}


