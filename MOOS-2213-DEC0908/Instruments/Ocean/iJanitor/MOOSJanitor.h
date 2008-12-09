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
//   This file is part of a  MOOS Instrument. 
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
// MOOSJanitor.h: interface for the CMOOSJanitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSJANITOR_H__2503B2AA_9748_437E_9298_6D9D7D104268__INCLUDED_)
#define AFX_MOOSJANITOR_H__2503B2AA_9748_437E_9298_6D9D7D104268__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMOOSJanitor : public CMOOSInstrument  
{
public:
    class CResourceCircuit
    {
    public:
        std::string  m_sConnectedResource;
        int        m_nCircuit;
        bool    m_bInitialState;
        bool    m_bCurrentState;
        bool    m_bDesiredState;
    };
    typedef std::map<std::string,CResourceCircuit> RESOURCEMAP;

    CMOOSJanitor();
    virtual ~CMOOSJanitor();

    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool OnConnectToServer();
    bool OnStartUp();


protected:
    bool DoDiagnostics();
    bool GetLeaks();
    bool GetGroundFaults();
    bool GetSwitchState(const std::string & sResource, bool & bState);
    bool GetTemperature();
    bool m_bAutoWatchDog;
    bool OnSwitch(CMOOSMsg & Msg);
    bool HitTailConeWD();
    bool SetSwitch(const std::string & sResource,bool bVal);
    bool SetUpSwitches();
    bool IsBluefinVehicle();
    bool BootBluefinVehicle();
    std::string m_sVehicleType;
    RESOURCEMAP m_Resources;
    double m_dfLastWDHit;
    double m_dfLastDiagnostic;

    double m_dfTemperature;

};

#endif // !defined(AFX_MOOSJANITOR_H__2503B2AA_9748_437E_9298_6D9D7D104268__INCLUDED_)
