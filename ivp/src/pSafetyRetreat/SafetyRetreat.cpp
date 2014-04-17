/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SafetyRetreat.cpp                                    */
/*    DATE: August 2010                                          */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iterator>
#include "SafetyRetreat.h"
#include "MBUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SafetyRetreat::SafetyRetreat()
{
  // Initialize configuration variables
  m_retreat_duration  = 1200;   // 20 minutes
  m_retreat_message   = "UP_RETREAT";
  m_retreat_cue       = "THREAT_DETECTED";
  m_retreat_notify    = "SAFETY_RETREAT";
  m_verbose           = true; 

  // Initialize state variables
  m_mark_time = 0;
  m_iteration = 0;
  m_osx       = 0;
  m_osy       = 0;
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
    // double mtime = msg.GetTime();
    // bool   mdbl  = msg.IsDouble();
    // bool   mstr  = msg.IsString();
    // string msrc  = msg.GetSource();

    if(tolower(key) == tolower(m_retreat_cue)) {
      string sval  = msg.GetString(); 
      handleContactDetection(sval);
    }
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
  m_iteration++;


  if((m_mark_time == 0) || (m_unresolved_contacts.size() == 0))
    m_mark_time = MOOSTime();

  double elapsed_time = (MOOSTime() - m_mark_time);

#if 0
  if((m_iteration % 20) == 0) 
    cout << "cue: " << m_retreat_cue << m_iteration << endl;
    cout << "elapsed_time:" << elapsed_time << endl;
#endif

  if(elapsed_time > m_retreat_duration)
    handleContactsResolved();

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

  //if(m_verbose) {
  if(1) {
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
// Procedure: handleContactDetection()

bool SafetyRetreat::handleContactDetection(string contact_name)
{
  if(m_verbose) {
    cout << termColor("red"); 
    cout << "Contact detected, handling retreat" << endl;
    cout << termColor() << endl;
  }

  if(m_retreat_areas.size() == 0)
    return(false);
  
  m_mark_time = MOOSTime();
  if(!vectorContains(m_unresolved_contacts, contact_name))
    m_unresolved_contacts.push_back(contact_name);

  unsigned int index = closestPolygon();
  XYPolygon poly = m_retreat_areas[index];
  string poly_spec = poly.get_spec();
  
  string msg = "polygon = " + poly_spec;
  msg += " # duration = " + doubleToString(m_retreat_duration);
  Notify(m_retreat_message, msg);
  Notify(m_retreat_notify, "true");
  
  return(true);
}

//---------------------------------------------------------
// Procedure: handleContactsResolved

void SafetyRetreat::handleContactsResolved()
{
  unsigned int i, vsize = m_unresolved_contacts.size();
  for(i=0; i<vsize; i++)
    Notify("CONTACT_RESOLVED", m_unresolved_contacts[i]);

  m_unresolved_contacts.clear();
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





