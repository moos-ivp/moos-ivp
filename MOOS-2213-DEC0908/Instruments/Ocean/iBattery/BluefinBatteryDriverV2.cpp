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
// BluefinBatteryDriverV2.cpp: implementation of the CBluefinBatteryDriverV2 class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <MOOSGenLib/MOOSGenLib.h>
#include <MOOSLIB/MOOSLib.h>
#include <iomanip>

#include <strstream>

#include "BluefinBatteryDriverV2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define BLUEFIN_SMART_FULL 33.8
#define BLUEFIN_SMART_EMPTY 25.9

CBluefinBatteryDriverV2::CBluefinBatteryDriverV2()
{
    m_bEchoing = true;
}

CBluefinBatteryDriverV2::~CBluefinBatteryDriverV2()
{

}

bool CBluefinBatteryDriverV2::Initialise()
{
    MOOSPause(1000);

    //start clean
    m_pPort->Flush();

    //look for carriage return
    m_pPort->SetTermCharacter('\r');

    //who is out there?
    DiscoverAndMakeBatteries();

    //fetch battery information
    GetData();

    //if batteries have timed out turn them off and on
    //to remove timeout flag
    BATTERY_MAP::iterator p;
    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack =p->second;

        SwitchPack(rPack,true);

        if(rPack.m_sError != "NONE")
        {
            MOOSTrace("Clearing battery initial error flag \"%s\" for battery %d\n",
                rPack.m_sError.c_str(),
                rPack.m_nPackAddress);
            SwitchPack(rPack,false);
            SwitchPack(rPack,true);

            GetData();

            if(rPack.m_sError !="NONE")
            {
                MOOSTrace("Battery %d still has an error - it will be ignored!\n",
                    rPack.m_nPackAddress);
                rPack.m_bIgnore = true;
            }
        }
    }
     
    return true;
}

bool CBluefinBatteryDriverV2::Switch(bool bOn)
{
    BATTERY_MAP::iterator p;

    double dfMeanVoltage = 0;
    
    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack = p->second;
        SwitchPack(rPack,bOn);
    }
    
    return true;
}

string CBluefinBatteryDriverV2::GetCellsString()
{
    ostrstream os;
    BATTERY_MAP::iterator p;

    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack = p->second;

        os<<"PACK="<<rPack.m_nPackAddress<<":";
        
        os<<"V="<<rPack.m_dfVoltage<<",";
        os<<"Cells=[";

        for(int nCell = 0; nCell<CELLS_PER_PACK;nCell++)
        {
            os<<rPack.m_Cells[nCell].m_dfVoltage<<",";
        }

        os<<"];";
    }

    os<<ends;

    string sResult = os.str();

    os.rdbuf()->freeze(0);

    return sResult;
}



bool CBluefinBatteryDriverV2::GetData()
{
    SetEmptyVolts(BLUEFIN_SMART_EMPTY);
    SetFullVolts(BLUEFIN_SMART_FULL);


    BATTERY_MAP::iterator p;

    double dfMeanVoltage = 0;
    int nIncluded = 0;
    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack = p->second;
        QueryBattery(rPack);
        
        if(!rPack.m_bIgnore)
        {
            dfMeanVoltage += rPack.m_dfVoltage;
            nIncluded++;
        }

        //read cells state....
        QueryCellsState(rPack);


    }
    if(nIncluded>0)
    {
        //calculate mean voltage..
        m_Status.m_dfVoltage = dfMeanVoltage/nIncluded;
    }
    else
    {
       m_Status.m_dfVoltage = 0;
    }

    return true;
}

bool CBluefinBatteryDriverV2::DiscoverAndMakeBatteries()
{
    MOOSTrace("Discovering Packs...\n");
    string sQuery = "!?\r";
    string sReply;
    if(WriteAndRead(sQuery,sReply,2.0,true))
    {
        MOOSTrace("Boards discovered: %s\n",sReply.c_str());


        istrstream is(sReply.c_str());
        is.flags(ios::hex);

        while(!is.eof())
        {
            int nPack=0;
            is>>ws;
            is>>nPack;
            if(nPack>0)
            {
                MOOSTrace("Creating Pack %d\n",nPack);
                CBatteryPack NewPack;
                NewPack.m_nPackAddress = nPack;
                NewPack.m_bIgnore = false;
                m_BatteryMap[nPack] = NewPack;
            }
        }
    }

    MOOSTrace("There are %d batteries in the vehicle\n",m_BatteryMap.size());

    return true;
}

//this is a custom function for reading and rwriting
//in development RS232->485 converter echoed commands
bool CBluefinBatteryDriverV2::WriteAndRead(string sOut,
                                           string &sReply,
                                           double dfTimeOut,
                                           bool bNoTerm)
{
    if(!m_pPort->Write((char *)sOut.c_str(),sOut.size()))
        return false;

    if(m_bEchoing)
    {
        string sWhatISaid;
        if(!m_pPort->GetTelegram(sWhatISaid,1.0))
        {
            MOOSTrace("no command echo found\n!");
        }
    }

    if(bNoTerm)
    {
        //this is an unterminated reply...
        char Tmp[2000];
        int nRead = m_pPort->ReadNWithTimeOut(Tmp,sizeof(Tmp),dfTimeOut);

        if(nRead>0 && nRead< sizeof(Tmp))
        {
            Tmp[nRead] = '\0';
            sReply = string(Tmp);
            return true;
        }
        else
        {
            MOOSTrace("could not read unterminated telegram in %f seconds\n",dfTimeOut);
            return false;
        }
    }
    else
    {
        //we expect a standard reply
        if(dfTimeOut>0)
        {
            if(!m_pPort->GetTelegram(sReply,dfTimeOut))
            {
                MOOSTrace("Failed telgram fetch\n");
                return false;
            }
        }
        return true;
    }

    return false;
}

bool CBluefinBatteryDriverV2::QueryBattery(CBatteryPack &rPack)
{
    ostrstream os;
    os.flags(ios::hex);

    //format stream
    os<<"#";
    os.fill('0');
    os<<setw(2)<<rPack.m_nPackAddress;
    os<<"Q0\r";
    os<<ends;

    //make the string..
    string sEnquire = os.str();

    //free memory
    os.rdbuf()->freeze(0);

    string sReply;
    if(!WriteAndRead(sEnquire,sReply,1.0))
    {
        MOOSTrace("failed battery status fetch on pack %d\n",rPack.m_nPackAddress);
        return false;
    }

    //OK if we are here things look good!
    //start parsing

    string sHeader,sState,sVoltage,sCurrent,sMaxTemp,sMinCell,sMaxCell,sLeak,sCapacity,sTime;
    istrstream is(sReply.c_str());
    is>>sHeader>>ws;
    is>>sState>>ws;
    is>>sVoltage>>ws;
    is>>sCurrent>>ws;
    is>>sMaxTemp>>ws;
    is>>sMinCell>>ws;
    is>>sMaxCell>>ws;
    is>>sLeak>>ws;
    is>>sCapacity>>ws;
    is>>sTime>>ws;


    rPack.m_sComment = MOOSFormat("%s, Current = %s A,Temp = %s deg, Capacity = %sWh, CellMax = %sV CellMin = %sV",
        rPack.m_sState.c_str(),
        sCurrent.c_str(),
        sMaxTemp.c_str(),
        sCapacity.c_str(),
        sMaxCell.c_str(),
        sMinCell.c_str());

    //voltage is useful...
    rPack.m_dfVoltage = atof(sVoltage.c_str());

    //state is useful...
    ProcessStateString(rPack,sState);



    return true;
}

bool CBluefinBatteryDriverV2::SwitchPack(CBatteryPack &rPack, bool bOn)
{
    MOOSTrace("Switching Pack %d %s\n",rPack.m_nPackAddress,bOn?"ON":"OFF");
    
    ostrstream os;
    os.flags(ios::hex);

    //format stream
    os<<"#";
    os.fill('0');
    os<<setw(2)<<rPack.m_nPackAddress;
    os<<"B";
    os<<(bOn?"D":"F");
    os<<"\r";
    os<<ends;

    //make the string..
    string sSwitch = os.str();

    //free memory
    os.rdbuf()->freeze(0);

    string sReply;
    if(!WriteAndRead(sSwitch,sReply,1.0))
    {
        MOOSTrace("failed switch pack %d\n",rPack.m_nPackAddress);
        return false;
    }

    //and now disable watch dog - make an option later on....
    //format stream
    if(bOn)
    {
    ostrstream os;
    os.flags(ios::hex);
        os<<"#";
        os.fill('0');
        os<<setw(2)<<rPack.m_nPackAddress;
        os<<"MF\r"<<ends;
        string sWDOff = os.str();
        os.rdbuf()->freeze(0);

        if(!WriteAndRead(sWDOff,sReply,1.0))
        {
            MOOSTrace("failed to disable watch dog on %d\n",rPack.m_nPackAddress);
            return false;
        }

    }

    return true;
    
}

bool CBluefinBatteryDriverV2::ProcessStateString(CBatteryPack & rPack,string sState)
{
    MOOSRemoveChars(sState," ");
    if(sState.size()!=2)
        return false;

    switch(sState[0])
    {
    case 'f': rPack.m_eState = OFF; rPack.m_sState = "OFF"; break;
    case 'd': rPack.m_eState = ON; rPack.m_sState = "ON";break;
    case 'c': rPack.m_eState = CHARGING; rPack.m_sState = "CHARGING";break;
    case 'b': rPack.m_eState = BALANCING;rPack.m_sState = "BALANCING"; break;
    }

    switch(sState[1])
    {
    case '-': rPack.m_sError = "NONE"; break;
    case 'V': rPack.m_sError = "OVERVOLTAGE"; break;
    case 'v': rPack.m_sError = "UNDERVOLTAGE"; break;
    case 'I': rPack.m_sError = "OVERCURRENT"; break;
    case 'T': rPack.m_sError = "CELL OVER TEMPERATURE"; break;
    case 'O': rPack.m_sError = "OIL OVERTEMPERATURE"; break;
    case 'C': rPack.m_sError = "CELL OVERVOLTAGE"; break;
    case 'c': rPack.m_sError = "CELL UNDERVOLTAGE"; break;
    case 'H': rPack.m_sError = "HARDWARE FAULT"; break;
    case 'h': rPack.m_sError = "TRANSIENT FAULT"; break;
    case 'm': rPack.m_sError = "TIMEOUT"; break;
    }

    return true;

}

bool CBluefinBatteryDriverV2::QueryCellsState(CBatteryPack &rPack)
{
    ostrstream os;
    os.flags(ios::hex);

    //format stream
    os<<"#";
    os.fill('0');
    os<<setw(2)<<rPack.m_nPackAddress;
    os<<"Q1\r";
    os<<ends;

    //make the string..
    string sEnquire = os.str();

    //free memory
    os.rdbuf()->freeze(0);

    string sReply;
    if(!WriteAndRead(sEnquire,sReply,1.0))
    {
        MOOSTrace("failed battery cell status fetch on pack %d\n",rPack.m_nPackAddress);
        return false;
    }

    string sHeader = MOOSChomp(sReply," ");
    int nRead = 0;
    while(!sReply.empty() && nRead < CELLS_PER_PACK)
    {
        string sVal = MOOSChomp(sReply," ");
        double dfVal = atof(sVal.c_str());
        rPack.m_Cells[nRead++].m_dfVoltage = dfVal;
    }

    return true;
}

string CBluefinBatteryDriverV2::GetErrorString()
{
    BATTERY_MAP::iterator p;

    m_sError = "";
    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack = p->second;
        string sPackError = MOOSFormat("Pack[%d] Error = \"%s\"",
                rPack.m_nPackAddress, 
                rPack.m_sError.c_str());
        m_sError+=sPackError+";";;
    }
    
    return m_sError;
}

string CBluefinBatteryDriverV2::GetCommentString()
{
    BATTERY_MAP::iterator p;

    m_sComment = "";
    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack = p->second;
        string sPackComment = MOOSFormat("Pack[%d] Comment = \"%s\"",
                rPack.m_nPackAddress, 
                rPack.m_sComment.c_str());
        m_sComment+=sPackComment+";";
    }

    return m_sComment;
}

bool CBluefinBatteryDriverV2::IsError()
{
    BATTERY_MAP::iterator p;

    for(p = m_BatteryMap.begin();p!=m_BatteryMap.end();p++)
    {
        CBatteryPack & rPack = p->second;
        if(rPack.m_sError!="NONE" && rPack.m_sState =="ON")
            return true;
    }
    return false;
}








