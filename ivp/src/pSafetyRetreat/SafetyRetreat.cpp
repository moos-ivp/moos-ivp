/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: SafetyRetreat.cpp                               */
/*    DATE: August 2010                                     */
/************************************************************/

#include <iterator>
#include "SafetyRetreat.h"
#include "MBUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SafetyRetreat::SafetyRetreat()
{
  m_retreat_duration = 1200;   // 20 minutes
  m_verbose = false; 
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SafetyRetreat::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    // string sval  = msg.GetString(); 
    // double mtime = msg.GetTime();
    // bool   mdbl  = msg.IsDouble();
    // bool   mstr  = msg.IsString();
    // string msrc  = msg.GetSource();

    if(tolower(key) == tolower(m_retreat_cue))
      handleSafetyRetreat();
    else if(key == "NAV_X")
      m_osx = dval;
    else if(key == "NAV_Y")
      m_osy = dval;    
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SafetyRetreat::OnConnectToServer()
{
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void SafetyRetreat::RegisterVariables()
{
  if(m_retreat_cue != "")
    m_Comms.Register(m_retreat_cue, 0);
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool SafetyRetreat::Iterate()
{
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SafetyRetreat::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pSafetyRetreat starting....\n");

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string line  = stripBlankEnds(*p);
      string param = tolower(stripBlankEnds(biteString(line, '=')));
      string value = stripBlankEnds(line);

      if(param == "polygon")
	handleNewPolygon(value);
      else if((param == "retreat_cue_var") && !strContainsWhite(value))
	m_retreat_cue = value;
      else if((param == "retreat_message_var") && !strContainsWhite(value))
	m_retreat_message = value;
      else if((param == "retreat_notify_var") && !strContainsWhite(value))
	m_retreat_notify = value;
      else if(param == "duration") {
	if(isNumber(value)) {
	  double dval = atof(value.c_str());
	  if(dval > 0)
	    m_retreat_duration = dval;
	}
      }
      else if(param == "verbose") {
	string str = tolower(value);
	if((str == "true") || (str == "verbose"))
	  m_verbose = true;
      }
    }
  }

  if(m_verbose) {
    cout << termColor("blue"); 
    cout << "Safe Areas: " << m_retreat_areas.size() << endl;
    cout << "Retreat Cue Var: [" << m_retreat_cue << "]" << endl;
    cout << "Retreat Msg Var: [" << m_retreat_message << "]" << endl;
    cout << "Retreat Notify Var: [" << m_retreat_notify << "]" << endl;
    cout << "Retreat duration: " << m_retreat_duration << endl;
    cout << termColor() << endl;
  }
  
  RegisterVariables();
  
  return(true);
}

//---------------------------------------------------------
// Procedure: handleNewPolygon()

bool SafetyRetreat::handleNewPolygon(string poly)
{
  XYPolygon new_poly = string2Poly(poly);
  if(new_poly.is_convex())
    m_retreat_areas.push_back(new_poly);

  return(true);
}


//---------------------------------------------------------
// Procedure: handleSafetyRetreat()

bool SafetyRetreat::handleSafetyRetreat()
{
  if(m_verbose) {
    cout << termColor("red"); 
    cout << "Threat detected, handling retreat" << endl;
    cout << termColor() << endl;
  }

  if(m_retreat_areas.size() == 0)
    return(false);
  
  unsigned int index = closestPolygon();
  XYPolygon poly = m_retreat_areas[index];
  string poly_spec = poly.get_spec();
  
  string msg = "polygon = " + poly_spec;
  msg += " # duration = " + doubleToString(m_retreat_duration);
  m_Comms.Notify(m_retreat_message, msg);
  m_Comms.Notify(m_retreat_notify, "true");
  
  return(true);
}


//---------------------------------------------------------
// Procedure: closestPolygon
//   Purpose: Determine which of the N polygons designated as
//            retreat areas is currently closest to ownship.
//      Note: Does not consider ownship trajectory. Room for 
//            further improvement.


unsigned int SafetyRetreat::closestPolygon()
{
  unsigned int i, vsize = m_retreat_areas.size();
  if(vsize == 0)
    return(0);

  unsigned int lowix = 0;
  double lowdist = m_retreat_areas[0].dist_to_poly(m_osx, m_osy);

  for(i=1; i<vsize; i++) {
    double dist = m_retreat_areas[i].dist_to_poly(m_osx, m_osy);
    if(dist < lowdist) {
      lowdist = dist;
      lowix = i;
    }
  }

  return(lowix);
}
