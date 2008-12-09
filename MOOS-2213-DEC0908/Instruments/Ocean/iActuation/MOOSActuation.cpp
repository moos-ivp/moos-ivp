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
// MOOSActuation.cpp: implementation of the CMOOSActuation class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <MOOSLIB/MOOSLib.h>
#include "MOOSActuation.h"
#include "MOOSActuationDriver.h"
#include "MOOSSAILDriver.h"
#include "MOOSJRKerrDriver.h"
#include "MOOSASCDriver.h"
#include "MOOSBluefinDriver.h"

#include <iostream>
#include <math.h>

#define ACTUATION_WATCHDOG_PERIOD 10.0
#define HW_WATCHDOG_PERIOD 4.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSActuation::CMOOSActuation()
{

    m_bSimMode=false;

    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(5);
    SetCommsFreq(8);

    m_pDriver = NULL;

    m_dfLastRPMTime = MOOSTime();

}

CMOOSActuation::~CMOOSActuation()
{

}

bool CMOOSActuation::Iterate()
{

    for(int i = 0; i<sizeof(m_Motors)/sizeof(m_Motors[0]);i++)
    {
        if(m_Motors[i].HasExpired())
        {
            const char *Names[] = {"RUDDER","ELEVATOR","THRUST"};
            if(m_Port.IsVerbose())
            {
                MOOSTrace("watchdog reset %s=0\n",Names[i]);
            }
            //here we set the desired thriust to zero as
            //no one is telling us what to do - we may be
            //the only process left in the known universe
            MOOSTrace("*---*\n");
            m_Motors[i].SetDesired(0,MOOSTime());
        }

        CMOOSActuation::MotorName Name = (CMOOSActuation::MotorName)i;
        DoIO(Name);
    }


    if(!IsSimulateMode())
    {
        if(MOOSTime()-m_dfLastRPMTime>4.0)
        {
            m_Comms.Notify("ACTUATION_RPM",m_pDriver->GetRPM(),MOOSTime());
            m_dfLastRPMTime = MOOSTime();
        }

        //here we look after hardware wathcdog - we tell teh
        //hardware that the actuation driver process is still here
        //so don't shut down  on us!
        if(!HandleHWWatchDog())
            return false;
    }

    return true;
}



bool CMOOSActuation::DoIO(CMOOSActuation::MotorName Name)
{

    bool bResult = false;

    if(m_Motors[Name].IsUpdateRequired())
    {

        if(IsSimulateMode())
        {
            //here we send commands to the simulator
            //to instruct it to change thrust etc..
            string sVarName;
            switch(Name)
            {
            case THRUST:    sVarName = "SIM_DESIRED_THRUST";    break;
            case RUDDER:    sVarName = "SIM_DESIRED_RUDDER";    break;
            case ELEVATOR:    sVarName = "SIM_DESIRED_ELEVATOR";    break;
            }

            bResult = m_Comms.Notify(sVarName,m_Motors[Name].GetDesired());
        }
        else
        {
            double dfRudder, dfElevator;

            switch(Name)
            {

                //here the call to the driver itself are made...
            case THRUST:
                bResult = m_pDriver->SetThrust(m_Motors[Name].GetDesired());
                break;

            case RUDDER:
                dfRudder   = m_Motors[Name].GetDesired();
                dfElevator = m_Motors[ELEVATOR].GetDesired();
                MOOSTrace(MOOSFormat("Rudder in:  %.3f <Elev in %.3f>\n",  dfRudder, dfElevator));

                if(m_RollTransform.IsTransformRequired())
                {
                    m_RollTransform.Transform(dfRudder, dfElevator);

                    //if transforming, must also set elevator
                    bResult &= m_pDriver->SetRudder(dfRudder);
                    bResult &= m_pDriver->SetElevator(dfElevator);
                }
                else
                {
                    bResult = m_pDriver->SetRudder(dfRudder);
                }

                MOOSTrace(MOOSFormat("Rudder out:  %.3f <Elev out %.3f>\n",  dfRudder, dfElevator));
                break;

            case ELEVATOR:
                dfRudder   = m_Motors[RUDDER].GetDesired();
                dfElevator = m_Motors[Name].GetDesired();
                MOOSTrace(MOOSFormat("Elev in:  %.3f <Rudder in %.3f>\n", dfElevator, dfRudder));

                if(m_RollTransform.IsTransformRequired())
                {
                    m_RollTransform.Transform(dfRudder, dfElevator);

                    //if transforming, must also set elevator
                    bResult &= m_pDriver->SetRudder(dfRudder);
                    bResult &= m_pDriver->SetElevator(dfElevator);
                }
                else
                {
                    bResult = m_pDriver->SetElevator(dfElevator);
                }

                MOOSTrace(MOOSFormat("Elev out:  %.3f <Rudder out %.3f>\n", dfElevator, dfRudder));
                break;
            }

            OnActuationSet();


        }

        //and here we remember what the current setting is
        if(bResult == true)
        {
            m_Motors[Name].SetValue(m_Motors[Name].GetDesired(),MOOSTime());

        }

    }
    else
    {
        bResult = true;
    }


    return bResult;
}





bool CMOOSActuation::OnConnectToServer()
{

    //want to be told about pretty much every change..
    m_Comms.Register("DESIRED_THRUST",0.01);
    m_Comms.Register("DESIRED_ELEVATOR",0.01);
    m_Comms.Register("DESIRED_RUDDER",0.01);
    m_Comms.Register("ZERO_RUDDER",0.01);
    m_Comms.Register("ZERO_ELEVATOR",0.01);
    m_Comms.Register("RESET_ACTUATION",1.0);

    //to support transformations
    m_Comms.Register("INS_ROLL",0.01);

    return true;
}

bool CMOOSActuation::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;
    double dfTimeNow = MOOSTime();

    //to support transformations
    if(m_Comms.PeekMail(NewMail,"INS_ROLL",Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            m_RollTransform.SetValue(Msg.m_dfVal, dfTimeNow);
        }
    }

    if(m_Comms.PeekMail(NewMail,"DESIRED_THRUST",Msg,false,true))
    {
        m_Motors[THRUST].SetDesired(Msg.m_dfVal,dfTimeNow);
    }

    if(m_Comms.PeekMail(NewMail,"DESIRED_ELEVATOR",Msg,false,true))
    {
        m_Motors[ELEVATOR].SetDesired(Msg.m_dfVal,Msg.m_dfTime);
    }

    if(m_Comms.PeekMail(NewMail,"DESIRED_RUDDER",Msg,false,true))
    {
        m_Motors[RUDDER].SetDesired(Msg.m_dfVal,Msg.m_dfTime);
    }

    if(m_Comms.PeekMail(NewMail,"ZERO_RUDDER",Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            m_pDriver->SetZeroRudder();
        }
    }

    if(m_Comms.PeekMail(NewMail,"RESET_ACTUATION",Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            Reset();
        }
    }

    if(m_Comms.PeekMail(NewMail,"ZERO_ELEVATOR",Msg))
    {
        if(!Msg.IsSkewed(MOOSTime()))
        {
            m_pDriver->SetZeroElevator();
        }
    }

    return true;
}



bool CMOOSActuation::OnStartUp()
{

    //call base class version first...
    CMOOSInstrument::OnStartUp();


    if(IsSimulateMode())
    {
        SetAppFreq(5);
        SetCommsFreq(10);
    }
    else
    {
        if(!m_MissionReader.GetConfigurationParam("DRIVER",m_sDriverType))
        {
            MOOSTrace("Driver type not specified!!\n");
            MOOSPause(2000);
            return false;
        }

        //added to support actuation transforms 11/4/02
        string sTfm;
        if(!m_MissionReader.GetConfigurationParam("TRANSFORM",sTfm))
        {
            MOOSDebugWrite("Tailcone Transformation: NO");
            m_RollTransform.SetTransformRequired(false);
        }
        else
        {
            bool bTfm = MOOSStrCmp(sTfm, "TRUE") ? true : false;
            m_RollTransform.SetTransformRequired(bTfm);

            MOOSDebugWrite(MOOSFormat("Tailcone Transformation: %s", (bTfm ? "YES" : "NO")));
        }

        if(MOOSStrCmp(m_sDriverType,"SAIL"))
        {
            m_pDriver = new CMOOSSAILDriver;
            MOOSTrace("Loading SAIL driver\n");
        }
        else if(MOOSStrCmp(m_sDriverType,"ASC"))
        {
            m_pDriver = new CMOOSASCDriver;
            MOOSTrace("Loading ASC driver\n");
        }
        else if(MOOSStrCmp(m_sDriverType,"BLUEFIN"))
        {
            m_pDriver = new CMOOSBluefinDriver;
            MOOSTrace("Loading BLUEFIN driver\n");
        }
        else if(MOOSStrCmp(m_sDriverType,"JRKERR"))
        {
            m_pDriver = new CMOOSJRKerrDriver;
            MOOSTrace("Loading JRKerr driver\n");
        }
        else
        {
            MOOSTrace("Only SAIL,ASC and Bluefin Drivers supported in this release\n");
            return false;
        }

        //try to open
        if(!SetupPort())
        {
            return false;
        }

        m_pDriver->SetPort(&m_Port);

        double dfTmp=0;
        if(m_MissionReader.GetConfigurationParam("RudderOffset",dfTmp))
        {
            m_pDriver->SetRudderOffset(dfTmp);
        }

        if(m_MissionReader.GetConfigurationParam("ElevatorOffset",dfTmp))
        {
            m_pDriver->SetElevatorOffset(dfTmp);
        }


        if(Reset()==false)
        {
            return false;
        }



    }

    return true;
}

/**
*Roll transformation handled in this object.
*/
CMOOSActuation::RollTransform::RollTransform()
{
    m_dfVal = 0;
    m_dfDesired = 0;
    m_dfTimeSet = MOOSTime();
    m_dfTimeRequested = m_dfTimeSet;
    m_bTransform = false;
}

bool CMOOSActuation::RollTransform::Transform(double &dfRudder, double &dfElevator)
{

    //m_dfVal is the roll which comes from DB as radians
    MOOSTrace(MOOSFormat("Roll for Tform: %.3f\n", m_dfVal));
    double roll = m_dfVal;

    //input is in degrees, so convert to radians
    dfRudder   = MOOSDeg2Rad(dfRudder);
    dfElevator = MOOSDeg2Rad(dfElevator);
    MOOSTrace(MOOSFormat("radian conv: %.3f %.3f\n", dfRudder, dfElevator));

    double dfR =  cos(roll)*tan(dfRudder) + sin(roll)*tan(dfElevator);
    double dfE = -sin(roll)*tan(dfRudder) + cos(roll)*tan(dfElevator);

    MOOSTrace(MOOSFormat("new val: %.3f %.3f\n", dfR, dfE));

    //convert back to degrees!
    dfRudder   = MOOSRad2Deg(atan(dfR));
    dfElevator = MOOSRad2Deg(atan(dfE));

    return true;
}

bool CMOOSActuation::RollTransform::IsTransformRequired()
{
    return m_bTransform;
}

void CMOOSActuation::RollTransform::SetTransformRequired(bool bTForm)
{
    m_bTransform = bTForm;
}

CMOOSActuation::ActuatorDOF::ActuatorDOF()
{
    m_dfVal = 0;
    m_dfDesired = 0;
    m_dfTimeSet = MOOSTime();
    m_dfRefreshPeriod = ACTUATION_WATCHDOG_PERIOD;
    m_dfTimeRequested = m_dfTimeSet;
}

bool CMOOSActuation::ActuatorDOF::IsUpdateRequired()
{
    if(m_dfVal!=m_dfDesired)
        return true;

    if(MOOSTime()-m_dfTimeSet>m_dfRefreshPeriod)
        return true;



    return false;
}

bool CMOOSActuation::ActuatorDOF::HasExpired()
{
    double dfTimeNow = MOOSTime();
    double dfDT =dfTimeNow-m_dfTimeRequested;
    return (dfDT)>m_dfRefreshPeriod;
}

bool CMOOSActuation::ActuatorDOF::SetDesired(double dfVal,double dfTime)
{
    if(dfTime>=m_dfTimeRequested)
    {
        m_dfDesired =dfVal;
        m_dfTimeRequested = dfTime;
        return true;
    }
    else
    {
        return false;
    }
}

double CMOOSActuation::ActuatorDOF::GetDesired()
{
    return m_dfDesired;
}


bool CMOOSActuation::ActuatorDOF::SetValue(double dfVal,double dfTime)
{
    m_dfVal = dfVal;
    m_dfTimeSet = dfTime;
    return true;
}
/*
bool CMOOSActuation::IsASC()
{
return MOOSStrCmp(m_sDriverType,"ASC");
}


    bool CMOOSActuation::IsBluefin()
    {
    return MOOSStrCmp(m_sDriverType,"BLUEFIN");
    }
*/

bool CMOOSActuation::HandleHWWatchDog()
{
    if(IsSimulateMode())
        return true;

#define MAX(a,b) ((a)>(b)?(a):(b))
    double dfLastSet = MAX(MAX(m_Motors[THRUST].GetTimeSet(),m_Motors[RUDDER].GetTimeSet()), m_Motors[ELEVATOR].GetTimeSet());
    double dfDT = MOOSTime()-dfLastSet;

    if(dfDT>HW_WATCHDOG_PERIOD)
    {
        bool bResult = m_pDriver->SetThrust(m_Motors[THRUST].GetDesired());

        OnActuationSet();

        if(bResult)
        {
            MOOSTrace("***\n");
            m_Motors[THRUST].SetValue(m_Motors[THRUST].GetDesired(),MOOSTime());
        }
        else
        {
            return false;
        }


    }

    return true;
}

double CMOOSActuation::ActuatorDOF::GetTimeSet()
{
    return m_dfTimeSet;
}

bool CMOOSActuation::Reset()
{
    //ask for the resource to be turned on...
    m_Comms.Notify("JANITOR_SWITCH",m_sResourceName+":ON");
    MOOSTrace("Requesting \"%s\" power switch...",m_sResourceName.c_str());
    MOOSPause(2000);
    MOOSTrace("OK\n");

    double dfTimeNow = MOOSTime();

    m_Motors[THRUST].SetDesired(0,dfTimeNow);
    m_Motors[RUDDER].SetDesired(0,dfTimeNow);
    m_Motors[ELEVATOR].SetDesired(0,dfTimeNow);

    if(!IsSimulateMode() && !m_pDriver->Initialise())
    {
        MOOSTrace("Driver failed initialisation\n");
        return false;

    }
    return true;
}

bool CMOOSActuation::OnActuationSet()
{
    //we publish a variable to say we are still here...
    return m_Comms.Notify("ACTUATION_WD_HIT",MOOSTime());

}

