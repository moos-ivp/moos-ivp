/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USC_MOOSApp.cpp                                      */
/*    DATE: Jan 4th, 2011                                        */
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

#include <iostream>
#include <cmath>
#include "USC_MOOSApp.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "CurrentField.h"

using namespace std;

//------------------------------------------------------------------------
// Constructor

USC_MOOSApp::USC_MOOSApp() 
{
  m_pending_cfield = false;
  m_post_var = "USM_FORCE_VECTOR";
  m_posx     = 0;
  m_posy     = 0;
  
  m_posx_init = false;
  m_posy_init = false;
}

//------------------------------------------------------------------------
// Procedure: OnNewMail
//      Note: 

bool USC_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;
    string key = Msg.m_sKey;
    double dval = Msg.m_dfVal;
    string sval = Msg.m_sVal;

    if(key == "NAV_X") {
      m_posx = dval;
      m_posx_init = true;
    }
    else if(key == "NAV_Y") {
      m_posy = dval;
      m_posy_init = true;
    }
  }
  
  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnStartUp
//      Note: 

bool USC_MOOSApp::OnStartUp()
{
  cout << "SimCurrent Starting" << endl;
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine  = *p;
    string param  = stripBlankEnds(MOOSChomp(sLine, "="));
    string value  = stripBlankEnds(sLine);
    //double dval   = atof(value.c_str());

    param = toupper(param);

    if(param == "CURRENT_FIELD") {
      bool ok = setCurrentField(value);
      if(!ok)
	cout << "Bad Current Field" << endl;
      m_pending_cfield = true;
    }
    else if(param == "CURRENT_FIELD_ACTIVE")
      m_cfield_active = (tolower(value) == "true");
  }

  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if(!m_MissionReader.GetValue("LatOrigin", latOrigin))
    cout << "pSimCurrent: LatOrigin not set in *.moos file." << endl;
  else if(!m_MissionReader.GetValue("LongOrigin", longOrigin))
    cout << "pSimCurrent: LongOrigin not set in *.moos file" << endl;
  else
    m_current_field.initGeodesy(latOrigin, longOrigin);
  
  
  registerVariables();
  cout << "SimCurrent started" << endl;
  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

bool USC_MOOSApp::OnConnectToServer()
{
  registerVariables();
  cout << "SimCurrent connected" << endl;
  return(true);
}

//------------------------------------------------------------------------
// Procedure: registerVariables()
//      Note: 

void USC_MOOSApp::registerVariables()
{
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
}

//------------------------------------------------------------------------
// Procedure: OnDisconnectFromServer
//      Note: 

bool USC_MOOSApp::OnDisconnectFromServer()
{
  return(true);
}

//------------------------------------------------------------------------
// Procedure: Iterate
//      Note: This is where it all happens.

bool USC_MOOSApp::Iterate()
{
  if(m_pending_cfield)
    postCurrentField();

  if(!m_posx_init || !m_posy_init)
    return(false);

  double force_x, force_y;
  m_current_field.getLocalForce(m_posx, m_posy, force_x, force_y);

  double ang = relAng(0, 0, force_x, force_y);
  double mag = hypot(force_x, force_y);

  string ang_str = doubleToStringX(ang, 2);
  string mag_str = doubleToStringX(mag, 2);

  string vector_str = ang_str + "," + mag_str;

  if(vector_str != m_prev_posting) {
    Notify(m_post_var, vector_str);
    m_prev_posting = vector_str;
  }

  return(true);
}

//------------------------------------------------------------------------
// Procedure: postCurrentField
//      Note: Publishes the following two variables:
//            USC_CFIELD_SUMMARY - one posting for the whole field.
//            VIEW_VECTOR - one for each element in the field.
//  Examples:
//  USC_CFIELD_SUMMARY = field_name=bert, radius=12, elements=19
//         VIEW_VECTOR = x=12, y=-98, mag=3.4, ang=78, label=02


void USC_MOOSApp::postCurrentField()
{
  unsigned int fld_size = m_current_field.size();
  if(fld_size == 0)
    return;

  m_pending_cfield = false;

  string cfield_name   = m_current_field.getName();
  string cfield_radius = doubleToString(m_current_field.getRadius());
  cfield_radius = dstringCompact(cfield_radius);
  string cfield_size = doubleToString(fld_size);
  cfield_size = dstringCompact(cfield_size);

  string summary = "field_name=" + cfield_name;
  summary += ", radius=" + cfield_radius;
  summary += ", elements=" + cfield_size;
  Notify("USC_CFIELD_SUMMARY", summary);

  unsigned int i;
  for(i=0; i<fld_size; i++) {
    double xval = m_current_field.getXPos(i);
    double yval = m_current_field.getYPos(i);
    double fval = m_current_field.getForce(i);
    double dval = m_current_field.getDirection(i);
    
    string xstr = doubleToStringX(xval,2);
    string ystr = doubleToStringX(yval,2);
    string fstr = doubleToStringX(fval,2);
    string dstr = doubleToStringX(dval,2);
    string id   = uintToString(i);
    
    string msg = "xpos=" + xstr + ",ypos=" + ystr + ",mag=" + fstr;
    msg += ",ang=" + dstr + ",label=" + cfield_name + "_" + id;
    Notify("VIEW_VECTOR", msg);
  }
}


//--------------------------------------------------------------------- 
// Procedure: setCurretField                                      

bool USC_MOOSApp::setCurrentField(string filename)
{
  m_current_field = CurrentField();
  m_current_field.populate(filename);
  m_current_field.print();
  return(true);
}






