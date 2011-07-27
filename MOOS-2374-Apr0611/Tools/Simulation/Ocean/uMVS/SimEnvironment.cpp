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

// SimEnvironment.cpp: implementation of the CSimEnvironment class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>

#include "SimEnvironment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimEnvironment::CSimEnvironment()
{
    m_dfTideHeight = 30;
    m_dfMagneticOffset = 0.0;

}

CSimEnvironment::~CSimEnvironment()
{

}

double  CSimEnvironment::GetAltitude(double dfX,double dfY,double dfZ)
{
    double dfTmp = m_Terrain.GetAltitude(    dfX,
                                            dfY,
                                            dfZ);

    return dfTmp;
    
}

double CSimEnvironment::GetDepth(double dfZ)
{
    return m_dfTideHeight-dfZ;
}

double CSimEnvironment::GetTideHeight()
{
    return m_dfTideHeight;
}


bool CSimEnvironment::Initialise(const char *sTerrainFile)
{
    return m_Terrain.Load(sTerrainFile);
}

bool CSimEnvironment::RemoveOldSignals(double dfTimeNow)
{
    //remove old acoustic signals
    ACOUSTIC_SIGNAL_LIST::iterator q,t;
    for(q = m_AcousticSignals.begin();q!=m_AcousticSignals.end();q++)
    {
        CAcousticSignal & rSignal = *q;
        if(rSignal.HasDecayed(dfTimeNow))
        {
            t=q;
            t++;
            m_AcousticSignals.erase(q);
            q=t;

//            MOOSTrace("removing decayed signal\n");
        }        
    }

    return true;
}

bool CSimEnvironment::AddSignal(CAcousticSignal NewSignal)
{
  //  MOOSTrace("Adding Signal!\n");
    m_AcousticSignals.push_back(NewSignal);

#ifdef SIM_ENV_VERBOSE
    MOOSTrace("Add Signal - Acoustic Signals are now [%d]:\n",m_AcousticSignals.size());
    ACOUSTIC_SIGNAL_LIST::iterator p;
    for(p = m_AcousticSignals.begin();p!=m_AcousticSignals.end();p++)
    {
        MOOSTrace("\tSignal[%d] from %s on Channel[%d]\n",
            p->m_nID,
            p->GetSrcName().c_str(),
            p->GetChannel());
    }
#endif
    return true;
}

double CSimEnvironment::GetStartTime()
{
    return m_dfStartTime;
}

bool CSimEnvironment::SetStartTime(double dfStartTime)
{
    
    m_dfStartTime = dfStartTime;

    return true;
}

bool CSimEnvironment::Clean()
{
    m_AcousticSignals.clear();

    return true;
}

double CSimEnvironment::GetElapsedTime(double dfTimeNow)
{
    return dfTimeNow-m_dfStartTime;
}

bool CSimEnvironment::AddReport(const std::string & sName, const std::string &sData)
{
    m_MailOut.push_back(CMOOSMsg(MOOS_NOTIFY,sName,sData.c_str()));
    return true;
}
