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
// BatteryDriver.cpp: implementation of the CBatteryDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "BatteryDriver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBatteryDriver::CBatteryStatus::CBatteryStatus()
{
    m_dfPercent = -1;
    m_dfVoltage = -1;
    m_eStatus = UNKNOWN;
    m_dfFull = -1;
    m_dfEmpty = -1;
    m_dfLow = 0;


}

CBatteryDriver::CBatteryDriver()
{

}

CBatteryDriver::~CBatteryDriver()
{

}

bool CBatteryDriver::SetSerialPort(CMOOSSerialPort *pPort)
{
    m_pPort = pPort;

    return true;
}

bool CBatteryDriver::SetLowVolts(double dfVolts)
{
    m_Status.m_dfLow = dfVolts;
    return true;
}


bool CBatteryDriver::SetFullVolts(double dfVolts)
{
    m_Status.m_dfFull = dfVolts;
    return true;
}

bool CBatteryDriver::SetEmptyVolts(double dfVolts)
{
    m_Status.m_dfEmpty = dfVolts;
    return true;
}


bool CBatteryDriver::GetStatus(CBatteryStatus & Status)
{
    if(IsError())
    {
        m_Status.m_sError = GetErrorString();
        m_Status.m_eStatus = CBatteryStatus::BAD;
    }
    else
    {
        //OK no device specific errors so see if we
        //are breaking any rules seet in the mission file...
        m_Status.Calculate();
    }
    
    Status = m_Status;

    return true;
}

string CBatteryDriver::CBatteryStatus::GetStatusAsString()
{
    switch(m_eStatus)
    {
    case     UNKNOWN:    return "UNKNOWN";        break;
    case    GOOD:        return "GOOD";            break;
    case    LOW:        return "LOW";            break;
    case    BAD:        return "BAD";            break;
    case    DANGEROUS:    return "DANGEROUS";        break;
    case    SENSOR_ERROR:    return "SENSOR_ERROR"; break;
    default:
            return "ILLEGAL CODE STATE"; break;//!!!
    }
}

double CBatteryDriver::CBatteryStatus::GetPercentCharge()
{
    return m_dfPercent;
}

double CBatteryDriver::CBatteryStatus::GetVoltage()
{
    return m_dfVoltage;
}

bool CBatteryDriver::CBatteryStatus::Calculate()
{
    //how full our we?
    m_dfPercent = (m_dfVoltage-m_dfEmpty)/(m_dfFull-m_dfEmpty)*100.0;

    if(m_dfVoltage>m_dfLow)
    {
        m_eStatus = GOOD;
    }
    else if(m_dfVoltage<m_dfLow)
    {
        m_eStatus = LOW;
    }
    else if(m_dfVoltage<m_dfEmpty)
    {
        m_eStatus = BAD;
    }

    return true;
}

CBatteryDriver::CBatteryStatus::Condition CBatteryDriver::CBatteryStatus::GetStatus()
{
    return m_eStatus;
}

bool CBatteryDriver::IsError()
{
    return false;
}

string CBatteryDriver::GetErrorString()
{
    return m_sError;
}

bool CBatteryDriver::Switch(bool bOn)
{
    return true;
}

string CBatteryDriver::GetCellsString()
{
    return "";
}


string CBatteryDriver::GetCommentString()
{
    return "";
}
