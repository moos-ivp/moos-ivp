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
// MOOSInstrument.cpp: implementation of the CMOOSInstrument class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif
#include <MOOSLIB/MOOSLib.h>
#include <iostream> 
#include <sstream>
#include <iomanip>
#include <cctype>
using namespace std;
#include "MOOSInstrument.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSInstrument::CMOOSInstrument()
{
    m_bPublishRaw = false;
    m_dfMagneticOffset = 0;
    m_sPrompt = "";
    m_sInstrumentErrorMessage = "";
    
}

CMOOSInstrument::~CMOOSInstrument()
{

}

bool CMOOSInstrument::SetupPort()
{
    STRING_LIST sParams;

    if(!m_MissionReader.GetConfiguration(m_sAppName,sParams))
    {
    MOOSTrace("%s ReadConfiguration() failed to read configuration\n",m_sAppName.c_str());
    MOOSPause(3000);
    return false;
    }
    
    ///////////////////////////////////////////////////////////
    // create the port....
    
    if(!m_Port.Configure(sParams))
    {
    MOOSTrace("failed port creation\n");
    return false;
    }
    
    m_Port.Flush();
    return true;
}

bool CMOOSInstrument::InitialiseSensorN(int nAttempts, string sSensorName)
{
    int i = 0;
    while(!InitialiseSensor())
    {
    if(++i>nAttempts)
    {
            MOOSTrace("tried %d time to initialise \"%s\"...giving up\n",i,sSensorName.c_str());
        
        return false;
    }
    else
    {
        MOOSTrace("Attempting to initialise %s\n",sSensorName.c_str());
    }
    }
    
    return true;
}

bool CMOOSInstrument::OnStartUp()
{
    string sRaw;
    m_bPublishRaw = false;
    if(m_MissionReader.GetConfigurationParam("PUBLISHRAW",sRaw))
    {
        m_bPublishRaw = MOOSStrCmp(sRaw,"TRUE");
    }

    m_MissionReader.GetValue("CIRCUITNAME",m_sResourceName);

    return true;
}

bool CMOOSInstrument::InitialiseSensor()
{
    MOOSTrace("warning base class CMOOSInstrument::InitialiseSensor() called - NULL action\n");
    return false;
}

double CMOOSInstrument::GetMagneticOffset()
{
    double dfTmp=0;
    if(m_MissionReader.GetValue("MAGNETICOFFSET",dfTmp))
    {
        m_dfMagneticOffset=dfTmp;
    }
    else
    {
        MOOSTrace("WARNING: No magnetic offset specified  in Mission file (Field name = \"MagneticOffset\")\n");
        m_dfMagneticOffset =  0;
    }
    return m_dfMagneticOffset;
}

void CMOOSInstrument::SetPrompt(string sPrompt)
{
    m_sPrompt = sPrompt;
}

void CMOOSInstrument::SetInstrumentErrorMessage(string sError)
{
    m_sInstrumentErrorMessage = sError;
}


bool CMOOSInstrument::DoNMEACheckSum(string sNMEA)
{
    unsigned char xCheckSum=0;

    string sToCheck;
    MOOSChomp(sNMEA,"$");
    sToCheck = MOOSChomp(sNMEA,"*");
    string sRxCheckSum = sNMEA;

    //now calculate what we think check sum should be...
    string::iterator p;
    for(p = sToCheck.begin();p!=sToCheck.end();p++)
    {
        xCheckSum^=*p;
    }

    ostringstream os;
    
    os.flags(ios::hex);
    os<<(int)xCheckSum;//<<ends;
    string sExpected = os.str();
    
    ///now compare to what we recived..

    return MOOSStrCmp(sExpected,sRxCheckSum);


}
/** sMsg should NOT have $ sign at the front
input sMsg, output is $sMsg*CHKSUM
eg Msg= "MOOS,MOOSDATA"
output would be something like "$MOOS,MOOSDATA*A7\r\n"*/
//changed June 4th after Scott R. Sideleau, emailed me that
//it isn't quite NMEA compliant
string CMOOSInstrument::Message2NMEA(string sMsg)
{
    unsigned char xCheckSum=0;
    //now calculate what we think check sum should be...
    string::iterator p;
    for(p = sMsg.begin(); p != sMsg.end(); p++)
    {
        xCheckSum ^= *p;
    }
    
    ostringstream os;
    
    os.flags(ios::hex);
    os<<(int)xCheckSum; //<<ends;
    string sChkSum = os.str();
    std::transform(sChkSum.begin(), sChkSum.end(), sChkSum.begin(), \
                   (int(*)(int)) std::toupper);
    
    string sOutput = "$" + sMsg + "*" + sChkSum + "\r\n";
    
    return sOutput;
}
