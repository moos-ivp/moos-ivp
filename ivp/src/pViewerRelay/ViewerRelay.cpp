/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ViewerRelay.cpp                                 */
/*    DATE: Sep 16th 2007                                   */
/************************************************************/

#include <iterator>
#include <iostream>
#include "ViewerRelay.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool ViewerRelay::OnNewMail(MOOSMSG_LIST &NewMail)
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

    key = toupper(key);
    
    if(key == "MVIEWER_LCLICK")
      handleNewViewerLeftClick(sval);
    //    if(key == "MVIEWER_RCLICK")
    //      handleNewViewerRightClick(sval);
    if(key == "MVIEWER_RCLICK_RP")
      handleNewViewerRightClickRP(sval);
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
  m_Comms.Register("MVIEWER_LCLICK", 0);
  m_Comms.Register("MVIEWER_RCLICK", 0);
  m_Comms.Register("MVIEWER_RCLICK_RP", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool ViewerRelay::Iterate()
{
  // happens AppTick times per second
  cout << "." << flush;
	
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
  string var_name = "DEPLOY_TO_PT_" + v_name;
  string var_val  = "x=" + x_meters + "," + "y=" + y_meters;

  m_Comms.Notify(var_name, var_val);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleNewViewerRightClick

bool ViewerRelay::handleNewViewerRightClick(string str)
{
  string x_meters, y_meters, v_name;

  bool x_ok, y_ok, v_ok;
  
  x_ok   = tokParse(str, "x", ',', '=', x_meters);
  y_ok   = tokParse(str, "y", ',', '=', y_meters);
  v_ok   = tokParse(str, "vname", ',', '=', v_name);
  
  if(!v_ok)
    return(false);
  else
    v_name = toupper(stripBlankEnds(v_name));
  
  if(x_ok && y_ok) {
    string var_name = "DEPLOY_TO_LOITER_" + v_name;
    string var_val  = "x=" + x_meters + "," + "y=" + y_meters;
    m_Comms.Notify(var_name, var_val);
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------
// Procedure: handleNewViewerRightClickRP

bool ViewerRelay::handleNewViewerRightClickRP(string str)
{
  string vname, range, bearing, contact;
  bool   vname_ok, range_ok, bearing_ok, contact_ok;
  
  vname_ok   = tokParse(str, "vname", ',', '=', vname);
  range_ok   = tokParse(str, "range", ',', '=', range);
  bearing_ok = tokParse(str, "bearing", ',', '=', bearing);
  contact_ok = tokParse(str, "contact", ',', '=', contact);
  
  if(!vname_ok || !range_ok || !bearing_ok || !contact_ok)
    return(false);

  vname = toupper(stripBlankEnds(vname));
  
  string var_name = "DEPLOY_TO_RELPOS_" + vname;
  string var_val  = "range=" + range + ",";
  var_val += "bearing=" + bearing + ",";
  var_val += "contact=" + contact;
  m_Comms.Notify(var_name, var_val);
  
  return(true);  
}

