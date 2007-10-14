/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IMS_MOOSApp.cpp                                      */
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

#include <iostream>
#include "IMS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------------------
// Constructor

IMS_MOOSApp::IMS_MOOSApp() 
{
  m_sim_prefix = "MARINESIM";
  m_model      = 0;
}

//------------------------------------------------------------------------
// Procedure: OnNewMail
//      Note: 

bool IMS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &Msg = *p;
 
    double dfTimeDiff = MOOSTime() - Msg.m_dfTime;
    if(dfTimeDiff > 1.0) {
      MOOSTrace("Mail too old [%s] %f\n",Msg.m_sKey.c_str(),dfTimeDiff);
      continue;
    }

    string reset_str = "SIM_RESET_" + toupper(m_vehicle_id);

    if(m_model) {
      if(Msg.m_sKey == "DESIRED_THRUST")
	m_model->setThrust(Msg.m_dfVal);
      else if(Msg.m_sKey == "DESIRED_RUDDER")
	m_model->setRudder(Msg.m_dfVal);
      else if(Msg.m_sKey == "DESIRED_ELEVATOR")
	m_model->setElevator(Msg.m_dfVal);
      else if(Msg.m_sKey == "SIM_PAUSED")
	m_model->setPaused(toupper(Msg.m_sVal) == "TRUE");
      else if(Msg.m_sKey == "SIM_THETA_FORCE")
	m_model->setPushTheta(Msg.m_dfVal);
      else if(Msg.m_sKey == "SIM_X_FORCE")
	m_model->setPushX(Msg.m_dfVal);
      else if(Msg.m_sKey == "SIM_Y_FORCE")
	m_model->setPushY(Msg.m_dfVal);
      else if(Msg.m_sKey == reset_str) {
	string str = stripBlankEnds(Msg.m_sVal);
	vector<string> svector = parseString(str, ',');
	if(svector.size() == 4) {
	  string x = stripBlankEnds(svector[0]);
	  m_model->setPositionX(atof(x.c_str()));
	  string y = stripBlankEnds(svector[1]);
	  m_model->setPositionY(atof(y.c_str()));
	  string spd = stripBlankEnds(svector[2]);
	  m_model->setSpeed(atof(spd.c_str()));
	  string hdg = stripBlankEnds(svector[3]);
	  m_model->setHeadingDEG(atof(hdg.c_str()));
	}
      }
      else
      MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
    }
  }

  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnStartUp
//      Note: 

bool IMS_MOOSApp::OnStartUp()
{
  if(!m_model) {
    MOOSTrace("Sim not started due to NULL IMS_Model \n");
    return(false);
  }
  else
    MOOSTrace("sim starting\n");
  
  // Set the vehicle_id to the community name - possibly overide later.
  m_MissionReader.GetValue("Community", m_vehicle_id);
  
  m_model->reset(MOOSTime());

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = stripBlankEnds(MOOSChomp(sLine, "="));
    string sVal      = stripBlankEnds(sLine);
    double dVal      = atof(sVal.c_str());

    // Backward support of older param style
    if(sVarName=="MaxTransVel")  sVarName="MAX_TRANS_VEL";
    if(sVarName=="MaxRotVel")    sVarName="MAX_ROTAT_VEL";
    if(sVarName=="StartLon")     sVarName="START_X";
    if(sVarName=="StartLat")     sVarName="START_Y";
    if(sVarName=="StartHeading") sVarName="START_HEADING";
    if(sVarName=="StartDepth")   sVarName="START_DEPTH";
    if(sVarName=="StartSpeed")   sVarName="START_SPEED";

    sVarName = toupper(sVarName);

    if(sVarName == "MAX_TRANS_VEL")
      m_model->setMaxTransVel(dVal);
    else if(sVarName == "MAX_ROTAT_VEL")
      m_model->setMaxRotatVel(dVal);
    else if(sVarName == "START_X")
      m_model->setPositionX(dVal);
    else if(sVarName == "START_Y")
      m_model->setPositionY(dVal);
    else if(sVarName == "START_HEADING")
      m_model->setHeadingDEG(dVal);
    else if(sVarName == "START_SPEED")
      m_model->setSpeed(dVal);
    else if(sVarName == "START_DEPTH")
      m_model->setDepth(dVal);
    else if(sVarName == "FLOAT_RATE")
      m_model->setFloatRate(dVal);
    else if(sVarName == "PREFIX")
      m_sim_prefix = sVal;
    else if(sVarName == "FORCE_THETA")
      m_model->setPushTheta(dVal);
    else if(sVarName == "FORCE_X")
      m_model->setPushX(dVal);
    else if(sVarName == "FORCE_Y")
      m_model->setPushY(dVal);
    else if(sVarName == "VEHICLE_ID")
      m_vehicle_id = sVal;

    else if(sVarName == "SIM_PAUSE")
      m_model->setPaused(toupper(sVal) == "TRUE");
  }

  if(m_vehicle_id == "") {
    MOOSTrace("SIM Vehicle ID not set - via community or explicitly.\n");
    return(false);
  }

  registerVariables();
  MOOSTrace("Sim started \n");
  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

bool IMS_MOOSApp::OnConnectToServer()
{
  registerVariables();
  MOOSTrace("Sim connected\n");
  
  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

void IMS_MOOSApp::registerVariables()
{
  m_Comms.Register("DESIRED_RUDDER", 0);
  m_Comms.Register("DESIRED_THRUST", 0);
  m_Comms.Register("DESIRED_ELEVATOR", 0);
  m_Comms.Register("SIM_X_FORCE", 0);
  m_Comms.Register("SIM_Y_FORCE", 0);
  m_Comms.Register("SIM_PAUSE", 0);

  if(m_vehicle_id != "")
    m_Comms.Register("SIM_RESET_" + toupper(m_vehicle_id), 0);
}

//------------------------------------------------------------------------
// Procedure: OnDisconnectFromServer
//      Note: 

bool IMS_MOOSApp::OnDisconnectFromServer()
{
  m_Comms.Notify(m_sim_prefix+"_STATE", "off",MOOSTime());
  return(true);
}

//------------------------------------------------------------------------
// Procedure: Iterate
//      Note: This is where it all happens.

bool IMS_MOOSApp::Iterate()
{
  if(!m_model)
    return(false);

  double ctime = MOOSTime();

  m_model->propagate(ctime);

  m_Comms.Notify(m_sim_prefix+"_X", m_model->getPositionX(), ctime);
  m_Comms.Notify(m_sim_prefix+"_Y", m_model->getPositionY(), ctime);
  m_Comms.Notify(m_sim_prefix+"_HEADING", m_model->getHeading(), ctime);
  m_Comms.Notify(m_sim_prefix+"_SPEED", m_model->getSpeed(), ctime);
  m_Comms.Notify(m_sim_prefix+"_DEPTH", m_model->getDepth(), ctime);
  m_Comms.Notify(m_sim_prefix+"_YAW", m_model->getYaw(), ctime);
  m_Comms.Notify(m_sim_prefix+"_STATE", "off",ctime);
  return(true);
}


//------------------------------------------------------------------------
// Procedure: handleSimReset
//
//  "new_x=20, new_y=-35, new_speed=2.2, new_heading=180, new_depth=20"


void IMS_MOOSApp::handleSimReset(const string& str)
{
  if(!m_model)
    return;
  
  bool xset = false;
  bool yset = false;
  bool spdset = false;
  bool hdgset = false;
  
  double new_x, new_y, new_speed, new_heading;
  
  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    svector[i] = tolower(stripBlankEnds(svector[i]));
    vector<string> ivector = parseString(svector[i],'=');
    int isize = ivector.size();
    if(isize == 2) {
      ivector[0] = stripBlankEnds(ivector[0]);
      ivector[1] = stripBlankEnds(ivector[1]);
      bool oknum = isNumber(ivector[1]);
      if(oknum) {
	double dval = atof(ivector[1].c_str());
	if(ivector[0] == "new_x")
	  m_model->setPositionX(dval);
	if(ivector[0] == "new_y")
	  m_model->setPositionY(dval);
	if(ivector[0] == "new_heading")
	  m_model->setHeadingDEG(dval);
	if(ivector[0] == "new_speed")
	  m_model->setSpeed(dval);
	if(ivector[0] == "new_depth")
	  m_model->setDepth(dval);
      }
    }
  }
}


