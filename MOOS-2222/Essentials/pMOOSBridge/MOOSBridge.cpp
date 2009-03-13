///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Core Component. 
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
// MOOSBridge.cpp: implementation of the CMOOSBridge class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSBridge.h"
#include "MOOSCommunity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define DEFAULT_BRIDGE_FREQUENCY 20


using namespace std;

CMOOSBridge::CMOOSBridge()
{
    m_nBridgeFrequency    = DEFAULT_BRIDGE_FREQUENCY;
}

CMOOSBridge::~CMOOSBridge()
{
    
}

bool CMOOSBridge::Run(const string &sMissionFile,const string & sMOOSName)
{
    if(!m_MissionReader.SetFile(sMissionFile))
        return false;
    
    m_MissionReader.SetAppName(sMOOSName);
    
    if(!Configure())
    {
        return MOOSFail("MOOSBridge failed to configure itself - probably a configuration block error\n");;
    }
    while(1)
    {
        MarshallLoop();
        MOOSPause(1);
    }
    return true;
}

#define BOUNCE_WITH_GUSTO 0
bool CMOOSBridge::MarshallLoop()
{
    COMMUNITY_MAP::iterator p,q;
    MOOSMSG_LIST InMail;
    for(p = m_Communities.begin();p!=m_Communities.end();p++)
    {
        CMOOSCommunity* pSrcCommunity = (p->second);
        
        if(pSrcCommunity->m_CommClient.Fetch(InMail))
        {
            int nMail = InMail.size();
            for(q = m_Communities.begin();q!=m_Communities.end();q++)
            {
                CMOOSCommunity* pDestCommunity = q->second;
                if(pDestCommunity!=pSrcCommunity)
                {
                    MOOSMSG_LIST::iterator w;
                    for(w = InMail.begin();w!=InMail.end();w++)
                    {

                        CMOOSCommunity::SP Index(w->GetKey(),pSrcCommunity->GetCommsName() );

                        if(pDestCommunity->WantsToSink(Index))
                        {    
                            //decrement mail count
                            nMail--;

                            CMOOSMsg MsgCopy = *w;
                            MsgCopy.m_sKey = pDestCommunity->GetAlias(Index);
                            pDestCommunity->m_CommClient.Post(MsgCopy);
#if(BOUNCE_WITH_GUSTO)
                            MOOSTrace("Bouncing %s in %s -> %s on %s t = %f\n",
                                w->GetKey().c_str(),
                                pSrcCommunity->GetFormattedName().c_str(),
                                MsgCopy.GetKey().c_str(),
                                pDestCommunity->GetFormattedName().c_str(),
                                MsgCopy.GetTime());
#endif
                        }
                    }
                }
            }

            // Chee Wee points out thus logic is not quite right if dispatching
            //to multiple communities - to do
            if(nMail>0)
            {
                MOOSTrace("There %s %d msg%s from community %s that can't be delivered\n",
                    nMail>1?"are":"is",
                    nMail,
                    nMail>1?"s":"",
                    p->first.c_str());

            }
        }
    }
    
    return true;
}

bool CMOOSBridge::Configure()
{
    STRING_LIST sParams;
    
    if(!m_MissionReader.GetConfiguration(m_MissionReader.GetAppName(),sParams))
        return MOOSFail("ERROR - Could not find a configuration block called %s \n",m_MissionReader.GetAppName().c_str()) ;
    
    //if user set LOOPBACK = TRUE then both src and destination communities can be identical
    //default is FALSE this means if src=dest then the bridging will be ignored
    bool bAllowLoopBack = false;
    m_MissionReader.GetConfigurationParam("LOOPBACK",bAllowLoopBack);


    //capture default file scope settings - maybe useful later
    string sDefaultCommunity = "#1";
    string sDefaultHost = "LOCALHOST";
    string sDefaultPort = "9000";

    if(!m_MissionReader.GetValue("COMMUNITY",sDefaultCommunity))
    {
        MOOSTrace("WARNING : Cannot read ::MOOS-scope variable COMMUNITY - assuming %s\n",sDefaultCommunity.c_str());
    }
    
    if(!m_MissionReader.GetValue("SERVERPORT",sDefaultPort))
    {
        MOOSTrace("WARNING :Cannot read ::MOOS-scope variable SERVERPORT - assuming %s\n",sDefaultPort.c_str());
    }
    
    if(!m_MissionReader.GetValue("SERVERHOST",sDefaultHost))
    {
        MOOSTrace("WARNING :Cannot read ::MOOS-scope variable SERVERHOST - assuming %s\n",sDefaultHost.c_str());
    }

    //how fast should the bridge operate in Hz (setting this to zero is a special case and
    //makes all registrations with dfPeriod = 0)
    m_nBridgeFrequency = DEFAULT_BRIDGE_FREQUENCY;
    m_MissionReader.GetConfigurationParam("BridgeFrequency",m_nBridgeFrequency);


    STRING_LIST::iterator q;
    
    for(q = sParams.begin();q!=sParams.end();q++)
    {
        string sLine = *q;
        //NB is alias's aren't specified the sink name is the source name
        //also you don't need as many alias's as sources...
        //SHARE = COMMUNITYNAME@HOSTNAME:PORT [VAR1,VAR2,VAR3,....] -> COMMUNITYNAME@HOSTNAME:PORT [VarAlias1,....]
        // or using mission file defaults i.e file scope constants
        //SHARE = [VAR1,VAR2,VAR3,....] -> COMMUNITYNAME@HOSTNAME:PORT [VarAlias1,....]
        string sCmd = MOOSChomp(sLine,"=");
        
        if(MOOSStrCmp(sCmd,"SHARE"))
        {
            string sSrc = MOOSChomp(sLine,"->");
            string sDest = sLine;

            string sSrcCommunity =sDefaultCommunity ;
            string sSrcCommunityHost = sDefaultHost;
            string sSrcCommunityPort = sDefaultPort;
            if(sSrc[0]=='[')
            {
    
                //tell user what we are doing - this is the short-hand set up...
                MOOSTrace("Using abbreviated configuration protocol Source: %s@%s:%s\n",
                    sSrcCommunity.c_str(),
                    sSrcCommunityPort.c_str(),
                    sSrcCommunityHost.c_str());


                MOOSChomp(sSrc,"[");
            }
            else
            {
                sSrcCommunity = MOOSChomp(sSrc,"@");
                sSrcCommunityHost = MOOSChomp(sSrc,":");            
                sSrcCommunityPort = MOOSChomp(sSrc,"[");
            }

            string sVars =MOOSChomp(sSrc,"]"); 
            
            string sDestCommunity = MOOSChomp(sDest,"@");
            string sDestCommunityHost = MOOSChomp(sDest,":");            
            string sDestCommunityPort = MOOSChomp(sDest,"[");
            string sAliases = MOOSChomp(sDest,"]");
        
            //look for loopback - not always wanted
            if(MOOSStrCmp(sDestCommunityHost,sSrcCommunityHost) && 
                MOOSStrCmp(sDestCommunityPort,sSrcCommunityPort))
            {
                if(bAllowLoopBack==false)
                {
                    MOOSTrace("\t Ignoring Loop Back - (bridge not built)\n");
                    continue;
                }
            }
            
            //convert to numeric after format checking
            long lSrcPort=0;
            if(sSrcCommunity.empty() || sSrcCommunityHost.empty() ||sSrcCommunityPort.empty())
            {
                MOOSTrace("error on SHARED configuration %s\n correct line format is \nSHARE = COMMUNITYNAME@HOSTNAME:PORT [VAR1,VAR2,VAR3,....] -> COMMUNITYNAME@HOSTNAME:PORT\n",q->c_str());        
                continue;
            }
            else
            {
                lSrcPort = atoi(sSrcCommunityPort.c_str());            
            }
            
            long lDestPort=0;
            if(sDestCommunity.empty() || sDestCommunityHost.empty() ||sDestCommunityPort.empty())
            {
                MOOSTrace("error on SHARED configuration %s\n correct line format is \nSHARE = COMMUNITYNAME@HOSTNAME:PORT [VAR1,VAR2,VAR3,....] -> COMMUNITYNAME@HOSTNAME:PORT\n",q->c_str());        
                continue;
            }
            else
            {
                lDestPort = atoi(sDestCommunityPort.c_str());            
            }
            
            //make two communities (which will be bridged)
            CMOOSCommunity* pSrcCommunity =  GetOrMakeCommunity(sSrcCommunity,
                                             sSrcCommunityHost,
                                             lSrcPort);

            CMOOSCommunity* pDestCommunity =  GetOrMakeCommunity(sDestCommunity,
                                              sDestCommunityHost,
                                              lDestPort);
            
            //populate bridge with variables to be shared (including translation)
            if(pSrcCommunity && pDestCommunity)
            {
                string sVar = MOOSChomp(sVars,",");
                while(!sVar.empty())
                {
                    pSrcCommunity->AddSource(sVar);
                    CMOOSCommunity::SP Index(sVar,pSrcCommunity->GetCommsName() );
                    pDestCommunity->AddSink(Index,MOOSChomp(sAliases,","));
                    
                    //suck another VAR
                    sVar =  MOOSChomp(sVars,",");
                }
            }                        
        }
        
    }
    return true;
}

CMOOSCommunity * CMOOSBridge::GetOrMakeCommunity(const string &sCommunity, const string &sCommunityHost, long lPort)
{
    CMOOSCommunity* pCommunity = NULL;
    COMMUNITY_MAP::iterator p = m_Communities.find(sCommunity);
    if(p==m_Communities.end())
    {
        pCommunity = new CMOOSCommunity;
        if(pCommunity->Initialise(    sCommunity,
            sCommunityHost,
            lPort,
            m_MissionReader.GetAppName(),
            m_nBridgeFrequency))
        {
            m_Communities[sCommunity] = pCommunity;
        }
        else
        {
            delete pCommunity;
            pCommunity = NULL;
            MOOSTrace("failed to create link to community %s@%s:%d\n",
                sCommunity.c_str(),
                sCommunityHost.c_str(),
                lPort);
        }
    }
    else
    {
        pCommunity = p->second;
    }

    return pCommunity;
}
