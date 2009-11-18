/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IMS_MOOSApp.h                                        */
/*    DATE: Oct 25th 2004                                        */
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

#ifndef IMS_MOOSAPP_HEADER
#define IMS_MOOSAPP_HEADER

#include <string>
#include "MOOSLib.h"
#include "VState.h"
#include "IMS_Model.h"

// tes 12-2-07 for CMOOSGeodesy class
#include "MOOSGeodesy.h"


class IMS_MOOSApp : public CMOOSApp  
{
public:
  IMS_MOOSApp();
  virtual ~IMS_MOOSApp() {};

  void setModel(IMS_Model* g_model) {m_model = g_model;};

  // virtual overide of base class CMOOSApp member. 
  // Here we do all the processing and IO 
  bool Iterate();

  // virtual overide of base class CMOOSApp member. 
  // Here we register for data we wish be informed about
  bool OnConnectToServer();
  bool OnDisconnectFromServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

 protected:
  void handleSimReset(const std::string&);
  void registerVariables();

protected:
  std::string m_sim_prefix;
  IMS_Model*  m_model;

  int         m_reset_count;

  // tes 12-2-07 added to allow simulated lat/long
  CMOOSGeodesy m_geodesy;
  bool m_geo_ok;

};
#endif


