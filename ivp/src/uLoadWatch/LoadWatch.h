/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LoadWatch.h                                          */
/*    DATE: Dec 24th, 2013                                       */
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

#ifndef U_LOAD_WATCH_HEADER
#define U_LOAD_WATCH_HEADER

#include <string>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class LoadWatch : public AppCastingMOOSApp
{
 public:
  LoadWatch();
  ~LoadWatch() {}
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
   
 protected:
  void registerVariables();
  
  void handleMailIterGap(std::string app, std::string mvar, double);
  void handleMailIterLen(std::string, double);
  bool handleConfigThresh(std::string);
  bool handleConfigNearThresh(std::string);

  void updateBreachSet(std::string app);
  void updateNearBreachSet(std::string app);
  
 private: // Configuration variables
  std::map<std::string, double> m_map_thresh;
  unsigned int                  m_breach_trigger;
   
 private: // State variables
  std::map<std::string, double>       m_map_app_gap_total;
  std::map<std::string, double>       m_map_app_gap_max;
  std::map<std::string, unsigned int> m_map_app_gap_count;
  std::map<std::string, unsigned int> m_map_app_gap_xcount;
  
  std::map<std::string, double>       m_map_app_len_total;
  std::map<std::string, double>       m_map_app_len_max;
  std::map<std::string, unsigned int> m_map_app_len_count;

  unsigned int          m_breach_count;  // For any app
  bool                  m_breach;        // For any app
  std::set<std::string> m_breach_set;

  unsigned int          m_near_breach_count;  // For any app
  bool                  m_near_breach;        // For any app
  std::set<std::string> m_near_breach_set;
  double                m_near_breach_thresh;  
};

#endif 

