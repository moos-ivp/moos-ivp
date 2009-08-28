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
// MOOSBluefinDriver.cpp: implementation of the CMOOSBluefinDriver class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include "MOOSBluefinDriver.h"

#define BLUEFIN_UNKNOWN -1;
#define BLUEFIN_THRUSTER 1
#define BLUEFIN_RUDDER 2
#define BLUEFIN_ELEVATOR 3
#define BLUEFIN_MAX_ELEVATOR_ANGLE 18.0
#define BLUEFIN_MAX_RUDDER_ANGLE 18.0

//this is for a 6 Mhz unit
#define BLUEFIN_ZERO_THRUST 92
#define BLUEFIN_FS_THRUST 40
#define VOLTS_2_RPM 1.87
#define BLUEFIN_THRUST_DEADZONE 10


#define BLUEFIN_MID_ANALOG_STROKE 144.0
#define ANALOG_2_COUNTS (4000.0/(248.0-67.0))

//#define BLUEFIN_ACTUATOR_FS 1608.69
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSBluefinDriver::CMOOSBluefinDriver()
{
    
    m_nSelectedActuation = BLUEFIN_UNKNOWN;
    
    
    
}

CMOOSBluefinDriver::~CMOOSBluefinDriver()
{

    
}



bool CMOOSBluefinDriver::Initialise()
{
    
    
    if(m_pPort != NULL )
    {
        
        STRING_LIST List;
        
        //Initialization isn't really needed, but it would be nice to
        //see the result of this. The TT8 will spit out several lines of
        //initialization data, which can be turned off as needed.
        
        //thruster
        List.push_back("BD1\r");
        List.push_back("M92\r");
        

        
        
        //rudder
        List.push_back("BD2\r");
//        List.push_back("SD400\r");
       List.push_back("SD300\r");
        List.push_back("SA10\r");
//        List.push_back("SM400\r");

        List.push_back("SM300\r");
        List.push_back("SF\r");
        List.push_back("SO\r");
        
        //elevator
        List.push_back("BD3\r");
        List.push_back("SD400\r");
        List.push_back("SA10\r");
        List.push_back("SM400\r");
        List.push_back("SF\r");
        List.push_back("SO\r");
        
        STRING_LIST::iterator p;
        
        for(p = List.begin();p!=List.end();p++)
        {
            string sCmd = *p;
            
            string sReply;
            
            if(m_pPort->IsVerbose())
            {
                MOOSTrace("iActuation Init() : Sending %s \n",sCmd.c_str());
            }
            //note bluefin tail cone does not reply to commands...
            //hence false
            if(!SendAndAck(sCmd,sReply,false))
            {
                MOOSTrace("Failed command\n");
            }
            MOOSPause(100);            
        }        
    }
    
    HomeActuators();

    //Bluefin hardware requires passing through a true zero point
    //briefly reversing does this....
    MOOSTrace("Energizing thruster...");
    SetThrust(-2);
    MOOSPause(1000);
    SetThrust(0);
    

    MOOSTrace("OK\n");
    
   
   
    return true;
}


string CMOOSBluefinDriver::GetBoardSelectString(int nActuator)
{
    switch(nActuator)
    {
    case BLUEFIN_THRUSTER: return "BD1\r";
    case BLUEFIN_RUDDER: return "BD2\r";
    case BLUEFIN_ELEVATOR: return "BD3\r";
    default:
        MOOSTrace("Unknown board! Dummy!\n");
        return "BD1\r";
    }
}

bool CMOOSBluefinDriver::SelectBoard(int nActuator)
{
    if(m_nSelectedActuation==nActuator)
        return true;
    
    string sReply;
    
    string sBoard = GetBoardSelectString(nActuator);
    
    //here we're checking to make sure the BD select command worked
    if(SendAndAck(sBoard,sReply,false))
    {
    //read EEPROM location zero - contains board ID #
    bool bResult = SendAndAck("RE0\r",sReply,true);
    if(bResult)
    {
        int nBoard = atoi(sReply.c_str());
        int nExpected = -1;
        switch(nActuator)
        {
        case BLUEFIN_THRUSTER: nExpected = 1;break;
        case BLUEFIN_RUDDER: nExpected = 2;break;
        case BLUEFIN_ELEVATOR: nExpected = 3;break;
        }
        if(nExpected!=nBoard)
        {
        MOOSTrace("Aw shucks board select didn't work\n\a");
        m_nSelectedActuation = BLUEFIN_UNKNOWN;
        return false;
        }
        else MOOSTrace("Switched to board %d\n", nBoard);
    }
    else
    {
        MOOSTrace("No reply when reading board ID\n\a");
        m_nSelectedActuation = BLUEFIN_UNKNOWN;
        return false;
    }
        m_nSelectedActuation = nActuator;
        return true;
    }
    else
    {
        m_nSelectedActuation = BLUEFIN_UNKNOWN;
        return false;
    }
}


bool CMOOSBluefinDriver::SetElevator(double dfAng,bool bAnalog)
{    

    //return true;


    if(fabs(dfAng)>BLUEFIN_MAX_ELEVATOR_ANGLE)
    {
        dfAng = BLUEFIN_MAX_ELEVATOR_ANGLE*fabs(dfAng)/dfAng;
    }

    
    return MoveTo(BLUEFIN_ELEVATOR, dfAng+m_dfElevatorOffset,bAnalog);      
}

bool CMOOSBluefinDriver::SetRudder(double dfAng,bool bAnalog)
{

    if(fabs(dfAng)>BLUEFIN_MAX_RUDDER_ANGLE)
    {
        dfAng = BLUEFIN_MAX_RUDDER_ANGLE*fabs(dfAng)/dfAng;
    }

    return MoveTo(BLUEFIN_RUDDER, dfAng+m_dfRudderOffset,bAnalog);      
}

bool CMOOSBluefinDriver::SetZeroElevator()
{
    
        
    
    return true;
}

bool CMOOSBluefinDriver::SetZeroRudder()
{
    
    return true;
}

bool CMOOSBluefinDriver::SetThrust(double dfPercent)
{
    if(!SelectBoard(BLUEFIN_THRUSTER))
        return false;
    
    
    int nThrust;
    
    if(dfPercent>100)
    {
        dfPercent = 100.0;
    }
    if(dfPercent<-100)
    {
        dfPercent = -100.0;
    }

    double dfOffset = dfPercent>0 ? 
                        BLUEFIN_THRUST_DEADZONE:
                        -BLUEFIN_THRUST_DEADZONE;
   
    if(dfPercent==0)
        dfOffset = 0;

    nThrust = (int)((dfPercent/100.0)* BLUEFIN_FS_THRUST + BLUEFIN_ZERO_THRUST+dfOffset );    
        
    if(nThrust<0)
        nThrust = 0;
    
    //now format motor command
    stringstream os;
    os<<"M"<<nThrust<<"\r"<<ends;
    string sCmd = os.str();
    
    
    string sReply;
    
    if(!SendAndAck(sCmd,sReply,false))
        return false;
    
    
    return true;
    
}



bool CMOOSBluefinDriver::HomeActuators()
{
    //home rudder
    SetElevator(0,true);

    //wait for it to get there....
    MOOSTrace("Homing elevator...");
    MOOSPause(2000);
    
    //set this to encoder position zero...
    string sReply;
    string sCmd = "HM0\r";
    SendAndAck(sCmd,sReply,false);
    
    MOOSTrace("done\n");


    //now do rudder
    SetRudder(0,true);

    //wait for it to get there....
    MOOSTrace("Homing rudder ");
    MOOSPause(2000);
    
    //set this to encoder position zero...
    sCmd = "HM0\r";
    SendAndAck(sCmd,sReply,false);
    
    MOOSTrace("done\n");
   
        
    return true;
    
    
}

//bool CMOOSBluefinDriver::HomeActuator(int nActuator)
//{
        
//    return true;
//}

bool CMOOSBluefinDriver::GetAnalogReading(double & dfPosition)
{
    string sReply;
    //stop immediately
//    SendAndAck("HI\r",sReply,false);
       
    //ability to average analog reading..
    int nMaxSamples = 4;
    int nSamples = 1;
    int nGood = 0;
    for(int n = 0;n<nMaxSamples && nGood<nSamples;n++)
    {
        string sCmd = "AD1\r";
        if(!SendAndAck(sCmd,sReply,true))
        {
            MOOSTrace("Failed Analog Read (try %d out of %d)\n\a",n+1,nMaxSamples);
        continue;
        }

    double dfJustRead = atof(sReply.c_str());

        if(dfJustRead> 0)
    {
        dfPosition+= dfJustRead;
        nGood++;
    }
    else
    {
        MOOSTrace("Experience has shown not to trust 0 volts!\n\a");
    }


    if(m_pPort->IsVerbose())
        {
            MOOSTrace("AD1 reply = %s\n",sReply.c_str());
        }
    
    }

    if(nGood==0)
    return false;

    //unwrap..
    dfPosition/=nGood;

    return true;

}

bool CMOOSBluefinDriver::MoveTo(int nActuation, double dfAngle,bool bAnalog/*=false*/)
{
    
    if(!SelectBoard(nActuation))
        return false;
 
    double dfStepPosition = 0;

    double dfAnalogRequired = AnalogFromAngle(nActuation,dfAngle,dfStepPosition);
    
    if(bAnalog)
    {    
    MOOSTrace("ANALOG!\n");
        //figure out where we are...    
    double dfAnalogPos =0;
    string sReply;
    SendAndAck("HI\r",sReply,false);
    
    if(!GetAnalogReading(dfAnalogPos))
        return false;
    
    double dfAnalogError =dfAnalogRequired -dfAnalogPos;

    if(fabs(dfAnalogError)<2)
        return true;

    double dfCountsToMove = dfAnalogError*ANALOG_2_COUNTS;
    
    //now format an incremental move..
    stringstream os;
    os<<"II"<<(int)dfCountsToMove<<"\r"<<ends;
    string sCmd = os.str();


    return SendAndAck(sCmd,sReply,false);
    }    
    else
    {
        //now format an ABSOLUTE move..
    MOOSTrace("NOT ANALOG\n");
    string sReply;
//    SendAndAck("HI\r",sReply,false);
//    SendAndAck("RC\r",sReply,true);
    
    if(fabs(dfStepPosition/ANALOG_2_COUNTS)<2)
        return true;

    stringstream os;
    os<<"MI"<<(int)dfStepPosition<<"\r"<<ends;
    string sCmd = os.str();
    return SendAndAck(sCmd,sReply,false);
  
    }

/*
    static ofstream Hmmm;
    if(!Hmmm.is_open())
    {
    Hmmm.open("HmmTest.txt");
    }

    if(nActuation == BLUEFIN_RUDDER)
    {
    Hmmm<<MOOSFormat("%.3f",MOOSTime()).c_str()<<"  ERROR: "<<dfAnalogError<<" DPOS: "<< dfAngle<<" ANPOS: "<<dfAnalogPos <<endl;    
    }
*/
    
   
    
}

double CMOOSBluefinDriver::AnalogFromAngle(int nActuation, double dfAng,double & dfStepPosition)
{
    double dfPercentRequired = 0;
    
    dfAng*=-1;

    double dfL1,dfL2,dfR0,dfTheta0;
    
    //for now we assume that the offsets applied by being able to
    //call anywhere zero do not affect the follwing equations
    //as the trim angles are likely to be small.
    switch(nActuation)
    {
    case BLUEFIN_RUDDER:
        dfL1     = 0.333;
        dfL2     =0.075;
        dfR0     = 0.302;
        dfTheta0 = 1.045;
        //     dfPercentRequired = dfAng/BLUEFIN_MAX_RUDDER_ANGLE*100;
        break;
    case BLUEFIN_ELEVATOR:
        dfL1     = 0.35;
        dfL2     =0.077;
        dfR0     = 0.311;
        dfTheta0 = 0.936;
        
        //       dfPercentRequired = dfAng/BLUEFIN_MAX_ELEVATOR_ANGLE*100;
        break;
    default:
        MOOSTrace("Only set stroke for rudder and elevator!\n");
        return 0;
    }
    
    double dfThetaCmd = MOOSDeg2Rad(dfAng)+dfTheta0;
    double dfRDes = sqrt(dfL1*dfL1-2*dfL1*dfL2*cos(dfThetaCmd)+dfL2*dfL2);
    dfStepPosition = (94488.189)*(dfRDes-dfR0);
    double dfAnalog = dfStepPosition/ANALOG_2_COUNTS+BLUEFIN_MID_ANALOG_STROKE;
 
    return dfAnalog;
}

double CMOOSBluefinDriver::GetRPM()
{
    return  0;
    if(!SelectBoard(BLUEFIN_THRUSTER))
        return 0;
    
    string sReply;
    
    int nDir = 1;
    if(SendAndAck("AD2\r",sReply,true))
    {
        double dfdir = atof(sReply.c_str());
        if(dfdir>0)
        {
            nDir = -1;
        }
    }   
    
    if(SendAndAck("AD1\r",sReply,true))
    {
        double dfVoltage = atof(sReply.c_str());

        if(m_pPort->IsVerbose())
        {
            MOOSTrace("RPM Voltage %f\n",dfVoltage);
        }
        
        double dfRPM = Volts2RPM(dfVoltage);
        
        return nDir*dfRPM;
    }
    
    return 0;
}

double CMOOSBluefinDriver::Volts2RPM(double dfVoltage)
{
    //here we write a polynomial fit to experimental data
    //matlab used for fit
    
    return VOLTS_2_RPM*dfVoltage;
}
