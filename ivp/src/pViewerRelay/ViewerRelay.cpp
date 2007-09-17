/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ViewerRelay.cpp                                 */
/*    DATE: Sep 16th 2007                                   */
/************************************************************/

#include <iterator>
#include "ViewerRelay.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ViewerRelay::ViewerRelay()
{
}

//---------------------------------------------------------
// Destructor

ViewerRelay::~ViewerRelay()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ViewerRelay::OnNewMail(MOOSMSG_LIST &NewMail)
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
    
    if(key == "MVIEWER_LCLICK")
      handleNewViewerLeftClick(sval);
    
    if(key == "MVIEWER_RCLICK")
      handleNewViewerRightClick(sval);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ViewerRelay::OnConnectToServer()
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

void ViewerRelay::RegisterVariables()
{
  m_Comms.Register("FOO_BAR", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool ViewerRelay::Iterate()
{
  // happens AppTick times per second
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool ViewerRelay::OnStartUp()
{
  // happens before connection is open
	
  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: handleNewViewerLeftClick

//  Incoming:
//     MVIEWER_LCLICK = "x=10.5, y=-99.0, vname=alpha"
//  
//  Outgoing:
//     DEPLOY_TO_POINT_ALPHA = "x=10.5, y=-99.0"


bool ViewerRelay::handleNewViewerLeftClick(string str)
{
  string x_meters, y_meters, v_name;

  bool x_ok, y_ok, v_ok;
  
  x_ok = tokParse(str, "x", ',', '=', x_meters);
  y_ok = tokParse(str, "y", ',', '=', y_meters);
  v_ok = tokParse(str, "vname", ',', '=', v_name);

  if(!x_ok || !y_ok || !v_ok)
    return(false);

  v_name = toupper(stripBlankEnds(v_name));
  string var_name = "DEPLOY_TO_POINT_" + v_name;
  string var_val  = "x=" + x_meters + "," + "y=" + y_meters;

  m_Comms.Notify(var_name, var_val);


  return(true);
}

//---------------------------------------------------------
// Procedure: handleNewViewerRightClick

bool ViewerRelay::handleNewViewerRightClick(string str)
{
  return(true);
}

