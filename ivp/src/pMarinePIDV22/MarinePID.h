/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MarinePID.h                                          */
/*    DATE: April 10 2006                                        */
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

#ifndef MARINE_PID_HEADER
#define MARINE_PID_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "PIDEngine.h"

class MarinePID : public AppCastingMOOSApp
{
public:
  MarinePID();
  virtual ~MarinePID() {}
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void registerVariables();
  bool buildReport();

  void postCharStatus();
  void postPenginePostings();
  void postPengineResults();

protected:
  PIDEngine m_pengine;
  bool      m_pid_ignore_nav_yaw;
  bool      m_pid_allstop_posted;
  bool      m_pid_verbose;
  double    m_pid_ok_skew;

  bool      m_enable_thrust_cap;
  double    m_thrust_cap;
};
#endif 






