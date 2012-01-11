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
#define DEFAULT_UDP_PORT 10000

#define BOUNCE_WITH_GUSTO 0
using namespace std;

CMOOSBridge::CMOOSBridge()
{
  m_nBridgeFrequency = DEFAULT_BRIDGE_FREQUENCY;
  m_sLocalCommunity  = "#1";
  m_bAllowLoopBack   = false;
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool CMOOSBridge::OnNewMail(MOOSMSG_LIST &NewMail)
{
  cout << "In OnNewMail!!!!!!!!!!!!!!!!!!!" << endl;
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string moosvar = msg.GetKey();
    string sval    = msg.GetString();

    // Example String "src_var=DB_CLIENTS,dest_community=henry,
    //    dest_host=128.30.24.246,dest_port=9201,dest_alias=BAR"
    if(moosvar == "PMB_REGISTER") {
      string src_var, dest_community, dest_host, dest_port, dest_alias;
      while(sval != "") {
	string pair = MOOSChomp(sval,",");
	string left = MOOSChomp(pair,"=");
	string right = pair;
	if(left == "SrcVarName")
	  src_var = right;
	else if(left == "DestCommunity")
	  dest_community = right;
	else if(left == "DestCommunityHost")
	  dest_host = right;
	else if(left == "DestCommunityPort")
	  dest_port = right;
	else if(left == "DestVarName")
	  dest_alias = right;
      }
      string sSrcCommunity = m_sLocalCommunity ;
      string sLocalHost = "LOCALHOST";
      string sLocalPort = "9000";    
      bool ok = ConfigureLine(m_sLocalCommunity, "LOCALHOST", "9000",
			      src_var, dest_community, dest_host, 
			      dest_port, dest_alias);
      if(ok)
	MOOSTrace("New Registration Successful!! \n");
      else
	MOOSTrace("New Registration Failed!! \n");
    }
  }
  return(true);      
}

//----------------------------------------------------------------------
// Procedure: Iterate

bool CMOOSBridge::Iterate()
{
  MarshallLoop();
  MOOSPause(50);
  return true;
}

//----------------------------------------------------------------
// Procedure: OnStartUp

bool CMOOSBridge::OnStartUp()
{
  if(!Configure())
    return MOOSFail("MOOSBridge failed to configure itself - probably a configuration block error\n");;
  
  RegisterVariables();
  return(true);
}

//----------------------------------------------------------------
// Procedure: OnConnectToServer

bool CMOOSBridge::OnConnectToServer()
{
  RegisterVariables();
  return(true);
}


//----------------------------------------------------------------
// Procedure: RegisterVariables

void CMOOSBridge::RegisterVariables()
{
  m_Comms.Register("PMB_REGISTER", 0);   
}


//----------------------------------------------------------------------
// Procedure: MarshallLoop

bool CMOOSBridge::MarshallLoop()
{
  COMMUNITY_MAP::iterator p,q;
  MOOSMSG_LIST InMail;
  for(p = m_Communities.begin();p!=m_Communities.end();p++) {
    CMOOSCommunity* pSrcCommunity = (p->second);
    
    if(pSrcCommunity->Fetch(InMail)) {
      int nMail = InMail.size();
      for(q = m_Communities.begin();q!=m_Communities.end();q++) {
	CMOOSCommunity* pDestCommunity = q->second;
	if(pDestCommunity!=pSrcCommunity) {
	  MOOSMSG_LIST::iterator w;
	  for(w = InMail.begin();w!=InMail.end();w++) {
	    CMOOSCommunity::SP Index(w->GetKey(),
				     pSrcCommunity->GetCommunityName() );
	    
	    if(pDestCommunity->WantsToSink(Index))  {    
	      //decrement mail count
	      nMail--;
	      
	      CMOOSMsg MsgCopy = *w;
	      MsgCopy.m_sKey = pDestCommunity->GetAlias(Index);
	      if(IsUDPShare(Index))
		{
		  if(pDestCommunity->HasUDPConfigured())
		    {
#if(BOUNCE_WITH_GUSTO) 
		      MOOSTrace("UDP posting %s to community %s@%s:%d as %s (source community is %s)\n",
				w->GetKey().c_str(),
				pDestCommunity->GetCommunityName().c_str(),
				pDestCommunity->GetUDPHost().c_str(),
				pDestCommunity->GetUDPPort(),                                              
				MsgCopy.m_sKey.c_str(),
				w->GetCommunity().c_str());
#endif
		      
		      //Send via UDP (directed to  single machine and port) - fire and forget...
		      m_UDPLink.Post(MsgCopy,pDestCommunity->GetUDPHost(),pDestCommunity->GetUDPPort());
		    }
		  else
		    {
		      MOOSTrace("cannot send %s via UDP to %s - destination community has no UDP port\n",MsgCopy.m_sKey.c_str(),pDestCommunity->GetFormattedName().c_str());
		    }
		}
	      else
		{
		  pDestCommunity->Post(MsgCopy);
		}
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
      //here we could look for broadcasts...
    }
  }
  
    
  //have we received any UDP mail? if so it was meant for our own DB
  //remember if UDP is being used there MUST be one MOOSBridge per community
  MOOSMSG_LIST UDPMail;
  
  if(m_UDPLink.Fetch(UDPMail))
    {
      //find our local community
      
      COMMUNITY_MAP::iterator qq = m_Communities.find(m_sLocalCommunity );
      if(qq!=m_Communities.end())
        {
	  CMOOSCommunity * pLocalCommunity = qq->second;
            MOOSMSG_LIST::iterator p;
            for(p = UDPMail.begin();p!=UDPMail.end();p++)
	      {
#if(BOUNCE_WITH_GUSTO) 
                MOOSTrace("Received %s from %s on UDP  - inserting into %s\n",p->GetKey().c_str(), p->GetCommunity().c_str(),pLocalCommunity->GetFormattedName().c_str());
#endif
		
                //now be careful we aren't looking to subscribing for this mail....now that would be some horrible positive
                //feedback loop! Can alos check as mikerb suggess on source community - it must not
                //be us!
                
                if(!pLocalCommunity->HasMOOSSRegistration(p->GetKey()) && pLocalCommunity->GetCommunityName()!= p->GetCommunity() )
		  {
		    pLocalCommunity->Post(*p);
		  }
                else
		  {
                    
		    MOOSTrace("no way!\n");
		  }
	      }
        }
      
    }
  
  return true;
}

//----------------------------------------------------------------------
// Procedure: IsUDPShare

bool CMOOSBridge::IsUDPShare(CMOOSCommunity::SP & Index)
{
  return !m_UDPShares.empty() && m_UDPShares.find(Index)!=m_UDPShares.end();
}

//----------------------------------------------------------------------
// Procedure: Configure

bool CMOOSBridge::Configure()
{
    STRING_LIST sParams;    
    if(!m_MissionReader.GetConfiguration(m_MissionReader.GetAppName(),sParams))
      return MOOSFail("ERROR - Could not find a configuration block called %s \n",
		      m_MissionReader.GetAppName().c_str()) ;
    
    //if user set LOOPBACK = TRUE then both src and destination
    //communities can be identical default is FALSE this means if
    //src=dest then the bridging will be ignored
    m_MissionReader.GetConfigurationParam("LOOPBACK",m_bAllowLoopBack);
    
    //capture default file scope settings - maybe useful later
    string sLocalHost = "LOCALHOST";
    string sLocalPort = "9000";
    
    if(!m_MissionReader.GetValue("COMMUNITY",m_sLocalCommunity))
      MOOSTrace("WARNING : Cannot read ::MOOS-scope variable COMMUNITY - assuming %s\n", 
		m_sLocalCommunity.c_str());
    
    if(!m_MissionReader.GetValue("SERVERPORT",sLocalPort))
      MOOSTrace("WARNING :Cannot read ::MOOS-scope variable SERVERPORT - assuming %s\n", 
		sLocalPort.c_str());
    
    if(!m_MissionReader.GetValue("SERVERHOST",sLocalHost))
      MOOSTrace("WARNING :Cannot read ::MOOS-scope variable SERVERHOST - assuming %s\n", 
		sLocalHost.c_str());
    
    //how fast should the bridge operate in Hz (setting this to zero is a special case and
    //makes all registrations with dfPeriod = 0)
    m_nBridgeFrequency = DEFAULT_BRIDGE_FREQUENCY;
    m_MissionReader.GetConfigurationParam("BridgeFrequency",m_nBridgeFrequency);
        
    STRING_LIST::iterator q;
    
    for(q = sParams.begin();q!=sParams.end();q++) {
      string original_line = *q;
      string sLine = *q;
      // NB is alias's aren't specified the sink name is the source
      // name also you don't need as many alias's as sources...  
      // SHARE = COMMUNITYNAME@HOSTNAME:PORT [VAR1,VAR2,VAR3,....] ->
      // COMMUNITYNAME@HOSTNAME:PORT [VarAlias1,....]  or using mission
      // file defaults i.e file scope constants SHARE =
      // [VAR1,VAR2,VAR3,....] -> COMMUNITYNAME@HOSTNAME:PORT [VarAlias1,....]
      string sCmd = MOOSChomp(sLine,"=");
        
      if(MOOSStrCmp(sCmd,"SHARE") || MOOSStrCmp(sCmd,"UDPSHARE") ) {
	bool   bUDP = MOOSStrCmp(sCmd,"UDPSHARE");	  
	string sSrc = MOOSChomp(sLine,"->");
	string sDest = sLine;
	
	string sSrcCommunity = m_sLocalCommunity ;
	string sSrcCommunityHost = sLocalHost;
	string sSrcCommunityPort = sLocalPort;
	if(sSrc[0]=='[') {
	  //tell user what we are doing - this is the short-hand set up...
	  MOOSTrace("Using abbreviated configuration protocol Source: %s@%s:%s\n",
		    sSrcCommunity.c_str(),
		    sSrcCommunityPort.c_str(),
		    sSrcCommunityHost.c_str());	  
	  MOOSChomp(sSrc,"[");
	}
	else {
	  sSrcCommunity = MOOSChomp(sSrc,"@");
	  sSrcCommunityHost = MOOSChomp(sSrc,":");            
	  sSrcCommunityPort = MOOSChomp(sSrc,"[");
	}
	string sVars =MOOSChomp(sSrc,"]"); 
        
	string sDestCommunity     = MOOSChomp(sDest,"@");
	string sDestCommunityHost = MOOSChomp(sDest,":");            
	string sDestCommunityPort = MOOSChomp(sDest,"[");
	string sAliases = MOOSChomp(sDest,"]");

	bool ok = ConfigureLine(sSrcCommunity, sSrcCommunityHost, 
				sSrcCommunityPort, sVars, sDestCommunity, 
				sDestCommunityHost, sDestCommunityPort, sAliases, bUDP);
	if(!ok) {
	  MOOSTrace("error on SHARED configuration %s\n correct line format is \n",
		    original_line.c_str());        
	  MOOSTrace("SHARE = COMMUNITYNAME@HOSTNAME:PORT [VAR1,VAR2,VAR3,....] -> COMMUNITYNAME@HOSTNAME:PORT\n");
	  continue;
	}
      }      
    }
    ///think about setting up UDP connections....there is one UDP Link per instance of a 
    //MOOSBridge. If poepl wnat UDP bridging they need on pMOOSBridge per community (ie the 
    //toplogy of N communities and just 1 bridge is not allowed
    int nLocalUDPPort = DEFAULT_UDP_PORT;
    if(m_MissionReader.GetConfigurationParam("UDPListen",nLocalUDPPort)) {
      //start the UDP listener
      m_UDPLink.Run(nLocalUDPPort);
      m_Comms.Notify("PMB_UDP_LISTEN", nLocalUDPPort);
    }
    else {
      MOOSTrace("warning no UDPListen port specified for local community - outgoing UDP comms only\n");
      
      //passing run with a -1 port means build the socket but don't bind or start a listen thread
      m_UDPLink.Run(-1);
    }
    //ensure we have at least the local MOOS-enabled community in existence - maybe all we want to do is map LocalMOOS->UDP_Out
    CMOOSCommunity * pLocalCommunity  = GetOrMakeCommunity(m_sLocalCommunity);

#if 1    
    if(pLocalCommunity!=NULL && !pLocalCommunity->IsMOOSClientRunning()) {
      //make a connection to the local DB
      pLocalCommunity->InitialiseMOOSClient(sLocalHost, 
					    atoi(sLocalPort.c_str()),
					    m_MissionReader.GetAppName()+"foo",
					    m_nBridgeFrequency);
    }
#endif
    return true;
}

//----------------------------------------------------------------
// Procedure: ConfigureLine

bool CMOOSBridge::ConfigureLine(string src_community,
				string src_community_host,
				string src_community_port,
				string src_vars,
				string dest_community,
				string dest_community_host,
				string dest_community_port,
				string dest_aliases,
				bool   use_udp)
{
  //look for loopback - not always wanted
  if(MOOSStrCmp(dest_community_host, src_community_host) && 
     MOOSStrCmp(dest_community_port, src_community_port)) {
    if(m_bAllowLoopBack==false) {
      MOOSTrace("\t Ignoring Loop Back - (bridge not built)\n");
      return(false);
    }
  }
  
  //convert to numeric after format checking
  if(src_community.empty() || src_community_host.empty() || src_community_port.empty())
    return(false);
  if(dest_community.empty() || dest_community_host.empty() || dest_community_port.empty())
    return(false);

  long lSrcPort = atoi(src_community_port.c_str());            
  long lDestPort = atoi(dest_community_port.c_str());            
  
  
  //we will force all broadcast address directives to be the same "community" called "ALL"
  if(MOOSStrCmp(dest_community_host, "BROADCAST") || MOOSStrCmp(dest_community, "ALL")) {
    //this is trixksy - need to qualify this generic address with the a port so each Bridge can
    //UDP broadcast to multiple addresses
    dest_community="ALL";
    dest_community_host = "BROADCAST-"+dest_community_port;
  }
  
  //make two communities (which will be bridged)
  CMOOSCommunity* pSrcCommunity  = GetOrMakeCommunity(src_community);          
  CMOOSCommunity* pDestCommunity = GetOrMakeCommunity(dest_community);
  
  if(!use_udp) {
    // depending on what kind of share this is we may want to simply specify
    // a UDP end point or start a MOOS client
    // we will register with each DB with a unique name
    std::string sFullyQualifiedMOOSName = m_MissionReader.GetAppName()+"@"+m_sLocalCommunity;
    
    //for (connecting to) the source community (where messages come from)
    if(!pSrcCommunity->IsMOOSClientRunning()) {
      pSrcCommunity->InitialiseMOOSClient(src_community_host,
					  lSrcPort,
					  sFullyQualifiedMOOSName,
					  m_nBridgeFrequency);
    }
    
    // for (connecting to) the destination community (where messages go to)
    if(!pDestCommunity->IsMOOSClientRunning()) {
      pDestCommunity->InitialiseMOOSClient(dest_community_host,
					   lDestPort,
					   sFullyQualifiedMOOSName,
					   m_nBridgeFrequency);
    }
  }
  else {
    // MOOSTrace("Setting UDP port for community %s as %s:%d\n",pDestCommunity->GetCommunityName().c_str(),
    // dest_community_host.c_str(),lDestPort);
    if(dest_community_host.find("BROADCAST-")!=std::string::npos  && MOOSStrCmp(dest_community,"ALL")) {
      //this is special
      pDestCommunity->SetUDPInfo("255.255.255.255", lDestPort);                
    }
    else {
      pDestCommunity->SetUDPInfo(dest_community_host, lDestPort);                
    }
  }
  
  //populate bridge with variables to be shared (including translation)
  if(pSrcCommunity && pDestCommunity) {
    string sVar = MOOSChomp(src_vars,",");
    while(!sVar.empty()) {
      pSrcCommunity->AddSource(sVar);
      CMOOSCommunity::SP Index(sVar,pSrcCommunity->GetCommunityName() );
      pDestCommunity->AddSink(Index,MOOSChomp(dest_aliases,","));
      
      if(use_udp) {
	//we need to store in the Bridge class what variables appearing in our
	//local community we are asked to forward on via UDP to some other
	//commnity
	m_UDPShares.insert(Index);
      }
      
      //suck another VAR
      sVar =  MOOSChomp(src_vars,",");
    }
  }                        
  return(true);
}


//----------------------------------------------------------------
// Procedure: GetOrMakeCommunity

CMOOSCommunity * CMOOSBridge::GetOrMakeCommunity(const string &sCommunity)
{
  CMOOSCommunity* pCommunity = NULL;
  COMMUNITY_MAP::iterator p = m_Communities.find(sCommunity);
  if(p==m_Communities.end()) {
    pCommunity = new CMOOSCommunity;
    pCommunity->Initialise(sCommunity);
    m_Communities[sCommunity] = pCommunity;
  }
  else
    pCommunity = p->second;
  
  return pCommunity;
}

