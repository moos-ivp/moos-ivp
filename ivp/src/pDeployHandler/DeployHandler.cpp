/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: DeployHandler.cpp                               */
/*    DATE: Sep 15th 2007                                   */
/************************************************************/

#include <iterator>
#include "DeployHandler.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DeployHandler::DeployHandler()
{
}

//---------------------------------------------------------
// Destructor

DeployHandler::~DeployHandler()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DeployHandler::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();

    key = toupper(key);
    if(key == "DEPLOY_TO_POSITION")
      bool ok = handleNewDeployToPosition(sval);

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
  m_Comms.Register("DEPLOY_TO_POSITION", 0);
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
  // happens before connection is open
	
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
  
  m_Comms.Notify("STATION_KEEP",  "false");
  m_Comms.Notify("DEPLOY_TRANSIT", "true");
  
  m_Comms.Notify("TRANSIT_UPDATES", deploy_str);

  return(true);
}

