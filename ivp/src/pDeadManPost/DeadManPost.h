/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DeadManPost.h                                        */
/*    DATE: June 14th, 2014                                      */
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

#ifndef P_DEAD_MAN_POST_HEADER
#define P_DEAD_MAN_POST_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "VarDataPair.h"

class DeadManPost : public AppCastingMOOSApp
{
 public:
  DeadManPost();
  ~DeadManPost() {}
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected:
  void registerVariables();
  bool addPostingIfComplete();
  bool possiblyMakePostings();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

  
 private: // Configuration variables
  
  std::string   m_heart_var;
  std::string   m_post_policy;
  double        m_max_noheart;
  bool          m_active_at_start;
  
  std::vector<VarDataPair> m_deadflags;

 private: // State variables
  double        m_last_heartbeat;
  unsigned int  m_total_heartbeats;
  double        m_delta_time;
  unsigned int  m_total_postings;
  bool          m_postings_done;
};

#endif 





