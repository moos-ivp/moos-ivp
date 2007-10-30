/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: ArtifactMapper.h                                */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#ifndef ArtifactMapper_HEADER
#define ArtifactMapper_HEADER

#include "MOOSLib.h"
#include "XYArtifactGrid.h"

/// A class for implementing an XYArtifactGrid
class ArtifactMapper : public CMOOSApp
{
public:
	ArtifactMapper();
	virtual ~ArtifactMapper();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	
	void processDelta(std::string);
	void processRefresh();
	void processDetectedArtifact(std::string);

protected:
	// insert local vars here
	XYArtifactGrid ArtGrid;
	std::string sLocalCommunity;
};

#endif 
