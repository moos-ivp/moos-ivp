/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: ArtifactMapper.cpp                              */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#include <iterator>
#include <sstream>
#include "ArtifactMapper.h"
#include "MBUtils.h"
// For debugging
#include <iostream>

//---------------------------------------------------------
// Constructor

ArtifactMapper::ArtifactMapper()
{
}

//---------------------------------------------------------
// Destructor

ArtifactMapper::~ArtifactMapper()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ArtifactMapper::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::reverse_iterator p;
	
	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
		CMOOSMsg &msg = *p;
		if (MOOSStrCmp(msg.GetKey(), "DETECTED_ARTIFACT")){
			// MOOSTrace("Receiving DETECTED_ARTIFACT Message...\n"); //Debug Line
			processDetectedArtifact(msg.GetString());
		}
		else if (MOOSStrCmp(msg.GetKey(), "ARTIFACTMAP_REFRESH") && MOOSStrCmp(msg.GetString(), "TRUE")) {
			processRefresh();
		}
		else if (MOOSStrCmp(msg.GetKey(), "GRID_DELTA")){
			MOOSTrace("Received grid update:\n");
			MOOSTrace("Community: %s\n", msg.GetCommunity().c_str());
			msg.Trace();
			processDelta(msg.GetString());
		};
		
	};
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ArtifactMapper::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Notify("GRID_CONFIG", ArtGrid.getConfigString());
	m_Comms.Register("DETECTED_ARTIFACT", 0);
	m_Comms.Register("ARTIFACTMAP_REFRESH", 0);
	m_Comms.Register("GRID_DELTA", 0);
	
	return true;
	}

//---------------------------------------------------------
// Procedure: Iterate()

bool ArtifactMapper::Iterate()
{
	// happens AppTick times per second
	
	// Publish the value and util list for grins and giggles.
	for( int i = 0; i < ArtGrid.size(); i++ ) {
		MOOSTrace("[%.2f, %.2f]", ArtGrid.getVal(i), ArtGrid.getUtil(i));
	};
	MOOSTrace("\n");
	
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ArtifactMapper::OnStartUp()
{
	MOOSTrace("Hello!!\n");
	
	// Read in the configuration blocks, initialize grid.
	std::string sGridPoly, sGridSize, sGridInit, sConfig;
	
	m_MissionReader.GetConfigurationParam("GRIDPOLY", sGridPoly);
	m_MissionReader.GetConfigurationParam("GRIDSIZE", sGridSize);
	m_MissionReader.GetConfigurationParam("GRIDINIT", sGridInit);
	
	m_MissionReader.GetValue("Community", sLocalCommunity);
	//MOOSTrace("Local community read as: %s \n", sLocalCommunity.c_str());
	//std::cout << "Local community read as: " << sLocalCommunity;
	
	sConfig = sGridPoly + "@" + sGridSize + "," + sGridSize + "@" + sGridInit;
	
	bool ok = ArtGrid.initialize(sConfig);
	if(!ok){
		MOOSDebugWrite("Failed to initize ArtifactMapper search grid, check config file");
		return false;};
	
	ArtGrid.setUtilRange(0.0, 1.0); // Hack to get probs displayed in pMarineViewer
	
	m_Comms.Notify("GRID_CONFIG", ArtGrid.getConfigString());
	
	// Test the capabilities of the ArtifactField for debugging
// 	std::cout << ArtGrid.getMinVal() << std::endl;
// 	std::cout << ArtGrid.getMaxVal() << std::endl;
// 	std::cout << ArtGrid.size() << std::endl;
// 	std::cout << ArtGrid.getMinUtil() << std::endl;
// 	std::cout << ArtGrid.getMaxUtil() << std::endl;
// 	std::cout << ArtGrid.getMinUtilPoss() << std::endl;
// 	std::cout << ArtGrid.getMaxUtilPoss() << std::endl;

	return true;
}

//---------------------------------------------------------
// Procedure: processDelta

void ArtifactMapper::processDelta(std::string sDelta)
{
	// Hand the update off to the artifact grid
	// NOTE:  The grid label in the passing string must match the local version
	// otherwise, the delta won't be incorporated.
	ArtGrid.processDelta(sDelta);
}

//---------------------------------------------------------
// Procedure: processRefresh

void ArtifactMapper::processRefresh()
{
	// Publish entire grid to DB
	MOOSTrace("Refreshing ArtifactMap\n"); // Debug line
	for(int i=0; i< ArtGrid.size(); i++){
		std::ostringstream osGridDelta; // Label@index,oldval,newval,oldutil,newutil
		double val = ArtGrid.getVal(i);
		double util = ArtGrid.getUtil(i);
		osGridDelta << ArtGrid.getLabel() << "@" << i << "," << val << "," << val; // Value update
		osGridDelta << "," << util << "," << util; // Utility update
		MOOSTrace("Refreshed element %i\n", i); // Debug line
		m_Comms.Notify("GRID_DELTA_LOCAL", osGridDelta.str());
	};
	
	m_Comms.Notify("ARTIFACTMAP_REFRESH", "FALSE");
}

//---------------------------------------------------------
// Procedure: processDetectedArtifact

void ArtifactMapper::processDetectedArtifact(std::string sArtifact)
{
	// Update local grid with data
	// Publish updates to DB
	std::string sx, sy, sp;
	double x, y, prob;
	if(tokParse(sArtifact, "X", ',', '=', sx) && 
		tokParse(sArtifact, "Y", ',', '=', sy) &&
		tokParse(sArtifact, "Prob", ',', '=', sp)){
			x = atof(sx.c_str());
			y = atof(sy.c_str());
			prob = atof(sp.c_str());
			for(int i=0; i< ArtGrid.size(); i++){
				if(ArtGrid.getElement(i).containsPoint(x, y)){
					// Assemble grid update string, old and new util
					if (ArtGrid.getVal(i) != prob || ArtGrid.getUtil(i) != prob){
						std::ostringstream osGridDelta; // Label@index,oldval,newval,oldutil,newutil
						osGridDelta << ArtGrid.getLabel() << "@" << i << "," << ArtGrid.getVal(i) << "," << prob; // Value update
						osGridDelta << "," << ArtGrid.getUtil(i) << "," << prob; // Utility update
						m_Comms.Notify("GRID_DELTA_LOCAL", osGridDelta.str());
						
						// Set internal values
						ArtGrid.setVal(i, prob);
						ArtGrid.setUtil(i, prob);
					};
				};
			};
	};
}
