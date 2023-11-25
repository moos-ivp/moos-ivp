/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PSG_MOOS_App.h                                       */
/*    DATE: Dec 7th 2011                                         */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef SEARCH_GRID_MOOS_APP_HEADER
#define SEARCH_GRID_MOOS_APP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYConvexGrid.h"
#include "ExFilterSet.h"

class SearchGrid : public AppCastingMOOSApp
{
 public:
  SearchGrid();
  virtual ~SearchGrid() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  bool buildReport();
  void registerVariables();
  void handleMailNodeReport(std::string);

  void postGrid();
  void postGridUpdates();
  
protected: // Config vars
  bool        m_report_deltas;
  std::string m_grid_label;
  std::string m_grid_var_name;

  ExFilterSet m_filter_set;

protected: // State vars
  
  XYConvexGrid m_grid;

  std::map<unsigned int, double> m_map_deltas;

};

#endif 

