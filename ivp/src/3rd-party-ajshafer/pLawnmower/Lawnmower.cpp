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
			//MOOSTrace("Receiving LAWNMOWER_POLYGON Message...\n"); //Debug Line
			//MOOSTrace("Contents are: %s\n", msg.GetString().c_str());
			
			// Parse out the options and pass on the request
			// A LAWNMOWER_POLYGON string looks like this, arguments in any order:
			// poly=polystring#x=blah#y=blah#ang=blah#radius=blah[#clockwise=truefalse][#snap=val][#full=truefalse];
			// x, y, clockwise are not necessary when using full output
			// x, y = initial position
			// ang = [0,360], N = 0, clockwise positive
			// radius = 1/2 distance between lines
			// clockwise is first turn, defaults to true;
			// snap is snap value, defaults to no snapping;
			
			bool ok = true;
			std::string sMsg = msg.GetString();
			
			XYSegList slOutput;
			
			XYPolygon poly;
			double px0, py0, ang, radius, snap;
			bool clockwise, full;
			std::string spoly, sclockwise, sfull;
			
			ok = tokParse(sMsg, "poly", '#', '=', spoly) && ok;
			if (ok) {ok = poly.initialize(spoly);}
			
			tokParse(sMsg, "full", '#', '=', sfull);
			if (toupper(sfull) == "TRUE"){ full = true; }
			else if (toupper(sfull) == "FALSE"){ full = false; }
			else { full = false; }
			
			
			ok = tokParse(sMsg, "ang", '#', '=', ang) && ok;
			ok = tokParse(sMsg, "radius", '#', '=', radius) && ok;
			if (!tokParse(sMsg, "snap", '#', '=', snap)) { snap = 0; }
			
			// We now have the full output ready
			if (ok) {
				if (full) {slOutput = generateLawnmowerFull(poly, ang, radius); }
				else {
					ok = tokParse(sMsg, "x", '#', '=', px0) && ok;
					ok = tokParse(sMsg, "y", '#', '=', py0) && ok;
					tokParse(sMsg, "clockwise", '#', '=', sclockwise);
					if (toupper(sclockwise) == "TRUE"){ clockwise = true; }
					else if (toupper(sclockwise) == "FALSE"){ clockwise = false; }
					else { clockwise = true; }			
					
					slOutput = generateLawnmower(poly, px0, py0, ang, radius, clockwise);
				}
				
				slOutput.set_label(poly.get_label());
				slOutput.apply_snap(snap);
				
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

