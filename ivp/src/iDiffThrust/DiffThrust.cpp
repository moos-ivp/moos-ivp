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

  m_post_thrust_mode_dif = false;
  m_post_thrust_mode_rev = false;

  m_last_input_tstamp = 0;

  m_allstop_prev = true;
  m_warning_prev = false;
  m_allstop_commanded = false;
  m_continued = false;
  m_thrust_mode_mail = false;

  m_thrust_mode_rev_observed = "n/a";
  m_thrust_mode_dif_observed = "n/a";
  
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
    string sval  = msg.GetString(); 

#if 0
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "DESIRED_THRUST_L")
      m_desired_thrust_l_observed = dval;
    else if(key == "DESIRED_THRUST_R") 
      m_desired_thrust_r_observed = dval;
    else if(key == "THRUST_MODE_REVERSE") {
      m_thrust_mode_rev_observed = sval;
      m_thrust_mode_mail = true;
    }
    else if(key == "THRUST_MODE_DIFFERENTIAL") {
      m_thrust_mode_dif_observed = sval;
      m_thrust_mode_mail = true;
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
  m_Comms.Register("THRUST_MODE_REVERSE", 0);
  m_Comms.Register("THRUST_MODE_DIFFERENTIAL", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool DiffThrust::Iterate()
{
  double curr_time = MOOSTime();
  double diff_time = (curr_time - m_last_input_tstamp);
  
  if(m_first_iteration) {
    Notify("DESIRED_THRUST_L", 0.0);
    Notify("DESIRED_THRUST_R", 0.0);
    if(m_post_thrust_mode_dif) 
      Notify("THRUST_MODE_DIFFERENTIAL", "true");
    if(m_post_thrust_mode_rev) 
      Notify("THRUST_MODE_REVERSE", "true");
  }

  if(((diff_time > 10) && !m_allstop_prev) || m_allstop_commanded) {
    Notify("DESIRED_THRUST_L", 0.0);
    Notify("DESIRED_THRUST_R", 0.0);
    m_allstop_prev = true;
    m_desired_thrust_l = 0;
    m_desired_thrust_r = 0;
    cout << "   -------   ALL-STOP   --------" << endl;
  }  
  else if(diff_time > 5) {
    if(!m_allstop_prev && !m_warning_prev) {
      cout << " Hit the Back-Slash '\\' key to avoid ALL-STOP" << endl;
      m_warning_prev = true;
    } 
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
     m_first_iteration || m_continued || m_allstop_commanded || m_thrust_mode_mail) {
    cout << "========================================(Hit H for Help)" << endl;
    cout << "     THRUST_MODE_REVERSE: " << m_thrust_mode_rev_observed << endl;
    cout << "THRUST_MODE_DIFFERENTIAL: " << m_thrust_mode_dif_observed << endl;
    cout << "DESIRED_THRUST_L: " << m_desired_thrust_l << 
      " (" << m_desired_thrust_l_observed << ")" << endl;
    cout << "DESIRED_THRUST_R: " << m_desired_thrust_r << 
      " (" << m_desired_thrust_r_observed << ")" << endl;
  }

  m_desired_thrust_l_prev = m_desired_thrust_l;
  m_desired_thrust_r_prev = m_desired_thrust_r;

  m_desired_thrust_l_observed_prev = m_desired_thrust_l_observed;
  m_desired_thrust_r_observed_prev = m_desired_thrust_r_observed;

  m_first_iteration = false;
  m_continued = false;
  m_allstop_commanded = false;
  m_thrust_mode_mail = false;

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
    m_allstop_commanded = true;
  }
  // Keep things going
  else if((c == 92) ||  (c == '|')) {
    m_last_input_tstamp = MOOSTime();
    m_continued = true;
    return(true);
  }
  else if((c == 'r') ||  (c == 'R')) {
    double avg = (m_desired_thrust_l + m_desired_thrust_r) / 2;
    if((avg < 0) && ((m_desired_thrust_l > 0) || (m_desired_thrust_r > 0)))
      avg = 0;
    if((avg > 0) && ((m_desired_thrust_l < 0) || (m_desired_thrust_r < 0)))
      avg = 0;
    m_desired_thrust_l = avg;
    m_desired_thrust_r = avg;
    cout << "avg: " << avg << endl;
  }
  else if((c == 'h') ||  (c == 'H')) {
    cout << "========================================" << endl;
    cout << "iDiffThrust                                               " << endl;
    cout << "       a:  Add 1% Thrust LEFT      A: Add 2% Thrust LEFT  " << endl;
    cout << "       z:  Sub 1% Thrust LEFT      A: Add 2% Thrust LEFT  " << endl;
    cout << "       k:  Add 1% Thrust RIGHT     K: Add 2% Thrust LEFT  " << endl;
    cout << "       m:  Sub 1% Thrust RIGHT     M: Add 2% Thrust LEFT  " << endl;
    cout << "   SPACE:  ALL-STOP                                       " << endl;
    cout << "       \\:  Continue                                      " << endl;
    cout << "   r or R:  Thrusts to be Equal (Average)                 " << endl;
    cout << "   h or H:  Help and Continue                             " << endl;
    m_continued = true;
  }
  else {
    cout << "Unhandled char: [" << c << "]" << endl;
    return(false);
  }

  m_last_input_tstamp = MOOSTime();

  if((m_allstop_prev == true) && 
     ((m_desired_thrust_l > 0) || (m_desired_thrust_r > 0))) {
    m_allstop_prev = false;
    m_warning_prev = false;
  }

  int dlft = (int)(m_desired_thrust_l);
  int drgt = (int)(m_desired_thrust_r);

  m_desired_thrust_l = (double)(dlft);
  m_desired_thrust_r = (double)(drgt);

  return(true);
}

