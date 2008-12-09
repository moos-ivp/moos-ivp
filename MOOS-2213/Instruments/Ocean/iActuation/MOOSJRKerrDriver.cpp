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
// MOOSJRKerrDriver.cpp: implementation of the CMOOSJRKerrDriver class.
//
// Driver for three JR Kerr / Kerr Automation Engineering
// PIC-SERVO-3PH motor controllers in a tailcone for an Odyssey II class vehicle
//
// This code does some slightly non-portable things like assume ints
// are always 32 bits and chars always 8 bits, but we'll live with it
// for now.
//
// 2/2002 - jmorash@alum.mit.edu
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "MOOSJRKerrDriver.h"

#define JRKERR_UNKNOWN -1;
#define JRKERR_ADDR_INITIAL    0x00
#define JRKERR_ADDR_GROUP      0xFF
//these addresses reflect the wiring in the aft sphere
#define JRKERR_ADDR_THRUSTER   0x01
#define JRKERR_ADDR_RUDDER     0x02
#define JRKERR_ADDR_ELEVATOR   0x03

//Command bytes are: upper nibble = number of following Data bytes, lower nibble = command number
#define JRKERR_RESET_POSITION       0x00
#define JRKERR_SET_ADDRESS          0x21
#define JRKERR_DEFINE_STATUS        0x12
#define JRKERR_READ_STATUS          0x13
#define JRKERR_LOAD_TRAJECTORY_POS  0xD4
#define JRKERR_LOAD_TRAJECTORY_PWM  0x24
#define JRKERR_START_MOTION         0x05
#define JRKERR_SET_GAINS            0xD6
#define JRKERR_STOP_MOTOR           0x17
#define JRKERR_IO_CONTROL           0x18
#define JRKERR_SET_HOME_MODE        0x19
#define JRKERR_SET_BAUD_RATE        0x1A
#define JRKERR_CLEAR_BITS           0x0B
#define JRKERR_SAVE_AS_HOME         0x0C
#define JRKERR_NOP                  0x0E
#define JRKERR_HARD_RESET           0x0F

#define JRKERR_MAX_ELEVATOR_ANGLE 45
#define JRKERR_MAX_RUDDER_ANGLE 45

#define JRKERR_ENCODER_TICKS_PER_DEGREE 13.194

#define JRKERR_FS_THRUST 255
#define JRKERR_ZERO_THRUST 0

#define HARDWARE_FAILURE_TO 4.0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSJRKerrDriver::CMOOSJRKerrDriver()
{
    m_bMoveDone = true;
    m_sLogFileName="JRKerrLog.txt";

    //these will be changed by SyncLog()
    m_dfRudder = 0;
    m_dfElevator = 0;
    m_dfRudderOffset = 0;
    m_dfElevatorOffset = 0;
}

CMOOSJRKerrDriver::~CMOOSJRKerrDriver()
{

}

void CMOOSJRKerrDriver::TraceKerrMessage(const char * pMsg,int nLen)
{


    //print out command sent (hex values)
    if(m_pPort->IsVerbose())
    {

        MOOSTrace("Sent %02X: %02X", (unsigned char)pMsg[1], (unsigned char)pMsg[2]);
        //print Data bytes, but not checksum
        for (int i=3; i < (nLen-1); i++)
        {
            MOOSTrace(" %02X", (unsigned char)pMsg[i]);
        }
        MOOSTrace("\n");
    }
}

bool CMOOSJRKerrDriver::SendCmd(int addr, int nCmd, const char *Data)
{
    //This function assembles, checksums and sends commands, then
    //deals with the returned status byte(s)

    int nDataLen=0;
    int nRead=0;
    char nStatByte=0;
    char str[32]="";


    char status[5]="";

    //Commands always start with 0xAA, followed by the module address,
    //then the command byte, then the associated Data bytes, then the
    //checksum.  Data length is implicit in the upper nibble of the
    //command byte. Checksum does not include the preamble (0xAA).

    nDataLen = (((unsigned char)nCmd & 0xF0) >> 4) & 0x0F;

    char Tx[100];
    int nNdx = 0;
    Tx[nNdx++] = (char)0xAA;
    Tx[nNdx++] = addr;
    Tx[nNdx++] = nCmd;
    int i=0;
    for (i=0;i<nDataLen;i++)
    {
        //send Data bytes
        Tx[nNdx++] = Data[i];
    }

    //now figure checksum. it's an 8bit wraparound 2's complement sum
    char cCheckSum = 0;
    for(i = 1;i<nNdx;i++)
    {
        cCheckSum+=Tx[i];
    }

    //append
    Tx[nNdx++] = cCheckSum;

    //write
    m_pPort->Write(Tx,nNdx);
    TraceKerrMessage(Tx,nNdx);

    //now deal with the returned Data.
    //first handle the initial status byte, which is always there.

    nRead = m_pPort->ReadNWithTimeOut(status,1);
    if (nRead > 0)
    {
        nStatByte = status[0];
    }
    else if (nCmd != JRKERR_HARD_RESET)
    {
        MOOSTrace("No response from JRKerr controller.\n");
        return false;
    }

    //then, if we're reading extra info,

    if (nCmd==JRKERR_READ_STATUS)
    {
        if (Data[0] & 0x20)
        {
            nRead = m_pPort->ReadNWithTimeOut(status,2);

            if (nRead > 0)
            {
                MOOSTrace("%02X: found device type %2d, version %2d\n", addr, status[0], status[1]);
            }
            else
            {
                MOOSTrace("Error reading device type and version\n");
            }
        }
    }

    //finally, read the status cksum
    nRead = m_pPort->ReadNWithTimeOut(status,1);

    if ((nRead <= 0) && (nCmd != JRKERR_HARD_RESET))
    {
        MOOSTrace("Error reading status checksum\n");
    }
    if (m_pPort->IsVerbose() && (nCmd!=JRKERR_HARD_RESET))
    {
        MOOSTrace("%02X replies: status 0x%02X\n", addr, (unsigned char)nStatByte);
    }

    if (nStatByte & 0x02)
    {
        MOOSTrace("Checksum error in last command\n");
    }

    if (nStatByte & 0x01)
    {
        m_bMoveDone = true;
    }
    else
    {
        m_bMoveDone = false;
    }

    return true;
}


bool CMOOSJRKerrDriver::LogPosition()
{
    //clobber open
    ofstream JRKerrLogFile;

    JRKerrLogFile.open(m_sLogFileName.c_str());

    if(JRKerrLogFile.is_open())
    {
        JRKerrLogFile<<"Rudder="<<m_dfRudder<<",Elevator="<<m_dfElevator<<endl;

        JRKerrLogFile.close();
    }
    return true;
}


bool CMOOSJRKerrDriver::SyncLog()
{
    //gentle open
    ifstream JRKerrLogFile;
    JRKerrLogFile.open(m_sLogFileName.c_str());

    if(JRKerrLogFile.is_open())
    {
        string sLine;
        char Tmp[1000];
        JRKerrLogFile.getline(Tmp,sizeof(Tmp));
        sLine = string(Tmp);
        if(!sLine.empty())
        {
            MOOSRemoveChars(sLine," \t");
            string sRudder = MOOSChomp(sLine,",");
            MOOSChomp(sRudder,"Rudder=");

            m_dfKerrRudderOffset = atof(sRudder.c_str());
            m_dfRudder = m_dfKerrRudderOffset;

            string sElevator  = sLine;
            MOOSChomp(sElevator,"Elevator=");
            m_dfKerrElevatorOffset = atof(sElevator.c_str());

            m_dfElevator = m_dfKerrElevatorOffset;
        }
        JRKerrLogFile.close();
    }
    return true;
}



bool CMOOSJRKerrDriver::Initialise()
{

    if(m_pPort==NULL)
        return false;

    //we keep a log file so fin position
    //is known if the driver crashes.
    SyncLog();

    if(m_pPort->IsVerbose())
    {
        MOOSTrace("iActuation Init() : Sending JRKerr init commands\n");
    }

    //First we send 16 null bytes, wait 'at least 1 ms' and flush the rx buffer
    //on the device
    for (int i=0; i<16; i++)
    {
        m_pPort->Write("\x00", 1);
    }
    MOOSPause(1);

    m_pPort->Flush();

    ///////////////////////////////////////////////////////////////////
    //              now send the commands
    ///////////////////////////////////////////////////////////////////
    char Data[14]="";
    bool bInitOK = true;

    //first, reset to powerup state
    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_GROUP, JRKERR_HARD_RESET, "\x00");

    //set addresses
    Data[0]=JRKERR_ADDR_THRUSTER;
    Data[1]=(char) 0xFF;
    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_INITIAL, JRKERR_SET_ADDRESS, Data);

    Data[0]=JRKERR_ADDR_RUDDER;
    Data[1]=(char)0xFF;
    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_INITIAL, JRKERR_SET_ADDRESS, Data);

    Data[0]=JRKERR_ADDR_ELEVATOR;
    Data[1]=(char)0xFF;
    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_INITIAL, JRKERR_SET_ADDRESS, Data);

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_THRUSTER, JRKERR_READ_STATUS, "\x20");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_RUDDER, JRKERR_READ_STATUS, "\x20");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_ELEVATOR, JRKERR_READ_STATUS, "\x20");

    //Set PID gains. Kp=5000, Kd=1000, Ki=0, IntLimit=0,
    //OutputLimit=255, CurrentLimit=0, ErrorLimit=10000, ServoRate=1
    //are fine values for all three actuators.
    //Multi-byte values are sent LSB first.

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_THRUSTER, JRKERR_SET_GAINS, "\x88\x13"
        "\xE8\x03"
        "\x00\x00"
        "\x00\x00"
        "\xFF"
        "\x00"
        "\x10\x27"
        "\x01");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_RUDDER, JRKERR_SET_GAINS,
        "\x88\x13\xE8\x03\x00\x00\x00\x00\xFF\x00\x10\x27\x01");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_ELEVATOR, JRKERR_SET_GAINS,
        "\x88\x13\xE8\x03\x00\x00\x00\x00\xFF\x00\x10\x27\x01");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_THRUSTER, JRKERR_STOP_MOTOR, "\x01");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_RUDDER, JRKERR_STOP_MOTOR, "\x01");

    bInitOK = bInitOK && SendCmd(JRKERR_ADDR_ELEVATOR, JRKERR_STOP_MOTOR, "\x01");



    if (!bInitOK)
    {
        MOOSTrace("JRKerr init failed!\n");
        return false;
    }
    else
    {
        MOOSTrace("JRKerr init OK\n");
        return true;
    }
}


bool CMOOSJRKerrDriver::DoPositionMove(int position, int velocity, int acceleration, int address)
{
    char nCmd[13]="";

    //wait until the previous move is complete.
    double dfTimeWaited = 0;
    while(!m_bMoveDone)
    {
        //this will set the m_bMoveDone flag when it's done moving - NOP just returns a status byte
        SendCmd(address, JRKERR_NOP, "\x00");
        MOOSPause(10);
        dfTimeWaited+=0.01;

        if(  dfTimeWaited>HARDWARE_FAILURE_TO)
        {
            MOOSTrace("JRKerr HW not responding! FAILED MOVE ASSUMED\n");
            return false;
        }
    }

    //command byte, then 4bytes pos, 4b vel, 4b accel
    //command byte = 10010111: start now, pos mode, load p,v,a
    nCmd[0]=(char)0x97;
    //now for some crazy fun byte-noodling. Send LSB first.
    nCmd[1]=(position & 0x000000FF);
    nCmd[2]=(position & 0x0000FF00)>>8;
    nCmd[3]=(position & 0x00FF0000)>>16;
    nCmd[4]=(position & 0xFF000000)>>24;
    nCmd[5]=(velocity & 0x000000FF);
    nCmd[6]=(velocity & 0x0000FF00)>>8;
    nCmd[7]=(velocity & 0x00FF0000)>>16;
    nCmd[8]=(velocity & 0xFF000000)>>24;
    nCmd[9]=(acceleration & 0x000000FF);
    nCmd[10]=(acceleration & 0x0000FF00)>>8;
    nCmd[11]=(acceleration & 0x00FF0000)>>16;
    nCmd[12]=(acceleration & 0xFF000000)>>24;

    return SendCmd(address, JRKERR_LOAD_TRAJECTORY_POS, nCmd);
}

bool CMOOSJRKerrDriver::SetElevator(double dfAng)
{


    //velocity and acceleration found by experiment, seem to work OK
    int position=0, velocity=100000, acceleration=100;

    if (dfAng > JRKERR_MAX_ELEVATOR_ANGLE)
    {
        dfAng = JRKERR_MAX_ELEVATOR_ANGLE;
    }
    if (dfAng < -JRKERR_MAX_ELEVATOR_ANGLE)
    {
        dfAng = -JRKERR_MAX_ELEVATOR_ANGLE;
    }
    position = int((dfAng - m_dfKerrElevatorOffset) * JRKERR_ENCODER_TICKS_PER_DEGREE);
    m_dfElevator = dfAng;
    LogPosition();

    return DoPositionMove(position, velocity, acceleration, JRKERR_ADDR_ELEVATOR);
}

bool CMOOSJRKerrDriver::SetRudder(double dfAng)
{



    //velocity and acceleration found by experiment, seem to work OK
    int position=0, velocity=100000, acceleration=100;

    if (dfAng > JRKERR_MAX_RUDDER_ANGLE)
    {
        dfAng = JRKERR_MAX_RUDDER_ANGLE;
    }
    if (dfAng < -JRKERR_MAX_RUDDER_ANGLE)
    {
        dfAng = -JRKERR_MAX_RUDDER_ANGLE;
    }
    position = int((dfAng - m_dfKerrRudderOffset) * JRKERR_ENCODER_TICKS_PER_DEGREE);
    m_dfRudder = dfAng;
    LogPosition();
    return DoPositionMove(position, velocity, acceleration, JRKERR_ADDR_RUDDER);
}

bool CMOOSJRKerrDriver::SetZeroElevator()
{
    m_dfElevator = 0;
    m_dfKerrElevatorOffset = 0;
    LogPosition();
    return SendCmd(JRKERR_ADDR_ELEVATOR, JRKERR_RESET_POSITION, "");
}

bool CMOOSJRKerrDriver::SetZeroRudder()
{

    m_dfRudder = 0;
    m_dfKerrRudderOffset = 0;
    LogPosition();
    return SendCmd(JRKERR_ADDR_RUDDER, JRKERR_RESET_POSITION, "");
}

bool CMOOSJRKerrDriver::SetThrust(double dfPercent)
{

    int nThrust;
    int nDirection=1; //1=fwd, 0=rev
    char nCmd[2]="";

    if(dfPercent>100)
    {
        dfPercent = 100.0;
    }
    if(dfPercent<-100)
    {
        dfPercent = -100.0;
    }

    nThrust = (int)((dfPercent/100.0) * JRKERR_FS_THRUST + JRKERR_ZERO_THRUST);

    if(nThrust<0) {
        nDirection=0;
        nThrust=-nThrust;
    }

    //control byte, then PWM byte
    //control byte = 0b1?001000 = start move now, PWM mode, ? = direction

    nCmd[0] = 0x88 | ((unsigned char)nDirection << 6);
    nCmd[1] = (unsigned char)nThrust;

    return SendCmd(JRKERR_ADDR_THRUSTER, JRKERR_LOAD_TRAJECTORY_PWM, nCmd);

}


double CMOOSJRKerrDriver::GetRPM()
{
    return 0;
}


