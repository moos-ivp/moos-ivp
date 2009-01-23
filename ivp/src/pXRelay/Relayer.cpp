/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: Relayer.cpp                                     */
/*    DATE: June 26th, 2008                                 */
/************************************************************/

#include <iterator>
#include "Relayer.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Relayer::Relayer()
{
  m_tally_recd = 0;
  m_tally_sent = 0;
  m_iterations = 0;

  m_start_time_postings   = 0;
  m_start_time_iterations = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Relayer::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p!=NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    if(key == m_incoming_var) 
      m_tally_recd++;
  }
  return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Relayer::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void Relayer::RegisterVariables()
{
  if(m_incoming_var != "")
    m_Comms.Register(m_incoming_var, 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool Relayer::Iterate()
{
  m_iterations++;

  unsigned int i, amt = (m_tally_recd - m_tally_sent);
  for(i=0; i<amt; i++) {
    m_tally_sent++;
    m_Comms.Notify(m_outgoing_var, m_tally_sent);
  }
  
  // If this is the first iteration just note the start time, otherwise
  // note the currently calculated frequency and post it to the DB.
  if(m_start_time_iterations == 0)
    m_start_time_iterations = MOOSTime();
  else {
    double delta_time = (MOOSTime() - m_start_time_iterations) + 0.01;
    double frequency  = (double)(m_iterations) / delta_time;
    m_Comms.Notify(m_outgoing_var+"_ITER_HZ", frequency);
  }
    

  // If this is the first time a received msg has been noted, just
  // note the start time, otherwise calculate and post the frequency.
  if(amt > 0) {
    if(m_start_time_postings == 0)
      m_start_time_postings = MOOSTime();
    else {
      double delta_time = (MOOSTime() - m_start_time_postings) + 0.01;
      double frequency = (double)(m_tally_sent) / delta_time;
      m_Comms.Notify(m_outgoing_var+"_POST_HZ", frequency);
    }
  }
  return(true);
}



//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: happens before connection is open

bool Relayer::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sLine = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "INCOMING_VAR")) {
      if(!strContains(sLine, " "))
	m_incoming_var = stripBlankEnds(sLine);
    }
    
    else if(MOOSStrCmp(sVarName, "OUTGOING_VAR")) {
      if(!strContains(sLine, " "))
	m_outgoing_var = stripBlankEnds(sLine);
    }
  }

  RegisterVariables();
  return(true);
}

