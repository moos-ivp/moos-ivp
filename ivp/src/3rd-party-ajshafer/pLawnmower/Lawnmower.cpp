/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: Lawnmower.cpp                                   */
/*    DATE: Nov 27 2007                                     */
/************************************************************/

#include <iterator>
#include "Lawnmower.h"
#include "ArtifactUtils.h"
#include "XYPolygon.h"
#include "XYSegList.h"

//---------------------------------------------------------
// Constructor

Lawnmower::Lawnmower()
{
}

//---------------------------------------------------------
// Destructor

Lawnmower::~Lawnmower()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Lawnmower::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::reverse_iterator p;
	
	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
		CMOOSMsg &msg = *p;
		if (MOOSStrCmp(msg.GetKey(), "LAWNMOWER_POLYGON")){
			// MOOSTrace("Receiving LAWNMOWER_POLYGON Message...\n"); //Debug Line
			// Parse out the options and pass on the request
			
			XYSegList slOutput;
			XYPolygon poly;
			double px0, py0, ang, radius;
			bool clockwise;
			
			slOutput = generateLawnmower(poly, px0, py0, ang, radius, clockwise);
		}
	}
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Lawnmower::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Register("LAWNMOWER_POLYGON", 0);
	
	return true;
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Lawnmower::Iterate()
{
	// happens AppTick times per second
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Lawnmower::OnStartUp()
{
	// happens before connection is open
	
	return true;
}

