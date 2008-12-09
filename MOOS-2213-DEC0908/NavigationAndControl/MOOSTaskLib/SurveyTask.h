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
// SurveyTask.h: interface for the CSurveyTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURVEYTASK_H__F4810393_BA34_45BA_A180_E4898A754D3A__INCLUDED_)
#define AFX_SURVEYTASK_H__F4810393_BA34_45BA_A180_E4898A754D3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <newmat.h>


using namespace NEWMAT;
using namespace std;

#include "XYPatternTask.h"
#include "TrackLineTask.h"    // Added by ClassView

#define XROW 1
#define YROW 2

class CSurveyTask : public CXYPatternTask  
{
public:
    virtual void SetTime(double dfTimeNow);
    CSurveyTask();
    virtual ~CSurveyTask();
    
    
    virtual bool SetParam(string sParam, string sVal);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool RegularMailDelivery(double dfTimeNow);
            bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool GetNotifications(MOOSMSG_LIST &List);


protected:
    virtual bool OnStart();
    double m_dfLead;
    double m_dfLegTimeOut;
    bool ActiveTracklineShouldRun();
    int m_nCurrentSurveyLine;
    CTrackLineTask m_ActiveTrackLine;
    
    int m_nTheta;
    int m_nNoLegs;
    int m_nNoArms;
    int m_nTotalSurveyLines;
    int m_nSpacing;
    bool Initialise();
    
    int m_nB;
    int m_nA;
    CXYPoint m_CenterOfSurvey;
private:
    bool SetNextLineInSurvey();
};

#endif // !defined(AFX_SURVEYTASK_H__F4810393_BA34_45BA_A180_E4898A754D3A__INCLUDED_)
