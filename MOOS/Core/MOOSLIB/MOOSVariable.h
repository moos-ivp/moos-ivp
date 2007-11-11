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
// MOOSVariable.h: interface for the CMOOSVariable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSVARIABLE_H__1217416E_1E6F_4435_B92D_14167642F4EF__INCLUDED_)
#define AFX_MOOSVARIABLE_H__1217416E_1E6F_4435_B92D_14167642F4EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSMsg.h"

#define DEFAULT_MOOS_VAR_COMMS_TIME 0.2

class CMOOSVariable  
{
public:
    std::string GetWriter();
    double GetAge(double dfTimeNow);
    std::string GetAsString(int nFieldWidth = 12);
    bool Set(const std::string & sVal,double dfTime);
    bool Set(double dfVal,double dfTime);

    std::string GetName();
    std::string GetStringVal();
    double GetTime();
    double GetDoubleVal();
    bool IsDouble();
    std::string GetPublishName();
    bool IsFresh();
    std::string GetSubscribeName();
    bool SetFresh(bool bFresh);
    bool Set(CMOOSMsg & Msg);
    double GetCommsTime(){return m_dfCommsTime;};
    CMOOSVariable();
    CMOOSVariable(std::string sName, std::string sSubscribe,std::string sPublish,double dfCommsTime = DEFAULT_MOOS_VAR_COMMS_TIME);
    virtual ~CMOOSVariable();


    


protected:
    double m_dfVal;
    std::string m_sVal;
    bool   m_bDouble;
    bool   m_bFresh; 
    double m_dfTimeWritten;
    double m_dfCommsTime; //time used when registering (how often should we receive updates?

    std::string m_sName;
    std::string m_sSrc;
    std::string m_sSubscribeName;
    std::string m_sPublishName;
};

#endif // !defined(AFX_MOOSVARIABLE_H__1217416E_1E6F_4435_B92D_14167642F4EF__INCLUDED_)
