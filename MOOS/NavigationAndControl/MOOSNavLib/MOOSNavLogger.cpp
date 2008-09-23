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
// MOOSNavLogger.cpp: implementation of the CMOOSNavLogger class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <newmat.h>
using namespace NEWMAT;
#include <time.h>
#include "MOOSNavLibDefs.h"
#include "MOOSObservation.h"
#include "math.h"
#include <iomanip>
#include "MOOSNavLogger.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavLogger::CMOOSNavLogger()
{

}

CMOOSNavLogger::~CMOOSNavLogger()
{

}

bool CMOOSNavLogger::LogState(double dfTimeNow, Matrix &Xhat, Matrix &Phat)
{
    m_StateLogFile.setf(ios::left);
    int nStates = Xhat.Nrows();
    int nWidth = 14;



    if(m_StateLogFile.is_open())
    {
        //write time
        m_StateLogFile<<setw(20)<<setprecision(12)<<dfTimeNow;
        m_StateLogFile<<setprecision(5);
        int i =0;
        for(i = 1;i<=nStates;i++)
        {
            m_StateLogFile<<setw(nWidth)<<Xhat(i,1)<<" ";
        }
        
        for(i = 1;i<=nStates;i++)
        {
            m_StateLogFile<<setw(nWidth)<<sqrt(Phat(i,i))<<" ";
        }

        m_StateLogFile<<endl;

        return true;

    }
    else
    {
        return false;
    }
}

bool CMOOSNavLogger::Initialise(const string &sFileName,
                                const string &sPath,
                                bool bTimeStamp)
{
    m_sStateFileName = MakeFileName(sFileName,"xlog",sPath,bTimeStamp);
    m_sObsFileName = MakeFileName(sFileName,"olog",sPath,bTimeStamp);

    m_StateLogFile.open(m_sStateFileName.c_str());
    m_ObsLogFile.open(m_sObsFileName.c_str());

    m_StateLogFile<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    m_StateLogFile<<"%% FILTER STATE LOG FILE:       "<<m_sStateFileName.c_str()<<endl;
    m_StateLogFile<<"%% FILE OPENED ON               "<<MOOSGetDate().c_str();
    m_StateLogFile<<"%% LOGSTART                     "<<setw(20)<<setprecision(12)<<MOOSTime()<<endl;
    m_StateLogFile<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";

    m_ObsLogFile<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    m_ObsLogFile<<"%% FILTER OBS LOG FILE:       "<<m_sObsFileName.c_str()<<endl;
    m_ObsLogFile<<"%% FILE OPENED ON               "<<MOOSGetDate().c_str();
    m_ObsLogFile<<"%% LOGSTART                     "<<setw(20)<<setprecision(12)<<MOOSTime()<<endl;
    m_ObsLogFile<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    
    m_nObsLogged = 0;

    return m_StateLogFile.is_open() && m_ObsLogFile.is_open();
}


string CMOOSNavLogger::MakeFileName(string sStem,
                                    const string & sExtension,
                                    string sPath,
                                    bool bTimeStamp)
{
    struct tm *Now;
    time_t aclock;
    time( &aclock );                 
    Now = localtime( &aclock );  

    char sTmp[1000];

    if(bTimeStamp)
    {
        // Print local time as a string 

        //ODYSSEYLOG_14_5_1993_____9_30.log
        sprintf(sTmp, "%s%s_%d_%d_%d_____%.2d_%.2d.%s",
                sPath.c_str(),
                sStem.c_str(),
                Now->tm_mday,
                Now->tm_mon+1,
                Now->tm_year+1900,
                Now->tm_hour,
                Now->tm_min,
                sExtension.c_str());
    }
    else
    {
        sprintf(sTmp,"%s%s.%s",sPath.c_str(),sStem.c_str(),sExtension.c_str());
    }

     return string(sTmp);

}



bool CMOOSNavLogger::LogObservation(double dfTimeNow,
                                    CMOOSObservation &rObs,                              
                                    int nUpdate,
                                    bool bNaN)
{

    if(!m_ObsLogFile.is_open())
        return false;
    
    if(m_nObsLogged++==0)
    {
        m_ObsLogFile<<"%% 1. UpdateTime"<<endl;
        m_ObsLogFile<<"%% 2. Update Number"<<endl;
        m_ObsLogFile<<"%% 3. ObsTime"<<endl;
        m_ObsLogFile<<"%% 4. ObsID"<<endl;
        m_ObsLogFile<<"%% 5. Type"<<endl;
        m_ObsLogFile<<"%% 6. Data"<<endl;
        m_ObsLogFile<<"%% 7. LBL Chan"<<endl;
        m_ObsLogFile<<"%% 8. Innov"<<endl;
        m_ObsLogFile<<"%% 9. Innov Std"<<endl;
        m_ObsLogFile<<"%% 10.DA Good"<<endl;
        
        m_ObsLogFile<<"%% ObsTypes:"<<endl;
        m_ObsLogFile<<"%%     X=1"<<endl;
        m_ObsLogFile<<"%%     Y=2 "<<endl;
        m_ObsLogFile<<"%%     YAW=3 "<<endl;
        m_ObsLogFile<<"%%     LBL_BEACON_2WR=4 "<<endl;
        m_ObsLogFile<<"%%     DEPTH=5"<<endl;
        m_ObsLogFile<<"%%     ALTITUDE=6"<<endl;
        m_ObsLogFile<<"%%     SPEED=7"<<endl;
        m_ObsLogFile<<"%%     THRUST=8"<<endl;
        m_ObsLogFile<<"%%     BODY_VEL_Y=9"<<endl;
        m_ObsLogFile<<"%%     BODY_VEL_X=10"<<endl;
        m_ObsLogFile<<"%%     BODY_VEL_Z=11"<<endl;
        m_ObsLogFile<<"%%     TIDE=12"<<endl;
        m_ObsLogFile<<"%%     RUDDER=13"<<endl;
        m_ObsLogFile<<"%%     ELEVATOR=14"<<endl;

    }


    m_ObsLogFile.setf(ios::left);
    int nWidth = 14;
    if(m_ObsLogFile.is_open())
    {
        //write update time
        m_ObsLogFile<<setw(20)<<setprecision(12)<<dfTimeNow;

        //write the nth update
        m_ObsLogFile<<setw(10)<<nUpdate;

        //write observation valid time
        m_ObsLogFile<<setw(20)<<setprecision(12)<<rObs.m_dfTime;
        

        //write obs ID
        m_ObsLogFile<<setw(6)<<setprecision(5)<<rObs.GetID()<<" ";
        
        //write obs type
        m_ObsLogFile<<setw(4)<<rObs.m_eType<<" ";

        //write data
        if(bNaN)
        {
            m_ObsLogFile<<setw(15)<<"NaN"<<" ";
        }
        else
        {
            m_ObsLogFile<<setw(15)<<rObs.m_dfData<<" ";
        }

        //write channel (only really useful for LBL...
        m_ObsLogFile<<setw(4)<<rObs.m_nChan<<" ";

        //write innovation
        m_ObsLogFile<<setw(15)<<rObs.m_dfInnov<<" ";

        //write covariance
        double dfSii = 0;
        if(rObs.m_dfInnovStd<0)
        {
            m_ObsLogFile<<setw(15)<<"NaN"<<" ";
        }
        else
        {
            m_ObsLogFile<<setw(15)<<rObs.m_dfInnovStd<<" ";
        }


        //write DA success
        m_ObsLogFile<<setw(4)<<rObs.m_bGoodDA<<" ";

        m_ObsLogFile<<endl;


        return true;

    }
    else
    {
        return false;
    }

}

bool CMOOSNavLogger::Comment(const string &sComment, double dfTime)
{
    
    m_ObsLogFile<<"%%  ";
    m_ObsLogFile<<setw(20)<<setprecision(12)<<dfTime;
    m_ObsLogFile<<sComment.c_str()<<endl;

    m_StateLogFile<<"%%  ";
    m_StateLogFile<<setw(20)<<setprecision(12)<<dfTime;
    m_StateLogFile<<sComment.c_str()<<endl;
  
    return true;

}
