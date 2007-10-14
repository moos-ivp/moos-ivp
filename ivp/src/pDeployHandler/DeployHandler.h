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
  virtual ~DeployHandler() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  bool handleNewDeployToPosition(std::string);
  bool handleNewLoiterPosition(std::string);
  bool handleNewRelativePosition(std::string);
  bool handleReclaim(std::string);

 protected:

  double  m_range_1;
  double  m_range_2;
  double  m_range_3;
  double  m_angle_1;
  double  m_angle_2;
  double  m_angle_3;

  // insert local vars here
};

#endif 
