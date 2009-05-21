/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.h                                    */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#ifndef TIMER_SCRIPT_MOOS_APP_HEADER
#define TIMER_SCRIPT_MOOS_APP_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"
#include "VarDataPair.h"

class TS_MOOSApp : public CMOOSApp
{
 public:
  TS_MOOSApp();
  virtual ~TS_MOOSApp();

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  
  std::vector<VarDataPair> m_pairs;
  

  // insert local vars here
};

#endif 
