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
// ADVInstrument.cpp: implementation of the CADVInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
using namespace std;

#include "ADVInstrument.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CADVInstrument::CADVInstrument()
{
     //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(10);
    SetCommsFreq(10);

}

CADVInstrument::~CADVInstrument()
{

}

/////////////////////////////////////////////
///this is where it all happens..
bool CADVInstrument::Iterate()
{

    if(GetData())
    {
        PublishData();
    }

    return true;
}


bool CADVInstrument::OnStartUp()
{
    CMOOSInstrument::OnStartUp();

    //here we make the variables that we are managing
    double dfADVPeriod = 0.2;
    //ADV Referenced Velocity
    AddMOOSVariable("VEL_X",        "",        "ADV_X",        dfADVPeriod);
    AddMOOSVariable("VEL_Y",        "",        "ADV_Y",        dfADVPeriod);
    AddMOOSVariable("VEL_Z",        "",        "ADV_Z",        dfADVPeriod);
    AddMOOSVariable("VEL_SAMPLE_NO","",        "ADV_SAMPLE",   dfADVPeriod);
    
    //Amplitudes
    AddMOOSVariable("AMP_1",        "",        "ADV_AMP_1",    dfADVPeriod);
    AddMOOSVariable("AMP_2",        "",        "ADV_AMP_2",    dfADVPeriod);
    AddMOOSVariable("AMP_3",        "",        "ADV_AMP_3",    dfADVPeriod);
    
    //Correlation
    AddMOOSVariable("COR_1",        "",        "ADV_COR_1",   dfADVPeriod);
    AddMOOSVariable("COR_2",        "",        "ADV_COR_2",        dfADVPeriod);
    AddMOOSVariable("COR_3",        "",        "ADV_COR_3",        dfADVPeriod);
    
    //Register for the variable that will provide for dynamic configuration
    //of the ADV
    AddMOOSVariable("UPDATE_CMD", "ADV_UPDATE", "ADV_UPDATE_REPLY",    dfADVPeriod);        
 
    RegisterMOOSVariables();

    //use the *pfn callback routine in the SerialPort to notify that we are interested
    //in using the prompt given back to us by the ADV
    //Error Message for Unrecognized Command: ERROR: Command not recognized: DGS
    //Common to error message strings is ERROR

    SetPrompt(">");
    SetInstrumentErrorMessage("ERROR");
    
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
        if(!InitialiseSensorN(10,"ADV"))
        {
            return false;
        }          

    }
    return true;
}



bool CADVInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    double dfTimeNow = MOOSTime();
    
    if(m_Comms.PeekMail(NewMail,"ADV_UPDATE",Msg))
    {
        MOOSTrace("..............new ADV_UPDATE: %s\n", Msg.m_sVal.c_str());
        return ExecuteADVUpdateCommands(Msg);
    }
    else
    {
        return UpdateMOOSVariables(NewMail);
    }
}

bool CADVInstrument::PublishData()
{  
    return PublishFreshMOOSVariables();
}



bool CADVInstrument::OnConnectToServer()
{
    if(IsSimulateMode())
    {
        RegisterMOOSVariables();
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////
// here we initialise the sensor, giving it start up values
bool CADVInstrument::InitialiseSensor()
{   
#define MAXTRIES 10
    int nTries = 0;

    TERMPAIRLIST InitCmds;
    
    //STRING_LIST sInitCmds;
    
    //The ADV replies with a '>' prompt
    m_Port.SetTermCharacter(m_sPrompt.c_str()[0]);
    
    //Wake up the sensor
    m_Port.Break();
    MOOSPause(1000);
    
    InitCmds.push_back(SendTermPair("OF ASCII\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("VR 4\r",m_sPrompt));
    InitCmds.push_back(SendTermPair("SR 3.0\r",m_sPrompt.c_str()));
    InitCmds.push_back(SendTermPair("Start\r",m_sPrompt.c_str()));


    TERMPAIRLIST::iterator p;

    string sWhatReply;
    
    double dfWhen;
    
    for(p = InitCmds.begin(); p != InitCmds.end(); p)
    {
        SendTermPair & rToDo = *p;
        //string sCmd = *p;

        //make sure to pause before reading the reply
        //@9600bps, ADV may not react fast enough
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
 *The ADV returns data about the velocity of the robot
 *Currently using it in ASCII mode.  The returned strings are 
 *recognized in the ParseADVReply(string &sWhat) method, thus eliminating the need
 *of a 'for()' loop to grab all data in this method. 
 */

bool CADVInstrument::GetData()
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
            MOOSTrace("ADV Must be streaming..\n");
            
            return false;
            
        }


        ParseADVReply(sWhat);
  

    }
    else
    {
    //in simulated mode there is nothing to do..all data
    //arrives via comms.
    }

    return true;

}


bool CADVInstrument::GoodReply(string sReply)
{
    char * pPrompt = NULL;
    char * pError = NULL;

    //look for the prompt and an error
    //keep track of where each resides
    pPrompt = strstr(sReply.c_str(), m_sPrompt.c_str());
    pError = strstr(sReply.c_str(), m_sInstrumentErrorMessage.c_str());

    if((pError != NULL) && (pPrompt != NULL))
        return false;
    else if((pError != NULL) && (pPrompt == NULL))
        return false;
    else if((pError == NULL) && (pPrompt == NULL))
        return false;
    else
        return true;
    
}

bool CADVInstrument::ExecuteADVUpdateCommands(CMOOSMsg &Msg)
{
#define FRESH_UPDATE 30.0
    string sCmdUpdateStatus;
    double dfTimeNow = MOOSTime();

    //only interested in the string message of comma separated
    //values for the ADV.
    string sNewCmdString = Msg.m_sVal;
    MOOSTrace("Update ADV: Cmd - [%s] - Size - %d  \n", sNewCmdString.c_str(), sNewCmdString.size());
    
    //check for skewedness
    if(NewCommandsAreSkewed(Msg, dfTimeNow, sNewCmdString))
        return false;

    //Stop the ADV from streaming and get it's attention
    if(!GetADVAttention())
        return false;
    
    //send the commands
    if(SendADVUpdatedCommands(sCmdUpdateStatus, sNewCmdString))
    {
        //Set the UPDATE_ADV_REPLY variable to indicate success
        MOOSTrace("Update ADV: Pinging Restarted\n");
        SetMOOSVar("UPDATE_CMD", sCmdUpdateStatus, dfTimeNow);
        
        //reset the Termination character
        m_Port.SetTermCharacter('\r');

        //start the sensor pinging again
        StartPinging();

    }
    else
    {
        MOOSTrace("Update ADV: Failure - Pinging Not Restarted\n");
        SetMOOSVar("UPDATE_CMD", sCmdUpdateStatus, dfTimeNow);
    }
    
    //show the outcome to the DB
    PublishFreshMOOSVariables();

    return true;
}

/**
 *Starts the ADV pinging by sending "CS" command followed by <CR>
 */
void CADVInstrument::StartPinging()
{

    SendTermPair keepPinging("Start\r", m_sPrompt);
    m_Port.Write((char *)keepPinging.sSend.c_str(),keepPinging.sSend.size());

    return;
}

/**
 *Cycles through the string of new commands and checks for a successful update
 *
 */
bool CADVInstrument::SendADVUpdatedCommands(string &sCmdUpdateStatus, string sNewCmdString)
{
    string sMOOSChompResult = MOOSChomp(sNewCmdString, ",");
    double dfWhen;
    string sWhatReply;
    bool bSuccess = true;

    while(!sMOOSChompResult.empty())
    {
        //take action here and talk to the ADV
        //have to add a <CR> to the command
        string sADVUpdateFormattedCmd = sMOOSChompResult + "\r";
        
        //we are writing and then reading too quickly for this device
        //MOOSPause(500);

        m_Port.Write((char *)sADVUpdateFormattedCmd.c_str(),sADVUpdateFormattedCmd.size());

        while(!m_Port.GetLatest(sWhatReply,dfWhen))
        {
            MOOSPause(10);
        }

        if(GoodReply(sWhatReply))
        {
            MOOSTrace("Update ADV: Succesful Update - %s\n", sADVUpdateFormattedCmd.c_str());

            //keep track of the success
            sCmdUpdateStatus += sMOOSChompResult + ": OK; ";
        }
        else
        {
            MOOSTrace("Update ADV: Failure - %s\n", sADVUpdateFormattedCmd.c_str());
            
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
bool CADVInstrument::NewCommandsAreSkewed(CMOOSMsg & Msg, double dfTimeNow, string sNewCmdString)
{
    double dfSkewTime;
    string sCmdUpdateStatus;

    if((sNewCmdString.size() == 0) || 
       Msg.IsSkewed(dfTimeNow, &dfSkewTime))
    {
    
        if((sNewCmdString.size() == 0))
        {
            sCmdUpdateStatus = "Update ADV: Cmd SIZE = 0 - Aborted Update\n";
                MOOSTrace("%s", sCmdUpdateStatus.c_str());
            SetMOOSVar("UPDATE_CMD",sCmdUpdateStatus,MOOSTime());
        }
        else
        {
            sCmdUpdateStatus = "Update ADV: Cmd [" + sNewCmdString + "] SKEWED - Aborted Update\n";
                MOOSTrace("%s\n SkewTime: %f\n", sCmdUpdateStatus.c_str(), dfSkewTime); 
            SetMOOSVar("UPDATE_CMD",sCmdUpdateStatus,MOOSTime());
        }
                    
            return true;

        }
    else if(dfSkewTime > FRESH_UPDATE)
    {
        sCmdUpdateStatus = "Update ADV: Cmd [" + sNewCmdString + "] STALE - Aborted Update\n";
        MOOSTrace("%s\n SkewTime: %f\n", sCmdUpdateStatus.c_str(), dfSkewTime);
        SetMOOSVar("UPDATE_CMD",sCmdUpdateStatus,MOOSTime()); 

        return true;
    }
    else
    {
        MOOSTrace("Update ADV: Time Stamp - %f\n", Msg.m_dfTime);
        MOOSTrace("Update ADV: MOOS Time Stamp - %f\n", dfTimeNow);
    
        return false;
    }

}

/**
 *Send a Break over the serial line and check to see if the ADV replies with its prompt.
 */
bool CADVInstrument::GetADVAttention()
{
    string sWhatReply;
    double dfWhen;
    int nTries = 0;    

    //change the Termination Character we are looking for
    //and stop the ADV from talking
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
            MOOSTrace("Update ADV: No Prompt Found - Retrying Break\n");
            m_Port.Break();
        }
    }

    MOOSTrace("Update ADV: Check Cable Connection - Aborted Retry\n");
    
    return false;
    
}

/**
 *This method interprets the ADV strings taken from the serial port.  Be aware
 *that changing the settings of the sensor will change what strings it returns to us!
 *This method only returns attitude data at the moment.  The ADVInstrument will have to 
 *
 *+Velocities are in mm/s, need to convert this to m/s for our world
 */
bool CADVInstrument::ParseADVReply(string &sReply)
{
    ADVINFO_LIST InfoList;
    string sCopy = sReply;
    double dfTimeNow = MOOSTime();
   
    string sWhat = MOOSChomp(sCopy," ");
    if(sWhat.length() > 0)
    {
        //This was a response we are not expecting
        MOOSTrace("Unknown ADV reply %s\n",sWhat.c_str());
        return false;   
    }
    else
    {
        while(!sCopy.empty())
        {
            //step through the white space
            while((sWhat = MOOSChomp(sCopy," ")) == "")
                ;
             InfoList.push_back(sWhat);
        }
    }

    ADVINFO_LIST::iterator p;
    if(p == InfoList.end() || InfoList.size() < 10){
        MOOSTrace("Unknown ADV reply %s\n",sWhat.c_str());
        return false;
    }
    else
    {
        p = InfoList.begin();
        
        //the first col is the sample no
        int nSamp     = (int)atof((*p++).c_str());
        int nXVel     = (int)atof((*p++).c_str());
        int nYVel     = (int)atof((*p++).c_str());
        int nZVel    = (int)atof((*p++).c_str());
        int nAmp1     = (int)atof((*p++).c_str());
        int nAmp2     = (int)atof((*p++).c_str());
        int nAmp3    = (int)atof((*p++).c_str());
        int nCor1     = (int)atof((*p++).c_str());
        int nCor2     = (int)atof((*p++).c_str());
        int nCor3    = (int)atof((*p++).c_str());


        //SetMOOSVar here
        //Convert to m/s
        SetMOOSVar("VEL_SAMPLE_NO", nSamp, dfTimeNow);
        SetMOOSVar("VEL_X",nXVel*.001,dfTimeNow);
        SetMOOSVar("VEL_Y",nYVel*.001,dfTimeNow);
        SetMOOSVar("VEL_Z",nZVel*.001,dfTimeNow);
        SetMOOSVar("AMP_1",nAmp1,dfTimeNow);
        SetMOOSVar("AMP_2",nAmp2,dfTimeNow);
        SetMOOSVar("AMP_3",nAmp3,dfTimeNow);
        SetMOOSVar("COR_1",nCor1,dfTimeNow);
        SetMOOSVar("COR_2",nCor2,dfTimeNow);
        SetMOOSVar("COR_3",nCor3,dfTimeNow);
        
    }
    
    //we are here so we processed the string
    if(PublishRaw())
    {
        //yep user want to see raw data...
        m_Comms.Notify("ADV_RAW", sReply);
    }

    return true;
}
