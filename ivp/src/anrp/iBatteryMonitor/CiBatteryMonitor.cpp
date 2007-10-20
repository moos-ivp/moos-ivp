// $Header: /home/cvsroot/project-marine-shell/src/iBatteryMonitor/CiBatteryMonitor.cpp,v 1.6 2007/08/03 19:59:34 anrp Exp $
// (c) 2004

// CiBatteryMonitor.cpp: implementation of the CiBatteryMonitor class.
////////////////////////////////////////////////////////

#include <iterator>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "CiBatteryMonitor.h"
#include "tokenize.h"
#include "sutil.h"
#include "remap.h"
#include "NMEAMessage.h"

using namespace std;

CiBatteryMonitor::CiBatteryMonitor()
{
}

CiBatteryMonitor::~CiBatteryMonitor()
{
}

bool CiBatteryMonitor::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator it;
	
	for(it = NewMail.begin(); it != NewMail.end(); it++) {
		CMOOSMsg &msg = *it;
	}

	NewMail.clear();
	
	return true;
}

bool CiBatteryMonitor::OnConnectToServer()
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
	
	pt = new CSerialPort(port);
	pt->SetBaudRate(speed);

	pthread_create(&thr, NULL, &trampoline, this);
	
	return true;
}

void CiBatteryMonitor::thread(void)
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
		
		//printf("%s\n", s);
		NMEAMessage m(s);
		
		if(m.Part(0) == "BAGSG") {
			double volts = atof(m.Part(1).c_str())/1000.0;
			double current = atof(m.Part(2).c_str())/100.0;
			double temp = atof(m.Part(5).c_str())/100.0;

			m_Comms.Notify("BATTERY_VOLTAGE", volts);
			m_Comms.Notify("BATTERY_CURRENT", current);
			m_Comms.Notify("BATTERY_TEMPERATURE", temp);
		}

		free(s);
	}
}

bool CiBatteryMonitor::Iterate()
{
	
	return true;
}

bool CiBatteryMonitor::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

