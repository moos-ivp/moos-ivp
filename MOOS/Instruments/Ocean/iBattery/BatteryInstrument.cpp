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
// BatteryInstrument.cpp: implementation of the CBatteryInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>


#include <iostream>
#include <strstream>
#include <math.h>
using namespace std;


#include "BatteryDriver.h"
#include "BluefinBatteryDriverV1.h"
#include "BluefinBatteryDriverV2.h"
#include "KeithleyBatteryDriver.h"
#include "BatteryInstrument.h"

#define DEFAULT_FULL_VOLTS 48.0
#define DEFAULT_EMPTY_VOLTS 40.0
#define BATTERY_PERIOD 5.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBatteryInstrument::CBatteryInstrument()
{
    m_dfLastIteration=0;
    m_pBatterySensor = NULL;
}

CBatteryInstrument::~CBatteryInstrument()
{
    if(m_pBatterySensor!=NULL)
    {
        delete m_pBatterySensor;
    }
}
bool CBatteryInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    if(m_Comms.PeekMail(NewMail,"BATTERY_CONTROL",Msg,true))
    {
    if(m_pBatterySensor!=NULL && !Msg.IsSkewed(MOOSTime()))
        {
            bool bSuccess = true;
            bool bOn = MOOSStrCmp(Msg.m_sVal,"ON");

            bSuccess = m_pBatterySensor->Switch(bOn);
            
            if(bSuccess)
            {
                string sState = (bOn?"ON":"OFF");
                string sFeedback = "Battery is "+ sState;
                this->MOOSDebugWrite(sFeedback);
            }


        }
    }
   return true;

}
bool CBatteryInstrument::OnConnectToServer()
{
    m_Comms.Register("BATTERY_CONTROL",0);

    return true;
}
bool CBatteryInstrument::Iterate()
{
    if(!IsSimulateMode())
    {
    if(MOOSTime()-m_dfLastIteration<BATTERY_PERIOD)
    {
        return true;
    }
    else
    {
        m_dfLastIteration = MOOSTime();
    }
    
    if(GetData())
    {
        PublishData();
    }    
    }
    return true;
}

bool CBatteryInstrument::OnStartUp()
{

    CMOOSInstrument::OnStartUp();

    //here we make the variables that we are managing
    double dfKeithleyPeriod = 2;

    //Keithley update @ 1Hz
    AddMOOSVariable("Keithley","SIM_VOLTAGE","KEITHLEY_BATT_VOLTAGE",dfKeithleyPeriod);
    AddMOOSVariable("KEITHLEY_RAW","","KEITHLEY_RAW",dfKeithleyPeriod);
           
    if(IsSimulateMode())
    {
        RegisterMOOSVariables();
    }    
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }

    if(!MakeDriver())
        return false;

    //what is the fully charged battery voltage?
        double dfFull=DEFAULT_FULL_VOLTS;
        m_MissionReader.GetConfigurationParam("FULLVOLTS",dfFull);
    m_pBatterySensor->SetFullVolts(dfFull);

        double dfEmpty=DEFAULT_EMPTY_VOLTS;
        m_MissionReader.GetConfigurationParam("EMPTYVOLTS",dfEmpty);
    m_pBatterySensor->SetEmptyVolts(dfEmpty);
        
    //figure out at what voltage we should issue a batery low message!
    string sWarning;
        m_MissionReader.GetConfigurationParam("LOWFLAG",sWarning);
    string sFlag = MOOSChomp(sWarning,"@");
    string sLevel = sWarning;
    
    if(!sLevel.empty())
    {
        double dfTmp = atof(sLevel.c_str());
        if(dfTmp>0)
        {
        m_sWarningFlag = sFlag;
        m_pBatterySensor->SetLowVolts(dfTmp);
        }
    }

    //tell teh driver what port to use..
        m_pBatterySensor->SetSerialPort(&m_Port);
        
        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"Battery"))
        {
            return false;
        }          
        
    }
    
    return true;
}


bool CBatteryInstrument::InitialiseSensor()
{
    return m_pBatterySensor->Initialise();          
}

bool CBatteryInstrument::GetData()
{
    return m_pBatterySensor->GetData();
}

bool CBatteryInstrument::PublishData()
{
    CBatteryDriver::CBatteryStatus CurrentStatus;
    
    if(m_pBatterySensor->GetStatus(CurrentStatus))
    {
        m_Comms.Notify("BATTERY_VOLTAGE",CurrentStatus.GetVoltage());
        m_Comms.Notify("BATTERY_CHARGE",CurrentStatus.GetPercentCharge());
        m_Comms.Notify("BATTERY_STATUS",CurrentStatus.GetStatusAsString());

        if(CurrentStatus.GetStatus()==CBatteryDriver::CBatteryStatus::LOW)
        {
            MOOSTrace("Battery is low: %f V!\n",CurrentStatus.GetVoltage());
            if(!m_sWarningFlag.empty())
            {
                m_Comms.Notify(m_sWarningFlag,CurrentStatus.GetVoltage());
            }
        }

        //publish cell details
        string sCells = m_pBatterySensor->GetCellsString();
        if(!sCells.empty())
        {
            m_Comms.Notify("BATTERY_CELL_DETAILS",sCells);            
        }

        //publish all errors
        string sError = m_pBatterySensor->GetErrorString();
        if(!sError.empty())
        {
            m_Comms.Notify("BATTERY_ERROR",sError);            
        }

        //publish cell comments
        string sComment = m_pBatterySensor->GetCommentString();
        if(!sComment.empty())
        {
            m_Comms.Notify("BATTERY_COMMENT",sComment);            
        }


        return true;
    }
    else
    {
        return false;
    }
    
}

bool CBatteryInstrument::MakeDriver()
{
    m_pBatterySensor=NULL;

    string sType;
    m_MissionReader.GetConfigurationParam("TYPE",sType);
    
    if(MOOSStrCmp(sType,"KEITHLEY"))
    {
        m_pBatterySensor = new CKeithleyBatteryDriver;
        MOOSTrace("Making Keithley battery driver\n");
    }
    else if (MOOSStrCmp(sType,"BLUEFIN_V1"))
    {
        m_pBatterySensor = new CBluefinBatteryDriverV1;
        MOOSTrace("Making Bluefin \"binary\" battery driver\n");
    }
    else if (MOOSStrCmp(sType,"BLUEFIN_V2"))
    {
        m_pBatterySensor = new CBluefinBatteryDriverV2;
        MOOSTrace("Making Bluefin \"smart\" battery driver\n");
    }
    else
    {
        MOOSTrace("Battery type must be one of KEITHLEY or BLUEFIN_V1 or BLUEFIN_V2 \n");
        MOOSTrace("Assuming KEITHLEY...! \n");
        m_pBatterySensor = new CKeithleyBatteryDriver;
    }

    return m_pBatterySensor!=NULL;
}
