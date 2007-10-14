// $Header: /home/cvs/repository/project-plusnet/src/iWinchCtl/CiWinchCtl.cpp,v 1.2 2006/08/22 23:55:06 cvs Exp $
// (c) 2005

// CiWinchCtl.cpp: implementation of the CiWinchCtl class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiWinchCtl.h"
#include "dtime.h"
#include <signal.h>
#include <math.h>

bool sigflag = false;

void sigh(int)
{
	sigflag = true;
}

CiWinchCtl::CiWinchCtl()
{
	// constructor
	signal(SIGINT, sigh);
	signal(SIGTERM, sigh);

	seeking = false;
}

CiWinchCtl::~CiWinchCtl()
{
	// destructor
}

bool CiWinchCtl::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		
		if(msg.m_sKey == "CTD_PRESSURE" && !msg.IsSkewed(MOOSTime())) {
			depth = msg.m_dfVal;
			depth_time = dtime();
		} else if(msg.m_sKey == "WINCH_ENCODER_POS" && !msg.IsSkewed(MOOSTime())) {
			encposcurrent = msg.m_dfVal;
		} else if(msg.m_sKey == "WINCH_DESIRED_DEPTH" && !msg.IsSkewed(MOOSTime())) {
			wanted_depth = msg.m_dfVal;
			seeking = true;
		}
			
	}

	NewMail.clear();
	
	return true;
}

bool CiWinchCtl::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
	
	m_Comms.Register("CTD_PRESSURE", 0);
	m_Comms.Register("WINCH_DESIRED_DEPTH", 0);
	m_Comms.Register("WINCH_ENCODER_POS", 0);

	m_MissionReader.GetConfigurationParam("EncoderJogDown", encjogdown);
	m_MissionReader.GetConfigurationParam("EncoderJogUp", encjogup);
	m_MissionReader.GetConfigurationParam("EncoderSeekDown", encseekdown);
	m_MissionReader.GetConfigurationParam("EncoderSeekUp", encseekup);
	
	return true;
}

bool CiWinchCtl::Iterate()
{
	if(!seeking) return true;
	if(depth_time + 2.0 < dtime()) {
		printf("Timeout! bad!\n");
		m_Comms.Notify("WINCH_DESIRED_ENCPOS", encposcurrent);
		return true;
	}
	// WINCH_DESIRED_ENCPOS
	
	double diff = wanted_depth - depth;
	
	if(fabs(diff) < 2.0) {
		m_Comms.Notify("WINCH_DESIRED_ENCPOS", encposcurrent);
		return true;
	}

	double outep = encposcurrent;
	if(diff > 4.0) {
		outep += encseekup;
	} else if(diff > 2.0 && diff < 4.0) {
		outep += encjogup;
	} else if(diff < -2.0 && diff > -4.0) {
		outep += encjogdown;
	} else if(diff < -4.0) {
		outep += encseekdown;
	}
	
	printf("Current position %lf, difference %lf (%lf-%lf), output %lf\n",
		encposcurrent, diff, wanted_depth, depth, outep);
	m_Comms.Notify("WINCH_DESIRED_ENCPOS", outep);
		
	return true;
}

bool CiWinchCtl::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

