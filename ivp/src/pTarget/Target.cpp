/****************************************************************
 *   Name: Don Eickstedt                                        *
 *   File: Target.cpp                                           *
 *   Date: 1 dec 2005                                           *
 *                                                              *
 *   Synopsis: pTarget is run on a simulated target vehicle.    *
 *   When the vehicle is inside a user-defined polygon, it will *
 *   broadcast its position.                                    *
 ****************************************************************/

#include <MOOSLib.h>
#include <MOOSGenLib.h>

#include "Target.h"

CTarget::CTarget()
{

}

CTarget::~CTarget()
{
    
}


bool CTarget::Iterate()
{

  //if vehicle is in the ops area, start broadcasting position
  if (OpArea.contains(xp,yp)){
    
    MOOSTrace("Target eneters ops area.....");
    m_Comms.Notify("TARGET_XPOS",xp);
    m_Comms.Notify("TARGET_YPOS",yp);

  }
    
    return true;
}




bool CTarget::OnConnectToServer()
{

  m_Comms.Register("NAV_X",0);
  m_Comms.Register("NAV_Y",0);

return true;
}



bool CTarget::OnStartUp()
{

    CMOOSApp::OnStartUp();   
   
    string poly;
    if(m_MissionReader.GetValue("polygon",poly))
    {
      OpArea.initialize(poly);      
    }
    else
    {
        MOOSTrace("pTarget:: OpArea not set - FAIL\n");
        
        return false;
    }

   
    return true;
}



bool CTarget::OnNewMail(MOOSMSG_LIST &NewMail)
{
   CMOOSMsg Msg;

  //NewMail.sort();
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &Msg = *p;
    
   
    if(Msg.m_sKey == "NAV_Y")
      {	
	yp = Msg.m_dfVal;
      }
    else if(Msg.m_sKey == "NAV_X")
      {	
	xp = Msg.m_dfVal;
      }
    else
      MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
  }
}




    

  

