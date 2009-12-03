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


// SimpleAUVSim.h: interface for the CSimpleAUVSim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEAUVSIM_H__179D7D4B_5915_4664_AEEA_8B81C324A189__INCLUDED_)
#define AFX_SIMPLEAUVSIM_H__179D7D4B_5915_4664_AEEA_8B81C324A189__INCLUDED_

#include "SimParams.h"    // Added by ClassView
#include "SixDOFAUV.h"    // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <newmat.h>
using namespace NEWMAT;

#include <MOOSGenLib/MOOSGenLib.h>

#include "AcousticNode.h"
#include "AcousticSignal.h"
#include "AcousticBeacon.h"

#include <list>
#include "SimEnvironment.h"    // Added by ClassView
using namespace std;


typedef std::list<CSimEntity*>        SIM_ENTITY_LIST;


class CMVSim : public CMOOSApp  
{
public:
    
    virtual bool OnConnectToServer();
    virtual bool OnStartUp();
    
    CMVSim();
    
    virtual ~CMVSim();

protected:
    bool LogStartConditions();
    bool OpenLogFile();
    bool MakeBeacon(std::string sConfig);
    bool MakeAUV(std::string sConfig);
    bool Clean();
    CSixDOFAUV * GetSubscriber(const std::string & sName);

    bool m_bRealTime;
    CSimEnvironment m_Environment;


    SIM_ENTITY_LIST            m_Entities;

    bool DoRegistrations();
    void PostResults();
    CSimParams m_Params;
    bool Initialise();
    bool m_bInitialised;
    double m_dfUpdateRate;


    bool    NeedToMail();
    double m_dfLastMailed;
    double m_dfStartTime;
    double m_dfSimulatorTime;
    double m_dfOldSimulatorTime;



    
    

    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST & NewMail);
};

#endif // !defined(AFX_SIMPLEAUVSIM_H__179D7D4B_5915_4664_AEEA_8B81C324A189__INCLUDED_)
