/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PSG_MOOS_App.h                                       */
/*   DATE: Dec 7th 2011                                         */
/****************************************************************/

#ifndef PSG_MOOS_APP_HEADER
#define PSG_MOOS_APP_HEADER

#include "MOOSLib.h"
#include "XYConvexGrid.h"

class PSG_MOOSApp : public CMOOSApp
{
 public:
  PSG_MOOSApp();
  virtual ~PSG_MOOSApp() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void registerVariables();
  void handleNodeReport(std::string);

  void postGrid();

 protected:
  XYConvexGrid m_grid;
};

#endif 
