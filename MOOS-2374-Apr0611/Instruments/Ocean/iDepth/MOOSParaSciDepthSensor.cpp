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
// MOOSParaSciDepthSensor.cpp: implementation of the CMOOSParaSciDepthSensor class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include "MOOSParaSciDepthSensor.h"
#include <cstring>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

CMOOSParaSciDepthSensor::CMOOSParaSciDepthSensor()
{
    m_dfResolution = 0.03;


}

CMOOSParaSciDepthSensor::~CMOOSParaSciDepthSensor()
{

}


bool CMOOSParaSciDepthSensor::Initialise()
{
    std::string sReply;

    //set units to be m of water
    string sUnits = "*0100EW*0100UN=8\r";
    if(m_pPort->Write((char*)sUnits.c_str(),sUnits.size()))
    {
        m_pPort->GetTelegram(sReply,1.0);

        MOOSTrace("set units cmd returns %s\n",sReply.c_str());
    }

    //set resolution to be as set in config file

    int nRes = (int)(7000.0/(150*28*m_dfResolution));

    char sTmp[100];

    sprintf(sTmp,"*0100EW*0100PR=%d\r",nRes);

    string sRes = sTmp;
    if(m_pPort->Write((char*)sRes.c_str(),sRes.size()))
    {
        m_pPort->GetTelegram(sReply,1.0);
        MOOSTrace("set units cmd returns %s\n",sReply.c_str());
    }

    MOOSPause(200);

    //we zero the depth sensor at start up...
    Zero();

    return true;
}


bool CMOOSParaSciDepthSensor::GetDepth()
{

    const char *     sGetDepthString =  "*0100P3\r";

    if( m_pPort->Write(sGetDepthString,
             strlen(sGetDepthString)))
    {
        //last thing we did was write so now rea
        string sReply;
        if(m_pPort->GetTelegram(sReply,3.0))
        {
            //sReply now has ASCII depth
            return ParseDepthString(sReply);
        }
        else
        {
            MOOSTrace("Depth Sensor: Failed Read on serial port\n");
            return false;
        }
    }


    return true;

}

bool CMOOSParaSciDepthSensor::SetResolution(double dfResolution)
{
    m_dfResolution = dfResolution;
    return true;
}

bool CMOOSParaSciDepthSensor::Zero()
{
    //zero sensor
    string sZero = "*0100EW*0100ZS=1\r";

    if(m_pPort->Write((char*)sZero.c_str(),sZero.size()))
    {
        string sReply;
        m_pPort->GetTelegram(sReply,1.0);
        MOOSTrace("zeroing cmd returns %s\n",sReply.c_str());

        //and read once to do the zeroing
        GetDepth();

    }

    return true;

}


bool CMOOSParaSciDepthSensor::ParseDepthString(string Str)
{
    MOOSChomp(Str,"*0001");

    if(!Str.empty())
    {
        //we keep a local copy of depth just for ease of debugging
        m_dfDepth = atof(Str.c_str());

        return true;

    }
    else
    {
        return false;
    }

}
