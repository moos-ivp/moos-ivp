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
//   This file is part of a  MOOS Utility Component. 
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

// AcousticSignal.cpp: implementation of the CAcousticSignal class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif
#include <math.h>

#define DEFAULT_SIGNAL_RANGE 4000.0
#define DEFAULT_SV 1498.0

#include "SimGlobalHelper.h"

#include "AcousticSignal.h"


using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAcousticSignal::CAcousticSignal()
{
    m_SrcPos.ReSize(3,1);
    m_SrcPos = 0;
    m_dfSV = DEFAULT_SV;
    m_dfStartTime = 0;

    m_dfMaximumRange = DEFAULT_SIGNAL_RANGE;
}

CAcousticSignal::CAcousticSignal(double dfX,double dfY,double dfZ,double dfStartTime)
{
    m_SrcPos.ReSize(3,1);
    m_SrcPos<<dfX<<dfY<<dfZ;
    m_dfSV = DEFAULT_SV;
    m_dfStartTime =dfStartTime;
    m_dfMaximumRange = DEFAULT_SIGNAL_RANGE;

}


CAcousticSignal::~CAcousticSignal()
{
    
}


double CAcousticSignal::GetExpectedIntersectionTime(Matrix & Location)
{
    Matrix dP = (m_SrcPos-Location.SubMatrix(1,3,1,1));
 //   MOOSTraceMatrix(m_SrcPos,"Src");
 //   MOOSTraceMatrix(Location,"Location");


    return m_dfStartTime+sqrt(dP.SumSquare())/m_dfSV;
}

double CAcousticSignal::Age(double dfTimeNow)
{
    return dfTimeNow-m_dfStartTime;
}

bool CAcousticSignal::HasDecayed(double dfTimeNow)
{
    return (dfTimeNow-m_dfStartTime)*m_dfSV>m_dfMaximumRange;
}

bool CAcousticSignal::SetChannel(AcousticChannel eChannel)
{
    m_eChannel = eChannel;

    return true;
}

AcousticChannel CAcousticSignal::GetChannel()
{
    return m_eChannel;
}

double CAcousticSignal::GetStartTime()
{
    return m_dfStartTime;
}


/// returns enum of channel or ACOUSTIC_CHAN_ERROR if string cannot be passed
AcousticChannel CAcousticSignal::ChannelFromString(string &sStr)
{
    string sTmp = sStr;
    MOOSToUpper(sTmp);
    MOOSChomp(sTmp,"CH");
    if(sTmp.empty())
    {
        //must be something like IIF etc
        string sTmp = sStr;
        MOOSToUpper(sTmp);
        if(sTmp.find("IIF")!=string::npos)
        {
            return ACOUSTIC_CHAN_IIF;
        }
        else if(sTmp.find("CIF")!=string::npos)
        {
            return ACOUSTIC_CHAN_CIF;
        }
        else if(sTmp.find("IRF")!=string::npos)
        {
            return ACOUSTIC_CHAN_IRF;
        }
        else if(sTmp.find("CRF")!=string::npos)
        {
            return ACOUSTIC_CHAN_CRF;
        }
        else
        {
            MOOSTrace("Unknown channel!\n");
            return ACOUSTIC_CHAN_ERROR;
        }


    }
    else
    {
        int nChan = atoi(sTmp.c_str());
        return (AcousticChannel)nChan;

    }

    return ACOUSTIC_CHAN_ERROR;
}

bool CAcousticSignal::SetSrcName(const string &sSrcName)
{
    m_sSrcName = sSrcName;

    return true;
}

string CAcousticSignal::GetSrcName()
{
    return m_sSrcName;
}
