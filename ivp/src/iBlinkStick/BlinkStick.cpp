/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: BlinkStick.cpp                                  */
/*    DATE: April 7th, 2024                                 */
/************************************************************/

#include <cstdlib>
#include <iterator>
#include "MBUtils.h"
#include "BlinkStick.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

BlinkStick::BlinkStick()
{
  m_qblink = "QBLINK";
  m_system_result = 0;
  m_total_cmds = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool BlinkStick::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "QBLINK") 
      handleMailQBlink(sval);

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool BlinkStick::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool BlinkStick::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool BlinkStick::OnStartUp()
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
    if(param == "qblink")
      handled = setNonWhiteVarOnString(m_qblink, value);
    
    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void BlinkStick::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register(m_qblink, 0);
}

//---------------------------------------------------------
// Procedure: handleMailQBlink()

bool BlinkStick::handleMailQBlink(string sval)
{
  string color;
  int    dim=80;
  int    time=-1;
  int    delay=-1;
  bool   side0=true;
  bool   side1=true;
  
  // Part 1: Parse the posting/request
  vector<string> svector = parseString(sval, ',');
  for(unsigned int i=0; i<svector.size();i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    if(param == "color") {
      if((value == "blue")   || (value == "brown")  ||
	 (value == "green")  || (value == "pink")   ||
	 (value == "red")    || (value == "orange") ||
	 (value == "yellow") || (value == "cyan")   ||
	 (value == "white")  || (value == "random") ||
	 (value == "purple") || (value == "off"))
	color = value;
    }
    else if(param == "time")
      setIntOnString(time, value);
    else if(param == "delay")
      setIntOnString(delay, value);
    else if(param == "side0")
      setBooleanOnString(side0, value);
    else if(param == "side1")
      setBooleanOnString(side1, value);
    else if(param == "dim") {
      setIntOnString(dim, value);
      if(dim < 1)
	dim = 1;
      else if(dim > 100)
	dim = 100;
    }
  }

  // Part 2: Build a non-off command
  if(color == "")
    return(false);
  
  if(color == "off") {
    string cmd = "qblink.sh off";
    m_system_result = system(cmd.c_str());
    reportEvent(cmd);
    return(true);
  }

  // Part 3: Build a non-off command
  string cmd = color + " --dim=" + intToString(dim);
  if(time > 0)
    cmd += " --time=" + intToString(time);
  if(delay > 0)
    cmd += " --delay=" + intToString(delay);

  string sides = "-2";
  if(!side0)
    sides = "-1";
  else if(!side1)
    sides = "-0";

  // Part 4: Post the system command
  cmd += " " + sides;
  m_system_result = system(cmd.c_str());
  reportEvent(cmd);
  m_total_cmds++;
  
  return(true);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool BlinkStick::buildReport() 
{
  m_msgs << "Config:                        " << endl;
  m_msgs << " QBlink Var:                   " << m_qblink << endl;
  m_msgs << "                               " << endl;
  m_msgs << "State:                         " << endl;
  m_msgs << " Total Cmds: " << uintToString(m_total_cmds) << endl;
  m_msgs << endl;
  
  return(true);
}




