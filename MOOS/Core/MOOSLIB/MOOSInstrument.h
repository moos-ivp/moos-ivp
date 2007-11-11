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
//   This file is part of a  MOOS Core Component. 
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
// MOOSInstrument.h: interface for the CMOOSInstrument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSINSTRUMENT_H__D81A485B_B5F7_49C9_8A24_F9B5B94447C6__INCLUDED_)
#define AFX_MOOSINSTRUMENT_H__D81A485B_B5F7_49C9_8A24_F9B5B94447C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSApp.h"
/** Class that derives from CMOOSApp and adds functionality of cross platform serial ports*/
class CMOOSInstrument : public CMOOSApp  
{
public:
    
    CMOOSInstrument();
    virtual ~CMOOSInstrument();

    /** A sensor port */
    #ifdef _WIN32
        CMOOSNTSerialPort m_Port;
    #else
        CMOOSLinuxSerialPort m_Port;
    #endif


    /** turns a string into NMEA string */
    static std::string Message2NMEA(std::string sMsg);
    /** performs NMEA string checksum */
    static bool DoNMEACheckSum(std::string sNMEA);
    
protected:
    /**CMOOSApp overide*/
    virtual bool OnStartUp();

    /** called from OnStartUp - overload to execute custom start up code for sensor*/
    virtual bool InitialiseSensor();

    /** called from OnStartUp class InitialiseSensor N times*/
    bool InitialiseSensorN(int nAttempts, std::string sSensorName);

    /** Set up the serial port by reading paremters from mission file*/
    virtual bool SetupPort();
    
    /** set to true if this instrument should publish the raw incoming data to the DB*/
    bool    m_bPublishRaw;

    /** returns true if instrument is publishing raw data */
    bool    PublishRaw(){return m_bPublishRaw;};

    /** a place holder for a the name of this sensor resource - rarely used */
    std::string m_sResourceName;

    /** some legacy stuff that should be removed...*/
    double  m_dfMagneticOffset;
    /** some legacy stuff that should be removed...*/
    double GetMagneticOffset();

        void SetInstrumentErrorMessage(std::string sError);
    void SetPrompt(std::string sPrompt);

    std::string m_sPrompt;
    std::string m_sInstrumentErrorMessage;

};

#endif // !defined(AFX_MOOSINSTRUMENT_H__D81A485B_B5F7_49C9_8A24_F9B5B94447C6__INCLUDED_)
