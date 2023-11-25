/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DeadManPost.cpp                                      */
/*    DATE: June 14th, 2014                                      */
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
#include <cstdlib>
#include "MBUtils.h"
#include "ACTable.h"
#include "DeadManPost.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DeadManPost::DeadManPost()
{
  // Intitialize config variables
  m_post_policy     = "once";
  m_max_noheart     = 10;
  m_active_at_start = true;

  // Intitialize state variables
  m_last_heartbeat = 0;
  m_total_postings = 0;
  m_delta_time     = 0;
  m_postings_done  = false;
  m_total_heartbeats = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DeadManPost::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    
#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key == m_heart_var) {
      m_last_heartbeat = MOOSTime();
      m_total_heartbeats++;
      if(m_post_policy == "reset") {
	m_postings_done = false;
      }
    }
      
    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DeadManPost::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool DeadManPost::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(m_last_heartbeat == 0)
    m_last_heartbeat = MOOSTime();

  if(!m_active_at_start && (m_total_heartbeats == 0))
    m_delta_time = 0;
  else
    m_delta_time = MOOSTime() - m_last_heartbeat;
  

  possiblyMakePostings();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool DeadManPost::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "heartbeat_var") {
      if(!strContainsWhite(value)) {
	m_heart_var = value;
	handled = true;
      }
    }
    else if(param == "deadflag") {
      string varname = biteStringX(value, '=');
      string varval  = value;
      if(!strContainsWhite(varname) && (varval != "")) {
	VarDataPair pair(varname, varval, "auto");
	m_deadflags.push_back(pair);
	handled = true;
      }
    }
    else if(param == "post_policy") {
      string val = tolower(value);
      if((val=="once") || (val=="repeat") || (val=="reset")) {
	m_post_policy = val;
	handled = true;
      }
    }
    else if((param == "max_noheart") && isNumber(value)) {
      m_max_noheart = atof(value.c_str());
      handled = true;
    }
    else if(param == "active_at_start") 
      handled = setBooleanOnString(m_active_at_start, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void DeadManPost::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register(m_heart_var, 0);
}


//------------------------------------------------------------
// Procedure: possiblyMakePostings

bool DeadManPost::possiblyMakePostings() 
{
  if(m_postings_done && (m_post_policy!="repeat"))
    return(false);

  if(!m_active_at_start && (m_total_heartbeats == 0))
    return(false);
  
  unsigned int new_postings = 0;

  // If heartbeat duration has been exceeded, make postings.
  if(m_delta_time > m_max_noheart) {
    for(unsigned int i=0; i<m_deadflags.size(); i++) {
      string moos_var = m_deadflags[i].get_var();
      string moos_val = m_deadflags[i].get_sdata();
      if(m_deadflags[i].is_string()) 
	Notify(moos_var, moos_val);
      else {
	double dval = m_deadflags[i].get_ddata();
	moos_val = doubleToStringX(dval,3);
	Notify(moos_var, dval);
      }
      reportRunWarning("dead_flag: " + moos_var + "=" + moos_val);
      new_postings++;
    }
    m_postings_done = true;
  }

  
  m_total_postings += new_postings;
  if(new_postings > 0)
    return(true);
  return(false);
}

  
//------------------------------------------------------------
// Procedure: buildReport()

bool DeadManPost::buildReport() 
{
  double time_remaining = m_max_noheart - m_delta_time;
  if(time_remaining < 0)
    time_remaining = 0;
  
  m_msgs << "Configuration:" << endl;
  m_msgs << "-------------------------------------" << endl;
  m_msgs << "      heart_var: " << m_heart_var      << endl;
  m_msgs << "    max_noheart: " << m_max_noheart    << endl;
  m_msgs << "active_at_start: " << boolToString(m_active_at_start) << endl;
  m_msgs << "    post_policy: " << m_post_policy << endl;
  m_msgs << "      deadflags: (" << m_deadflags.size() << ")" << endl;
  for(unsigned int i=0; i<m_deadflags.size(); i++) {
    m_msgs << "                ";
    m_msgs << "[" << i+1 << "] " << m_deadflags[i].getPrintable() << endl;
  }

  m_msgs << endl;
  m_msgs << "State:" << endl;
  m_msgs << "-------------------------------------" << endl;

  if(!m_active_at_start && (m_total_heartbeats == 0)) {
    m_msgs << "  Elapsed Time: N/A" << endl;
    m_msgs << "Time Remaining: N/A" << endl;
  }
  else {
    m_msgs << "  Elapsed Time: " << doubleToString(m_delta_time,2) << endl;
    m_msgs << "Time Remaining: " << doubleToString(time_remaining,2) << endl;
  }

  m_msgs << "    Heartbeats: " << m_total_heartbeats << endl;
  m_msgs << "Total Postings: " << m_total_postings << endl;
  
  return(true);
}









