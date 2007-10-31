/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: SensorSim.h                                          */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#ifndef SensorSim_HEADER
#define SensorSim_HEADER

#include "MOOSLib.h"
#include "ArtifactField.h"
#include "SensorModel.h"

class SensorSim : public CMOOSApp
{
public:
	SensorSim();
	virtual ~SensorSim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	ArtifactField ArtField;
	SensorModel   Sensor;
	double x, y; // Used for storing current X, Y
	std::string sLocalCommunity; // Local community
};

#endif 
