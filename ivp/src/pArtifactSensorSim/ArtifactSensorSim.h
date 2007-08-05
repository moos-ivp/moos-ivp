/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ArtifactSensorSim.h                             */
/*    DATE: Dec 29th 1963                                   */
/************************************************************/

#ifndef ArtifactSensorSim_HEADER
#define ArtifactSensorSim_HEADER

#include "MOOSLib.h"

class ArtifactSensorSim : public CMOOSApp
{
public:
	ArtifactSensorSim();
	virtual ~ArtifactSensorSim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
};

#endif 
