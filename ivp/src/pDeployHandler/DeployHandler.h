/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: DeployHandler.h                                 */
/*    DATE: Sep 15th 2007                                   */
/************************************************************/

#ifndef DEPLOY_HANDLER_HEADER
#define DEPLOY_HANDLER_HEADER

#include "MOOSLib.h"

class DeployHandler : public CMOOSApp
{
 public:
  DeployHandler();
  virtual ~DeployHandler();

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  // insert local vars here
};

#endif 
