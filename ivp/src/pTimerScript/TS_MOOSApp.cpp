/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.cpp                                  */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#include <iterator>
#include "TS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TS_MOOSApp::TS_MOOSApp()
{
}

//---------------------------------------------------------
// Destructor

TS_MOOSApp::~TS_MOOSApp()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();

//    if(key == "FOOBAR")
//      handleNewFOOBAR(sval);

  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TS_MOOSApp::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
	
  RegisterVariables();  

  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void TS_MOOSApp::RegisterVariables()
{
  m_Comms.Register("FOO_BAR", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool TS_MOOSApp::Iterate()
{
  // happens AppTick times per second
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TS_MOOSApp::OnStartUp()
{
  // happens before connection is open
	
  RegisterVariables();
  return(true);
}

