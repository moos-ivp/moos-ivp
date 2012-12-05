///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Core Component. 
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
    bool Expired(double dfTimeNow);
    double m_dfPeriod;
    string m_sClientName;
    double m_dfLastTimeSent;

    CMOOSRegisterInfo();
    virtual ~CMOOSRegisterInfo();


};

#endif // !defined(AFX_MOOSREGISTERINFO_H__9428D0A3_78A7_4932_A3B6_F593D8BCBDC1__INCLUDED_)
