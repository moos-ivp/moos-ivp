/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: DeployHandler.cpp                               */
/*    DATE: Sep 15th 2007                                   */
/************************************************************/

#include <iterator>
#include "DeployHandler.h"

//---------------------------------------------------------
// Constructor

DeployHandler::DeployHandler()
{
}

//---------------------------------------------------------
// Destructor

DeployHandler::~DeployHandler()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DeployHandler::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DeployHandler::OnConnectToServer()
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

void DeployHandler::RegisterVariables()
{
  m_Comms.Register("FOO_BAR", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool DeployHandler::Iterate()
{
  // happens AppTick times per second
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool DeployHandler::OnStartUp()
{
  // happens before connection is open
	
  RegisterVariables();
  return(true);
}

