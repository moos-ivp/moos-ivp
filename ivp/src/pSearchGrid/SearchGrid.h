/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PSG_MOOS_App.h                                       */
/*   DATE: Dec 7th 2011                                         */
/****************************************************************/

#ifndef SEARCH_GRID_MOOS_APP_HEADER
#define SEARCH_GRID_MOOS_APP_HEADER

#include "MOOSLib.h"
#include "XYConvexGrid.h"

class SearchGrid : public CMOOSApp
{
 public:
  SearchGrid() {};
  virtual ~SearchGrid() {};

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
