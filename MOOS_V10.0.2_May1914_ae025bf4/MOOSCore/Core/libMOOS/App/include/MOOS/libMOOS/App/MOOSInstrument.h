///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
// MOOSInstrument.h: interface for the CMOOSInstrument class.
//
//////////////////////////////////////////////////////////////////////

#ifndef moosinstrumenth
#define moosinstrumenth

#include "MOOS/libMOOS/Utils/MOOSUtils.h"
#include "MOOS/libMOOS/App/MOOSApp.h"


/** @brief Class that derives from CMOOSApp and adds functionality of cross platform serial ports.
* @author Paul Newman
* @ingroup App
*/
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

#endif
