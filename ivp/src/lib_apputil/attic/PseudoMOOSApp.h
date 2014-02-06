/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PseudoMOOSApp.h                                      */
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

#ifndef PSEUDO_MOOS_APP_HEADER
#define PSEUDO_MOOS_APP_HEADER

#include "CommsClient.h"
#include "AppMsg.h"
#include <list>

typedef std::list<AppMsg> APPMSG_LIST;

class MOOSAppWrapper;
class PseudoMOOSApp {
  friend class MOOSAppWrapper;
public:
  PseudoMOOSApp();
  virtual ~PseudoMOOSApp() {};

  virtual bool  Iterate()               {return(true);};
  virtual bool  OnNewMail(APPMSG_LIST&) {return(true);};
  virtual bool  OnStartUp()             {return(true);};
  virtual bool  OnConnectToServer()     {return(true);};

 protected:
  void  (MOOSAppWrapper::*GetConfiguration)(std::list<std::string>&);

  MOOSAppWrapper *m_owner;
  CommsClient     m_Comms;

};
#endif

