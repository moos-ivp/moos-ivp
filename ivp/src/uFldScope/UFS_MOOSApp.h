/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UFS_MOOSApp.h                                        */
/*    DATE: Nov 23rd 2011                                        */
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

#ifndef UFS_MOOS_APP_HEADER
#define UFS_MOOS_APP_HEADER

#include <set>
#include <vector>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "UFS_Config.h"
#include "VarDataPair.h"

class UFS_MOOSApp : public AppCastingMOOSApp
{
 public:
  UFS_MOOSApp();
  virtual ~UFS_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  void registerVariables();
  bool addScope(std::string);
  bool addLayout(std::string);

  bool addPosting(std::string moosvar, std::string msgval);
  std::string getPosting(std::string moosvar, std::string keyval);

  void makeReportRaw();

  bool configInLayout(const UFS_Config&) const;

  void outputRawReport() const;

 protected: // Config Variables
  // A representation of all the info the user desires to show
  std::vector<UFS_Config>            m_config;

  // A map from MOOS variable to the key indicating the vname
  // For example: NODE_REPORT -> henry
  std::map<std::string, std::string> m_map_varkeys;

  std::vector<std::vector<std::string> > m_layouts;

 protected: // State Variables
  std::vector<std::string> m_postings_moosvar;  // e.g. "NODE_REPORT"
  std::vector<std::string> m_postings_keyval;   // e.g. "henry"
  std::vector<std::string> m_postings_msgval;   // e.g. "X=2,y=44,TIME..."

  // A 2D report structure - one vehicle per row, one fld per column
  std::vector<std::vector<std::string> > m_raw_report;
  
  unsigned int m_total_reports;
  
  // Mode States
  bool         m_layout_applied;
  unsigned int m_layout_index;

};
#endif 









