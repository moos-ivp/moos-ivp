/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PXR_MOOSApp.cpp                                      */
/*    DATE: June 26th, 2008                                      */
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
#include "PXR_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

PXR_MOOSApp::PXR_MOOSApp()
{
  m_tally_recd = 0;
  m_tally_sent = 0;
  m_iterations = 0;

  m_start_time_postings   = 0;
  m_start_time_iterations = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PXR_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    
    // We don't need the below, but we keep anyway for clarity
    // string str = msg.GetString();
    // double dbl = msg.GetDouble();
    // bool   is_string = msg.IsDouble();
    // bool   is_double = msg.IsDouble();

    if(key == m_incoming_var) 
      m_tally_recd++;
  }
  cout << "In On New Mail!!!!!!!!!!!!" << endl; 
  return(false);
  return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PXR_MOOSApp::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void PXR_MOOSApp::RegisterVariables()
{
  if(m_incoming_var != "")
    m_Comms.Register(m_incoming_var, 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool PXR_MOOSApp::Iterate()
{
  m_iterations++;

  unsigned int i, amt = (m_tally_recd - m_tally_sent);
  for(i=0; i<amt; i++) {
    m_tally_sent++;
    Notify(m_outgoing_var, m_tally_sent);
  }
  
  // If this is the first iteration just note the start time, otherwise
  // note the currently calculated frequency and post it to the DB.
  if(m_start_time_iterations == 0)
    m_start_time_iterations = MOOSTime();
  else {
    double delta_time = (MOOSTime() - m_start_time_iterations);
    double frequency = 0;
    if(delta_time > 0)
      frequency  = (double)(m_iterations) / delta_time;
    Notify(m_outgoing_var+"_ITER_HZ", frequency);
  }
    

  // If this is the first time a received msg has been noted, just
  // note the start time, otherwise calculate and post the frequency.
  if(amt > 0) {
    if(m_start_time_postings == 0)
      m_start_time_postings = MOOSTime();
    else {
      double delta_time = (MOOSTime() - m_start_time_postings);
      double frequency  = 0;
      if(delta_time > 0)
	frequency = (double)(m_tally_sent) / delta_time;
      Notify(m_outgoing_var+"_POST_HZ", frequency);
    }
  }
  return(true);
}



//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: happens before connection is open

bool PXR_MOOSApp::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  //m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), sParams);

  //m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string line  = *p;
    cout << "[" << line << "]" << endl;

    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    if(param == "incoming_var")
      m_incoming_var = value;

    else if(param == "outgoing_var")
      m_outgoing_var = value;
  }

  RegisterVariables();
  return(true);
}










