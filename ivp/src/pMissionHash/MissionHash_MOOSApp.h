/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MissionHash_MOOSApp.                                 */
/*    DATE: July 15th, 2023                                      */
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

#ifndef MISSION_HASH_MOOSAPP_HEADER
#define MISSION_HASH_MOOSAPP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class MissionHash_MOOSApp : public AppCastingMOOSApp
{
public:
  MissionHash_MOOSApp();
  ~MissionHash_MOOSApp() {};

protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

protected:
  bool setMissionHash();
  void registerVariables();

private: // Configuration variables

  std::string m_mission_hash_var;  // MISSION_HASH
  std::string m_mhash_short_var;   // MHASH
  
private: // State variables

  double      m_last_mhash_post;

  std::string m_mission_hash;
  std::string m_mhash_short;



};

#endif 

