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
// INSInstrument.cpp: implementation of the CINSInstrument class.
//
//////////////////////////////////////////////////////////////////////
// CompassInstrument.cpp: implementation of the CINSInstrument class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include <sstream>
#include <math.h>
using namespace std;
#include "INSInstrument.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define CROSSBOW_POLLED_ANGLE_MODE_REPLY_LENGTH 30

bool INSPortReadCallBack(char * pData, int nBufferLen,int nRead)
{
    return nRead==CROSSBOW_POLLED_ANGLE_MODE_REPLY_LENGTH;
}

CINSInstrument::CINSInstrument()
{
    m_dfMagneticOffset;
    m_nTempCnt = 0;
}

CINSInstrument::~CINSInstrument()
{
    
}


/////////////////////////////////////////////
///this is where it all happens..
bool CINSInstrument::Iterate()
{
    if(GetData())
    {
        PublishData();
    }
    
    return true;
}

////////////////////////////////////////////////////////////
// tell the world
bool CINSInstrument::PublishData()
{
    return PublishFreshMOOSVariables();
    
}


bool CINSInstrument::OnStartUp()
{
    //call base class member first
    CMOOSInstrument::OnStartUp();
    
    //here we make the variables that we are managing
    double dfINSPeriod = 0.2;
    
    
    if(!m_MissionReader.GetConfigurationParam("TWIST",m_dfVehicleYToCrossBowX))
    {
        m_dfVehicleYToCrossBowX = 0;
    }
    
    //INS update @ 2Hz
    AddMOOSVariable("Heading",  "SIM_HEADING",  "INS_HEADING",  dfINSPeriod);
    AddMOOSVariable("Yaw",      "SIM_YAW",      "INS_YAW",      dfINSPeriod);
    AddMOOSVariable("Temperature", "",          "INS_TEMPERATURE", dfINSPeriod);
    
    //    AddMOOSVariable("YawRate",  "SIM_YAWRATE",  "INS_YAWRATE",  dfINSPeriod);
    AddMOOSVariable("Pitch",    "SIM_PITCH",    "INS_PITCH",    dfINSPeriod);
    AddMOOSVariable("Roll",     "SIM_ROLL",     "INS_ROLL",     dfINSPeriod);
    
    //we shall need the diference between true north and magnetic north.
    GetMagneticOffset();
    
    if(IsSimulateMode())
    {
        //not much to do...othe than register for input from
        //simulator ...
        RegisterMOOSVariables();
    }
    else
    {
        //try to open 
        if(!SetupPort())
        {
            return false;
        }
        
        m_Port.SetIsCompleteReplyCallBack(INSPortReadCallBack);
        
        //try 10 times to initialise sensor
        if(!InitialiseSensorN(10,"INS"))
        {
            return false;
        }          
    }
    
    
    return true;
}



bool CINSInstrument::OnNewMail(MOOSMSG_LIST &NewMail)
{
    return UpdateMOOSVariables(NewMail);
}




bool CINSInstrument::OnConnectToServer()
{
    if(IsSimulateMode())
    {
        //not much to do...
        return RegisterMOOSVariables();
        
    }
    else
    {
        
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////
// here we initialise the sensor, giving it start up values
bool CINSInstrument::InitialiseSensor()
{    
    //set to polled moade in angles...
    m_Port.Write("P",1);        
    MOOSPause(100);
    
    //set to angle mode
    m_Port.Write("a",1);     
    
    char Spare[10];
    m_Port.ReadNWithTimeOut(Spare,1);
    
    if(Spare[0]!='A')
    {
        MOOSTrace("Unexpected reply when setting angle mode (expecting 'A')\n");
        return false;
    }
    
    
    return true;
    
}

bool CINSInstrument::GetData()
{
    
    if(!IsSimulateMode())
    {
        //here we actually access serial ports etc
        if(m_Port.IsStreaming())
        {
            MOOSTrace("Crossbow must not be streaming\n");
            return false;
        }
        
        
        m_Port.Write("G",1);
        
        
        string sWhat;
        
        unsigned char Reply[30];
        
        //note local call back invoked here to specify termination
        int nRead = m_Port.ReadNWithTimeOut((char*)Reply,sizeof(Reply));
    
        
        if(nRead ==CROSSBOW_POLLED_ANGLE_MODE_REPLY_LENGTH)
        {
            if(Reply[0]!=255)
            {
                MOOSTrace("Unexpected Header in CrossBow reply\n");
            }
            
            short nRoll = (Reply[1]<<8) + Reply[2];
            short nPitch = (Reply[3]<<8) + Reply[4];
            short nYaw = (Reply[5]<<8) + Reply[6];
            
            double dfCBRoll   = nRoll*180.0/pow(2.0,15.0);
            double dfCBPitch  = nPitch*90.0/pow(2.0,15.0);
            double dfCBYaw    = nYaw*180.0/pow(2.0,15.0);
            
            //acount for alignment of crossbow in vehicle frame
            //this is the angle measured from the vehicle ceneter line(y)
            //to teh x axis of teh crossbow unit. Note that if this is non zero then
            //we'll have to do a real 3 transfgormation of axe sto use pitch correctly
            //this is good enough to get us going for now but more needs to be done.
            double dfHeading = dfCBYaw+m_dfVehicleYToCrossBowX;
            
            //no correct for magnetic offset
            dfHeading+=m_dfMagneticOffset;
            
            //convert to Yaw..
            double dfYaw = -dfHeading*PI/180.0;
            dfYaw = MOOS_ANGLE_WRAP(dfYaw);
            
            
            //look after pitch
            double dfPitch = MOOSDeg2Rad(dfCBPitch);
            
            //look after roll
            double dfRoll = MOOSDeg2Rad(dfCBRoll);
            
            
            //find the temperature every so often
            m_nTempCnt++;
            if((m_nTempCnt % 100) == 0)
            {
                short nTemp   = ( Reply[25] << 8 ) + Reply[26];
                double dfTemp = 44.4 * ( ((double)nTemp * 5.0/4096.0) - 1.375); 
                SetMOOSVar( "Temperature", dfTemp, MOOSTime() );
            }
            
            //set our local variables...
            SetMOOSVar("Heading",dfHeading,MOOSTime());
            SetMOOSVar("Yaw",dfYaw,MOOSTime());
            SetMOOSVar("Pitch",dfPitch,MOOSTime());
            SetMOOSVar("Roll",dfRoll,MOOSTime());
            
            
            if(m_Port.IsVerbose())
            {
                //this allows us to print data in verbose mode
                //when teh port couldn't as it is verbose
                MOOSTrace("Roll = %7.3f Pitch = %7.3f Yaw = %7.3f deg\n",
                    dfCBRoll,
                    dfCBPitch,
                    dfCBYaw);
            }
            
        }
        else
    {
        MOOSTrace("read %d byte while expecting %d\n",
              nRead,
              CROSSBOW_POLLED_ANGLE_MODE_REPLY_LENGTH);
    }
        
        
    }
    else
    {
        //in simulated mode there is nothing to do..all data
        //arrives via comms.
    }
    
    return true;
    
}


