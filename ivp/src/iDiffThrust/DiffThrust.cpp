/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: DiffThrust.cpp                                       */
/*   DATE: Sep 14th 2014                                        */
/****************************************************************/

#include <iterator>
#include "DiffThrust.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DiffThrust::DiffThrust()
{
  m_desired_thrust_l = 0;
  m_desired_thrust_r = 0;

  m_desired_thrust_l_prev = 0;
  m_desired_thrust_r_prev = 0;

  m_desired_thrust_l_observed = 0;
  m_desired_thrust_r_observed = 0;

  m_desired_thrust_l_observed_prev = 0;
  m_desired_thrust_r_observed_prev = 0;

  m_last_input_tstamp = 0;

  m_allstop_prev = true;
  m_new_thrust_mail = false;

  m_first_iteration = true;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool DiffThrust::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    string key   = msg.GetKey();

#if 0
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "DESIRED_THRUST_L") {
      m_desired_thrust_l_observed = dval;
      m_new_thrust_mail = true;
    }
    if(key == "DESIRED_THRUST_R") {
      m_desired_thrust_r_observed = dval;
      m_new_thrust_mail = true;
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DiffThrust::OnConnectToServer()
{
  RegisterVariables();  

  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void DiffThrust::RegisterVariables()
{
  m_Comms.Register("DESIRED_THRUST_L", 0);
  m_Comms.Register("DESIRED_THRUST_R", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool DiffThrust::Iterate()
{
  double curr_time = MOOSTime();
  double diff_time = (curr_time - m_last_input_tstamp);

  if((diff_time > 7) && !m_allstop_prev) {
    Notify("DESIRED_THRUST_L", 0.0);
    Notify("DESIRED_THRUST_R", 0.0);
    m_allstop_prev = true;
    m_desired_thrust_l = 0;
    m_desired_thrust_r = 0;
    cout << "   -------   ALL-STOP   --------" << endl;
  } 
  else if((diff_time > 4) && !m_allstop_prev) {
    Notify("DESIRED_THRUST_L", 0.0);
    Notify("DESIRED_THRUST_R", 0.0);
    cout << " Hit the Back-Slash '\' key to avoid ALL-STOP" << endl;
  } 

  // If change in DESIRED_THRUST_* then Post it!
  if(m_desired_thrust_l != m_desired_thrust_l_prev)
    Notify("DESIRED_THRUST_L", m_desired_thrust_l);
  if(m_desired_thrust_r != m_desired_thrust_r_prev)
    Notify("DESIRED_THRUST_R", m_desired_thrust_r);

  // Check if desired_thrust or observed thrust has changed since the last report
  if((m_desired_thrust_l_prev != m_desired_thrust_l) || 
     (m_desired_thrust_r_prev != m_desired_thrust_r) || 
     (m_desired_thrust_l_observed_prev != m_desired_thrust_l_observed) ||
     (m_desired_thrust_r_observed_prev != m_desired_thrust_r_observed) ||
     m_first_iteration) {
    cout << "========================================" << endl;
    cout << "DESIRED_THRUST_L: " << m_desired_thrust_l << 
      " (" << m_desired_thrust_l_observed << ")" << endl;
    cout << "DESIRED_THRUST_R: " << m_desired_thrust_r << 
      " (" << m_desired_thrust_r_observed << ")" << endl;
    m_new_thrust_mail = false;
  }

  m_desired_thrust_l_prev = m_desired_thrust_l;
  m_desired_thrust_r_prev = m_desired_thrust_r;

  m_desired_thrust_l_observed_prev = m_desired_thrust_l_observed;
  m_desired_thrust_r_observed_prev = m_desired_thrust_r_observed;

  m_first_iteration = false;

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool DiffThrust::OnStartUp()
{
  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: handleCharInput

bool DiffThrust::handleCharInput(char c)
{
  m_last_input_tstamp = MOOSTime();

  // Slower changes
  if(c == 'a')
    m_desired_thrust_l = m_desired_thrust_l + 1;
  else if(c == 'z')
    m_desired_thrust_l = m_desired_thrust_l - 1;
  
  else if(c == 'k')
    m_desired_thrust_r = m_desired_thrust_r + 1;
  else if(c == 'm')
    m_desired_thrust_r = m_desired_thrust_r - 1;
  
  // Faster changes
  else if(c == 'A')
    m_desired_thrust_l = m_desired_thrust_l + 2;
  else if(c == 'Z')
    m_desired_thrust_l = m_desired_thrust_l - 2;

  else if(c == 'K')
    m_desired_thrust_r = m_desired_thrust_r + 2;
  else if(c == 'M')
    m_desired_thrust_r = m_desired_thrust_r - 2;
  
  // Emergency Stop
  else if(c == ' ') {
    m_desired_thrust_l = 0;
    m_desired_thrust_r = 0;
  }
  else {
    cout << "Unhandled char: [" << c << "]" << endl;
    return(false);
  }

  if((m_allstop_prev == true) && 
     ((m_desired_thrust_l > 0) || (m_desired_thrust_r > 0)))
    m_allstop_prev = false;

  return(true);
}

