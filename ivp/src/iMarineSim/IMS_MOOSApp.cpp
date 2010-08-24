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
  m_sim_prefix  = "IMS";
  m_model       = 0;
  m_reset_count = 0;
}

//------------------------------------------------------------------------
// Procedure: OnNewMail
//      Note: 

bool IMS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;
    string key = Msg.m_sKey;
    double dval = Msg.m_dfVal;
    string sval = Msg.m_sVal;

    if(m_model) {
      if(key == "DESIRED_THRUST")
	m_model->setThrust(dval);
      else if(key == "DESIRED_RUDDER")
	m_model->setRudder(dval);
      else if(key == "DESIRED_ELEVATOR")
	m_model->setElevator(dval);
      else if((key == "SIM_PAUSED") || (key == "IMS_SIM_PAUSED"))
	m_model->setPaused(toupper(sval) == "TRUE");
      else if(key == "IMS_DECELERATION")
	m_model->setDeceleration(dval);
      else if((key == "MARINESIM_FLOAT_RATE") || 
	      (key == "IMS_FLOAT_RATE"))
	m_model->setFloatRate(dval);
      else if((key == "MARINESIM_FORCE_THETA") || 
	      (key == "IMS_FORCE_THETA"))
	m_model->setTorqueTheta(dval);
      else if((key == "MARINESIM_FORCE_X") || (key == "IMS_FORCE_X"))
	m_model->setForceX(dval);
      else if((key == "MARINESIM_FORCE_Y") || (key == "IMS_FORCE_Y"))
	m_model->setForceY(dval);
      else if(key == "IMS_FORCE_VECTOR")
	m_model->setForceVector(sval, false);
      else if(key == "IMS_FORCE_VECTOR_ADD")
	m_model->setForceVector(sval, true);
      else if((key == "MARINESIM_RESET") || (key == "IMS_RESET")) {
	m_reset_count++;
	m_Comms.Notify("IMS_RESET_COUNT", m_reset_count);
	string str = stripBlankEnds(sval);
	vector<string> svector = parseString(str, ',');
	int vsize = svector.size();
	for(int i=0; i<vsize; i++) {
	  string pair = stripBlankEnds(svector[i]);
	  vector<string> vpair = parseString(pair, '=');
	  if(vpair.size() == 2) {
	    string left = tolower(stripBlankEnds(vpair[0]));
	    string right = stripBlankEnds(vpair[1]);
	    if(left == "start_x")
	      m_model->setPositionX(atof(right.c_str()));
	    if(left == "start_y")
	      m_model->setPositionY(atof(right.c_str()));
	    if(left == "start_speed")
	      m_model->setSpeed(atof(right.c_str()));
	    if(left == "start_heading")
	      m_model->setHeadingDEG(atof(right.c_str()));
	    if(left == "start_depth")
	      m_model->setDepth(atof(right.c_str()));
	  }
	}
      }
      else
      MOOSTrace("Unrecognized command: [%s]\n",key.c_str());
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
    if(sVarName=="StartLon")     sVarName="START_X";
    if(sVarName=="StartLat")     sVarName="START_Y";
    if(sVarName=="StartHeading") sVarName="START_HEADING";
    if(sVarName=="StartDepth")   sVarName="START_DEPTH";
    if(sVarName=="StartSpeed")   sVarName="START_SPEED";

    sVarName = toupper(sVarName);

    if(sVarName == "START_X")
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
    else if(sVarName == "DECELERATION")
      m_model->setDeceleration(dVal);
    else if(sVarName == "PREFIX")
      m_sim_prefix = sVal;
    else if(sVarName == "FORCE_THETA")
      m_model->setTorqueTheta(dVal);
    else if(sVarName == "FORCE_VECTOR")
      m_model->setForceVector(sVal);
    else if(sVarName == "FORCE_X")
      m_model->setForceX(dVal);
    else if(sVarName == "FORCE_Y")
      m_model->setForceY(dVal);
    else if(sVarName == "THRUST_FACTOR")
      m_model->setThrustFactor(dVal);
    else if(sVarName == "SIM_PAUSE")
      m_model->setPaused(toupper(sVal) == "TRUE");
    else if(sVarName == "START_POS")
      m_model->setPosition(sVal);
  }

  // tes 12-2-07
  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if(!m_MissionReader.GetValue("LatOrigin", latOrigin)) {
    MOOSTrace("iMarineSim: LatOrigin not set in *.moos file.\n");
    m_geo_ok = false;
  } 
  else if(!m_MissionReader.GetValue("LongOrigin", longOrigin)) {
    MOOSTrace("iMarineSim: LongOrigin not set in *.moos file\n");
    m_geo_ok = false;      
  }
  else {
    m_geo_ok = true;
    // initialize m_geodesy
    if(!m_geodesy.Initialise(latOrigin, longOrigin)) {
      MOOSTrace("iMarineSim: Geodesy init failed.\n");
      m_geo_ok = false;
    }
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

  // The MARINESIM_ prefix is deprecated
  m_Comms.Register("MARINESIM_FLOAT_RATE", 0);
  m_Comms.Register("MARINESIM_FORCE_X", 0);
  m_Comms.Register("MARINESIM_FORCE_Y", 0);
  m_Comms.Register("MARINESIM_FORCE_VECTOR", 0);
  m_Comms.Register("MARINESIM_FORCE_VECTOR_ADD", 0);
  m_Comms.Register("MARINESIM_FORCE_THETA", 0);
  m_Comms.Register("MARINESIM_PAUSE", 0);
  m_Comms.Register("MARINESIM_RESET", 0);

  // The IMS_ prefix is the desired (newer) prefix supported
  m_Comms.Register("IMS_FLOAT_RATE", 0);
  m_Comms.Register("IMS_FORCE_X", 0);
  m_Comms.Register("IMS_FORCE_Y", 0);
  m_Comms.Register("IMS_FORCE_VECTOR", 0);
  m_Comms.Register("IMS_FORCE_VECTOR_ADD", 0);
  m_Comms.Register("IMS_FORCE_THETA", 0);
  m_Comms.Register("IMS_PAUSE", 0);
  m_Comms.Register("IMS_RESET", 0);

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

  double nav_x = m_model->getPositionX();
  double nav_y = m_model->getPositionY();

  m_Comms.Notify(m_sim_prefix+"_X", nav_x, ctime);
  m_Comms.Notify(m_sim_prefix+"_Y", nav_y, ctime);

  // tes 12-2-07 try to give a simulated lat / long
  if(m_geo_ok) {
    double lat, lon;
    //    m_geodesy.LocalGrid2LatLong(nav_x, nav_y, lat, lon);
    m_geodesy.UTM2LatLong(nav_x, nav_y, lat, lon);
    m_Comms.Notify(m_sim_prefix+"_LAT", lat, ctime);
    m_Comms.Notify(m_sim_prefix+"_LONG", lon, ctime);
  }

  double new_speed = m_model->getSpeed();
  new_speed = snapToStep(new_speed, 0.01);

  m_Comms.Notify(m_sim_prefix+"_HEADING", m_model->getHeading(), ctime);
  m_Comms.Notify(m_sim_prefix+"_SPEED", new_speed, ctime);
  m_Comms.Notify(m_sim_prefix+"_DEPTH", m_model->getDepth(), ctime);
  m_Comms.Notify(m_sim_prefix+"_YAW", m_model->getYaw(), ctime);
  m_Comms.Notify(m_sim_prefix+"_STATE", "off",ctime);

  string val = "ang=";
  val += dstringCompact(doubleToString(m_model->getForceAngle()));
  val += ", mag=";
  val += dstringCompact(doubleToString(m_model->getForceMagnitude()));
  val += ", xmag=";
  val += dstringCompact(doubleToString(m_model->getForceX()));
  val += ", ymag=";
  val += dstringCompact(doubleToString(m_model->getForceY()));
  m_Comms.Notify("IMS_FSUMMARY", val);

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



