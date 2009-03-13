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
// SurveyTask.cpp: implementation of the CSurveyTask class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif
#include <math.h>
#include <sstream>
#include <iostream>
using namespace std;

#include "SurveyTask.h"
#include "XYPatternTask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSurveyTask::CSurveyTask()
{
    m_nNoLegs = 0;
    m_nNoArms = 0;
    m_nTotalSurveyLines = 0;
    m_nCurrentSurveyLine = 0;
    m_nSpacing  = 0;
    m_nTheta = 0;

    m_nA = 0;
    m_nB = 0;

    m_CenterOfSurvey.SetX(0.0);
    m_CenterOfSurvey.SetY(0.0);

    m_nPriority = 3;
    m_bInitialised = false;
    m_dfVicinityRadius = 5;
    m_dfThrust = 0;
    m_dfLead = 1;
    m_dfLegTimeOut = 100.0;

}

CSurveyTask::~CSurveyTask()
{

}


bool CSurveyTask::Initialise()
{
    //Define the survey area
    m_nNoArms = m_nB / m_nSpacing;
    m_nNoLegs = m_nNoArms + 1;
    m_nTotalSurveyLines = m_nNoArms + m_nNoLegs;

    double dfRad = m_nTheta * PI/180;

    
    //XXX: using the center of the survey is not advised for trackline creation, 
    //XXX: as rotation does not occur properly if translation is done first
    //XXX: different strategy:
    //XXX: 1. Make Survey around origin
    //XXX: 2. Rotate Coordinates of Survey points
    //XXX: 3. Translate Coordinates to the Survey's center
    
    //start in the lower left corner of the box
    //XXX:11/19/02 - fixed survey problem of not being able to use
    //XXX:A and B values of different magnitudes
    double dfXStart = -(m_nB / 2);
    double dfYStart = -(m_nA / 2);

    CXYPoint StartPoint;
    StartPoint.SetX(dfXStart);
    StartPoint.SetY(dfYStart);
    
    //make sure we have enough room
    m_XYPoints.resize(m_nTotalSurveyLines + 2);
    m_XYPoints[0] = StartPoint;
        
    int nM = -1;
    int kCnt = 1;

    while(kCnt <= m_nTotalSurveyLines)
    {
        for(int j = 0; j < 2; j++)
        {
            if(j == 0)
            {
                //make the Point for the Leg
                CXYPoint NextPoint;
                nM *= -1;
                double dfY = m_XYPoints[kCnt - 1].GetY() + nM * m_nA; 
                NextPoint.SetY(dfY);
                NextPoint.SetX(m_XYPoints[kCnt - 1].GetX());

                m_XYPoints[kCnt++] = NextPoint;
            }
            else
            {
                //make the Point for the Arm
                CXYPoint NextPoint;
                double dfX = m_XYPoints[kCnt - 1].GetX() + m_nSpacing;
                NextPoint.SetX(dfX);
                NextPoint.SetY(m_XYPoints[kCnt - 1].GetY());

                m_XYPoints[kCnt++] = NextPoint;
            }
        }
    }

    //Turn the Survey points into a matrix, 
    //this allows for easy rotation 
    Matrix GridPoints(2, m_nTotalSurveyLines + 2);
    
    for(int i = 0;i < GridPoints.Ncols();i++)
    {
        GridPoints(XROW,i + 1)    = m_XYPoints[i].GetX();
        GridPoints(YROW,i + 1)    = m_XYPoints[i].GetY();
    }

    //now rotate the points if need be
    if(dfRad != 0)
    {
        //This matrix will rotate a box CW, i.e. in negative yaw (positive heading)
        Matrix Rotator(2,2);
        Rotator << cos(dfRad) << sin(dfRad)
                << -sin(dfRad) << cos(dfRad);
      
        //Rotate the Survey
        GridPoints = Rotator * GridPoints;
    }

    //XXX: Translate the Grid to the actual Survey's center
    //
    //now go thru the GridPoints and make that the actual 
    //points that we will survey with
    string sPoints;
    string sTrackline;
    int tCnt = 0;
    for(int j = 0; j < GridPoints.Ncols(); j++)
    {
        //point creation
        m_XYPoints[j].SetX(GridPoints(XROW, j + 1) + m_CenterOfSurvey.GetX());
        m_XYPoints[j].SetY(GridPoints(YROW, j + 1) + m_CenterOfSurvey.GetY());
        
        //debug information
        sTrackline += MOOSFormat("%f", m_XYPoints[j].GetX()); 
        sTrackline += ",";
        sTrackline    += MOOSFormat("%f", m_XYPoints[j].GetY()); 

        //announcement to SURVEY_POINTS
        sPoints += MOOSFormat("%f", m_XYPoints[j].GetX());
        sPoints += ",";
        sPoints        += MOOSFormat("%f", m_XYPoints[j].GetY());

        if((j % 2) == 0)
        {
            sPoints        += "->";
            sTrackline    += "->";
        }
        else
        {
            DebugNotify(MOOSFormat("Trackline %d: %s\n", ++tCnt, sTrackline.c_str()));
            sPoints += "|";

            //reset the debug information
            sTrackline = "";
        }
    }

    //announce what we have done by adding a list of points to our notifications
    CMOOSMsg Msg(MOOS_NOTIFY, "SURVEY_POINTS", sPoints.c_str());
    m_Notifications.push_back(Msg);

    //create the first trackline
    SetNextLineInSurvey();

    m_bInitialised = true;

    return m_bInitialised;
}

//returns false if we haven't received data in a while..bad news!
bool CSurveyTask::RegularMailDelivery(double dfTimeNow)
{
 
    return m_ActiveTrackLine.RegularMailDelivery(dfTimeNow);
    
}


bool CSurveyTask::Run(CPathAction &DesiredAction)
{

    if(!m_bInitialised)
    {
        Initialise();
    }

    if(ShouldRun())
    {
    
        if(m_nCurrentSurveyLine < m_nTotalSurveyLines)
        {
            if(ActiveTracklineShouldRun())
                return m_ActiveTrackLine.Run(DesiredAction);
            else
                return SetNextLineInSurvey();
        }
        else
        {
            OnComplete();
        }
    
    }
    return true;
}

bool CSurveyTask::OnNewMail(MOOSMSG_LIST &NewMail)
{

    CMOOSBehaviour::OnNewMail(NewMail);
    //make sure we pass this information on to the active trackline
    //MOOSTrace("Survey - new mail %f\n", MOOSTime());
    if(ActiveTracklineShouldRun())
    {
    //    MOOSTrace("Survey - new mail - passing %f\n", MOOSTime());
        m_ActiveTrackLine.OnNewMail(NewMail);
    }
    return true;
}

bool CSurveyTask::GetRegistrations(STRING_LIST &List)
{
    CMOOSBehaviour::GetRegistrations(List);
    return m_ActiveTrackLine.GetRegistrations(List);
}

bool CSurveyTask::SetParam(string sParam, string sVal)
{

    MOOSToUpper(sParam);
    MOOSToUpper(sVal);


    //this is for us...
    if(MOOSStrCmp(sParam,"CENTER"))
    {
        string sTmpX = MOOSChomp(sVal,",");
        string sTmpY = MOOSChomp(sVal,",");
        
        m_CenterOfSurvey.SetX(atof(sTmpX.c_str()));
        m_CenterOfSurvey.SetY(atof(sTmpY.c_str()));
    }
    else if(MOOSStrCmp(sParam,"A"))
    {
        m_nA = atoi(sVal.c_str());        
    }
    else if(MOOSStrCmp(sParam,"B"))
    {
        m_nB = atoi(sVal.c_str());        
    }
    else if(MOOSStrCmp(sParam,"LEGTIMEOUT"))
    {
        m_dfLegTimeOut = atof(sVal.c_str());        
    }
    else if(MOOSStrCmp(sParam,"SPACING"))
    {
        m_nSpacing = atoi(sVal.c_str());        
    }
    else if(MOOSStrCmp(sParam,"ROTATION"))
    {
        m_nTheta = atoi(sVal.c_str());        
    }
       else if(MOOSStrCmp(sParam,"LEAD"))
    {
        m_dfLead = atof(sVal.c_str());        
    }
    else  if(!CXYPatternTask::SetParam(sParam,sVal))
    {
        //hmmm - it wasn't for us at all: base class didn't understand either
        MOOSTrace("Param \"%s\" not understood!\n",sParam.c_str());
        return false;
    }


    return true;

}



bool CSurveyTask::SetNextLineInSurvey()
{

    string sP1, sP2, sRadius, sThrust, sLead,sLegTimeOut;
    string sTracklineName = m_sName; 

    if(!m_bInitialised)
    {
        
        //always start by heading to the first position around the center
        m_nCurrentSurveyLine = 0;
        sTracklineName += MOOSFormat("_TRACKLINE_%d",(m_nCurrentSurveyLine + 1));
        m_ActiveTrackLine.SetName(sTracklineName);
        m_ActiveTrackLine.SetGains(m_Gains);

    }
    else
    {
        string sStatus = MOOSFormat("Survey Trackline %d Complete", (m_nCurrentSurveyLine + 1));
        //do notification of completion here for other tasks to fire off of
        string sFlag = MOOSFormat("SURVEY_TRACKLINE_%d_DONE", (m_nCurrentSurveyLine + 1));
        CMOOSMsg DoneMsg(MOOS_NOTIFY, sFlag.c_str(), "");

        //augment our current point we are heading to
        m_nCurrentSurveyLine++;
        sTracklineName += MOOSFormat("_TRACKLINE_%d",(m_nCurrentSurveyLine + 1));
        
        //also provide some intelligible info
        CMOOSMsg Msg(MOOS_NOTIFY, "SURVEY_STATUS", sStatus.c_str());
        m_Notifications.push_back(Msg);
        m_Notifications.push_back(DoneMsg);

        //reinitializing a trackline does not affect the Gains
        m_ActiveTrackLine.ReInitialise();
        m_ActiveTrackLine.SetName(sTracklineName);
    }
    
    //format the variables
    sP1 = MOOSFormat("%f,%f",
                m_XYPoints[m_nCurrentSurveyLine].GetX(),
                m_XYPoints[m_nCurrentSurveyLine].GetY());

    sP2 = MOOSFormat("%f,%f",    
                m_XYPoints[m_nCurrentSurveyLine + 1].GetX(),
                m_XYPoints[m_nCurrentSurveyLine + 1].GetY());

    sRadius = MOOSFormat("%f", m_dfVicinityRadius);
    sLead    = MOOSFormat("%f", m_dfLead);
    sThrust = MOOSFormat("%f", m_dfThrust);
    sLegTimeOut =  MOOSFormat("%f", m_dfLegTimeOut);

    m_ActiveTrackLine.SetParam("P1", sP1);
    m_ActiveTrackLine.SetParam("P2", sP2);
    m_ActiveTrackLine.SetParam("LEAD", sLead);
    m_ActiveTrackLine.SetParam("RADIUS", sRadius);
    m_ActiveTrackLine.SetParam("THRUST", sThrust);
    m_ActiveTrackLine.SetParam("TimeOut", sLegTimeOut);
    //always make the initialstate ON
    m_ActiveTrackLine.SetParam("INITIALSTATE", "ON");

    //announce where we are going to
    //XXX: need to add an end flag that we are DONE on the SURVEYNAME_TRACKLINE_NO
    ostringstream os;

    os<<"SURVEY - transiting to: pos["<<(m_nCurrentSurveyLine + 1)<<"]"<<
        " -> "<<m_XYPoints[m_nCurrentSurveyLine + 1].GetX()<<","<<
        m_XYPoints[m_nCurrentSurveyLine + 1].GetY()<<endl<<ends;

    string sInfo = os.str();
    //os.rdbuf()->freeze(0);
    DebugNotify(sInfo);
    CMOOSMsg Msg(MOOS_NOTIFY,"SURVEY_INFO",sInfo.c_str());
    m_Notifications.push_back(Msg);

    m_ActiveTrackLine.Start();

    return true;
}

bool CSurveyTask::ActiveTracklineShouldRun()
{
    return m_ActiveTrackLine.ShouldRun();
}

void CSurveyTask::SetTime(double dfTimeNow)
{
    m_dfIterateTime = dfTimeNow;
    //and pass this time along to our trackline
//    if(ActiveTracklineShouldRun())
        m_ActiveTrackLine.SetTime(dfTimeNow);
}

bool CSurveyTask::OnStart()
{
    //dummy call starts owned class..
    m_ActiveTrackLine.Start();
    return true;
}

bool CSurveyTask::GetNotifications(MOOSMSG_LIST &List)
{
    //for our child...
    m_ActiveTrackLine.GetNotifications(List);

    //for ourselves
    List.splice(List.begin(),m_Notifications);

    return true;
}



