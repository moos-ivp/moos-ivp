/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Drone.cpp                                            */
/*    DATE: March 2005                                           */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iterator>
#include <iostream>
#include <string>
#include "Drone.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor

Drone::Drone()
{
  counter = 0;
  first_notify = false;
}

//----------------------------------------------------------
// Procedure: OnNewMail

bool Drone::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
  }
  return(true);
}

//----------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: register for variables here
//            possibly look at the mission file?
//            m_MissionReader.GetConfigurationParam("Name", <string>);
//            m_Comms.Register("VARNAME", is_float(int));

bool Drone::OnConnectToServer()
{
  return(true);
}

//----------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool Drone::Iterate()
{
  bool update = false;
  if((MOOSTime() - last_time) > 10) {
    last_time = MOOSTime();
    if(counter == 0)
      counter = 1;
    else
      counter = 0;
    cout << "Counter = " << counter << endl;
    update = true;
  }

  if(update || !first_notify) {
    first_notify = true;
    string str = doubleToString(counter);
    str = dstringCompact(str);
    m_Comms.Notify("DRONE_COUNTER", str);
  }

  return(true);
}

//----------------------------------------------------------
// Procedure: OnStartUp
//      Note: Happens before connection is open

bool Drone::OnStartUp()
{
  last_time = MOOSTime();
  return(true);
}





