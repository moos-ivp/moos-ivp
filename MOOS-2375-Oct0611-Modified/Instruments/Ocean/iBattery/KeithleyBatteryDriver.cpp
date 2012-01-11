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
// KeithleyBatteryDriver.cpp: implementation of the CKeithleyBatteryDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "KeithleyBatteryDriver.h"
#include <MOOSGenLib/MOOSGenLib.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeithleyBatteryDriver::CKeithleyBatteryDriver()
{
    m_bError = false;
}

CKeithleyBatteryDriver::~CKeithleyBatteryDriver()
{
    
}

bool CKeithleyBatteryDriver::Initialise()
{
    //nothing to do
    return true;
}


bool CKeithleyBatteryDriver::GetData()
{

    //The basic communication
    //$ starts the string
    //1 is the address of the device (if other Keithleys
    //are in the chain, each must be addressed separately
    //RD reads data, end the string with a <cr>
    const char * sGetKeithleyString = "$1RD\r";

    m_pPort->Flush();

    if( m_pPort->Write(sGetKeithleyString,strlen(sGetKeithleyString)) )
    {
        string sWhat;

        char Tmp[20];
        char * pStr = Tmp;
        int nRead = m_pPort->ReadNWithTimeOut(Tmp,20,1.5);

        if(nRead>0)
        {
            if(Tmp[0]=='\0')
            {
                pStr+=1;
            }
            Tmp[nRead]='\0';

            sWhat = pStr;

            ParseKeithleyString(sWhat);


        }


    }
    else
    {
        return false;
    }

    return true;
}

bool CKeithleyBatteryDriver::ParseKeithleyString(string &sReply)
{
    m_bError = false;    
    string sCopy = sReply;

    //Error messages are in the form ?<address#> MESSAGE<cr>
    //Example:  ?1 SYNTAX ERROR
    //If the message doesn't contain a "?", assume a valid message
    if (sReply.find("?") == string::npos)
    {
        //Valid messages are in the form *+00000.00<cr>
        //The * starts the message
        //+ could be - for negative readings
        //(Since we only remove the "+", negative values should be read correctly)
        MOOSRemoveChars(sReply, "*+");

        m_Status.m_dfVoltage = atof(sReply.c_str());
        m_Status.m_eStatus   = CBatteryDriver::CBatteryStatus::GOOD;
        
    }
    else
    {
        m_Status.m_eStatus = CBatteryDriver::CBatteryStatus::SENSOR_ERROR;
        m_sError           = "Keithley saw ?";
        m_bError           = true;
    }

    return true;
}


bool CKeithleyBatteryDriver::IsError()
{
    return m_bError;
}
