// $Header: /home/cvsroot/project-marine-shell/src/iOS5000/CiOS5000.cpp,v 1.6 2007/08/03 19:59:34 anrp Exp $
// (c) 2004

// CiOS5000.cpp: implementation of the CiOS5000 class.
////////////////////////////////////////////////////////

#include <iterator>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "CiOS5000.h"
#include "tokenize.h"
#include "sutil.h"
#include "remap.h"

using namespace std;

CiOS5000::CiOS5000()
{
}

CiOS5000::~CiOS5000()
{
}

bool CiOS5000::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator it;
	
	for(it = NewMail.begin(); it != NewMail.end(); it++) {
		CMOOSMsg &msg = *it;
	}

	NewMail.clear();
	
	return true;
}

bool CiOS5000::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet

	string port;
	int speed;

	m_MissionReader.GetConfigurationParam("Port", port);
	m_MissionReader.GetConfigurationParam("Speed", speed);

	m_MissionReader.GetConfigurationParam("PreRotation", prerotation);
	
	pt = new CSerialPort(port);
	pt->SetBaudRate(speed);

	pthread_create(&thr, NULL, &trampoline, this);
	
	return true;
}

void CiOS5000::thread(void)
{
	while(1) {
		pt->ReadUntilChar('$');
		pt->ReadUntilChar('\r');
		int st = pt->FindCharIndex('$');
		if(st == -1) {
			pt->AllQueueFlush();
			continue;
		}
		free(pt->Read(st));
		int en = pt->FindCharIndex('\r');

		char *s = pt->Read(en+1);
		if(s == NULL) {
			pt->AllQueueFlush();
			continue;
		}

		char *head, *roll, *pitch, *temp;

		head = strchr(s, 'C');
		roll = strchr(s, 'R');
		pitch = strchr(s, 'P');
		temp = strchr(s, 'T');

		if(head) {
			double h = atof(++head);
			h+=prerotation;
			while(h > 360) h -= 360.0;
			while(h < 0) h += 360.0;
			double y = -h * M_PI/180.0;

			m_Comms.Notify("COMPASS_HEADING", h);
			m_Comms.Notify("COMPASS_YAW", y);

			printf("Heading: %lf\n", h);
		}

		if(roll) {
			double r = atof(++roll);

			m_Comms.Notify("COMPASS_ROLL", r);
		}

		if(pitch) {
			double p = atof(++pitch);

			m_Comms.Notify("COMPASS_PITCH", p);
		}

		if(temp) {
			double t = atof(++temp);
			
			m_Comms.Notify("COMPASS_TEMPERATURE", t);
		}

		free(s);
	}
}

bool CiOS5000::Iterate()
{
	
	return true;
}

bool CiOS5000::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

