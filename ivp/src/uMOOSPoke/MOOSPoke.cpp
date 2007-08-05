///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//	
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. 
//	  
//   This file is part of a  MOOS Instrument. 
//		
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//		  
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//			
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//////////////////////////    END_GPL    //////////////////////////////////
// MOOSPoke.cpp: implementation of the CMOOSPoke class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSPoke.h"
#include "version.h"

#include <time.h>
#include <iostream>

using namespace std;

#define DEFAULT_CSV_LOG_INTERVAL (3600) // seconds => 1 hour

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSPoke::CMOOSPoke()
{

}

CMOOSPoke::~CMOOSPoke()
{
 
}

bool CMOOSPoke::Iterate()
{

        double dfNow;

	dfNow=MOOSTime();
	
	if (dfNow - m_dfStartTime>0.5)
		exit(0);

	return true;
}


bool CMOOSPoke::OnStartUp()
{
	string sInterval; 
        string sAddress;
        CMOOSApp::OnStartUp();

	m_dfStartTime=MOOSTime();

        return true;
}



bool CMOOSPoke::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;
	STRING_LIST::iterator s;
	for(p = NewMail.begin();p!=NewMail.end();p++)
	{
		CMOOSMsg & rMsg = *p;
		// if the msg is new, and it's on the list
		if (!rMsg.IsSkewed(MOOSTime())){
                        
                }
	}
	return UpdateMOOSVariables(NewMail);
}

//
//  the body of the work, here
//
bool CMOOSPoke::OnConnectToServer()
{
    
	if (m_bValueIsDouble) 
		m_Comms.Notify(m_sVarName,m_dfValue,MOOSTime());
	else 
		m_Comms.Notify(m_sVarName,m_sValue,MOOSTime());

	return true;
}


bool CMOOSPoke::Initialise()
{    

        return true;
}

