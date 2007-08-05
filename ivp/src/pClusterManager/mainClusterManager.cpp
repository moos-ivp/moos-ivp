/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: pClusterManager.cpp                                  */
/*    DATE: May 27th 2007                                        */
/*****************************************************************/

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "ClusterManager.h"

int main(int argc, char *argv[])
{
  ClusterManager cluster_manager;
  
  char *sMissionFile = "pClusterManager.moos";
  if(argc > 1)
    sMissionFile = argv[1];

  cluster_manager.Run("pClusterManager", sMissionFile);
  
  return(0);
}








