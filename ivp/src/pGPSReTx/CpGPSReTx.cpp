// $Header: /home/cvsroot/project-marine-shell/src/pGPSReTx/CpGPSReTx.cpp,v 1.6 2007/08/03 19:59:34 anrp Exp $
// (c) 2004

// CpGPSReTx.cpp: implementation of the CpGPSReTx class.
////////////////////////////////////////////////////////

#include <iterator>
#include <stdint.h>
#include <inttypes.h>
#include "CpGPSReTx.h"
#include "tokenize.h"
#include "sutil.h"
#include "remap.h"
#include "dtime.h"

using namespace std;

CpGPSReTx::CpGPSReTx()
{
}

CpGPSReTx::~CpGPSReTx()
{
}

bool CpGPSReTx::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator it;
	
	for(it = NewMail.begin(); it != NewMail.end(); it++) {
		CMOOSMsg &m = *it;

		if(m.m_sKey == "GPS_RAW" && m.m_sVal.find("GPRMC") != m.m_sVal.npos) {
			last_gprmc = m.m_sVal;
		}
	}

	NewMail.clear();
	
	return true;
}

bool CpGPSReTx::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet

	string host;
	int port;

	m_MissionReader.GetConfigurationParam("Host", host);
	m_MissionReader.GetConfigurationParam("Port", port);

	m_MissionReader.GetConfigurationParam("MinTimeBetweenGPRMC", min_time_between_tx);

	last_tx = 0;
	
	m_Comms.Register("GPS_RAW", 0);
	
	return true;
}

bool CpGPSReTx::Iterate()
{	
	sock->NonBlockingRead();
	sock->AllQueueFlush();
	if(last_tx + min_time_between_tx < dtime()) {
		last_tx = dtime();
		printf("Txing %s\n", last_gprmc.c_str());
		sock->AppendWriteQueue(last_gprmc.c_str());
		sock->FullQueueWrite();
	}

	return true;
}

bool CpGPSReTx::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

