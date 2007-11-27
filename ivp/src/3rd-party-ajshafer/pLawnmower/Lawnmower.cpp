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
#include "MBUtils.h"
#include "AngleUtils.h"

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
			// A LAWNMOWER_POLYGON string looks like this, arguments in any order:
			// poly=polystring#x=blah#y=blah#ang=blah#radius=blah[#clockwise=truefalse];
			// x, y = initial position
			// ang = [0,360], N = 0, clockwise positive
			// radius = 1/2 distance between lines
			// clockwise is first turn, defaults to true;
			
			bool ok = true;
			std::string sMsg = msg.GetString();
			
			XYSegList slOutput;
			
			XYPolygon poly;
			double px0, py0, ang, radius;
			bool clockwise = true;
			std::string spoly, spx0, spy0, sang, sradius, sclockwise;
			
			ok = tokParse(sMsg, "poly", '#', '=', spoly) && ok;
			ok = tokParse(sMsg, "x", '#', '=', spx0) && ok;
			ok = tokParse(sMsg, "y", '#', '=', spy0) && ok;
			ok = tokParse(sMsg, "ang", '#', '=', sang) && ok;
			ok = tokParse(sMsg, "radius", '#', '=', sradius) && ok;
			if (!tokParse(sMsg, "clockwise", '#', '=', sclockwise)) {sclockwise="TRUE";}
			if (toupper(sclockwise) == "TRUE"){ clockwise = true; }
			else if (toupper(sclockwise) == "FALSE"){ clockwise = false; }
			
			if (ok) {ok = poly.initialize(spoly);}
			if (ok) {ok = isNumber(spx0) && isNumber(spy0) && 
			              isNumber(sang) && isNumber(sradius);}
			if (ok) {
				px0 = atof(spx0.c_str());
				py0 = atof(spy0.c_str());
				ang = angle360(atof(sang.c_str()));
				radius = atof(sradius.c_str());
			}
			
			// Passing request
			if (ok) {
				slOutput = generateLawnmower(poly, px0, py0, ang, radius, clockwise);
				slOutput.set_label(poly.get_label());
				
				std::string sOutput = slOutput.get_spec();
				
				m_Comms.Notify("VIEW_POLYGON" , poly.get_spec()); 
				m_Comms.Notify("VIEW_SEGLIST", sOutput);
				m_Comms.Notify("LAWNMOWER_SEGLIST", sOutput);
			}
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

