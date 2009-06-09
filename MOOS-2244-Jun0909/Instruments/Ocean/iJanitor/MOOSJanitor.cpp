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
// MOOSJanitor.cpp: implementation of the CMOOSJanitor class.
//
//////////////////////////////////////////////////////////////////////
#include <MOOSLIB/MOOSLib.h>
#include <sstream>
#include <cstring>
#include <iomanip>
#include "MOOSJanitor.h"
#define GROUND_FAULT 100
#define BLUEFIN_WATCHDOG_PERIOD 1.0
#define DIAGNOSTIC_INTERVAL 5.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

CMOOSJanitor::CMOOSJanitor()
{
    m_sVehicleType="BLUEFIN";
    m_dfLastWDHit = 0;
    m_dfLastDiagnostic = 0;
    m_bAutoWatchDog = false;
}

CMOOSJanitor::~CMOOSJanitor()
{

}

bool CMOOSJanitor::Iterate()
{
   
    if(m_bAutoWatchDog)
    {
        HitTailConeWD();
    }

//    DoDiagnostics();

    return true;
}


bool CMOOSJanitor::OnStartUp()
{
    //call base class member first
    CMOOSInstrument::OnStartUp();
    
    m_MissionReader.GetValue("VEHICLETYPE",m_sVehicleType);

    string sWD;
    m_MissionReader.GetValue("AUTOWATCHDOG",sWD);

    m_bAutoWatchDog = MOOSStrCmp(sWD,"TRUE");

    if(IsSimulateMode())
    {
        //not much to do...
        RegisterMOOSVariables();
    }
    else
    {
        if(IsBluefinVehicle())
        {
            //try to open 
            if(!SetupPort())
            {
                return false;
            }
            
            if(!SetUpSwitches())
            {
                return false;
            }

        }
    }            



    return true;
}



bool CMOOSJanitor::OnNewMail(MOOSMSG_LIST &NewMail)
{
    CMOOSMsg Msg;

    if(m_Comms.PeekMail(NewMail,"JANITOR_RESTART",Msg,true))
    {
        SetUpSwitches();
    }

    if(m_Comms.PeekMail(NewMail,"ACTUATION_WD_HIT",Msg,true))
    {
        if(IsBluefinVehicle())        
        {
            HitTailConeWD();
        }
    }

    while(m_Comms.PeekMail(NewMail,"JANITOR_SWITCH",Msg,true))
    {
        if(IsBluefinVehicle())        
        {
            OnSwitch(Msg);
        }
    }

    return UpdateMOOSVariables(NewMail);
}

bool CMOOSJanitor::OnConnectToServer()
{

    //register for a restart signal
    m_Comms.Register("JANITOR_RESTART",0);

    //register for explicit circuit control
    m_Comms.Register("JANITOR_SWITCH",0);

    if(IsBluefinVehicle())
    {
        //register for wathc dog hit from iACtuation...
        m_Comms.Register("ACTUATION_WD_HIT",3.0);
    }
    
    return true;
}



bool CMOOSJanitor::BootBluefinVehicle()
{

    return true;
}

bool CMOOSJanitor::IsBluefinVehicle()
{
    return MOOSStrCmp("BLUEFIN",m_sVehicleType);
}

bool CMOOSJanitor::SetUpSwitches()
{
    STRING_LIST sParams;
    
    if(!m_MissionReader.GetConfiguration(GetAppName(),sParams))
    {
        return false;
    }

    STRING_LIST::iterator q;

    bool bOK = true;

    for(q = sParams.begin();q!=sParams.end();q++)
    {
        string sLine = *q;
        string sTok,sVal;
        if(m_MissionReader.GetTokenValPair(sLine,sTok,sVal))
        {
            if(MOOSStrCmp("SWITCH",sTok))
            {
                //this is a switch command...
                // eg Switch = GPS            @ 1 : On
                
                string sResourceName = MOOSChomp(sVal,"@");
                string sCircuit = MOOSChomp(sVal,":");
                string sInitial = sVal;
                
                CResourceCircuit NewResource;

                NewResource.m_bInitialState = MOOSStrCmp(sInitial,"ON");
                NewResource.m_nCircuit = atoi(sCircuit.c_str());
                NewResource.m_sConnectedResource = sResourceName;

                m_Resources[sResourceName] = NewResource;

                if(!SetSwitch(sResourceName,NewResource.m_bInitialState))
                {
                    MOOSTrace("Control of Switch %s failed...\n\a",sResourceName.c_str());
                    bOK = false;
                }

            }
        }        
    }

    return bOK;
}

bool CMOOSJanitor::SetSwitch(const string &sResource, bool bVal)
{
    RESOURCEMAP::iterator p = m_Resources.find(sResource);

    if(p!=m_Resources.end())
    {
        CResourceCircuit & rResource =  p->second;
        int nCircuit = rResource.m_nCircuit;

        stringstream os;

        if(nCircuit>15)
            return false;
        
        //figure out stem of command
        char i2x[]="0123456789ABCDEF";
        os<<(bVal==true?"N":"F")<<i2x[nCircuit]<<ends;
        
        string sStem = os.str();

        //pre-pend # for command
        string sTx = "#"+sStem;

        

        m_Port.Write((char *)sTx.c_str(),sTx.size());

        //send command
        string sReply;
        if(!m_Port.GetTelegram(sReply,2))
        {
            MOOSTrace("CMOOSJanitor::SetSwitch() PowerBoard Not responding... \n");
            return false;
        }

        //now parse reply
        if(sReply.find("ERR")!=string::npos)
        {
            //gross error...
            MOOSTrace("CMOOSJanitor::SetSwitch() Power Board returns \"ERR\" to command \"%s\"...\n",
                        sTx.c_str());
            return false;
        }

        string sExpected ="$"+sStem+" 1";
        if(sReply.find(sExpected)!=string::npos)
        {
            //reply is good
            rResource.m_bCurrentState = bVal;
            MOOSTrace("CMOOSJanitor: Turning resource \"%s\" %s [circuit %d]\n",
                rResource.m_sConnectedResource.c_str(),
                bVal==true?"ON":"OFF",
                nCircuit);

                        
        }
        else
        {
            MOOSTrace("CMOOSJanitor::SetSwitch() Power Board returns \"%s\" to command \"%s\"...\n",
                        sReply.c_str(),
                        sTx.c_str());
            return false;
        }

    }
    else
    {
        MOOSTrace("CMOOSJanitor::SetSwitch() Resource \"%s\" not found\n",sResource.c_str());
        return false;
    }

    return true;
}

bool CMOOSJanitor::HitTailConeWD()
{
    if(MOOSTime()-m_dfLastWDHit<BLUEFIN_WATCHDOG_PERIOD)
        return true;

    //reset wathcdog on power board..
    string sTx = "#TA";
    m_Port.Write((char*)sTx.c_str(),sTx.size());
    string sRx;

    if(!m_Port.GetTelegram(sRx,1.0))
    {
        MOOSTrace("PowerBoard not responding to wathcdog reset...\n");
        return false;
    }
    else
    {
        m_dfLastWDHit = MOOSTime();
        MOOSTrace("[%f] Tail Cone WD Hit.\n",
            m_dfLastWDHit-GetAppStartTime());
    }

    return true;
}

bool CMOOSJanitor::OnSwitch(CMOOSMsg &Msg)
{
    //now, what are we being asked to do?
    //Msg.m_sVal = "Resource : On"
    if(Msg.m_cDataType==MOOS_STRING)
    {
        string sStr = Msg.m_sVal;
        MOOSRemoveChars(sStr," ");
        string sResource = MOOSChomp(sStr,":");
        string sState = sStr;
        bool bState = MOOSStrCmp(sState,"ON");

        return SetSwitch(sResource,bState);
    }

    //wrong data type!!!
    return false;
}

bool CMOOSJanitor::GetTemperature()
{


    //ok so do it..
    char sTx[]="#K1";
    if(!m_Port.Write(sTx,strlen(sTx)))
    {
        return false;
    }
    
    string sReply;
    if(!m_Port.GetTelegram(sReply,2.0))
    {
        return false;
    }

    //process reply
    MOOSChomp(sReply,"$K1 ");
    stringstream is(sReply.c_str());
    is.flags(ios::hex);
    int nHex;
    is>>nHex;

    m_dfTemperature = nHex*500.0/1024.0;

    m_Comms.Notify("SPHERE_TEMP",m_dfTemperature);
        
    bool bOn;

    if(GetSwitchState("FAN",bOn))
    {
        if(m_dfTemperature>30.0 && !bOn )
        {
            MOOSDebugWrite("Its getting hot in here...turning fan on");
            SetSwitch("FAN",true);
        }
        if(m_dfTemperature<27.0 && bOn)
        {
            MOOSDebugWrite("Its cool enough. Turning fan off");
            SetSwitch("FAN",false);
        }
    }
    return true;
}

bool CMOOSJanitor::GetSwitchState(const string &sResource, bool &bState)
{
    RESOURCEMAP::iterator p = m_Resources.find(sResource);

    if(p!=m_Resources.end())
    {
        CResourceCircuit & rResource =  p->second;
        bState = rResource.m_bCurrentState;
        return true;
    }

    return false;
}

bool CMOOSJanitor::GetGroundFaults()
{
    //ok so do it..
    vector<string> Names;
    Names.push_back("Bat Bus +");
    Names.push_back("Bat Bus -");
    Names.push_back("ISO 12V+");
    Names.push_back("ISO 12V-");


    bool bGroundFault = false;
    unsigned int i;
    for(i = 0;i<Names.size();i++)
    {
        string sTx = MOOSFormat("#G%d",i);
        if(m_Port.Write((char*)sTx.c_str(),sTx.size()))
        {
            string sReply;
            if(m_Port.GetTelegram(sReply,2.0))
            {
                //process reply
                string sExpected = MOOSFormat("$G%d",i);

                MOOSChomp(sReply,sExpected);
                if(!sReply.empty())
                {
                    stringstream is(sReply.c_str());
                    is.flags(ios::hex);
                    int nHex;
                    is>>nHex;
                    

                    if(nHex>GROUND_FAULT)
                    {
                        string sGF;
                        if(i != 1)
                        {
                            sGF = MOOSFormat("JANITOR: Warning, suspected Ground fault in %s [%d]",
                            Names[i].c_str(),
                            nHex);                        
                        }
                        else
                        {
                            sGF = MOOSFormat("JANITOR: Warning, suspected Ground fault in %s [%d] is shore power on?",
                            Names[i].c_str(),
                            nHex);                        
                        }
                        MOOSDebugWrite(sGF);
                    }
                }
            }
        }
    }

    return true;   
}

bool CMOOSJanitor::GetLeaks()
{
    //ok so do it..
    vector<string> Names;
    Names.push_back("Sphere");
    Names.push_back("Aft JBox");
    Names.push_back("Forward JBox");

    unsigned int i;
    for(i = 0;i<Names.size();i++)
    {
        string sTx = MOOSFormat("#L%d",i);
        if(m_Port.Write((char*)sTx.c_str(),sTx.size()))
        {
            string sReply;
            if(m_Port.GetTelegram(sReply,2.0))
            {
                //process reply
                string sExpected = MOOSFormat("$L%d",i);

                MOOSChomp(sReply,sExpected);
                if(!sReply.empty())
                {
                    int nLeak = atoi(sReply.c_str());
                    if(nLeak !=0)
                    {
                        string sLeak = MOOSFormat("JANITOR: SEVERE!! Leak in %s",Names[i].c_str());
                        MOOSDebugWrite(sLeak);
                    }
                }
            }
        }
    }

    return true;
}

bool CMOOSJanitor::DoDiagnostics()
{

    if(MOOSTime()-m_dfLastDiagnostic<DIAGNOSTIC_INTERVAL)
    {
        return true;
    }
    else
    {
        m_dfLastDiagnostic = MOOSTime();
    }

    GetTemperature();
    GetGroundFaults();
    GetLeaks();

    return true;

}
