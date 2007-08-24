/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT                                             */
/*    FILE: ArtifactMapperMain.cpp                                    */
/*    DATE: Aug 15 2007                                     */
/************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ArtifactMapper.h"

int main(int argc, char *argv[])
{
  char *sMissionFile = "ArtifactMapper.moos";
	
  if(argc > 1)
    sMissionFile = argv[1];
	
  ArtifactMapper ArtifactMapper;
	
  ArtifactMapper.Run("pArtifactMapper", sMissionFile);

  return(0);
}

