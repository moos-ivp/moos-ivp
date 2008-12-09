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
// MOOSNavLSQEngine.h: interface for the CMOOSNavLSQEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVLSQENGINE_H__310A3F56_1319_4C86_95BC_9986EB653113__INCLUDED_)
#define AFX_MOOSNAVLSQENGINE_H__310A3F56_1319_4C86_95BC_9986EB653113__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSNavEngine.h"

#define RANGES_FOR_PURE_LBL_FIX 3

//typedef map<int,OBSLIST> CHANNEL_OBSLIST_MAP;
typedef map<string,OBSLIST> SOURCE_OBSLIST_MAP;

class CMOOSNavLSQEngine : public CMOOSNavEngine  
{
public:
    enum LSQStatus
    {
        LSQ_NO_SOLUTION,
        LSQ_IN_PROGRESS,
        LSQ_SOLVED,
        LSQ_BIG_CHANGE
    };

    CMOOSNavLSQEngine();
    virtual ~CMOOSNavLSQEngine();
    virtual bool Iterate(double dfTimeNow);
    virtual bool AddData(const CMOOSMsg &Msg);
    virtual bool Initialise(STRING_LIST  sParams);

protected:
    bool PublishData();

    bool DoWStatistic();
    virtual bool OnSolved();
    bool IsReasonableMerit();
    void DoDebug();

    bool        IterateRequired(double dfTimeNow);
    LSQStatus    Solve();

    double    m_dfLSQWindow;
    int        m_nLSQIterations;
    bool    m_bTryToSolve;
    double  m_dfLastSolveAttempt;
    bool    m_bGuessed;
    
};

#endif // !defined(AFX_MOOSNAVLSQENGINE_H__310A3F56_1319_4C86_95BC_9986EB653113__INCLUDED_)
