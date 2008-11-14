///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by the Seagrant AUV lab (R.Damus et.al)
//   and Paul Newman at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. rdamus@mit.edu
//      
//   This file is part of a  MOOS Instrument
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
// DVLInstrument.cpp: implementation of the CDVLInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include <cstring>

using namespace std;

#include "DVLInstrument.h"


#ifndef PI
#define PI 3.141592653589793
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDVLInstrument::CDVLInstrument()
{
    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(10);
    SetCommsFreq(10);
    m_dfLastSummary = 0;
    m_dfAlignment = -45;
    m_bExposeVelocities = false;
    m_eType = MOOS_DVL_SENSOR_RDI;           
}

CDVLInstrument::~CDVLInstrument()
{

}


/////////////////////////////////////////////
///this is where it all happens..
bool CDVLInstrument::Iterate()
{

    if(GetData())
    {
        PublishData();
    }

    return true;
}


bool CDVLInstrument::OnStartUp()
{
    CMOOSInstrument::OnStartUp();


    //here we make the variables that we are managing
    double dfDVLPeriod = 0.2;
    //Earth Referenced Distance Traveled
    AddMOOSVariable("EARTH_EAST",           "SIM_X",        "DVL_X",        dfDVLPeriod);
    AddMOOSVariable("EARTH_NORTH",    "SIM_Y",        "DVL_Y",        dfDVLPeriod);
    AddMOOSVariable("EARTH_UP",            "SIM_Z",        "DVL_Z",        dfDVLPeriod);
    AddMOOSVariable("EARTH_FRESH",        "",                "DVL_FRESHNESS",dfDVLPeriod);

    //Earth Referenced Velocity Data
    AddMOOSVariable("EARTH_VEL_EAST",    "",        "DVL_EARTH_VEL_X",        dfDVLPeriod);
    AddMOOSVariable("EARTH_VEL_NORTH",    "",        "DVL_EARTH_VEL_Y",        dfDVLPeriod);
    AddMOOSVariable("EARTH_VEL_UP",    "",        "DVL_EARTH_VEL_Z",        dfDVLPeriod);
    AddMOOSVariable("EARTH_VEL_STATUS",    "",        "DVL_EARTH_VEL_STATUS",    dfDVLPeriod);
    
    //AUV referenced Velocity Data
    AddMOOSVariable("AUV_VEL_EAST",        "",        "DVL_BODY_VEL_X",        dfDVLPeriod);   
    AddMOOSVariable("AUV_VEL_NORTH",    "",        "DVL_BODY_VEL_Y",        dfDVLPeriod);
    AddMOOSVariable("AUV_VEL_UP",    "",        "DVL_BODY_VEL_Z",        dfDVLPeriod);
    AddMOOSVariable("AUV_VEL_STATUS",    "",        "DVL_AUV_VEL_STATUS",    dfDVLPeriod);

    //AUV referenced Attitude Data
    AddMOOSVariable("PITCH",    "SIM_PITCH",    "DVL_PITCH",    dfDVLPeriod);
    AddMOOSVariable("ROLL",        "SIM_ROLL",        "DVL_ROLL",        dfDVLPeriod);
    AddMOOSVariable("YAW",        "SIM_YAW",      "DVL_YAW",        dfDVLPeriod);
    AddMOOSVariable("HEADING",    "SIM_HEADING",    "DVL_HEADING",    dfDVLPeriod);
    AddMOOSVariable("SPEED",    "SIM_SPEED",    "DVL_SPEED",    dfDVLPeriod);        
    AddMOOSVariable("ALTITUDE",    "SIM_ALTITUDE",    "DVL_ALTITUDE",    dfDVLPeriod);        
   
    //Register for the variable that will provide for dynamic configuration
    //of the DVL
    AddMOOSVariable("UPDATE_CMD", "DVL_UPDATE", "DVL_UPDATE_REPLY",    dfDVLPeriod);        
 
    RegisterMOOSVariables();

    //get DVL alignment
    m_dfAlignment = -45;
    m_MissionReader.GetConfigurationParam("Alignment",m_dfAlignment);


    //search the setup file for the Magnetic Offset
    GetMagneticOffset();

    //use the *pfn callback routine in the SerialPort to notify that we are interested
    //in using the prompt given back to us by the RDI
    //Error Message for Unrecognized Command: ERR 010:  UNRECOGNIZED COMMAND
    //Error Message for Out of bounds Command: ERR 045: PARAMETER OUT OF BOUNDS
    //Common to both error message strings is ERR

    SetPrompt(">");
    SetInstrumentErrorMessage("ERR");
    
    if(IsSimulateMode())
    {
        SetAppFreq(10);
        SetCommsFreq(8);
    }
    else
    {

        //try to open 
        if(!SetupPort())
        {
            return false;
        }
        
        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"DVL"))
        {
            return false;
        }          

    }
    return true;
}



bool CDVLInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    double dfTimeNow = MOOSTime();
    

    if(m_Comms.PeekMail(NewMail,"DVL_SUMMARY_REQUIRED",Msg))
    {
        m_bExposeVelocities = MOOSStrCmp(Msg.m_sVal,"TRUE");
    }

    if(m_Comms.PeekMail(NewMail,"DVL_UPDATE",Msg))
    {
        MOOSTrace("..............new DVL_UPDATE: %s\n", Msg.m_sVal.c_str());
        ExecuteDVLUpdateCommands(Msg);
    }

    return UpdateMOOSVariables(NewMail);
}

bool CDVLInstrument::PublishData()
{
    if(m_bExposeVelocities && MOOSTime()-m_dfLastSummary>2.0)
    {
        m_dfLastSummary = MOOSTime();

        CMOOSVariable * pBVX = GetMOOSVar("AUV_VEL_EAST");
        CMOOSVariable * pBVY = GetMOOSVar("AUV_VEL_NORTH");

        string sText  = "NO DVL";

        if(pBVX && pBVY)
        {

            sText = MOOSFormat("DVL Body Vel Y = %7.2f X = %7.2f %s",
                pBVY->GetDoubleVal(),
                pBVX->GetDoubleVal(),
                (pBVX->IsFresh() && pBVY->IsFresh()) ? "OK":"STALE");
               
        }

        MOOSDebugWrite(sText);

    }

    return PublishFreshMOOSVariables();
}



bool CDVLInstrument::OnConnectToServer()
{
    if(IsSimulateMode())
    {
        RegisterMOOSVariables();

    }

    m_Comms.Register("DVL_SUMMARY_REQUIRED",0.0);

    return true;
}


///////////////////////////////////////////////////////////////////////////
// here we initialise the sensor, giving it start up values
bool CDVLInstrument::InitialiseSensor()
{   
#define MAXTRIES 10
    int nTries = 0;

    
    TERMPAIRLIST InitCmds;
    
    //STRING_LIST sInitCmds;
    
    //The DVL replies with a '>' prompt
    m_Port.SetTermCharacter(m_sPrompt.c_str()[0]);
    
    //Wake up the sensor
    m_Port.Break();
    MOOSPause(1000);
  
    //XXX: Navigator DVL for Caribou does not recognize all the commands
    //XXX: that the Workhorse on Xanthos does.  Therefore, commented out
    //XXX: lines reflect this change - 3/12/02
  
    InitCmds.push_back(SendTermPair("CB411\r", m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("CF11010\r",m_sPrompt.c_str()));
//    InitCmds.push_back(SendTermPair("CD111110000\r",m_sPrompt));
    InitCmds.push_back(SendTermPair("PD6\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("TC0000\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("TE00:00:00.00\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("TP00:01.00\r",m_sPrompt.c_str()));
//    InitCmds.push_back(SendTermPair("WP1\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("BM5\r",m_sPrompt.c_str()));
//    InitCmds.push_back(SendTermPair("BX00050\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("BP1\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("BK0\r",m_sPrompt.c_str()));
//    InitCmds.push_back(SendTermPair("BA030\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("BS\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("EX10110\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("EZ1011101\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("ED0\r",m_sPrompt.c_str()));
 
    
    string sEA = MOOSFormat("EA%d\r",-(int)m_dfAlignment*100);
    string sEB = MOOSFormat("EB%d\r",(int)m_dfAlignment*100);

    InitCmds.push_back(SendTermPair(sEA,m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair(sEB,m_sPrompt.c_str()));

    InitCmds.push_back(SendTermPair("CS\r",m_sPrompt.c_str()));
    

    //sInitCmds.push_back("CB411\r\n");


    //STRING_LIST::iterator p;
    TERMPAIRLIST::iterator p;

    string sWhatReply;
    
    double dfWhen;
    
    for(p = InitCmds.begin(); p != InitCmds.end(); p)
    {
        SendTermPair & rToDo = *p;
        //string sCmd = *p;

        //make sure to pause before reading the reply
        //@9600bps, DVL may not react fast enough
        MOOSPause(500);

        while(!m_Port.GetLatest(sWhatReply,dfWhen))
        {
            MOOSPause(10);
        }

        //check reply...
        //if(GoodReply(sCmd, sWhatReply))
        if(GoodReply(sWhatReply))
        {
            m_Port.Write((char *)rToDo.sSend.c_str(),rToDo.sSend.size());
            //m_Port.Write((char *)rToDo.sSend.c_str(),rToDo.sSend.size());
            //m_Port.Write((char*)sCmd.c_str(),sCmd.size());

            p++;

        }
        else
        {
            MOOSTrace("D'oh!\n");
            if(nTries++ > MAXTRIES)
                return false;
        }

        
    }

    //Need to put the device back in a mode where it looks for <CR>
    //to delimit a Telegram
    m_Port.SetTermCharacter('\r');

    return true;
}


/**
 *The DVL returns data about the Attitude of the vehicle: Pitch, Roll, Heading.
 *Currently using it in PD6 mode, with Bottom Track enabled, thus we will receive
 *information about the velocity in XYZ w.r.t. bottom.  The returned strings are 
 *recognized in the ParseRDIReply(string &sWhat) method, thus eliminating the need
 *of a 'for()' loop to grab all data in this method. 
 */

bool CDVLInstrument::GetData()
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
            MOOSTrace("DVL Must be streaming..\n");
            
            return false;
            
        }


        ParseRDIReply(sWhat);
  

    }
    else
    {
    //in simulated mode there is nothing to do..all data
    //arrives via comms.
    }

    return true;

}

/**
 *This method interprets the RDI strings taken from the serial port.  Be aware
 *that changing the settings of the sensor will change what strings it returns to us!
 *This method only returns attitude data at the moment.  The DVLInstrument will have to 
 *Publish new variables in order to accomodate the bottom track related information.
 *RDI Setup: 
 *+Output Data format PD6
 *+No Water Reference Data
 *+Bottom Track enabled 
 *+Velocities are in mm/s, need to convert this to m/s for our world
 */
bool CDVLInstrument::ParseRDIReply(string &sReply)
{
    string sCopy = sReply;
    string sWhat = MOOSChomp(sReply,",");
    
    double dfTimeNow = MOOSTime();

    if(sWhat==":SA")
    {
        //Vehicle Attitude Information
        double dfPitch     = atof(MOOSChomp(sReply,",").c_str());
        double dfRoll     = atof(MOOSChomp(sReply,",").c_str());
        double dfHeading = atof(MOOSChomp(sReply,",").c_str());

        SetMOOSVar("PITCH",PI/180.0*dfPitch,dfTimeNow);
        SetMOOSVar("YAW",PI/180.0*dfHeading,dfTimeNow);
        SetMOOSVar("ROLL",PI/180.0*dfRoll,  dfTimeNow);
        SetMOOSVar("HEADING",dfHeading,  dfTimeNow);

        
    }
    else if (sWhat == ":BI")
    {
        //Bottom Track, Instrument Referenced Velocity Data
        //
        //X axis: Beam1 Beam2 xdcr movement relative to bottom
        //Y axis: Beam4 Beam3 xdcr movement relative to bottom
        //Z axis: Transducre movement away from bottom
        //Error is the error velocity
        //The status is either 'A' = Good or 'V' = Bad
        int nXaxisVel     = (int)atof(MOOSChomp(sReply,",").c_str());
        int nYaxisVel     = (int)atof(MOOSChomp(sReply,",").c_str());
        int nZaxisVel    = (int)atof(MOOSChomp(sReply,",").c_str());
        int nErrorVel    = (int)atof(MOOSChomp(sReply,",").c_str());
        string sStatus   = MOOSChomp(sReply,",");
        
        if (sStatus == "A")
        {
            //SetMOOSVar here
            //Convert to m/s
        }
        else if (sStatus == "V")
        {
            //alert to failure
        }
    
    }
    else if (sWhat == ":BS")
    {
        //Bottom Track, Ship Referenced Velocity Data
        //
        //RDI defines positive as Right Handed with Y extending through the Bow
        //and X extending out through Starboard.  This makes Z positive upwards,
        //extending away from the bottom.
        //
        //Transverse is the Port-Stbd ship movement
        //Longitudinal is the Aft-Fwd ship movement
        //Normal is the ship movement away from bottom
        //The status is either 'A' = Good or 'V' = Bad
        int nTransVel    = (int)atof(MOOSChomp(sReply,",").c_str());
        int nLongVel     = (int)atof(MOOSChomp(sReply,",").c_str());
        int nNormalVel   = (int)atof(MOOSChomp(sReply,",").c_str());
        string sStatus   = MOOSChomp(sReply,",");

        if (sStatus == "A")
        {
            //SetMOOSVar here
            //Convert to m/s
            SetMOOSVar("AUV_VEL_EAST",nTransVel*.001,dfTimeNow);
            SetMOOSVar("AUV_VEL_NORTH",nLongVel*.001,dfTimeNow);
            SetMOOSVar("AUV_VEL_UP",nNormalVel*.001,dfTimeNow);
            SetMOOSVar("AUV_VEL_STATUS",sStatus,dfTimeNow);
        }
        else if (sStatus == "V")
        {
            //alert to failure
        SetMOOSVar("AUV_VEL_STATUS",sStatus,dfTimeNow);
        }

    
    }
    else if (sWhat == ":BE")
    {
        //Bottom Track, Earth Referenced Velocity Data
        //
        //East is the ADCP movement to East
        //North is the ADCP movement to North
        //Upward is the ADCP movement to Surface
        //The status is either 'A' = Good or 'V' = Bad
        int nEastVel     = (int)atof(MOOSChomp(sReply,",").c_str());
        int nNorthVel     = (int)atof(MOOSChomp(sReply,",").c_str());
        int nUpwardVel   = (int)atof(MOOSChomp(sReply,",").c_str());
        string sStatus   = MOOSChomp(sReply,",");

        if (sStatus == "A")
        {
            //SetMOOSVar here
            //Convert to m/s
            SetMOOSVar("EARTH_VEL_NORTH",nNorthVel*.001,dfTimeNow);
            SetMOOSVar("EARTH_VEL_EAST",nEastVel*.001,dfTimeNow);
            SetMOOSVar("EARTH_VEL_UP",nUpwardVel*.001,dfTimeNow);
            SetMOOSVar("EARTH_VEL_STATUS",sStatus,dfTimeNow);
        }
        else if (sStatus == "V")
        {
            //alert to failure
            SetMOOSVar("EARTH_VEL_STATUS",sStatus,dfTimeNow);
        }
        
    }
    else if (sWhat == ":BD")
    {
        //Bottom Track, Earth Referenced Distance Data
        //
        //East is the distance East in meters
        //North is the distance North in meters
        //Upward is the distance Upward in meters
        //Bottom is the range to bottom in meters
        //Time is the Time since last good velocity estimate in seconds
        double dfEastDist      = atof(MOOSChomp(sReply,",").c_str());
        double dfNorthDist     = atof(MOOSChomp(sReply,",").c_str());
        double dfUpDist        = atof(MOOSChomp(sReply,",").c_str());
        double dfBottomDist    = atof(MOOSChomp(sReply,",").c_str());
        double dfTimeSinceGood = atof(MOOSChomp(sReply,",").c_str());

        if(dfBottomDist!=0 && dfTimeSinceGood<3.0)
        {
            //no status here so use time and altitude as measure of success
            SetMOOSVar("ALTITUDE",dfBottomDist,dfTimeNow);
            SetMOOSVar("EARTH_NORTH",dfNorthDist,dfTimeNow);
            SetMOOSVar("EARTH_EAST",dfEastDist,dfTimeNow);
            SetMOOSVar("EARTH_UP",dfUpDist,dfTimeNow);
            SetMOOSVar("EARTH_FRESH",dfTimeSinceGood,dfTimeNow);
        }            
    }
    else if(sWhat == ":TS")
    {
        MOOSChomp(sReply,",");

        double dfSalinity = atof(MOOSChomp(sReply,",").c_str());
        double dfTemp = atof(MOOSChomp(sReply,",").c_str());
        double dfDepth = atof(MOOSChomp(sReply,",").c_str());
        double dfSoundSpeed = atof(MOOSChomp(sReply,",").c_str());
        double dfBIT = atof(MOOSChomp(sReply,",").c_str());
    }
    else
    {
        //This was a response we are not expecting
        MOOSTrace("Unknown RDI reply %s\n",sWhat.c_str());
        return false;
    }


    //we are here so we processed the string
    if(PublishRaw())
    {
        //yep user want to see raw data...
        m_Comms.Notify("DVL_RAW",sCopy);
    }

    return true;
}





bool CDVLInstrument::GoodReply(string sReply)
{

    //look for the prompt and an error
    //keep track of where each resides
    const char *  pPrompt = strstr(sReply.c_str(), m_sPrompt.c_str());
    const char *  pError = strstr(sReply.c_str(), m_sInstrumentErrorMessage.c_str());

    if((pError != NULL) && (pPrompt != NULL))
    return false;
    else if((pError != NULL) && (pPrompt == NULL))
    return false;
    else if((pError == NULL) && (pPrompt == NULL))
    return false;
    else
    return true;
    
}


bool CDVLInstrument::ExecuteDVLUpdateCommands(CMOOSMsg &Msg)
{
#define FRESH_UPDATE 30.0
    string sCmdUpdateStatus;
    double dfTimeNow = MOOSTime();

    //only interested in the string message of comma separated
    //values for the DVL.
    string sNewCmdString = Msg.m_sVal;
    MOOSTrace("Update DVL: Cmd - [%s] - Size - %d  \n", sNewCmdString.c_str(), sNewCmdString.size());
    
    //check for skewedness
    if(NewCommandsAreSkewed(Msg, dfTimeNow, sNewCmdString))
    return false;

    //Stop the DVL from streaming and get it's attention
    if(!GetDVLAttention())
    return false;
    
    //send the commands
    if(SendDVLUpdatedCommands(sCmdUpdateStatus, sNewCmdString))
    {
    //Set the UPDATE_DVL_REPLY variable to indicate success
        MOOSTrace("Update DVL: Pinging Restarted\n");
        SetMOOSVar("UPDATE_CMD", sCmdUpdateStatus, dfTimeNow);
    
    //reset the Termination character
    m_Port.SetTermCharacter('\r');

    //start the sensor pinging again
    StartPinging();

    }
    else
    {
        MOOSTrace("Update DVL: Failure - Pinging Not Restarted\n");
        SetMOOSVar("UPDATE_CMD", sCmdUpdateStatus, dfTimeNow);
    }
    
    //show the outcome to the DB
    PublishFreshMOOSVariables();

    return true;
}

/**
 *Starts the DVL pinging by sending "CS" command followed by <CR>
 */
void CDVLInstrument::StartPinging()
{

    SendTermPair keepPinging("CS\r", m_sPrompt);
    m_Port.Write((char *)keepPinging.sSend.c_str(),keepPinging.sSend.size());

    return;
}

/**
 *Cycles through the string of new commands and checks for a successful update
 *
 */
bool CDVLInstrument::SendDVLUpdatedCommands(string &sCmdUpdateStatus, string sNewCmdString)
{
    string sMOOSChompResult = MOOSChomp(sNewCmdString, ",");
    double dfWhen;
    string sWhatReply;
    bool bSuccess = true;

    while(!sMOOSChompResult.empty())
    {
        //take action here and talk to the DVL
        //have to add a <CR> to the command
        string sDVLUpdateFormattedCmd = sMOOSChompResult + "\r";
        
        //we are writing and then reading too quickly for this device
        //MOOSPause(500);

        m_Port.Write((char *)sDVLUpdateFormattedCmd.c_str(),sDVLUpdateFormattedCmd.size());

        while(!m_Port.GetLatest(sWhatReply,dfWhen))
        {
            MOOSPause(10);
        }

        if(GoodReply(sWhatReply))
        {
            MOOSTrace("Update DVL: Succesful Update - %s\n", sDVLUpdateFormattedCmd.c_str());

            //keep track of the success
            sCmdUpdateStatus += sMOOSChompResult + ": OK; ";
        }
        else
        {
            MOOSTrace("Update DVL: Failure - %s\n", sDVLUpdateFormattedCmd.c_str());
            
            //keep track of the failure
            sCmdUpdateStatus += sMOOSChompResult + ": FAIL; ";
            
            if(bSuccess)
                bSuccess = !bSuccess;
        }
    
        sMOOSChompResult = MOOSChomp(sNewCmdString, ",");
        
    }

    return bSuccess;
}

/**
 *Make sure that the commands being sent are not skewed commands.
 *that is left over from a database write from a while ago
 *and that the user has actually entered in a new command.
 *Should also eventually check this against allowable commands to be
 *sent to the device.
 */
bool CDVLInstrument::NewCommandsAreSkewed(CMOOSMsg & Msg, double dfTimeNow, string sNewCmdString)
{
    double dfSkewTime;
    string sCmdUpdateStatus;

    if((sNewCmdString.size() == 0) || 
       Msg.IsSkewed(dfTimeNow, &dfSkewTime))
    {
        if((sNewCmdString.size() == 0))
    {
        sCmdUpdateStatus = "Update DVL: Cmd SIZE = 0 - Aborted Update\n";
            MOOSTrace("%s", sCmdUpdateStatus.c_str());
        SetMOOSVar("UPDATE_CMD",sCmdUpdateStatus,MOOSTime());
    }
    else
    {
        sCmdUpdateStatus = "Update DVL: Cmd [" + sNewCmdString + "] SKEWED - Aborted Update\n";
            MOOSTrace("%s\n SkewTime: %f\n", sCmdUpdateStatus.c_str(), dfSkewTime); 
        SetMOOSVar("UPDATE_CMD",sCmdUpdateStatus,MOOSTime());
    }
                    
        return true;

    }
    else if(dfSkewTime > FRESH_UPDATE)
    {
    sCmdUpdateStatus = "Update DVL: Cmd [" + sNewCmdString + "] STALE - Aborted Update\n";
    MOOSTrace("%s\n SkewTime: %f\n", sCmdUpdateStatus.c_str(), dfSkewTime);
    SetMOOSVar("UPDATE_CMD",sCmdUpdateStatus,MOOSTime()); 

    return true;
    }
    else
    {
        MOOSTrace("Update DVL: Time Stamp - %f\n", Msg.m_dfTime);
        MOOSTrace("Update DVL: MOOS Time Stamp - %f\n", dfTimeNow);
    
    return false;
    }

}

/**
 *Send a Break over the serial line and check to see if the DVL replies with its prompt.
 */
bool CDVLInstrument::GetDVLAttention()
{
    string sWhatReply;
    double dfWhen;
    int nTries = 0;    

    //change the Termination Character we are looking for
    //and stop the DVL from talking
    m_Port.SetTermCharacter(m_sPrompt.c_str()[0]);
    m_Port.Break();
        
    //check reply...
    while(nTries++ < MAXTRIES)
    {  
        //wait a second to let it stop streaming
        MOOSPause(1000);

        while(!m_Port.GetLatest(sWhatReply,dfWhen))
        {
            MOOSPause(10);
        }

        if(GoodReply(sWhatReply))
        {
        return true;
        }
        else
        {
            MOOSTrace("Update DVL: No Prompt Found - Retrying Break\n");
            m_Port.Break();
        }
    }

    MOOSTrace("Update DVL: Check Cable Connection - Aborted Retry\n");
    
    return false;
    
}
