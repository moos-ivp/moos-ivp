/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_MOOSApp.cpp                             */
/*    DATE: Jan 28th 2012                                         */
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

#include <iterator>
#include "HazardSensor_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardSensor_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_model.setCurrTime(MOOSTime());
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    m_model.handleMsg(msg.GetKey(),msg.GetDouble(), msg.GetString(),
		      msg.GetTime(), msg.IsDouble(), msg.IsString(),
		      msg.GetSource(), msg.GetCommunity());
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardSensor_MOOSApp::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void HazardSensor_MOOSApp::RegisterVariables()
{
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("UHZ_SENSOR_REQUEST", 0);
  m_Comms.Register("UHZ_SENSOR_CLEAR", 0);
  m_Comms.Register("UHZ_CLASSIFY_REQUEST", 0);
  m_Comms.Register("UHZ_CONFIG_REQUEST", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool HazardSensor_MOOSApp::Iterate()
{
  m_model.setCurrTime(MOOSTime());
  m_model.iterate();
  postMessages(m_model.getMessages());
  postMessages(m_model.getQueueMessages());
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool HazardSensor_MOOSApp::OnStartUp()
{
  cout << termColor("blue");
  MOOSTrace("\nSimulated Hazard Sensor starting...\n");
  m_model.setCurrTime(MOOSTime());
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::iterator p;
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string param  = biteStringX(sLine, '=');
    string value  = sLine;
    bool ok = false;
    if((tolower(param) == "apptick") ||
       (tolower(param) == "commstick"))
      ok = true;
    else {
      unsigned int pass, total_passes = 3;
      for(pass=0; pass<total_passes; pass++)
	ok = ok || m_model.setParam(param, value, pass);
      if(!ok) {
	cout << termColor("red");
	cout << "WARNING: Unhandled configuration line:" << endl;
	cout << termColor("black");
	cout << "   Parameter=" << param << " Value=" << value << endl;
	cout << termColor("blue");
      }
    }
  }
  
  RegisterVariables();

  postMessages(m_model.getVisuals());
  
  double time_warp = GetMOOSTimeWarp();
  m_model.setTimeWarp(time_warp);

  m_model.perhapsSeedRandom();
  m_model.sortSensorProperties();
  MOOSTrace("Simulated Hazard Sensor started. \n\n");
  cout << termColor() << flush;
  return(true);
}


//---------------------------------------------------------
// Procedure: postMessages()

void HazardSensor_MOOSApp::postMessages(vector<VarDataPair> msgs)
{
  unsigned int i, vsize = msgs.size();
  for(i=0; i<vsize; i++) {
    string varname = msgs[i].get_var();
    if(msgs[i].is_string())
      m_Comms.Notify(varname, msgs[i].get_sdata());
    else
      m_Comms.Notify(varname, msgs[i].get_ddata());
  }
}

