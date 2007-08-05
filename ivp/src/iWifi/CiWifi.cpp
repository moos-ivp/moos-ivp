// $Header: /home/cvsroot/project-marine-shell/src/iWifi/CiWifi.cpp,v 1.4 2007/07/26 14:17:32 mikerb Exp $
// (c) 2004

// CiWifi.cpp: implementation of the CiWifi class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiWifi.h"
#include <cstdlib>
#include <iostream>

using namespace std;

CiWifi::CiWifi()
{
	// constructor
	interface = "wlan0";
}

CiWifi::~CiWifi()
{
	// destructor
}

bool CiWifi::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::reverse_iterator p;
	
	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
		CMOOSMsg &msg = *p;
	}
	
	return true;
}

bool CiWifi::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	
	return true;
}

bool CiWifi::Iterate()
{
	// happens AppTick times per second
	string command = "/sbin/iwconfig " + interface + " | grep Link";
	FILE* pipe = popen(command.c_str(), "r");
	if(pipe == NULL)
		return false;

	string line;
	
	char buffer[128];
	while(!feof(pipe))
		if(fgets(buffer, 128, pipe) != NULL)
			line += buffer;	
	pclose(pipe);

	MOOSChomp(line, "=");
	string numer = MOOSChomp(line, "/");
	int num = atoi(numer.c_str());
	string denom = MOOSChomp(line, " ");
	int den = atoi(denom.c_str());

	double quality = 0;
	if(den!=0)
	  quality = (double)num/(double)den;
	m_Comms.Notify("WIFI_QUALITY", quality);
	return true;
}

bool CiWifi::OnStartUp()
{
	// happens before connection is open
	m_MissionReader.GetConfigurationParam(m_sAppName, "interface", interface);		
	return true;
}

