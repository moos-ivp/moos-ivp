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
// BatteryDriver.h: interface for the CBatteryDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTERYDRIVER_H__DD1ADDC7_1BB7_4CF2_A54E_961ED256C1E7__INCLUDED_)
#define AFX_BATTERYDRIVER_H__DD1ADDC7_1BB7_4CF2_A54E_961ED256C1E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <string>
using namespace std;

class CMOOSSerialPort;

class CBatteryDriver  
{
public:
    virtual string GetCellsString();
    virtual string GetCommentString();
    virtual string GetErrorString();

    virtual bool Switch(bool bOn);
    virtual bool IsError();
    class CBatteryStatus
    {
    public:
        enum Condition
        {
            UNKNOWN,
            SENSOR_ERROR,
            GOOD,
            LOW,
            BAD,
            DANGEROUS,
        };

    
    public:
        Condition GetStatus();
        bool Calculate();
        double GetVoltage();
        double GetPercentCharge();
        string GetStatusAsString();
        CBatteryStatus();

        double m_dfVoltage;
        double m_dfPercent;
        double m_dfFull;
        double m_dfEmpty;
        double m_dfLow;
        
        string m_sError;
        Condition m_eStatus;

    };

                    CBatteryDriver();
    virtual            ~CBatteryDriver();

    
    virtual bool    Initialise()=0;
    virtual bool    GetData()=0;
            bool    GetStatus(CBatteryStatus & Status);
            bool    SetSerialPort(CMOOSSerialPort * pPort);
            bool    SetFullVolts(double dfVolts);
            bool    SetEmptyVolts(double dfVolts);
            bool    SetLowVolts(double dfVolts);
protected:
    /** A sensor port */
    CMOOSSerialPort* m_pPort;
    CBatteryStatus        m_Status;
    string m_sError;
    string m_sComment;

};

#endif // !defined(AFX_BATTERYDRIVER_H__DD1ADDC7_1BB7_4CF2_A54E_961ED256C1E7__INCLUDED_)
