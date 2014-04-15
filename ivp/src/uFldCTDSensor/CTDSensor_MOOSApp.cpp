/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CTDSensor_MOOSApp.cpp                            */
/*    DATE: Jan 28th 2012                                        */
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
#include "CTDSensor_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"
#include <string>
#include <vector>

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

CTDSensor_MOOSApp::CTDSensor_MOOSApp(){
  true_front="";
}

bool CTDSensor_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
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

bool CTDSensor_MOOSApp::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void CTDSensor_MOOSApp::RegisterVariables()
{
  m_Comms.Register("DEPLOY_ALL", 0);
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("UCTD_SENSOR_REQUEST", 0);
  AppCastingMOOSApp::RegisterVariables();

}


//---------------------------------------------------------
// Procedure: Iterate()

bool CTDSensor_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();
  m_model.setCurrTime(MOOSTime());
  m_model.iterate();
  postMessages(m_model.getMessages());  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool CTDSensor_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  cout << termColor("blue") << "Simulated CTD Sensor starting..." << endl;
  m_model.setCurrTime(MOOSTime());
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string param  = tolower(biteStringX(sLine, '='));
    string value  = sLine;
    if((param != "apptick") && (param != "commstick"))
      m_model.setParam(param, value);
}

  RegisterVariables();

  // Configure field model and sensor

  m_model.FieldModelConfig();
  m_model.SensorConfig();
  
  true_front=m_model.postTrueParameters();
  
  double time_warp = GetMOOSTimeWarp();
  m_model.setTimeWarp(time_warp);


  //  m_model.perhapsSeedRandom();
  cout << "Simulated CTD Sensor started." << endl;
  cout << termColor() << flush;
  return(true);
}


//---------------------------------------------------------
// Procedure: postMessages()

void CTDSensor_MOOSApp::postMessages(vector<VarDataPair> msgs)
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

bool CTDSensor_MOOSApp::buildReport()
{
  m_msgs << "CTDSensor"<<endl;
  m_msgs << "------------------------"<<endl;
  m_msgs << "True Front Values:"<<endl;
  vector <string> myvec = parseString(true_front,",");
  for (unsigned int i=0;i<myvec.size();i++){
    m_msgs<<myvec[i]<<endl;
  }  
  return(true);
}

