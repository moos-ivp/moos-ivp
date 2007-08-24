/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: ArtifactMapper.h                                          */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#ifndef ArtifactMapper_HEADER
#define ArtifactMapper_HEADER

#include "MOOSLib.h"
#include "XYArtifactGrid.h"

class ArtifactMapper : public CMOOSApp
{
public:
	ArtifactMapper();
	virtual ~ArtifactMapper();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here
	XYArtifactGrid ArtGrid;
};

#endif 
