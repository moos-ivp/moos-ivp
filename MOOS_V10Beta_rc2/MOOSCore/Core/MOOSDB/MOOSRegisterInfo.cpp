///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
// MOOSRegisterInfo.cpp: implementation of the CMOOSRegisterInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSRegisterInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSRegisterInfo::CMOOSRegisterInfo()
{
    m_dfLastTimeSent = 0;
    m_dfPeriod = 0.5;
}

CMOOSRegisterInfo::~CMOOSRegisterInfo()
{

}


bool CMOOSRegisterInfo::Expired(double dfTimeNow)
{
    if(m_dfPeriod==0.0)
        return true;

    return dfTimeNow-m_dfLastTimeSent>=m_dfPeriod ;
}

void CMOOSRegisterInfo::SetLastTimeSent(double dfTimeSent)
{
    m_dfLastTimeSent = dfTimeSent;
}
