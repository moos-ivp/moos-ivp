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
  m_tally      = 0;
  m_start_time = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Relayer::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;

  bool done=false;
  for(p = NewMail.rbegin(); !done && (p!=NewMail.rend()); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

    if(key == m_incoming_var) { 
      m_tally++;
      done = true;
      m_Comms.Notify(m_outgoing_var, m_tally);
      if(m_start_time == 0)
	m_start_time = MOOSTime();
      else {
	double delta = MOOSTime() - m_start_time;
	double frequency = (double)(m_tally) / delta;
	m_Comms.Notify(m_outgoing_var+"_HZ", frequency);
      }
    }
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

