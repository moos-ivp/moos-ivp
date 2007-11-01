/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: SensorSim.cpp                                   */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#include <iterator>
#include <vector>
#include <sstream>
#include "SensorSim.h"
#include "MBUtils.h"
#include "FileBuffer.h"

//---------------------------------------------------------
// Constructor

SensorSim::SensorSim()
{
}

//---------------------------------------------------------
// Destructor

SensorSim::~SensorSim()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SensorSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
	// Set x, y to be the most recent versions of NAV_X,Y

	CMOOSMsg mMessage;
	if(m_Comms.PeekMail(NewMail, "NAV_X", mMessage, false, true))	x = mMessage.GetDouble();
	if(m_Comms.PeekMail(NewMail, "NAV_Y", mMessage, false, true))	y = mMessage.GetDouble();
	
//	MOOSMSG_LIST::reverse_iterator p;
	
// 	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
// 		CMOOSMsg &msg = *p;
// 	}
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SensorSim::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	
	// Setting notification frequency to be frequency of this application
	m_Comms.Register("NAV_X", m_dfFreq);
	m_Comms.Register("NAV_Y", m_dfFreq);
	
	return true;
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SensorSim::Iterate()
{
	// happens AppTick times per second
	
	// For each tick we should query the sensor to see
	// what it sees and then publish DetectedArtifact = X=10,Y=10,Prob=.85

	// Query sensor
	std::ostringstream ssInput;
	ssInput << "X=" << x << "," << "Y=" << y;
	std::string sQueryString = ssInput.str();
	
	//MOOSTrace(sQueryString);
	
	std::vector<std::string> vecSensorOutput = Sensor.querySensor(sQueryString, ArtField);
	
	MOOSTrace("Detected %i artifacts from sensor at iteration %i\n", vecSensorOutput.size(), GetIterateCount());
	
	std::vector<std::string>::const_iterator p;
	
	for (p = vecSensorOutput.begin(); p != vecSensorOutput.end(); p++){
		m_Comms.Notify("DETECTED_ARTIFACT", *p);
	};
	
	
	// Plot detectable radius around kayak
	std::ostringstream osPlotter;
	//Sensor.getSensorRadius()
	// "radial:x,y,radius,numpoints,[snapval],[label]
	
	osPlotter << "radial:" << x << "," << y << "," << "10" << "," << "12" << "," << "0.0" << "," << sLocalCommunity;
	m_Comms.Notify("VIEW_POLYGON", osPlotter.str()); 
	
	return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SensorSim::OnStartUp()
{
	// happens before connection is open
	
	// Read the artifacts into the artifact field
	// Form 1: ArtifactFile = foo.art
	// File is generated from app_artfieldgenerator
	// in the form of Form 2:
	// Form 2: Artifact = X=47,Y=-88.5
	const double dMineSize=0.5;
	
	// happens before connection is open

	CMOOSApp::OnStartUp();
  	MOOSTrace("pSensorSim starting....\n");

	// Add artifacts to artifact field, set sensor settings
  	std::list<std::string> sParams;
  	if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
  		std::list<std::string>::const_iterator p;
		for(p = sParams.begin(); p!=sParams.end(); p++) {
	    		std::string sLine  = stripBlankEnds(*p);
	    		std::string sKey   = stripBlankEnds(MOOSChomp(sLine, "="));
	    		sKey = tolower(sKey);
	    		if(MOOSStrCmp(sKey, "Artifact")) {
	    			// No error checking, just add string into artifact list
// 	    			MOOSTrace("Adding artifact %s \n", sLine.c_str());
	    			ArtField.addArtifact(sLine);
	    		}
	    		else if(MOOSStrCmp(sKey, "ArtifactFile")) {
	    			// Process file
// 	    			MOOSTrace("Trying to add from file %s \n", sLine.c_str());
	    			
	    			std::vector<std::string> ArtVector = fileBuffer(sLine.c_str());
					std::vector<std::string>::iterator p;
					
					// Format each line to strip out "Artifact = "
					for(p = ArtVector.begin(); p!=ArtVector.end(); p++) {
						std::string sJunkVar = MOOSChomp(*p, "=");
						MOOSTrimWhiteSpace(*p);
						if(!p->empty()) {ArtField.addArtifact(*p);}; // Don't want to add any empty lines
					}
	    		}
	    		else if(MOOSStrCmp(sKey, "Sensor")) {
	    			// Tell Sensor what sensor to use
	    			bool ok = Sensor.setSensorModel(sLine);
	    			if(!ok){MOOSDebugWrite("Error setting sensor type");};
	    		}
	    		else if(MOOSStrCmp(sKey, "Sensor_Radius")) {
	    			// Tell Sensor what sensor detection radius is 
	    			Sensor.setSensorRadius(atof(sLine.c_str()));
	    		}
	    	};
	};
	
	MOOSTrace("Added %i artifacts to the ArtifactField\n", ArtField.size());
	
	// Plot the mines to the viewer
	//MOOSTrace("Contents of ArtField\n"); //Debug line
	for(int i = 0; i < ArtField.size(); i++) {
		std::string sx, sy;
		
		if(tokParse(ArtField.getArtifact(i), "X", ',', '=', sx) && tokParse(ArtField.getArtifact(i), "Y", ',', '=', sy)){			
			std::ostringstream osInit;
			std::string sInitializer;
						
			osInit << sx << "," << sy << "," << dMineSize << "," << "Artifact" << i;
			sInitializer = osInit.str();

			m_Comms.Notify("VIEW_POINT", sInitializer);
			MOOSPause(100); // Wait 50 ms
			};
		
		//MOOSTrace(ArtField.getArtifact(i) + "\n"); //Debug line
	}
	//MOOSTrace("End of ArtField\n");
	
	m_MissionReader.GetValue("Community", sLocalCommunity);
	
	return true;
}
