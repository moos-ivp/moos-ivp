// $Header: /home/cvs/repository/project-plusnet/src/iWinch/CiWinch.cpp,v 1.2 2006/08/22 23:55:06 cvs Exp $
// (c) 2005

// CiWinch.cpp: implementation of the CiWinch class.
////////////////////////////////////////////////////////

#include <iterator>
#include "CiWinchSim.h"
#include "dtime.h"
#include "math.h"

CiWinchSim::CiWinchSim()
{
  current_winch_depth = 0.0; //meters
  winch_tick  = 0.25;        // meters/tick
  desired_winch_depth = 0.0; //meters

  winch_low_limit = 60.0;    //meters
  winch_high_limit = 0.0;    //meters

}

CiWinchSim::~CiWinchSim()
{
	
}

bool CiWinchSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	
	for(p = NewMail.begin(); p != NewMail.end(); p++) {
		CMOOSMsg &msg = *p;
		
		if(msg.m_sKey == "WINCH_DESIRED_DEPTH") 
		  {
		    desired_winch_depth = msg.m_dfVal;
		  }
	}

	NewMail.clear();
	
	return true;
}

bool CiWinchSim::OnConnectToServer()
{	
	m_Comms.Register("WINCH_DESIRED_DEPTH", 0);
	
	return true;
}

bool CiWinchSim::Iterate()
{
	
  double depth_diff = (desired_winch_depth-current_winch_depth);

  if(fabs(depth_diff) >= winch_tick)
    current_winch_depth = current_winch_depth + (Sign(depth_diff)*winch_tick);
    
  m_Comms.Notify("CTD_DEPTH",current_winch_depth);

  return true;
}

bool CiWinchSim::OnStartUp()
{
	return true;
}

int CiWinchSim::Sign(double val)
{
  if (val < 0.0)
    return (-1);
  else
    return (1);
}
