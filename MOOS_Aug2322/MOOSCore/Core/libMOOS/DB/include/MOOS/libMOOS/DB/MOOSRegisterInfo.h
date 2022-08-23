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
//   are made available under the terms of the GNU Public License
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/gpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
// MOOSRegisterInfo.h: interface for the CMOOSRegisterInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSREGISTERINFO_H__9428D0A3_78A7_4932_A3B6_F593D8BCBDC1__INCLUDED_)
#define AFX_MOOSREGISTERINFO_H__9428D0A3_78A7_4932_A3B6_F593D8BCBDC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

class CMOOSRegisterInfo  
{
public:
    void SetLastTimeSent(double dfTimeSent);
    double GetLastTimeSent();

    bool Expired(double dfTimeNow);
    double m_dfPeriod;
    string m_sClientName;
    double m_dfLastTimeSent;

    CMOOSRegisterInfo();
    virtual ~CMOOSRegisterInfo();


};

#endif // !defined(AFX_MOOSREGISTERINFO_H__9428D0A3_78A7_4932_A3B6_F593D8BCBDC1__INCLUDED_)
