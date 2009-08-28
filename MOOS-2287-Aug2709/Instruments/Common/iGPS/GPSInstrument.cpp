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
// GPSInstrument.cpp: implementation of the CGPSInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include "GPSInstrument.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <assert.h>
using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPSInstrument::CGPSInstrument()
{
    m_sType = "VANILLA";
    m_bCombineMessages = false;
}

CGPSInstrument::~CGPSInstrument()
{

}


/////////////////////////////////////////////
///this is where it all happens..
bool CGPSInstrument::Iterate()
{
    if(GetData())
    {
        PublishData();
    }

    return true;
}


bool CGPSInstrument::OnStartUp()
{
    
    CMOOSInstrument::OnStartUp();

    //set up Geodetic conversions
    double dfLatOrigin;
    double dfLongOrigin;


    m_MissionReader.GetConfigurationParam("TYPE",m_sType);

    // If FALSE then outputs old-style messages, where each piece
    // of info (N,E,X,Y,#Sats) comes in a separate message.
    // If TRUE then outputs all data in a single message, with a proper timestamp.
    m_MissionReader.GetConfigurationParam("CombineMessages", m_bCombineMessages);

    string sVal;
    if(m_MissionReader.GetValue("LatOrigin",sVal))
    {
        dfLatOrigin = atof(sVal.c_str());
    }
    else
    {
        MOOSTrace("LatOrigin not set - FAIL\n");

        return false;

    }

    if(m_MissionReader.GetValue("LongOrigin",sVal))
    {
        dfLongOrigin = atof(sVal.c_str());
    }
    else
    {
        MOOSTrace("LongOrigin not set - FAIL\n");

        return false;
    }

    if(!m_Geodesy.Initialise(dfLatOrigin,dfLongOrigin))
    {
        MOOSTrace("Geodesy Init failed - FAIL\n");

        return false;
    }


    //here we make the variables that we are managing
    double dfGPSPeriod = 1.0;

    //GPS update @ 2Hz
    if (m_bCombineMessages)
    {
        AddMOOSVariable("GPS","SIM_GPS", "GPS", 0);
    }
    else
    {
        AddMOOSVariable("X","SIM_X","GPS_X",dfGPSPeriod);
        AddMOOSVariable("Y","SIM_Y","GPS_Y",dfGPSPeriod);
        AddMOOSVariable("N","","GPS_N",dfGPSPeriod);
        AddMOOSVariable("E","","GPS_E",dfGPSPeriod);
        AddMOOSVariable("Satellites","","GPS_SAT",dfGPSPeriod);
    }

    AddMOOSVariable("Raw","","GPS_RAW",dfGPSPeriod);




    if(IsSimulateMode())
    {
        //not much to do...
        RegisterMOOSVariables();
    }
    else
    {
        //try to open
        if(!SetupPort())
        {
            return false;
        }

        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"GPS"))
        {
            return false;
        }
    }


    return true;
}



bool CGPSInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    return UpdateMOOSVariables(NewMail);
}



bool CGPSInstrument::PublishData()
{
    return PublishFreshMOOSVariables();
}



bool CGPSInstrument::OnConnectToServer()
{
    if(IsSimulateMode())
    {
        //not much to do...
        RegisterMOOSVariables();
    }
    else
    {


    }
    return true;
}



bool CGPSInstrument::InitialiseSensor()
{
    if (MOOSStrCmp(m_sType, "ASHTECH"))
    {
        const char * sInit = "$PASHS,NME,GGA,A,ON\r\n";
        MOOSTrace("Sending %s\n", sInit);
        m_Port.Write(sInit, strlen(sInit));

        MOOSPause(2000);
        string sReply;
        double dfTime;

        if (m_Port.GetLatest(sReply, dfTime)) {
            MOOSTrace("Rx %s", sReply.c_str());
        } else {
            MOOSTrace("No reply\n");
        }

    }
    else if (MOOSStrCmp(m_sType, "GARMIN"))
    {

        const char *sInitA = "$PGRMO,,2\r\n";
        MOOSTrace("Sending %s\n", sInitA);
        MOOSPause(2000);
        m_Port.Write(sInitA, strlen(sInitA));

        const char *sInitB = "$PGRMO,GPGGA,1\r\n";
        MOOSTrace("Sending %s\n", sInitB);
        MOOSPause(2000);
        m_Port.Write(sInitB, strlen(sInitB));

        const char *sInitC = "$PGRMO,GPRMC,1\r\n";
        MOOSTrace("Sending %s\n", sInitC);
        MOOSPause(2000);
        m_Port.Write(sInitC, strlen(sInitC));


        string sReply;
        double dfTime;

        if (m_Port.GetLatest(sReply, dfTime))
        {
            MOOSTrace("Rx %s\n", sReply.c_str());
        }
        else
        {
            MOOSTrace("No reply\n");
        }

        const char *sInit = "$PGRMC,,,,,,,,,,,,2,1,\r\n";
        MOOSTrace("Sending %s\n", sInit);
        m_Port.Write(sInit, strlen(sInit));


        if(m_Port.GetLatest(sReply, dfTime))
        {
            MOOSTrace("Rx %s\n", sReply.c_str());
        }
        else
        {
            MOOSTrace("No reply\n");
        }
    }
    else if (MOOSStrCmp(m_sType, "SERES"))
    {
        //Some information about the Seres unit:
        //Default baud is 9600.
        //If the unit is sending binary messgaes, try sending a $JRESET message
        //This can happen after poking it with a diagnostic program like SLXMon.

        const char * sInit = "$JASC,GPGGA,5\r\n";
        MOOSTrace("Sending %s\n", sInit);
        m_Port.Write(sInit, strlen(sInit));

        MOOSPause(2000);
        string sReply;
        double dfTime;

        if (m_Port.GetLatest(sReply, dfTime)) {
            MOOSTrace("Rx %s", sReply.c_str());
        } else {
            MOOSTrace("No reply\n");
        }

    }
    return true;
}




/**
*
*/

bool CGPSInstrument::GetData()
{
    if(!IsSimulateMode())
    {
        //here we actually access serial ports etc

        string sWhat;

        double dfWhen;

        if(m_Port.IsStreaming())
        {
            if(!m_Port.GetLatest(sWhat,dfWhen))
            {
                return false;
            }
        }
        else
        {
            if(!m_Port.GetTelegram(sWhat,0.5))
            {
                return false;
            }
        }

        double dfTimeNow = MOOSTime();

        //MOOSTrace("Rx:  %s",sWhat.c_str());
        if(PublishRaw())
        {
            SetMOOSVar("Raw",sWhat,dfTimeNow);
        }

        // Try to make sense of the string
        CGPSData data;
        if (!ParseNMEAString(sWhat, data))
        {
            return false;
        }


        // Publish all the bits that we can.  Each component is published separately
        // (as they were originally), but at the same time a large string is built up
        // containing all the info in one place
        stringstream buf;
        buf << "time=";
        buf.setf(ios::fixed);
        buf << setprecision(3) << dfTimeNow;

        if (data.bGood)
        {
            double dfNLocal, dfELocal;
            if(m_Geodesy.LatLong2LocalUTM(data.dfLat_deg,data.dfLong_deg,dfNLocal,dfELocal))
            {
                if (!m_bCombineMessages)
                {
                    SetMOOSVar("N", dfNLocal, dfTimeNow);
                    SetMOOSVar("E", dfELocal, dfTimeNow);
                }
                buf << ",N=" << dfNLocal << ",E=" << dfELocal;
            }

            double dfXLocal, dfYLocal;
            if(m_Geodesy.LatLong2LocalGrid(data.dfLat_deg,data.dfLong_deg,dfYLocal,dfXLocal))
            {
                if (!m_bCombineMessages)
                {
                    SetMOOSVar("X", dfXLocal, dfTimeNow);
                    SetMOOSVar("Y", dfYLocal, dfTimeNow);
                }
                buf << ",X=" << dfXLocal << ",Y=" << dfYLocal;
            }

        }

        // Always say how many satellites we have
        SetMOOSVar("Satellites", data.nSatellites, dfTimeNow);
        buf << ",Sats=" << setprecision(0) << data.nSatellites;

        if (m_bCombineMessages)
            SetMOOSVar("GPS", buf.str(), dfTimeNow);
    }
    else
    {
        //in simulated mode there is nothing to do..all data
        //arrives via comms.
    }

    return true;
}


bool CGPSInstrument::ParseNMEAString(const std::string &sNMEAString, CGPSData & out_data)
{
    CGPSData data;

    // Keep a copy for later..
    string sNMEAChomp = sNMEAString;

    string sWhat = MOOSChomp(sNMEAChomp, ",");

    bool bGood = true;

    // GGA and GLL headers format the NMEA string differently
    // We only pick up GGA records
    if(sWhat == "$GPGGA")
    {

        // First of all, is this a good NMEA string?
        if(!DoNMEACheckSum(sNMEAString))
        {
            MOOSDebugWrite("GPS Failed NMEA check sum");
            return false;
        }

        // Begin to extract data
        string sTok;

        // We're not currently doing anything with the GPS time
        // Time is in format hhmmss[.s]
        sTok = MOOSChomp(sNMEAChomp,",");

        // Latitude
        sTok = MOOSChomp(sNMEAChomp,",");
        bGood = (bGood && sTok.size());
        data.dfLat_deg = m_Geodesy.DMS2DecDeg(atof(sTok.c_str()));

        // North/South
        sTok = MOOSChomp(sNMEAChomp,",");
        if(sTok == "S")
        {
            data.dfLat_deg *= -1.0;
        }

        // Longitude
        sTok = MOOSChomp(sNMEAChomp,",");
        bGood = (bGood && sTok.size());
        data.dfLong_deg = m_Geodesy.DMS2DecDeg(atof(sTok.c_str()));

        // East/West
        sTok = MOOSChomp(sNMEAChomp,",");
        if(sTok=="W")
        {
            data.dfLong_deg *= -1.0;
        }

        // Quality measures
        //MJC says: I think this is wrong. GGA Message spec says this field should be Quality Indicator
        //0 = no position, 1 = undifferentially corrected position, 2 =
        //differentially corrected position, 9= position computed using almanac
        //HDOP field comes after Number of Sattelites
        //However, the value read into the HDOP variable is never actually used, so it's not currently having any effect.
        sTok = MOOSChomp(sNMEAChomp,",");
        data.dfHDOP    = atof(sTok.c_str());

        sTok = MOOSChomp(sNMEAChomp,",");
        data.nSatellites = atoi(sTok.c_str());
        bGood = (bGood && data.nSatellites > 3);

        data.bGood = bGood;

        // Copy result to output variable
        out_data = data;

        return true;
    }

    return false;
}

