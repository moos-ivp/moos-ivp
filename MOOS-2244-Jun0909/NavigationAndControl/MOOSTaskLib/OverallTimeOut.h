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
// OverallTimeOut.h: interface for the COverallTimeOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERALLTIMEOUT_H__1AE58995_2C61_44BA_B1E1_DC07DDAD8391__INCLUDED_)
#define AFX_OVERALLTIMEOUT_H__1AE58995_2C61_44BA_B1E1_DC07DDAD8391__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimerTask.h"

class COverallTimeOut : public CTimerTask  
{
public:
    COverallTimeOut();
    virtual ~COverallTimeOut();
    
    virtual bool    RegularMailDelivery(double dfTimeNow);
    bool    Run(CPathAction &DesiredAction);
    bool    GetNotifications(MOOSMSG_LIST & List);
    bool    SetParam(string sParam, string sVal);

protected:
    bool m_bInitialised;
    double m_dfTimeRemaining;
    virtual bool OnTimeOut();
    bool Initialise();
};

#endif // !defined(AFX_OVERALLTIMEOUT_H__1AE58995_2C61_44BA_B1E1_DC07DDAD8391__INCLUDED_)
