/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USC_MOOSApp.h                                        */
/*    DATE: Jan 4th, 2011                                        */
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

#ifndef USC_MOOSAPP_HEADER
#define USC_MOOSAPP_HEADER

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "CurrentField.h"


class USC_MOOSApp : public CMOOSApp  
{
public:
  USC_MOOSApp();
  virtual ~USC_MOOSApp() {}

  bool Iterate();
  bool OnConnectToServer();
  bool OnDisconnectFromServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  bool setCurrentField(std::string);
  void postCurrentField();
  void registerVariables();


 protected: // Configuration variables
  std::string  m_post_var;

 protected: // State variables
  CurrentField m_current_field;
  bool         m_pending_cfield;
  bool         m_cfield_active;
  double       m_posx;
  double       m_posy;
  bool         m_posx_init;
  bool         m_posy_init;

  std::string  m_prev_posting;
};
#endif







