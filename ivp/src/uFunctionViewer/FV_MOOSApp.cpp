/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FV_MOOSApp.cpp                                       */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
#include <iostream>
#include <iterator>
#include "FV_MOOSApp.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "AOF_Rings.h"
#include "BuildUtils.h"

using namespace std; 

//----------------------------------------------------------
// Procedure: Constructor

FV_MOOSApp::FV_MOOSApp()
{
  loc_x      = 50;
  loc_y      = 50;
  iteration  = 0;
  start_time = 0;
  delta      = 10;
}

//----------------------------------------------------------
// Procedure: Destructor

FV_MOOSApp::~FV_MOOSApp()
{
}

//----------------------------------------------------------
// Procedure: OnNewMail()

bool FV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    if(msg.m_sKey == ipf_name) {
      string ipf_str = msg.m_sVal;
      demuxer.addMuxPacket(ipf_str, MOOSTime());
    }
  }
  return(true);
}

//----------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: Register for variables here. 
//            Possibly read info from the mission file.
//            m_MissionReader.GetConfigurationParam("Name", <string>);
//            m_Comms.Register("VARNAME", is_float(int));

bool FV_MOOSApp::OnConnectToServer()
{
  if(!m_MissionReader.GetValue("ipf_name", ipf_name)) {
    MOOSTrace("Function to be monitored not provided\n");
    return(false);
  }

  if(ipf_name != "")
    m_Comms.Register(ipf_name, 0);
  return(true);
}


//----------------------------------------------------------
// Procedure: Iterate()

bool FV_MOOSApp::Iterate()
{
  iteration++;
  
  if((iteration %10)==0) {
    double hz = (double)(iteration) / ((MOOSTime() - start_time));
    cout << "Iteration: (" << iteration << ")";
    cout << "(" << hz << "/sec)" << endl;
  }

  string str = demuxer.getDemuxString();
  if(str != "")
    gui->setIPF(str);

  return(true);
}

//----------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool FV_MOOSApp::OnStartUp()
{
  start_time = MOOSTime();  
  return(true);
}





