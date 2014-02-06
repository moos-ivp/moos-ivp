/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MOOSAppWrapper.h                                     */
/*    DATE: June 14th 2012                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef MOOS_APP_WRAPPER_HEADER
#define MOOS_APP_WRAPPER_HEADER

#include <vector>
#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "PseudoMOOSApp.h"

class MOOSAppWrapper : public CMOOSApp
{
public:
  MOOSAppWrapper() {m_pseudo_moos_app=0;};
  virtual ~MOOSAppWrapper() {};

  bool  Iterate();
  bool  OnNewMail(MOOSMSG_LIST&);
  bool  OnStartUp();
  bool  OnConnectToServer();

  void  SetMOOSApp(PseudoMOOSApp* app);

 protected: // Callbacks
  void  GetConfiguration(std::list<std::string>&);

  void  NotifyStr(const std::string&, const std::string&, double);
  void  NotifyStrAux(const std::string&, const std::string&, const std::string&, double);
  void  NotifyDbl(const std::string&, double, double);
  void  NotifyDblAux(const std::string&, double, const std::string&, double);
  void  NotifyPtr(const std::string&, void*, unsigned int, double);
  void  NotifyPtrAux(const std::string&, void*, unsigned int, const std::string&, double);

 protected:
  AppMsg  MOOSMsg2AppMsg(const CMOOSMsg&);

 private:
  PseudoMOOSApp *m_pseudo_moos_app;
};

#endif

