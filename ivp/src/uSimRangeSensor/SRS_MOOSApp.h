/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT Dept. of Mechanical Engineering             */
/*    FILE: SRS_MOOSApp.h                                   */
/*    DATE: Feb 1st, 2011                                   */
/************************************************************/

#ifndef SIM_RANGE_SENSOR_HEADER
#define SIM_RANGE_SENSOR_HEADER

#include "MOOSLib.h"
#include "SRS_Model.h"
#include "VarDataPair.h"

class SRS_MOOSApp : public CMOOSApp
{
 public:
  SRS_MOOSApp() {};
  virtual ~SRS_MOOSApp() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  void postMessages(std::vector<VarDataPair>);

 protected:
  SRS_Model  m_model;
};

#endif 
