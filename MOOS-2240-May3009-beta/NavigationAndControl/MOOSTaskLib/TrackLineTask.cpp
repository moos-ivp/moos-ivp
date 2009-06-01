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
// TrackLineTask.cpp: implementation of the CTrackLineTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif
#include <math.h>
#include "TrackLineTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MINIMUM_LEAD_DISTANCE 1.0
CTrackLineTask::CTrackLineTask()
{
    m_dfLead = MINIMUM_LEAD_DISTANCE ;
    m_dfX1 = 0;
    m_dfY1 = 0;
    m_dfX2 = 0;
    m_dfY2 = 0;

    m_eMode = NOTSET;

}

CTrackLineTask::~CTrackLineTask()
{

}


//returns false if we haven't received data in a while..bad news!
bool CTrackLineTask::RegularMailDelivery(double dfTimeNow)
{
    
    return CGoToWayPoint::RegularMailDelivery(dfTimeNow);
    
}


bool CTrackLineTask::Run(CPathAction &DesiredAction)
{
    if(CalculateLocalGoal())
    {
        return CGoToWayPoint::Run(DesiredAction);
    }
    return false;
}

bool CTrackLineTask::OnNewMail(MOOSMSG_LIST &NewMail)
{
    return CGoToWayPoint::OnNewMail(NewMail);
}

bool CTrackLineTask::GetRegistrations(STRING_LIST &List)
{

    //always call base class version
    return CGoToWayPoint::GetRegistrations(List);
}

bool CTrackLineTask::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);



    //this is for us...
    if(MOOSStrCmp(sParam,"P1"))
    {
        string sTmpX = MOOSChomp(sVal,",");
        string sTmpY = MOOSChomp(sVal,",");
        string sTmpZ = MOOSChomp(sVal,",");

        m_dfX1 =   atof(sTmpX.c_str());
        m_dfY1 =   atof(sTmpY.c_str());
    }
    else if(MOOSStrCmp(sParam,"P2"))
    {

        string sTmpX = MOOSChomp(sVal,",");
        string sTmpY = MOOSChomp(sVal,",");
        string sTmpZ = MOOSChomp(sVal,",");

        m_dfX2 =   atof(sTmpX.c_str());
        m_XDOF.SetDesired(m_dfX2);

        m_dfY2 =   atof(sTmpY.c_str());
        m_YDOF.SetDesired(m_dfY2);

    }
    else if(MOOSStrCmp(sParam,"LEAD"))
    {
        //useful for later...
        m_dfLead = atof(sVal.c_str());
        if(m_dfLead<MINIMUM_LEAD_DISTANCE)
            m_dfLead = MINIMUM_LEAD_DISTANCE;

    }
    else  if(!CGoToWayPoint::SetParam(sParam,sVal))
    {
        //hmmm - it wasn't for us at all: base class didn't understand either
        MOOSTrace("Param \"%s\" not understood!\n",sParam.c_str());
        return false;
    }


    return true;

}

bool CTrackLineTask::CalculateLocalGoal()
{

    return CalculateLocalGoalV2();

    double dfx1 = m_dfX1;
    double dfx2 = m_dfX2;
    double dfy1 = m_dfY1;
    double dfy2 = m_dfY2;
    double dfx12 = m_dfX2-m_dfX1;
    double dfy12 = m_dfY2-m_dfY1;
    double dfS12 = sqrt((dfx12*dfx12+dfy12*dfy12));

    double dfxv = m_XDOF.GetCurrent();
    double dfyv = m_YDOF.GetCurrent();

    //we now solv to find the coordinates of the point
    //on or tack line that is closest to nour current position
    //solving for xn,yn, lambda where 0<lambda<1 where lamda is
    //a scalar realting Xn and P1 Xn = X1+Lambda(X2-X1)
    Matrix A(3,3);
    A   <<dfx12 <<dfy12 <<  0.0
        <<1.0   <<0.0   << -dfx12
        <<0.0   <<1.0   << -dfy12;

    Matrix d(3,1);

    d<< dfxv*dfx12 + dfy12*dfyv<< dfx1<< dfy1;
    Matrix Sol;
    try
    {
        Sol = A.i()*d;
    }
    catch(...)
    {
        Sol.Release();
        MOOSTrace("failed inversion in TrackLinetask!");
        Stop("Failed Matrix Inversion");
        return false;
    }

    double dfLambda = Sol(3,1);

    double dfLead = m_dfLead;

    //are we outside the line?
    if(dfLambda<0) 
    {
        //head for first ppint
        Sol(1,1) = dfx1;
        Sol(2,1) = dfy1;
        Sol(3,1) = 0; 
        dfLead = 0;
    }

    if(dfLambda>=1)
    {
        //head for second point..
        Sol(1,1) = dfx2;
        Sol(2,1) = dfy2;
        Sol(3,1) = 0; 
        dfLead = 0;
    }

    if(dfS12*(1.0-dfLambda)<dfLead)
    {
        //don;t lead past end of line!
        Sol(1,1) = dfx2;
        Sol(2,1) = dfy2;
        Sol(3,1) = 0; 
        dfLead = 0;
    }


    //now lead us along the line... 
    double dfTargetX = Sol(1,1)+dfLead*dfx12/dfS12;
    double dfTargetY = Sol(2,1)+dfLead*dfy12/dfS12;

    m_XDOF.SetDesired(dfTargetX);
    m_YDOF.SetDesired(dfTargetY);


    return true;
}

double CTrackLineTask::GetDistanceToGo()
{
    return sqrt(pow(m_dfX2-m_XDOF.GetCurrent(),2)+pow(m_dfY2-m_YDOF.GetCurrent(),2));
}


bool CTrackLineTask::CalculateLocalGoalV2()
{
    double dfx1 = m_dfX1;
    double dfx2 = m_dfX2;
    double dfy1 = m_dfY1;
    double dfy2 = m_dfY2;
    double dfx12 = m_dfX2-m_dfX1;
    double dfy12 = m_dfY2-m_dfY1;
    double dfS12 = sqrt((dfx12*dfx12+dfy12*dfy12));

    double dfxv = m_XDOF.GetCurrent();
    double dfyv = m_YDOF.GetCurrent();


    double dfTheta = atan2(dfy2-dfyv,dfx2-dfxv);

    double dfLineTheta = atan2(dfy12,dfx12);    

    double dfRangeToGoal = sqrt(pow(dfy2-dfyv,2)+pow(dfx2-dfxv,2));

    double dfxe = dfRangeToGoal*cos(dfTheta-dfLineTheta);

    double dfTheHenrikRatio  = 0.5;


    
    double dfxp = dfx2-dfxe*cos(dfLineTheta);
    double dfyp = dfy2-dfxe*sin(dfLineTheta);


    double dfXGoal = dfx2;
    double dfYGoal = dfy2;


    if(dfTheHenrikRatio*dfxe>m_dfLead)
    {   
        //PMN MODE n(v clever)

        SetMode(CTrackLineTask::TRANSIT);

        dfxe = m_dfLead;
        dfXGoal = dfxp+m_dfLead*cos(dfLineTheta);
        dfYGoal = dfyp+m_dfLead*sin(dfLineTheta);

    }
    else
    {
        //HS Method (moderately clever ;-) )
        SetMode(CTrackLineTask::APPROACH);
        dfXGoal = (1-dfTheHenrikRatio)*dfxp+dfTheHenrikRatio*dfx2;
        dfYGoal = (1-dfTheHenrikRatio)*dfyp+dfTheHenrikRatio*dfy2;
    }
        

    m_XDOF.SetDesired(dfXGoal);
    m_YDOF.SetDesired(dfYGoal);


    return true;

}

bool CTrackLineTask::SetMode(CTrackLineTask::Mode eMode)
{
    if(m_eMode!=eMode)
    {
        //state change
        switch(eMode)
        {
        case APPROACH: MOOSTrace("switching to APPROACH mode\n"); break;
        case TRANSIT: MOOSTrace("switching to TRANSIT mode\n"); break;
        default:
            break;
        }
    }

    m_eMode = eMode;

    return true;
}
