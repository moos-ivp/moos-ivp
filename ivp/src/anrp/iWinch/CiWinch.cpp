// $Header: /home/cvs/repository/project-plusnet/src/iWinch/CiWinch.cpp,v 1.2 2006/08/22 23:55:06 cvs Exp $
// (c) 2005

// CiWinch.cpp: implementation of the CiWinch class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiWinch.h"
#include "dtime.h"
#include <signal.h>

bool sigflag = false;

void sigh(int)
{
	sigflag = true;
}

CiWinch::CiWinch()
{
	// constructor
	signal(SIGINT, sigh);
	signal(SIGTERM, sigh);
}

CiWinch::~CiWinch()
{
	// destructor
}

bool CiWinch::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		
		if(msg.m_sKey == "WINCH_DESIRED_ENCPOS" && !msg.IsSkewed(MOOSTime())) {
			winch->Seek(msg.m_dfVal);
		} else if(msg.m_sKey == "WINCH_ENABLED" && !msg.IsSkewed(MOOSTime())) {
			if(msg.m_dfVal == 0) {
				winch->Disable();
			} else {
				winch->Enable();
			}
		}
	}

	NewMail.clear();
	
	return true;
}

bool CiWinch::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", <max frequency at which to get
	//                             updates, 0 = max>);
	// note, you cannot ask the server for anything in this function yet
	
	m_Comms.Register("WINCH_DESIRED_ENCPOS", 0);
	m_Comms.Register("WINCH_ENABLED", 0);
	
	string pt; int bd;
	
	m_MissionReader.GetConfigurationParam("Port", pt);
	m_MissionReader.GetConfigurationParam("Speed", bd);

	winch = new CWinch(pt, bd);

	string tmp;
	m_MissionReader.GetConfigurationParam("FeedbackType", tmp);
	MOOSToUpper(tmp);

	if(tmp == "SPOOL") {
		double ld, sw, sd, ll, ecprs;

		m_MissionReader.GetConfigurationParam("LineDiameter", ld);
		m_MissionReader.GetConfigurationParam("SpoolWidth", sw);
		m_MissionReader.GetConfigurationParam("SpoolDiameter", sd);
		m_MissionReader.GetConfigurationParam("LineLength", ll);
		m_MissionReader.GetConfigurationParam("EncoderCountsPerRevolutionOfSpool", ecprs);

		winch->ConfigureSpool(ecprs, ld, sw, sd, ll);
	} else if (tmp == "LINE") {
		double lpt, ecprw;
		m_MissionReader.GetConfigurationParam("LinePerTurn", lpt);
		m_MissionReader.GetConfigurationParam("EncoderCountsPerRevolutionOfWheel", ecprw);

		winch->ConfigureLineFeeder(ecprw, lpt);
	}	
	
	m_MissionReader.GetConfigurationParam("ReversedRelation", tmp);
	MOOSToUpper(tmp);
	winch->SetReversed(tmp == "true" ? true : false);
	winch->GetZeroCurrentLevel();
	winch->Enable();
	
	return true;
}

bool CiWinch::Iterate()
{
	// happens AppTick times per second
	
	m_Comms.Notify("WINCH_ENCODER_POS", winch->GetEncoderPosition());
	m_Comms.Notify("WINCH_CURRENT_RAW", winch->GetCurrent());

	if(sigflag == true) { // time to exit
		delete winch;
		exit(1);
	}
	
	return true;
}

bool CiWinch::OnStartUp()
{
	// happens after connection is completely usable
	// ... not when it *should* happen. oh well...
	
	return true;
}

