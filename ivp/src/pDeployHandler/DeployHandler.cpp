/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: DeployHandler.cpp                               */
/*    DATE: Sep 15th 2007                                   */
/************************************************************/

#include <iostream>
#include <iterator>
#include "DeployHandler.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DeployHandler::DeployHandler()
{
  m_angle_1 = 200;
  m_angle_2 = 160;
  m_angle_3 = 160;

  m_range_1 = 100;
  m_range_2 = 100;
  m_range_3 = 100;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool DeployHandler::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.GetKey();
    //double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //bool   mstr  = msg.IsString();
    //string msrc  = msg.GetSource();

    bool ok = true;
    key = toupper(key);
    if(key == "DEPLOY_TO_PT")
      ok = handleNewDeployToPosition(sval);
    else if(key == "DEPLOY_TO_LOITER")
      ok = handleNewLoiterPosition(sval);
    else if(key == "DEPLOY_TO_RELPOS")
      ok = handleNewRelativePosition(sval);
    else if(key == "RECLAIM")
      ok = handleReclaim(sval);
    else
      ok = false;

    if(!ok)
      cout << "Unrecognized mail: " << key << endl;
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DeployHandler::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
	
  RegisterVariables();  

  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void DeployHandler::RegisterVariables()
{
  m_Comms.Register("DEPLOY_TO_PT", 0);
  m_Comms.Register("DEPLOY_TO_LOITER", 0);
  m_Comms.Register("DEPLOY_TO_RELPOS", 0);
  m_Comms.Register("RECLAIM", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool DeployHandler::Iterate()
{
  // happens AppTick times per second
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool DeployHandler::OnStartUp()
{
  MOOSTrace("pDeployHandler starting....\n");
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "ANGLE_1"))
      m_angle_1 = atof(sLine.c_str());
    if(MOOSStrCmp(sVarName, "ANGLE_2"))
      m_angle_2 = atof(sLine.c_str());
    if(MOOSStrCmp(sVarName, "ANGLE_3"))
      m_angle_3 = atof(sLine.c_str());

    if(MOOSStrCmp(sVarName, "RANGE_1"))
      m_range_1 = atof(sLine.c_str());
    if(MOOSStrCmp(sVarName, "RANGE_2"))
      m_range_2 = atof(sLine.c_str());
    if(MOOSStrCmp(sVarName, "RANGE_3"))
      m_range_3 = atof(sLine.c_str());
  }

  RegisterVariables();
  return(true);
}


//---------------------------------------------------------
// Procedure: handleNewDeployToPosition
//
// Examples of strings to parse:
// 
// (1)  "x=24.5, y=-19.2"
// (2)  "lat = 88.209343, lon = -12.873241"

bool DeployHandler::handleNewDeployToPosition(string str)
{
  str = tolower(str);

  double x_meters, y_meters, deploy_speed;
  bool   in_meters = true;
  bool   speed_req = false;

  speed_req = tokParse(str, "speed", ',', '=', deploy_speed);

  in_meters = in_meters && tokParse(str, "x", ',', '=', x_meters);
  in_meters = in_meters && tokParse(str, "y", ',', '=', y_meters);

  if(!in_meters) {
  }

  string deploy_str = "points=";
  deploy_str += doubleToString(x_meters, 2);
  deploy_str += ",";
  deploy_str += doubleToString(y_meters, 2);
  
  if(speed_req) {
    deploy_str += " # speed=";
    deploy_str += doubleToString(speed_req);
  }
  
  m_Comms.Notify("LOITER",           "false");
  m_Comms.Notify("STATION_KEEP",     "false");
  m_Comms.Notify("DEPLOY_TRANSIT",   "true");
  m_Comms.Notify("RELATIVE_STATION", "false");
  
  m_Comms.Notify("TRANSIT_UPDATES", deploy_str);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleNewLoiterPosition
//
// Examples of strings to parse:
// 
// (1)  "x=24.5, y=-19.2"
// (2)  "lat = 88.209343, lon = -12.873241"

bool DeployHandler::handleNewLoiterPosition(string str)
{
  str = tolower(str);

  double x_meters, y_meters, deploy_speed;
  bool   in_meters = true;
  bool   speed_req = false;

  speed_req = tokParse(str, "speed", ',', '=', deploy_speed);

  in_meters = in_meters && tokParse(str, "x", ',', '=', x_meters);
  in_meters = in_meters && tokParse(str, "y", ',', '=', y_meters);

  if(!in_meters) {
  }

  string deploy_str = "center_assign=";
  deploy_str += doubleToString(x_meters, 2);
  deploy_str += ",";
  deploy_str += doubleToString(y_meters, 2);
  
  if(speed_req) {
    deploy_str += " # speed=";
    deploy_str += doubleToString(speed_req);
  }
  
  m_Comms.Notify("STATION_KEEP",     "false");
  m_Comms.Notify("DEPLOY_TRANSIT",   "false");
  m_Comms.Notify("RELATIVE_STATION", "false");
  m_Comms.Notify("LOITER", "true");
  
  m_Comms.Notify("LOITER_UPDATES", deploy_str);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleNewRelativePosition
//

bool DeployHandler::handleNewRelativePosition(string str)
{
  str = tolower(str);

  double range, bearing;
  string contact;

  bool range_ok   = tokParse(str, "range",   ',', '=', range);
  bool bearing_ok = tokParse(str, "bearing", ',', '=', bearing);
  bool contact_ok = tokParse(str, "contact", ',', '=', contact);

  cout << "range:   " << range   << endl;
  cout << "bearing: " << bearing << endl;
  cout << "contact: " << contact << endl;
  
  if(!range_ok || !bearing_ok || !contact_ok)
    return(false);
  
  string deploy_str = "rstation_position=";
  deploy_str += doubleToString(range, 2);
  deploy_str += ",";
  deploy_str += doubleToString(bearing, 2);
  deploy_str += "#contact=" + contact;

  m_Comms.Notify("LOITER",           "false");
  m_Comms.Notify("STATION_KEEP",     "false");
  m_Comms.Notify("DEPLOY_TRANSIT",   "false");
  m_Comms.Notify("RELATIVE_STATION", "true");
  
  m_Comms.Notify("RSTATION_UPDATES", deploy_str);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleReclaim
//

bool DeployHandler::handleReclaim(string str)
{
  str = tolower(stripBlankEnds(str));
  if((str != "one") && (str != "two"))
    return(false);

  
  string deploy_str = "rstation_position=";

  if(str == "one") {
    deploy_str += doubleToString(m_range_1, 2);
    deploy_str += ",";
    deploy_str += doubleToString(m_angle_1, 2);
  }
  else if(str == "two") {
    deploy_str += doubleToString(m_range_2, 2);
    deploy_str += ",";
    deploy_str += doubleToString(m_angle_2, 2);
  }
  else {
    deploy_str += doubleToString(m_range_3, 2);
    deploy_str += ",";
    deploy_str += doubleToString(m_angle_3, 2);
  }
  deploy_str += "#contact=PTSUR";

  m_Comms.Notify("LOITER",           "false");
  m_Comms.Notify("STATION_KEEP",     "false");
  m_Comms.Notify("DEPLOY_TRANSIT",   "false");
  m_Comms.Notify("RELATIVE_STATION", "true");
  
  m_Comms.Notify("RSTATION_UPDATES", deploy_str);

  return(true);
  
}
