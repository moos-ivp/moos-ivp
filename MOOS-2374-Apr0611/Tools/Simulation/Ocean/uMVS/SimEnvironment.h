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


// SimEnvironment.h: interface for the CSimEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMENVIRONMENT_H__1CF844D2_0B14_4CD1_A8E3_4A07FA3D81C7__INCLUDED_)
#define AFX_SIMENVIRONMENT_H__1CF844D2_0B14_4CD1_A8E3_4A07FA3D81C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSGenLib/MOOSGenLib.h>
#include <MOOSUtilityLib/MOOSTerrain.h>
#include <MOOSLIB/MOOSLib.h>

#include "AcousticSignal.h"
typedef std::list<CAcousticSignal>    ACOUSTIC_SIGNAL_LIST;

class CSimEnvironment  
{
public:
    bool AddReport(const std::string & sName,const std::string & sData);
    double GetElapsedTime(double dfTimeNow);
    bool Clean();
    bool SetStartTime(double dfStartTime);
    double GetStartTime();
    bool AddSignal(CAcousticSignal NewSignal);
    bool RemoveOldSignals(double dfTimeNow);
    bool Initialise(const char * pTerrainFile="");
    CSimEnvironment();
    virtual ~CSimEnvironment();

    double  GetAltitude(double dfX,double dfY,double dfZ);
    double    GetDepth(double dfZ);
    double    GetTideHeight();
    double    m_dfTideHeight;
    double m_dfMagneticOffset;

    CMOOSTerrain m_Terrain;

    ACOUSTIC_SIGNAL_LIST    m_AcousticSignals;
    MOOSMSG_LIST            m_MailOut;

protected:
    double m_dfStartTime;
};

#endif // !defined(AFX_SIMENVIRONMENT_H__1CF844D2_0B14_4CD1_A8E3_4A07FA3D81C7__INCLUDED_)
