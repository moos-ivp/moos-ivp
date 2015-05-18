/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SafetyRetreat.h                                      */
/*    DATE: August 2010                                          */
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

#ifndef SAFETY_RETREAT_HEADER
#define SAFETY_RETREAT_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "XYPolygon.h"

class SafetyRetreat : public CMOOSApp
{
 public:
  SafetyRetreat();
  virtual ~SafetyRetreat() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected: 
  bool handleNewPolygon(std::string);
  bool handleContactDetection(std::string);
  void handleContactsResolved();


  unsigned int closestPolygon();

 protected:
  // A set of polygons where the vehicle may retreat to. Which 
  // polygon chosen is based on current vehicle position.
  std::vector<XYPolygon> m_retreat_areas;

  // A MOOS variable that will trigger the retreat alert msg.
  std::string   m_retreat_cue;

  // A MOOS variable containing the retreat alert message.
  std::string   m_retreat_message;

  // A MOOS variable containing the retreat notification.
  std::string   m_retreat_notify;

  // Number of seconds recommended for the retreat
  double        m_retreat_duration;
  double        m_mark_time;
  
  // True if more terminal I/O is desired for debugging.
  bool          m_verbose;

  double        m_osx;
  double        m_osy;

  unsigned int  m_iteration;

  std::vector<std::string> m_unresolved_contacts;
};

#endif 




