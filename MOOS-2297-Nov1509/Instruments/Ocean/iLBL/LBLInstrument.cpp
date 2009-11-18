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
// LBLInstrument.cpp: implementation of the CLBLInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include <math.h>
using namespace std;


#include "LBLInstrument.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLBLInstrument::CLBLInstrument()
{
    m_bInhibit = false;
    m_dfLastPingTime = 0;
    m_dfPingPeriod = 1.5;
}

CLBLInstrument::~CLBLInstrument()
{

}





/////////////////////////////////////////////
///this is where it all happens..
bool CLBLInstrument::Iterate()
{

 
    if(GetData())
    {
        PublishData();
    }

    return true;
}


bool CLBLInstrument::GetData()
{
    if(!IsSimulateMode())
    {
        if(MOOSTime()-m_dfLastPingTime<m_dfPingPeriod)
        {
            return false;
        }
        
        m_dfLastPingTime = MOOSTime();

        if(!m_bInhibit)
        {
            if(m_AVTRAK.GetRanges())
            {
                string sResult;
                if(m_AVTRAK.GetTOFString(sResult))
                {
                    SetMOOSVar("LBL_TOF",sResult,MOOSTime());
                }
                else
                {
                    return false;
                }

                for(int i = 1;i<=MAX_AVTRAK_CHANNELS;i++)
                {
                    string sName;
                    double dfTimeRx;
                    double dfTOF;
                    if(m_AVTRAK.GetTOFByChannel(i,sName,dfTimeRx,dfTOF))
                    {
                        m_Comms.Notify(sName,dfTOF,dfTimeRx);
                    }
                }
            }
        }
    }
    return true;
}


////////////////////////////////////////////////////////////
// tell the world
bool CLBLInstrument::PublishData()
{
    return PublishFreshMOOSVariables();
}


bool CLBLInstrument::OnStartUp()
{
    //call base class member first
    CMOOSInstrument::OnStartUp();
    
    //here we make the variables that we are managing
    AddMOOSVariable("LBL_TOF",  "SIM_LBL_TOF",  "LBL_TOF",  0);


    if(IsSimulateMode())
    {
        //not much to do...othe than register for input from
        //simulator ...
        RegisterMOOSVariables();

        //we want to be very responsive....
        SetCommsFreq(20);
        SetAppFreq(10);
        
    }
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }

        m_AVTRAK.SetSerialPort(&m_Port);

        //try 10 times to initialise sensor
        if(!InitialiseSensorN(2,"LBL"))
        {
    //        return false;
        }          


        //now set stuff up
        string sChannels;
        if(m_MissionReader.GetConfigurationParam("RXCHANNELS",sChannels))
        {
            INT_VECTOR Chans;
            while(!sChannels.empty())
            {
                int nChan = atoi(MOOSChomp(sChannels,",").c_str());
                Chans.push_back(nChan);
            }

            m_AVTRAK.SetRxChannel(Chans);
        }

        double dfRxTimeOut = 2.0;
        m_MissionReader.GetConfigurationParam("RXTIMEOUT",dfRxTimeOut);
        m_AVTRAK.SetAcousticTimeOut(dfRxTimeOut);


        m_MissionReader.GetConfigurationParam("PINGEVERY",m_dfPingPeriod);


    }



  



    return true;
}



bool CLBLInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;

    if(m_Comms.PeekMail(NewMail,"LBL_INHIBIT",Msg,true))
    {
        if(MOOSStrCmp(Msg.m_sVal,"TRUE"))
        {
            m_bInhibit  = true;
        }
        else
        {
            m_bInhibit  = false;
        }
    }
    if(IsSimulateMode())
    {
        //we want a very quick response here - don't update a moos
        //variable which may be wriiten over again before the next
        //mailout is perfomed - forward it now instead
        while(m_Comms.PeekMail(NewMail,"SIM_LBL_TOF",Msg,true))
        {
            //this message has now beed removed from the NewMail list 
            //(true passed in PeekMail)
            m_Comms.Notify("LBL_TOF",Msg.m_sVal,Msg.m_dfTime);
            /*MOOSTrace("LBLMsg: MOOStime = %.3f MsgTime = %.3f Data = %s\n",
                        MOOSTime(),
                        Msg.m_dfTime,
                        Msg.m_sVal.c_str());*/
        //    Msg.Trace();
        }
    }

    return UpdateMOOSVariables(NewMail);
}




bool CLBLInstrument::OnConnectToServer()
{

    m_Comms.Register("LBL_INHIBIT",0);


    if(IsSimulateMode())
    {
        //not much to do...
        m_Comms.Register("SIM_LBL_TOF",0);
        
        return RegisterMOOSVariables();
        
    }
    
    return true;
}





// here we initialise the sensor, giving it start up values
bool CLBLInstrument::InitialiseSensor()
{    
    return m_AVTRAK.Reset();    
}


