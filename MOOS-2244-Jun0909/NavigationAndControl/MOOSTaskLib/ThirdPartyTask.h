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
//   This file is part of a  MOOS Basic (Common) Application. 
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
// ThirdpartyTask.h: interface for the CThirdpartyTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THIRDPARTYTASK_H__02B87207_0D05_4EDE_BFB2_B7AB96FC5B70__INCLUDED_)
#define AFX_THIRDPARTYTASK_H__02B87207_0D05_4EDE_BFB2_B7AB96FC5B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSBehaviour.h"

class CThirdPartyTask : public CMOOSBehaviour  
{
public:
    CThirdPartyTask();
    virtual ~CThirdPartyTask();
      virtual bool SetParam(string sParam, string sVal);
            bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool RegularMailDelivery(double dfTimeNow);
    virtual bool GetNotifications(MOOSMSG_LIST & List);
    virtual bool Initialise();


protected:
    bool Enable(bool bEnable);
    bool OnStatusInstruction(string sInstruction, double dfTimeNow);
    bool OnActuationInstruction(string sInstruction,double dfTimeNow);
    bool OnNewInstruction(string sInstruction,double dfTimeNow);
    string m_sClient;
    string m_sJob;
    bool m_bInitialised;

    CMOOSVariable m_Rudder;
    CMOOSVariable m_Elevator;
    CMOOSVariable m_Thrust;

    bool m_bEnabled;

};

#endif // !defined(AFX_THIRDPARTYTASK_H__02B87207_0D05_4EDE_BFB2_B7AB96FC5B70__INCLUDED_)
