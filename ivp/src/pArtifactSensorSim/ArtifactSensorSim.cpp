/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ArtifactSensorSim.cpp                           */
/*    DATE: Dec 29th 1963                                   */
/************************************************************/

#include <iterator>
#include "ArtifactSensorSim.h"
//---------------------------------------------------------
// Constructor

ArtifactSensorSim::ArtifactSensorSim()
{
}

//---------------------------------------------------------
// Destructor

ArtifactSensorSim::~ArtifactSensorSim()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ArtifactSensorSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::reverse_iterator p;
	
	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
		CMOOSMsg &msg = *p;
	}
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ArtifactSensorSim::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	
	return true;
}

//---------------------------------------------------------
// Procedure: Iterate()

bool ArtifactSensorSim::Iterate()
{
	// happens AppTick times per second
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ArtifactSensorSim::OnStartUp()
{
	// happens before connection is open
	
	return true;
}

