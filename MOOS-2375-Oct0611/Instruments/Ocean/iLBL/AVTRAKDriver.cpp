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
// AVTRAKDriver.cpp: implementation of the CAVTRAKDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "AVTRAKDriver.h"
#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CAVTRAKDriver::CAVTRAKDriver()
{


    m_dfAcousticTimeOut = 5.0;


}

CAVTRAKDriver::~CAVTRAKDriver()
{

}

bool CAVTRAKDriver::SetRxChannel(INT_VECTOR Channels)
{
    m_RxOrder = Channels;
    sort(m_RxOrder.begin(),m_RxOrder.end());

    return SetRangingParams();

}

bool CAVTRAKDriver::SetRangingParams()
{
    INT_VECTOR::iterator p;

    stringstream os;

    os.setf(ios::fixed);

    os<<">SR"<<setprecision(1)<<m_dfAcousticTimeOut;

    for(p = m_RxOrder.begin();p!=m_RxOrder.end();p++)
    {
        os<<","<<*p;
    }
    os<<"\r\n"<<ends;


    string sCmd = os.str();


    m_pPort->Write((char *)sCmd.c_str(),sCmd.length());

    string sReply;
    if(!m_pPort->GetTelegram(sReply,2.0))
    {
        MOOSTrace("Failed AVTRAK Ranging Configuration, no reply\n");
        return false;
    }
    else
    {

        if(sReply.find("OK")!=string::npos)
        {
            MOOSChomp(sReply,"<SR");
            
            m_dfAcousticTimeOut = atof(MOOSChomp(sReply,",").c_str());

            m_RxOrder.clear();
            while(!sReply.empty())
            {
                int nChan = atoi(MOOSChomp(sReply,",").c_str());
                m_RxOrder.push_back(nChan);
            }
        }
        else
        {
            MOOSTrace("Failed AVTRAK Ranging Configuration : %s \n",sReply.c_str());
            return false;
        }

    }


    return true;
}

bool CAVTRAKDriver::GetRanges()
{

    m_TOFs.clear();

    stringstream os;

    os<<">MR\r\n"<<ends;

    string sCmd = os.str();
    

    m_pPort->Write((char *)sCmd.c_str(),sCmd.length());


    double dfLastTxTime = MOOSTime()+AVTRAK_RANGING_TX_DELAY;

    string sReply;
    if(!m_pPort->GetTelegram(sReply,m_dfAcousticTimeOut+1.0))
    {
        MOOSTrace("Failed AVTRAK Ranging Command, no reply\n");
        return false;
    }
    else
    {
        if(m_pPort->IsVerbose())
        {

        }
        MOOSChomp(sReply,"<MR");

        INT_VECTOR::iterator p = m_RxOrder.begin();

        while(!sReply.empty() && p!=m_RxOrder.end())
        {
            int     nChan = *p++;
            double  dfTOF = atof(MOOSChomp(sReply,",").c_str());

            CTwoWayTOF TOF;

            TOF.m_nChannel = nChan;
            TOF.m_dfTOF = dfTOF/1000000;

            //here we apply the sonardyne correction!!
            //specila lover insider information!!
            TOF.m_dfTOF-=0.8e-3; 

        if(TOF.m_dfTOF>0)
        {
        
        TOF.m_dfTxTime = dfLastTxTime;
            
        m_TOFs.push_back(TOF);

        MOOSTrace("Chan[%d] = %f s %f m\n",
                    TOF.m_nChannel,
                    TOF.m_dfTOF,
                    (TOF.m_dfTOF-GetTAT(TOF.m_nChannel))/2*1500);
        }
        }
    }

    return true;

}

bool CAVTRAKDriver::GetTOFString(string & sResult)
{

    //format is Tx=tt.tt,Ch[i]=tt.tt,Ch[j]=tt.tt....
    if(m_TOFs.empty())
        return false;

    stringstream os;

    os.setf(ios::fixed);
    os.precision(3);
     
    os<<"Tx="<<m_TOFs.front().m_dfTxTime;

    os.precision(6);

    TOF_VECTOR::iterator p;

    for(p = m_TOFs.begin();p!=m_TOFs.end();p++)
    {
        CTwoWayTOF TOF = *p;
        if(TOF.m_dfTOF!=0)
        {
            os<<",Ch["<<TOF.m_nChannel<<"]="<<TOF.m_dfTOF;
        }
    }
    os<<ends;

    sResult = os.str();

    return true;
}



bool CAVTRAKDriver::SetTransceiverMode()
{
    string sReply,sCmd;

    sCmd = ">TC\r\n",
    m_pPort->Write((char*)sCmd.c_str(),sCmd.size());


    if(m_pPort->GetTelegram(sReply,2.0))
    {
        if(sReply.find("OK")!=string::npos)
        {
            return true;
        }
    }

    return false;
}

bool CAVTRAKDriver::Reset()
{
    //locals
    string sReply,sCmd;

    //break comms lines to reset via hardware
    m_pPort->Break();

    if(m_pPort->GetTelegram(sReply,4.0))
    {
        if(sReply.find("    TYPE 7995 AVTRAK OK")==string::npos)
        {
            return false;
        }
    }
    else
    {
        MOOSTrace("AVTRAK Hardware Reset Failed\n");
        return false;
    }

    MOOSTrace("Waiting...");
    MOOSPause(1000);
    MOOSTrace("OK\n");


    //sofware reset...

/*
    sCmd = ">RS\r\n",
    m_pPort->Write((char*)sCmd.c_str(),sCmd.size());

    if(m_pPort->GetTelegram(sReply,2.0))
    {
        if(sReply.find("OK")!=string::npos)
        {
            return true;
        }
    }
*/
    return true;
}

bool CAVTRAKDriver::SetSerialPort(CMOOSSerialPort * pPort)
{
#ifndef _WIN32
    m_pPort = (CMOOSLinuxSerialPort*) pPort;
#else
    m_pPort = (CMOOSNTSerialPort*) pPort;
#endif
    return true;
}


bool CAVTRAKDriver::SetAcousticTimeOut(double dfTimeOut)
{
    if(dfTimeOut>0)
    {
        m_dfAcousticTimeOut = dfTimeOut;

        return SetRangingParams();
        
    }
    return false;
}

double CAVTRAKDriver::GetTAT(int nChan)
{
    switch(nChan)
    {
    case 4: return 0.625; break;
    case 5: return 0.5625; break;
    case 6: return 0.5; break;
    case 7: return 0.4375; break;
    case 8: return 0.375; break;
    case 9: return 0.3125; break;
    case 10: return 0.25; break;
    case 11: return 0.1875; break;
    case 12: return 0.1250; break;
    case 13: return 0.0625; break;
    default:
        return 0;
    }
}

bool CAVTRAKDriver::GetTOFByChannel(int nChan, string &sName, double &dfTime, double &dfTOF)
{
    TOF_VECTOR::iterator p;

    for(p = m_TOFs.begin();p!=m_TOFs.end();p++)
    {
        CTwoWayTOF TOF = *p;
        if(TOF.m_dfTOF>0)
        {
            if(TOF.m_nChannel == nChan)
            {
                sName = MOOSFormat("LBL_CH_%d",nChan);
                dfTime = TOF.m_dfTxTime+TOF.m_dfTOF;
                dfTOF = TOF.m_dfTOF;
                return true;
            }
        }
    }

    return false;
}
