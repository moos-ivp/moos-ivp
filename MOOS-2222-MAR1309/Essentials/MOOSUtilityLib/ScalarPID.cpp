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
// ScalarPID.cpp: implementation of the CScalarPID class.
//
//////////////////////////////////////////////////////////////////////

#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include "ScalarPID.h"
#include <math.h>
#include <iostream>
#include <iomanip>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScalarPID::CScalarPID()
{
    m_dfKp  =   0;
    m_dfKd  =   0;
    m_dfKi  =   0;
    m_dfe   =   0;
    m_dfeSum =  0;
    m_dfeOld =  0;
    m_dfeDiff = 0;
    m_dfDT =    0;
    m_dfOldTime =0;
    m_dfOut =   0;
    m_nIterations=0;
    m_dfIntegralLimit = 0;
    m_dfOutputLimit   = 0;

    m_nHistorySize = 10;
    m_dfGoal = 0;


}


CScalarPID::CScalarPID(double dfKp,
                       double dfKd,
                       double dfKi,
                       double dfIntegralLimit,
                       double dfOutputLimit)
{
    m_dfKp      =   dfKp;
    m_dfKd      =   dfKd;
    m_dfKi      =   dfKi;
    m_dfeSum    =   0;
    m_dfeOld    =   0;
    m_dfeDiff   =   0;
    m_dfDT      =   0;
    m_dfOldTime =  0;
    m_dfe       =   0;
    m_dfOut     =   0;

    m_dfIntegralLimit = dfIntegralLimit;
    m_dfOutputLimit   = dfOutputLimit;

    m_nIterations=  0;


}

CScalarPID::~CScalarPID()
{

}

bool CScalarPID::Run(double dfeIn, double dfErrorTime,double &dfOut)
{
    m_dfe  = dfeIn;
    
    //figure out time increment...
    if(m_nIterations++!=0)
    {
        
        m_dfDT = dfErrorTime-m_dfOldTime;

        if(m_dfDT<0)
        {
            MOOSTrace("CScalarPID::Run() : negative or zero sample time\n");
            return false;
        }
        else if(m_dfDT ==0)
        {
            //nothing to do...
            dfOut = m_dfOut;
            Log();
            return true;
        }

        //figure out differntial
        double dfDiffNow = (dfeIn-m_dfeOld)/m_dfDT;
        m_DiffHistory.push_front(dfDiffNow);
        while(m_DiffHistory.size()>=m_nHistorySize)
        {
            m_DiffHistory.pop_back();
        }
        
        m_dfeDiff = 0;
        list<double>::iterator p;
        for(p = m_DiffHistory.begin();p!=m_DiffHistory.end();p++)
        {
            m_dfeDiff   += *p;   
        }
        m_dfeDiff/=m_DiffHistory.size();
    }
    else
    {
        //this is our first time through
        m_dfeDiff = 0;
    }


    if(m_dfKi>0)
    {
        //calculate integral term  
        m_dfeSum    +=  m_dfKi*m_dfe*m_dfDT;

        //prevent integral wind up...
        if(fabs(m_dfeSum)>=fabs(m_dfIntegralLimit))
        {


            int nSign = (int)(fabs(m_dfeSum)/m_dfeSum);
            m_dfeSum = nSign*fabs(m_dfIntegralLimit);

        
        }
    }
    else
    {
        m_dfeSum = 0;
    }
    //do pid control
    m_dfOut = m_dfKp*m_dfe+
              m_dfKd*m_dfeDiff+
              m_dfeSum; //note Ki is already in dfeSum


    //prevent saturation..
    if(fabs(m_dfOut)>=fabs(m_dfOutputLimit) )
    {        
        int nSign =(int)( fabs(m_dfOut)/m_dfOut);
        m_dfOut = nSign*fabs(m_dfOutputLimit);
    }

    //save old value..
    m_dfeOld    = m_dfe;
    m_dfOldTime = dfErrorTime;


    dfOut = m_dfOut;

    //do logging..
    Log();

    return true;
}

void CScalarPID::SetGains(double dfKp, double dfKd, double dfKi)
{
    m_dfKp      =   dfKp;
    m_dfKd      =   dfKd;
    m_dfKi      =   dfKi;
}

void CScalarPID::SetLimits(double dfIntegralLimit, double dfOutputLimit)
{
   m_dfIntegralLimit = dfIntegralLimit;
   m_dfOutputLimit   = dfOutputLimit;
}

bool CScalarPID::SetName(string sName)
{
    m_sName = sName;
    return true;
}

bool CScalarPID::SetLog(bool bLog)
{
    m_bLog = bLog;
    return true;
}

bool CScalarPID::Log()
{

    int nWidth = 17;
    
    if(m_bLog)
    {
        if(!m_LogFile.is_open())
        {
            string sName = MOOSFormat("%s%s%s.pid",
                m_sLogPath.c_str(),
                m_sName.c_str(),
                MOOSGetTimeStampString().c_str());
            m_LogFile.open(sName.c_str());
            if(!m_LogFile.is_open())
            {
                m_bLog = false;
                return false;
            }

            m_LogFile.setf(ios::left);

            m_LogFile<<"%% Kp = "<<m_dfKp<<endl;
            m_LogFile<<"%% Kd = "<<m_dfKd<<endl;
            m_LogFile<<"%% Ki = "<<m_dfKi<<endl;
            m_LogFile<<setw(20)<<"%T";
            m_LogFile<<setw(nWidth)<<"Kp";
            m_LogFile<<setw(nWidth)<<"Kd";
            m_LogFile<<setw(nWidth)<<"Ki";
            m_LogFile<<setw(nWidth)<<"DT";
            m_LogFile<<setw(nWidth)<<"Output";
            m_LogFile<<setw(nWidth)<<"InputError";
            m_LogFile<<setw(nWidth)<<"Kp*e";
            m_LogFile<<setw(nWidth)<<"Kd*de/dt";
            m_LogFile<<setw(nWidth)<<"Ki*int(e)";
            m_LogFile<<setw(nWidth)<<"Desired";
            m_LogFile<<setw(nWidth)<<"Actual"<<endl;
            
        }

           //do pid control
        //    m_dfOut = m_dfKp*m_dfe+
        //    m_dfKd*m_dfeDiff+
        //   m_dfKi*m_dfeSum;
        m_LogFile.setf(ios::left);
        m_LogFile<<setw(20)<<setprecision(12)<<m_dfOldTime<<' ';
        m_LogFile<<setprecision(5);
        m_LogFile<<setw(nWidth)<<m_dfKp<<' ';
        m_LogFile<<setw(nWidth)<<m_dfKd<<' ';
        m_LogFile<<setw(nWidth)<<m_dfKi<<' ';
        m_LogFile<<setw(nWidth)<<m_dfDT<<' ';
        m_LogFile<<setw(nWidth)<<m_dfOut<<' ';
        m_LogFile<<setw(nWidth)<<m_dfe<<' ';
        m_LogFile<<setw(nWidth)<<m_dfKp*m_dfe<<' ';
        m_LogFile<<setw(nWidth)<<m_dfKd*m_dfeDiff<<' ';
        m_LogFile<<setw(nWidth)<<m_dfeSum<<' '; //Ki is already in dfeSum
        m_LogFile<<setw(nWidth)<<m_dfGoal<<' '; 
        m_LogFile<<setw(nWidth)<<m_dfGoal-m_dfe<<' '; 

        m_LogFile<<endl;

    }

    m_LogFile.flush();

    return true;
}

bool CScalarPID::SetLogPath(string &sPath)
{
    m_sLogPath = sPath;
    return true;
}

bool CScalarPID::SetGoal(double dfGoal)
{
    m_dfGoal =dfGoal;
    return true;
}
