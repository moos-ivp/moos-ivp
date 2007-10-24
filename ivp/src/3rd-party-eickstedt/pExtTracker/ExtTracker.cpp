#include <MOOSLib.h>
#include <MOOSGenLib.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "ExtTracker.h"

using namespace std;

CExtTracker::CExtTracker()
{
  state = 0;
  ext_state = 0;
  tr_count = 0;
}

CExtTracker::~CExtTracker()
{
    
}


bool CExtTracker::Iterate()
{
 
   
    return true;
}

bool CExtTracker::OnConnectToServer()
{  
 
  m_Comms.Register("TRACK_STAT_V1",0);
  m_Comms.Register("TRACKING_SIGNAL",0);
  m_Comms.Register("BEARING_STATE",0);
  m_Comms.Register("VEH2_X",0);
  m_Comms.Register("VEH2_Y",0);

return true;
}



bool CExtTracker::OnStartUp()
{


  m_Comms.Notify("TRACKING","NO_TRACK");

  double iState;

    if(m_MissionReader.GetValue("initial_state",iState))
    {
      
      state = (int) iState;
    }
    else
    {
        MOOSTrace("ExtTracker:: initial state not set - FAIL\n");
        
        return false;
    }


  return true;   
   
}



bool CExtTracker::OnNewMail(MOOSMSG_LIST &NewMail)
{
   CMOOSMsg Msg;

  //NewMail.sort();
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &Msg = *p;
    
    if(Msg.m_sKey == "TRACK_STAT_V1")
      {
	if (ext_state == 2)
	  m_Comms.Notify("TRACK_STAT",Msg.m_sVal);
      }
    else if(Msg.m_sKey == "BEARING_STATE")
      {
	state = (int) Msg.m_dfVal;
      }
     else if(Msg.m_sKey == "VEH2_X")
      {
	m_Comms.Notify("V2_X",Msg.m_dfVal);
      }
    else if(Msg.m_sKey == "VEH2_Y")
      {
	m_Comms.Notify("V2_Y",Msg.m_dfVal);
      }
    else if(Msg.m_sKey == "TRACKING_SIGNAL")
      {	

	int old_state = ext_state;

	ext_state = (int) Msg.m_dfVal;

	if(ext_state == 2)
	  {
	    if (state ==2){
	         string tmsg = ("TRACKING");
	         m_Comms.Notify("TRACKING",tmsg);
	      }	      
	  }
	else if (ext_state == 0){
	  if (old_state == 2){
	    m_Comms.Notify("TRACK_CONTROL","OFF");
	    m_Comms.Notify("TRACKING","NO_TRACK");
	  }
	}
      }
    
    else
      MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
  }
  
  return UpdateMOOSVariables(NewMail);
}

