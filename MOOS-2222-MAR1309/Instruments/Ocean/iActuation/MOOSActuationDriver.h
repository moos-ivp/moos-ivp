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
// MOOSActuationDriver.h: interface for the CMOOSActuationDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSACTUATIONDRIVER_H__EF6C1718_74A2_460E_B3E4_6EED9BE4D6A4__INCLUDED_)
#define AFX_MOOSACTUATIONDRIVER_H__EF6C1718_74A2_460E_B3E4_6EED9BE4D6A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
using namespace std;

class CMOOSSerialPort;

#ifdef _WIN32
    class CMOOSNTSerialPort;
#else
    class CMOOSLinuxSerialPort;
#endif

class CMOOSActuationDriver  
{
 public:
     bool SetElevatorOffset(double dfElevator);
     bool SetRudderOffset(double dfAng);
    bool SetPort(CMOOSSerialPort* pPort);
    virtual bool Initialise();
    virtual bool SetThrust(double dfPercent);
    virtual bool SetRudder(double dfAng);
    virtual bool SetElevator(double dfAng);
    virtual bool SetZeroRudder();
    virtual bool SetZeroElevator();


    CMOOSActuationDriver();
    virtual ~CMOOSActuationDriver();

    bool m_bVerbose;

    double m_dfRudderOffset;
    double m_dfElevatorOffset;

    /** A sensor port */
#ifdef _WIN32
    CMOOSNTSerialPort * m_pPort;
#else
    CMOOSLinuxSerialPort * m_pPort;
#endif


    virtual double GetRPM(){return m_dfRPM;};


 protected:
    bool SendAndAck(const string & sCmd,string & sReply, bool bWait =true);

    double m_dfRPM;
};

#endif // !defined(AFX_MOOSACTUATIONDRIVER_H__EF6C1718_74A2_460E_B3E4_6EED9BE4D6A4__INCLUDED_)
