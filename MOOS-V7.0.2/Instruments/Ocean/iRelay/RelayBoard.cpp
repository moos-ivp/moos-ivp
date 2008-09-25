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
// RelayBoard.cpp: implementation of the CRelayBoard class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
using namespace std;

#include "RelayBoard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelayBoard::CRelayBoard()
{
    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(1);
    SetCommsFreq(5);

    //state strings
    m_sRECORDING    = "RECORDING";
    m_sSTOPPED      = "STOPPED";
    m_sRECORD       = "RECORD";
    m_sSTOP         = "STOP";
    m_sLIGHT        = "LIGHT";
    m_sDARK         = "DARK";
    m_sON           = "ON";
    m_sOFF          = "OFF";
    m_sVCR_POWER    = "VCR_POWER";
    m_sCAMERA_POWER = "CAMERA_POWER";
    m_sBOARD_POWER  = "BOARD_POWER";
    m_sLIGHT_POWER  = "LIGHT_POWER";

    m_RelayToggleMap[m_sCAMERA_POWER] = RELAY0_TOGGLE;
    m_RelayToggleMap[m_sVCR_POWER]    = RELAY1_TOGGLE;
    m_RelayToggleMap[m_sLIGHT_POWER]  = RELAY2_TOGGLE;
    m_RelayToggleMap[m_sRECORD]       = RELAY3_TOGGLE;
    m_RelayToggleMap[m_sBOARD_POWER]  = RELAY7_TOGGLE;

    m_RelayCheckMap[m_sCAMERA_POWER] = RELAY0;
    m_RelayCheckMap[m_sVCR_POWER]    = RELAY1;
    m_RelayCheckMap[m_sLIGHT_POWER]  = RELAY2;
    m_RelayCheckMap[m_sRECORD]       = RELAY3;
    m_RelayCheckMap[m_sBOARD_POWER]  = RELAY7;

    m_InputNameMap[m_sRECORDING]    = INPUT3;
}

CRelayBoard::~CRelayBoard()
{
    Stop();
    ToggleRelaySwitch(m_sLIGHT_POWER, m_sOFF);

}


/////////////////////////////////////////////
///this is where it all happens..
bool CRelayBoard::Iterate()
{

    if(GetData())
    {
        PublishData();
    }

    return true;
}


bool CRelayBoard::OnStartUp()
{
    CMOOSInstrument::OnStartUp();
    //here we make the variables that we are managing
    double dfRelayPeriod = 1.0;
    
    //Register for the variable that will provide for dynamic configuration
    //of the Relay board
    //This version of the iRelay only corresponds to the Video System
    //the video system states
    AddMOOSVariable("VIDEO_STATUS", "RELAY_VIDEO_UPDATE",  "RELAY_VIDEO_REPLY", dfRelayPeriod);
    AddMOOSVariable("RELAY_STATUS", "RELAY_UPDATE",        "RELAY_UPDATE_REPLY",dfRelayPeriod);
    

    RegisterMOOSVariables();

    if(IsSimulateMode())
    {
        SetAppFreq(2);
        SetCommsFreq(2);
    }
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }
        
        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"RELAY"))
        {
            return false;
        }          

    }
    return true;
}



bool CRelayBoard::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    double dfTimeNow = MOOSTime();
    string sVal;

    if(m_Comms.PeekMail(NewMail,"RELAY_VIDEO_UPDATE",Msg))
    {
        sVal = Msg.m_sVal;
        MOOSTrace("..............new : %s\n", sVal.c_str());
        if(sVal == m_sRECORD)
            return Record();
        else if(sVal == m_sSTOP)
               return Stop();
        else if(sVal == m_sLIGHT)
               return ToggleRelaySwitch(m_sLIGHT_POWER, m_sON);
        else if(sVal == m_sDARK)
               return ToggleRelaySwitch(m_sLIGHT_POWER, m_sOFF);
        else
            return true;
    }
    else if(m_Comms.PeekMail(NewMail,"RELAY_UPDATE",Msg))
    {
        return UpdateRelay(Msg);
    }

    else
    {
        return UpdateMOOSVariables(NewMail);
    }
}

bool CRelayBoard::PublishData()
{  
    return PublishFreshMOOSVariables();
}



bool CRelayBoard::OnConnectToServer()
{
    if(IsSimulateMode())
    {
        RegisterMOOSVariables();
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////
// here we initialise the sensor, giving it start up values
bool CRelayBoard::InitialiseSensor()
{   
    return ToggleRelaySwitch(m_sBOARD_POWER, m_sON);
}



bool CRelayBoard::GetData()
{
    if(!IsSimulateMode())
    {
        //   string sStatus;
        //we will publish the state of the relays
        // RELAY_FUNCTION_2_CHECK_MAP::iterator p;
        //for(p = m_RelayCheckMap.begin(); p != m_RelayCheckMap.end(); p++)
        //{
        //    string sRelay = p->first.c_str();
        //    sStatus += sRelay + (IsPowered(sRelay) ? ": true " : ": false ");
        //}
        //alert the DB
        //SetMOOSVar("RELAY_STATUS", sStatus, MOOSTime());

    }
    else
    {
        //in simulated mode there is nothing to do..all data
        //arrives via comms.
    }

    return true;

}

bool CRelayBoard::SetRelayHi(short val)
{
    string sRelay = "MK";
    short MASK;
    char pChar[4];
    
    //should check the range of val
    if(val>255 || val < 0)
    {
        MOOSTrace("Acceptable Relay range 0-255\n");
        return false;
    }
    else
    {
        //we need to make a mask out of what the state of the RelayBoard
        //already is so that we do not shut off relays in the process of
        //trying to turn only a specific one on
        short nState = (short)CheckAllRelayStates();
        MASK = nState | val;
        
        //make note of this value we are writing
        sprintf(pChar, "%d\r", MASK);
         //The format for setting the relay is MKddd
        //where 'ddd' corresponds to the 3 characters in a 0-255 value short
        sRelay += pChar;
        //write the new command
        m_Port.Write((char *)sRelay.c_str(), sRelay.size());
    }
   
    
    return true;
}

/**
*@return either 1 to represent the ON state of the relay
*or 0 to indicate the relay is OFF
*-1 indicates error
*/
int CRelayBoard::CheckRelayState(short val)
{
    string sRelay = "RPK";
    string sReply;
    double dfWhen = 0.5;
    char pChar[4];
    
    //should check the range of val
    if(val > 7 || val < 0)
    {
        MOOSTrace("Acceptable Relay range 0-7\n");
        return -1;
    }
    else
    {
        //make note of this value we are writing
        sprintf(pChar,"%d\r",val);
         //The format for setting the relay is MKddd
        //where 'ddd' corresponds to the 3 characters in a 0-255 value short
        sRelay += pChar;
        //write the new command
        m_Port.Write((char *)sRelay.c_str(), sRelay.size());

        while(!m_Port.GetTelegram(sReply,dfWhen))
        {
            MOOSPause(10);
        }
        
        short nRet = (short)atof(sReply.c_str());

        if(m_Port.IsVerbose())
        {
            MOOSTrace("checked: %d and found its state to be: %d\n", val, nRet);
        }

        return nRet;
    }
   
}

/**Allows querying of an individual input
*/
int CRelayBoard::CheckInputState(short val)
{
    string sRelay = "RPA";
    string sReply;
    double dfWhen = 0.5;
    char pChar[4];
    
    //should check the range of val
    if(val > 3 || val < 0)
    {
        MOOSTrace("Acceptable Input range 0-3\n");
        return -1;
    }
    else
    {
        //the relay needs 0.5 seconds to make the state change
        MOOSPause(5000);
        //make note of this value we are writing
        sprintf(pChar,"%d\r",val);
        sRelay += pChar;
        //write the new command
        m_Port.Write((char *)sRelay.c_str(), sRelay.size());

        while(!m_Port.GetTelegram(sReply,dfWhen))
        {
            MOOSPause(10);
        }
        
        short nRet = (short)atof(sReply.c_str());

        if(m_Port.IsVerbose())
        {
            MOOSTrace("checked input: %d and found its state to be: %d\n", val, nRet);
        }

        return nRet;
    }
    
}

/**Starts the video system recording*/
bool CRelayBoard::Record()
{
    int nInputToMonitor = -1;

    INPUT_NAME_2_INT_MAP::iterator p = m_InputNameMap.find(m_sRECORDING);
    if(p != m_InputNameMap.end())
    {
        nInputToMonitor = p->second;
        MOOSTrace("[REC]Looking for %d\n", nInputToMonitor);
    }
    else
    {
        MOOSTrace("You have entered an incorrect Input Name[REC]\n");
        return false;
    }

    //make sure the relay board has power
    //and then turn on the VCR and CAMERA
    ToggleRelaySwitch(m_sBOARD_POWER, m_sON);
    ToggleRelaySwitch(m_sVCR_POWER, m_sON);
    ToggleRelaySwitch(m_sCAMERA_POWER, m_sON);
    ToggleRelaySwitch(m_sRECORD, m_sON);       

    if(CheckInputState(nInputToMonitor) == LO)
    {
        SetMOOSVar("VIDEO_STATUS", m_sRECORDING,MOOSTime());
        return true;
    }
    else
    {
        string sProblem = m_sRECORDING + ": FAILED";
        SetMOOSVar("VIDEO_STATUS", sProblem,MOOSTime());
        return false;
    }
}

bool CRelayBoard::Stop()
{
    int nInputToMonitor = -1;

    INPUT_NAME_2_INT_MAP::iterator p = m_InputNameMap.find(m_sRECORDING);
    if(p != m_InputNameMap.end())
    {
        nInputToMonitor = p->second;
        MOOSTrace("[STOP]Looking for %d\n", nInputToMonitor);
    }
    else
    {
        MOOSTrace("You have entered an incorrect Input Name[STOP]\n");
        return false;
    }

    ToggleRelaySwitch(m_sRECORD, m_sOFF);       
    ToggleRelaySwitch(m_sCAMERA_POWER, m_sOFF);
    ToggleRelaySwitch(m_sVCR_POWER, m_sOFF);
    
    if(CheckInputState(nInputToMonitor) == HI)
    {
        SetMOOSVar("VIDEO_STATUS", m_sSTOPPED, MOOSTime());
        return true;
    }
    else
    {
        string sProblem = m_sSTOPPED + ": FAILED";
        SetMOOSVar("VIDEO_STATUS", sProblem,MOOSTime());
        return false;
    }

}


bool CRelayBoard::UpdateRelay(CMOOSMsg &Msg)
{
    string sNewRelayStates = Msg.m_sVal;
    string sMOOSChompResult = MOOSChomp(sNewRelayStates, ",");
    string sStatus;
       
    while(!sMOOSChompResult.empty())
    {
        string sRelay = MOOSChomp(sMOOSChompResult,":");
        string sState = sMOOSChompResult;
        if(ToggleRelaySwitch(sRelay,sState))
        {
            sStatus += sRelay + ": OK ";
        }
        else
        {
            sStatus += sRelay + ": FAIL ";
        }
        MOOSTrace("sRelay: %s sState: %s",sRelay.c_str(),sState.c_str());

        sMOOSChompResult = MOOSChomp(sNewRelayStates, ",");
    }

    //SetMOOSVar("RELAY_STATUS", sStatus, MOOSTime());
     MOOSTrace("sStatus: %s ", sStatus.c_str());
    return true;
    
}


/**.
 *Main access point for toggling a switch on the relay board
 */
bool CRelayBoard::ToggleRelaySwitch(string sRelay, string sDesiredState)
{
    int nRelayToSwitch = -1;
    //we know we are talking to the relay board's power switch
    RELAY_FUNCTION_2_TOGGLE_MAP::iterator p = m_RelayToggleMap.find(sRelay);
    if(p != m_RelayToggleMap.end())
    {
        nRelayToSwitch = p->second;
        MOOSTrace("[TBP]Toggling %d\n", nRelayToSwitch);
    }
    else
    {
        MOOSTrace("You have entered an incorrect Relay Name[TBP]\n");
        return false;
    }

    if(sDesiredState == m_sON)
    {
        if(!IsPowered(sRelay))
        {
           SetRelayHi(nRelayToSwitch);
        }
    }
    else if(sDesiredState == m_sOFF)
    {
        if(IsPowered(sRelay))
        {
           SetRelayLo(nRelayToSwitch);
        }
        
    }
    
    return ShowRelayBoardStatus();
}

/**
*@return whether or not a particular relay switch has power
*/
bool CRelayBoard::IsPowered(string sRelay)
{
    //figure out what relay we are trying to query
    int nRelayToCheck = -1;
    RELAY_FUNCTION_2_CHECK_MAP::iterator p = m_RelayCheckMap.find(sRelay);
   
    if(p != m_RelayCheckMap.end())
    {
        nRelayToCheck = p->second;

        if(m_Port.IsVerbose())
        {
             MOOSTrace("[IP]checking relay: %d\n", nRelayToCheck);
        }
    
    }

    
    return (CheckRelayState(nRelayToCheck) == HI);
}

bool CRelayBoard::SetRelayLo(short val)
{
    string sRelay = "MK";
    short MASK;
    char pChar[4];
    
    //should check the range of val
    if(val>255 || val < 0)
    {
        MOOSTrace("Acceptable Relay range 0-255\n");
        return false;
    }
    else
    {
        //we need to make a mask out of what the state of the RelayBoard
        //already is so that we do not shut off relays in the process of
        //trying to turn only a specific one off
        short nState = (short)CheckAllRelayStates();
        MASK = nState & ~val;
        
        //make note of this value we are writing
        sprintf(pChar, "%d\r", MASK);
         //The format for setting the relay is MKddd
        //where 'ddd' corresponds to the 3 characters in a 0-255 value short
        sRelay += pChar;
        //write the new command
        m_Port.Write((char *)sRelay.c_str(), sRelay.size());
    }
   
    
    return true;
    
}

/**@return the status of all relays in decimal format
*/
int CRelayBoard::CheckAllRelayStates()
{
    string sRelay = "PK\r";
    string sReply;
    double dfWhen = 1.0;
    
    //write the status command
    
    m_Port.Write((char *)sRelay.c_str(), sRelay.size());

    while(!m_Port.GetTelegram(sReply,dfWhen))
    {
        MOOSPause(10);
    }

    short nRet = (short)atof(sReply.c_str());

    if(m_Port.IsVerbose())
    {
        MOOSTrace("status of ALL relays: %d\n", nRet);
    }

    return nRet;

}

bool CRelayBoard::ShowRelayBoardStatus()
{
    
    string sStatus;
    //we will publish the state of the relays
    RELAY_FUNCTION_2_CHECK_MAP::iterator p;
    for(p = m_RelayCheckMap.begin(); p != m_RelayCheckMap.end(); p++)
    {
        string sRelay = p->first.c_str();
        sStatus += sRelay + (IsPowered(sRelay) ? ": ON " : ": OFF ");
    }
    //alert the DB
    SetMOOSVar("RELAY_STATUS", sStatus, MOOSTime());

    return true;
}
