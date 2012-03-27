/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_MOOSApp.h                               */
/*    DATE: Jan 28th, 2012                                       */
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

#ifndef UFLD_GENERIC_SENSOR_MOOSAPP_HEADER
#define UFLD_GENERIC_SENSOR_MOOSAPP_HEADER

#include "MOOSLib.h"
#include "GenericSensor_Model.h"
#include "VarDataPair.h"

class GenericSensor_MOOSApp : public CMOOSApp
{
 public:
  GenericSensor_MOOSApp() {};
  virtual ~GenericSensor_MOOSApp() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

  void setVerbose(std::string s) {m_model.setParam("verbose",s);}

 protected:
  void postMessages(std::vector<VarDataPair>);

 protected:
  GenericSensor_Model  m_model;
};

#endif 

