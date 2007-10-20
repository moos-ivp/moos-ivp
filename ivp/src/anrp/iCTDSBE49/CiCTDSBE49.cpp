// $Header: /home/cvsroot/project-plusnet-shell/src/iCTDSBE49/CiCTDSBE49.cpp,v 1.2 2006/08/29 01:55:42 mikerb Exp $
// (c) 2005

// CiCTDSBE49.cpp: implementation of the CiCTDSBE49 class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiCTDSBE49.h"

using namespace std;

CiCTDSBE49::CiCTDSBE49()
{
	// constructor
}

CiCTDSBE49::~CiCTDSBE49()
{
	// destructor
}

bool CiCTDSBE49::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		
		if(msg.m_sKey == "DESIRED_THRUST") {
			// ...
		}
	}

	NewMail.clear();
	
	return true;
}

void *CiCTDSBE49::trampoline(void *arg)
{
	((CiCTDSBE49 *)arg)->CommThreadProc();
	return NULL;
}

void CiCTDSBE49::CommThreadProc()
{
	pt->AppendWriteQueue("\r\n");
	pt->FullQueueWrite();
	pt->ReadUntilChar('\n', 5000000);
	
	for(int i=0; i<4; i++) {
		pt->AppendWriteQueue("STOP\r\n");
		pt->FullQueueWrite();
		pt->DurationRead(500000);
	}
	pt->AllQueueFlush();
	
	pt->AppendWriteQueue("\r\n");
	pt->FullQueueWrite();
	pt->ReadUntilChar('\n', 5000000);

	if(pt->ReadBufSize()) {
		pt->AllQueueFlush();
		char *cfg[] = {
			"OUTPUTSAL=y",
			"OUTPUTSV=y",
			"OUTPUTFORMAT=3",
			"NAVG=4",
			"AUTORUN=N",
			"MINCONDFREQ=10",
			NULL,
		};

		for(char **p = cfg; *p != NULL; p++) {
			pt->AppendWriteQueue(*p);
			pt->AppendWriteQueue("\r\n");
			pt->FullQueueWrite();
			pt->ReadUntilStr(2, "\r\n");
		}
	} else {
		fprintf(stderr, "no device?\n");
		throw;
	}

	pt->DurationRead(250000);
	pt->AllQueueFlush();
	pt->AppendWriteQueue("START\r\n");
	pt->FullQueueWrite();
	
	while(true) {
		pt->ReadUntilChar('\n', 5000000);
		if(pt->ReadBufSize()) {
			char *str = pt->Read(pt->ReadBufSize());
			MOOSTrace("str = [%s]\n", str);
			double t, c, p, d, s, v, r;
			double depth = p / 1.01325;
			
			int res = sscanf(str, "%lf, %lf, %lf, %lf, %lf",
					&t, &c, &p, &s, &v);

			if(res == 5) {
				m_Comms.Notify("CTD_TEMPERATURE", t);
				m_Comms.Notify("CTD_CONDUCTIVITY", c);
				m_Comms.Notify("CTD_PRESSURE", p);
				m_Comms.Notify("CTD_SALINITY", s);
				m_Comms.Notify("CTD_SOUND_VELOCITY", v);
				m_Comms.Notify("CTD_DEPTH", depth);
				
				//added 10/4/2007 for ctd logger process-dpe
				double tStamp = MOOSTime();
				char str[500];
				sprintf(str,"%lf,%lf,%lf,%lf,%lf,%lf,%lf",tStamp,
				       depth,t,c,p,s,v);
				
				// tes 10-3-07, added new string for easy logging
				// with pCTDLogger
				m_Comms.Notify("CTD_TCPSV", str);
			}
					
			free(str);
		} else {
			MOOSTrace("Timeout: device not there?\n");
			continue;
		}
	}
}

bool CiCTDSBE49::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
	
	string port; int bd;
	double d_bd;

	if(m_MissionReader.GetConfigurationParam("Port", port)) {
		m_MissionReader.GetConfigurationParam("BaudRate", d_bd);
		bd = (int)d_bd;
		
		int fd;

		fd = open(port.c_str(), O_NOCTTY | O_NDELAY | O_RDWR | O_NONBLOCK);
		if(fd == -1) {
			perror("Can't open port");
			throw;
		}
		pt = new CSerialPort(fd);
		pt->SetBaudRate(bd);
	} else {
		MOOSTrace("Please set Port in the mission file\n");
		throw;
	}

	pthread_create(&cTH, NULL, &(CiCTDSBE49::trampoline), this);
	
	return true;
}

bool CiCTDSBE49::Iterate()
{
	// happens AppTick times per second
	
	return true;
}

bool CiCTDSBE49::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

