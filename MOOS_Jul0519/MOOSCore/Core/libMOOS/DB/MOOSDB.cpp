/**
///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Public License
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/gpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/


// MOOSDB.cpp: implementation of the CMOOSDB class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif


#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/getpot/GetPot.hpp"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/MOOSVersion.h"
#include "MOOS/libMOOS/GitVersion.h"
#include "MOOS/libMOOS/DB/MOOSDBLogger.h"
#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"



#include "MOOS/libMOOS/DB/MOOSDB.h"
#include "assert.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>
#include <iterator>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



bool CMOOSDB::OnRxPktCallBack(const std::string & sWho,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx, void * pParam)
{
    CMOOSDB* pMe = (CMOOSDB*)(pParam);
    
    return pMe->OnRxPkt(sWho,MsgListRx,MsgListTx);
}

bool CMOOSDB::OnFetchAllMailCallBack(const std::string & sWho,MOOSMSG_LIST & MsgListTx, void * pParam)
{
    CMOOSDB* pMe = (CMOOSDB*)(pParam);

    return pMe->OnFetchAllMail(sWho,MsgListTx);
}

bool CMOOSDB::OnDisconnectCallBack(string & sClient, void * pParam)
{
    CMOOSDB* pMe = (CMOOSDB*)(pParam);
    
    return pMe->OnDisconnect(sClient);
}

bool CMOOSDB::OnConnectCallBack(string & sClient, void * pParam)
{
    CMOOSDB* pMe = (CMOOSDB*)(pParam);

    return pMe->OnConnect(sClient);
}


CMOOSDB::CMOOSDB()
{
    //here we set up default community names and DB Names
    m_sDBName = "MOOSDB#1";
    m_sCommunityName = "#1";
    m_dfSummaryTime = MOOS::Time();
    
    //her is the default port to listen on
    m_nPort = DEFAULT_MOOS_SERVER_PORT;
    
    m_bQuiet = false;

    //make our own variable called DB_TIME
    {
        CMOOSDBVar NewVar("DB_TIME");
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal= HPMOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = HPMOOSTime();
        m_VarMap["DB_TIME"] = NewVar;
    }
    
    //make our own variable called DB_TIME
    {
        CMOOSDBVar NewVar("DB_UPTIME");
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal= 0;
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = HPMOOSTime();
        m_VarMap["DB_UPTIME"] = NewVar;
    }

    //make our own variable called DB_CLIENTS
    {
        CMOOSDBVar NewVar("DB_CLIENTS");
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal= HPMOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = HPMOOSTime();
        m_VarMap["DB_CLIENTS"] = NewVar;
    }
    
    //make our own variable called DB_EVENT
    {
        CMOOSDBVar NewVar("DB_EVENT");
        NewVar.m_cDataType = MOOS_STRING;
        NewVar.m_dfVal= MOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = MOOSTime();
        m_VarMap["DB_EVENT"] = NewVar;
    }

    //make our own variable called DB_VARSUMMARY
    {
        CMOOSDBVar NewVar("DB_VARSUMMARY");
        NewVar.m_cDataType = MOOS_STRING;
        NewVar.m_dfVal= MOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = MOOSTime();
        m_VarMap["DB_VARSUMMARY"] = NewVar;
    }


    //make our own variable called DB_QOS
    {
        CMOOSDBVar NewVar("DB_QOS");
        NewVar.m_cDataType = MOOS_STRING;
        NewVar.m_dfVal= MOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = MOOSTime();
        m_VarMap["DB_QOS"] = NewVar;
    }

    //make our own variable called DB_RWSUMMARY
    {
        CMOOSDBVar NewVar("DB_RWSUMMARY");
        NewVar.m_cDataType = MOOS_STRING;
        NewVar.m_dfVal= MOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = MOOSTime();
        m_VarMap["DB_RWSUMMARY"] = NewVar;
    }






    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
    
    
}

CMOOSDB::~CMOOSDB()
{
    if(m_pCommServer.get()!=NULL)
        m_pCommServer->Stop();
}




void PrintHelpAndExit()
{
	std::cout<<MOOS::ConsoleColours::Yellow();
	std::cout<<"\nMOOSDB command line help:\n\n";
	std::cout<<MOOS::ConsoleColours::reset();
	std::cout<<"Common MOOS parameters:\n";
	std::cout<<"--moos_file=<string>               specify mission file name (default mission.moos)\n";
	std::cout<<"--moos_port=<positive_integer>     specify server port number (default 9000)\n";
	std::cout<<"--moos_time_warp=<positive_float>  specify time warp\n";
	std::cout<<"--moos_community=<string>          specify community name\n";
    std::cout<<"--moos_print_version               print build and version details\n";
    std::cout<<"--moos_suicide_channel=<str>       suicide monitoring channel (IP address) \n";
    std::cout<<"--moos_suicide_port=<int>          suicide monitoring port  \n";
    std::cout<<"--moos_suicide_phrase=<str>        suicide pass phrase  \n";
    std::cout<<"--moos_suicide_disable             disable suicide monitoring \n";
    std::cout<<"--moos_suicide_print               print suicide conditions \n";
    std::cout<<"--moos_no_colour                   dont use colour in printing \n";





	std::cout<<"\nDB Control:\n";

	std::cout<<"-d    (--dns)                      run with dns lookup\n";
	std::cout<<"-s    (--single_threaded)          run as a single thread (legacy mode)\n";
	std::cout<<"-b    (--moos_boost)               boost priority of communications\n";
	std::cout<<"--moos_timeout=<positive_float>    specify client timeout\n";
	std::cout<<"--response=<string-list>           specify tolerable client latencies in ms\n";
	std::cout<<"--warning_latency=<positive_float>    specify latency above which warning is issued in ms\n";
	std::cout<<"--tcpnodelay                       disable nagle algorithm \n";
	std::cout<<"--audit_port=<unsigned int>        specify port on which to transmit statistics\n";
    std::cout<<"--event_log=<file name>            specify file in which to record events\n";
    std::cout<<"--print_heart_beat                 indicate DB heartbeat every second\n";



//#ifdef MOOSDB_HAS_WEBSERVER
	std::cout<<"--webserver_port=<positive_integer> run webserver on given port\n";
//#endif
	std::cout<<"--help                             print help and exit\n";
	std::cout<<"\nexample:\n";
	std::cout<<"  ./MOOSDB --moos_port=9001 \n";
	std::cout<<"  ./MOOSDB --moos_port=9001 --response=x_app:20,y_app:100,*_instrument:0\n";
	exit(0);
}

void CMOOSDB::OnPrintVersionAndExit()
{
    std::cout<<"--------------------------------------------------\n";
    std::cout<<"MOOS version "<<MOOS_VERSION_NUMBER<<"\n";
    std::cout<<"Built on "<<__DATE__<<" at "<<__TIME__<<"\n";
    std::cout<<MOOS_GIT_VERSION<<"\n";
    std::cout<<"--------------------------------------------------\n";
    exit(0);
}


bool CMOOSDB::Run(int argc,  char * argv[] )
{

	MOOS::CommandLineParser P(argc,argv);

    //disable colour printing maybe
    if(P.GetFlag("--moos_no_colour")){
        MOOS::ConsoleColours::Enable(false);
    }


	//mission file could be first free parameter
	std::string mission_file = P.GetFreeParameter(0, "Mission.moos");

    //set up mission file
    m_MissionReader.SetFile(mission_file);
	
    ///////////////////////////////////////////////////////////
    //what is our community name?
    m_MissionReader.GetValue("COMMUNITY",m_sCommunityName);

    //is the community name being specified on the cli?
    P.GetVariable("--moos_community",m_sCommunityName);

    ///////////////////////////////////////////////////////////
    // make the DB name
    m_sDBName = "MOOSDB_"+m_sCommunityName;


    ///////////////////////////////////////////////////////////
    //what effective time speed up do we want?
    double dfWarp=1.0;
    if(m_MissionReader.GetValue("MOOSTimeWarp",dfWarp))
		SetMOOSTimeWarp(dfWarp);
    //overridden in command line?
    P.GetVariable("--moos_time_warp",dfWarp);
    if(dfWarp>0.0)
        SetMOOSTimeWarp(dfWarp);


    ///////////////////////////////////////////////////////////
    //is there a network - default  - true
	bool bDisableNameLookUp = true;
    m_MissionReader.GetValue("NoNetwork",bDisableNameLookUp);

    if(P.GetFlag("-d","--dns"))
    	bDisableNameLookUp = false;


	
    ///////////////////////////////////////////////////////////
	//get the port which the DB should listen on
    m_nPort = DEFAULT_MOOS_SERVER_PORT;
    m_MissionReader.GetValue("SERVERPORT",m_nPort);
    //command line overide?
    P.GetVariable("--moos_port",m_nPort);
    
    ///////////////////////////////////////////////////////////
    //is there an indication in the mission file that a webserver should run?

    int  nWebServerPort = -1;
	m_MissionReader.GetValue("WEBSERVERPORT",nWebServerPort);
    P.GetVariable("--webserver_port",nWebServerPort);


	//if either the mission file or command line have set a webserver port then launch one
	if(nWebServerPort>0)
		m_pWebServer.reset(new CMOOSDBHTTPServer(GetDBPort(), nWebServerPort));

	double dfClientTimeout = 5.0;
	m_MissionReader.GetValue("ClientTimeout",dfClientTimeout);
    P.GetVariable("--moos_timeout",dfClientTimeout);

    //do we want to fire up the event logger
    std::string sEventLogFileName;
    if(P.GetVariable("--event_log",sEventLogFileName))
    {
        m_EventLogger.Run(sEventLogFileName);
    }


    ///////////////////////////////////////////////////////////
	double dfWarningLatencyMS = 50;
	m_MissionReader.GetValue("WarningLatency",dfWarningLatencyMS);
    P.GetVariable("--warning_latency",dfWarningLatencyMS);


    if(P.GetFlag("--moos_print_version"))
        OnPrintVersionAndExit();

#ifdef DEFAULT_NO_NAGLE
    bool bTCPNoDelay = true;
#else
    bool bTCPNoDelay = false;
#endif
    m_MissionReader.GetValue("tcpnodelay",bTCPNoDelay);

    if(P.GetFlag("--tcpnodelay"))
    	bTCPNoDelay = true;



    ///////////////////////////////////////////////////////////
    //are we looking for help?
    if(P.GetFlag("-h","--help"))
    	PrintHelpAndExit();

    ///////////////////////////////////////////////////////////
	//are we looking for help?
	bool bBoost = P.GetFlag("--moos_boost","-b");

    ///////////////////////////////////////////////////////////
    //are we being asked to be old skool and use a single thread?
    bool bSingleThreaded = P.GetFlag("-s","--single_threaded");


    //is the community name being specified on the cli?
	unsigned int nAuditPort=9020;
	P.GetVariable("--audit_port",nAuditPort);





    std::string sSuicideAddress;
    if(P.GetVariable("--moos_suicide_channel",sSuicideAddress))
    {
        m_SuicidalSleeper.SetChannel(sSuicideAddress);
    }

    int nSuicidePort(0);
    if(P.GetVariable("--moos_suicide_port",nSuicidePort))
    {
        m_SuicidalSleeper.SetPort(nSuicidePort);
    }

    std::string sSuicidePhrase;
    if(P.GetVariable("--moos_suicide_phrase",sSuicidePhrase))
    {
        m_SuicidalSleeper.SetPassPhrase(sSuicidePhrase);
    }

    if(P.GetFlag("--moos_suicide_print"))
    {
        std::cerr<<"suicide terms and conditions are:\n";
        std::cerr<<" channel  "<<m_SuicidalSleeper.GetChannel()<<"\n";
        std::cerr<<" port     "<<m_SuicidalSleeper.GetPort()<<"\n";
        std::cerr<<" phrase   \""<<m_SuicidalSleeper.GetPassPhrase()<<"\"\n";

    }

    if(!P.GetFlag("--moos_suicide_disable"))
    {
        m_SuicidalSleeper.SetName(m_sDBName);
        m_SuicidalSleeper.Run();
    }





    
    LogStartTime();
    
    if(bSingleThreaded)
    {
        std::cout<<MOOS::ConsoleColours::yellow()<<"warning : running in single threaded mode performance will be affected by poor networks\n"<<MOOS::ConsoleColours::reset();
        m_pCommServer.reset(new CMOOSCommServer);
    }
    else
    {
        //std::cerr<<MOOS::ConsoleColours::green()<<"running in multi-threaded mode\n"<<MOOS::ConsoleColours::reset();
        m_pCommServer.reset(new MOOS::ThreadedCommServer);
    }

    m_pCommServer->SetQuiet(m_bQuiet);

    m_pCommServer->SetOnRxCallBack(OnRxPktCallBack,this);

    m_pCommServer->SetOnDisconnectCallBack(OnDisconnectCallBack,this);

    m_pCommServer->SetOnConnectCallBack(OnConnectCallBack,this);

    m_pCommServer->SetOnFetchAllMailCallBack(OnFetchAllMailCallBack,this);

    m_pCommServer->SetClientTimeout(dfClientTimeout);

    m_pCommServer->SetWarningLatencyMS(dfWarningLatencyMS);

    m_pCommServer->SetTCPNoDelay(bTCPNoDelay);

    m_pCommServer->BoostIOPriority(bBoost);

    m_pCommServer->SetCommandLineParameters(argc,argv);

    m_pCommServer->Run(m_nPort,m_sCommunityName,bDisableNameLookUp,nAuditPort);

    m_EventLogger.AddEvent("DBStart","MOOSDB",MOOSFormat("Port=%d",m_nPort));
        
    return true;
}

bool CMOOSDB::IsRunning()
{
	if(m_pCommServer.get()==NULL)
		return false;

	return m_pCommServer->IsRunning();
}


bool CMOOSDB::SetQuiet(bool bQuiet)
{
    m_bQuiet = bQuiet;
    if(m_pCommServer.get()!=NULL)
        m_pCommServer->SetQuiet(m_bQuiet);

    return true;
}


void CMOOSDB::UpdateDBClientsVar()
{
    STRING_LIST Clients;
    m_pCommServer->GetClientNames(Clients);

    std::ostringstream ss;
    std::copy(Clients.begin(),Clients.end(),ostream_iterator<std::string>(ss,","));

    CMOOSMsg DBC(MOOS_NOTIFY,"DB_CLIENTS",ss.str());
    DBC.m_sOriginatingCommunity = m_sCommunityName;
    DBC.m_sSrc = m_sDBName;
    OnNotify(DBC);

}

void CMOOSDB::UpdateQoSVar()
{
    CMOOSMsg DBQOS(MOOS_NOTIFY,"DB_QOS","");

    if(!m_pCommServer->GetTimingStatisticSummary(DBQOS.m_sVal))
        return;

    DBQOS.m_sSrc = m_sDBName;
    DBQOS.m_sOriginatingCommunity = m_sCommunityName;
    OnNotify(DBQOS);

}

template< class T>
void PrintCollection( const T & collection, ostream & out, const std::string & delim = "," )
{
    typename T::const_iterator q;
    for(q = collection.begin();q!=collection.end();)
    {
        out<<(*q);
        if(++q!=collection.end())
            out<<delim;
    }
}

void CMOOSDB::UpdateReadWriteSummaryVar()
{

    std::map<std::string,std::list<std::string> > Pub;
    std::map<std::string,std::list<std::string> > Sub;

    DBVAR_MAP::iterator p;

    for(p=m_VarMap.begin();p!=m_VarMap.end();++p)
    {
        CMOOSDBVar  & rVar = p->second;
        STRING_SET::iterator w;
        REGISTER_INFO_MAP::iterator v;

        std::stringstream ss;
        for(v = rVar.m_Subscribers.begin();v!=rVar.m_Subscribers.end();++v)
            Sub[v->second.m_sClientName].push_back(rVar.m_sName);

        for(w = rVar.m_Writers.begin();w!=rVar.m_Writers.end();++w)
            Pub[*w].push_back(rVar.m_sName);
    }


    STRING_LIST Clients;
    m_pCommServer->GetClientNames(Clients);
    STRING_LIST::iterator q;

    std::ostringstream ss;
    for(q=Clients.begin();q!=Clients.end();)
    {
        ss<<*q<<"=";
        PrintCollection(Sub[*q],ss,":");
        ss<<"&";
        PrintCollection(Pub[*q],ss,":");

        if(++q!=Clients.end())
            ss<<",";

    }

    CMOOSMsg DBS(MOOS_NOTIFY,"DB_RWSUMMARY",ss.str());
    DBS.m_sSrc = m_sDBName;
    DBS.m_sOriginatingCommunity = m_sCommunityName;
    OnNotify(DBS);

}

void CMOOSDB::UpdateDBTimeVars()
{
    CMOOSMsg DBT(MOOS_NOTIFY,"DB_TIME",MOOSTime());
    DBT.m_sOriginatingCommunity = m_sCommunityName;
    DBT.m_sSrc = m_sDBName;
    OnNotify(DBT);

    CMOOSMsg DBUpT(MOOS_NOTIFY,"DB_UPTIME",MOOSTime()-GetStartTime());
    DBUpT.m_sOriginatingCommunity = m_sCommunityName;
    DBUpT.m_sSrc = m_sDBName;
    OnNotify(DBUpT);

}

/**this will be called each time a new packet is recieved*/
bool CMOOSDB::OnRxPkt(const std::string & sClient,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx)
{

    MOOSMSG_LIST::iterator p;
    
    for(p = MsgListRx.begin();p!=MsgListRx.end();++p)
    {
        ProcessMsg(*p,MsgListTx);
    }
    

    double dfNow = MOOS::Time();
    if(dfNow-m_dfSummaryTime>2.0)
    {
        m_dfSummaryTime = dfNow;

        //good spot to update our internal time
        UpdateDBTimeVars();

        //and send clients an occasional membersip list
        UpdateDBClientsVar();

        //update a db summary var once in a while
        UpdateSummaryVar();

        //update quality of service summary
        UpdateQoSVar();

        //update variable which publishes who is reading and writing what
        UpdateReadWriteSummaryVar();
    }

    if(!MsgListRx.empty())
    {
        
        //now we fill in the packet with our replies to THIS CLIENT
        MOOSMSG_LIST_STRING_MAP::iterator q = m_HeldMailMap.find(sClient);
        
        if(q==m_HeldMailMap.end())
        {
            
            //CMOOSMsg & rMsg = MsgListRx.front();
            //there is no mail waiting to be sent to this client
            //should only happen at start up...
            //string sClient = MsgListRx.front().m_sSrc;
            
            MOOSMSG_LIST NewList;
            
            m_HeldMailMap[sClient] = NewList;
            
            q = m_HeldMailMap.find(sClient);
            
            assert(q!=m_HeldMailMap.end());
        }
                
        if(q!=m_HeldMailMap.end())
        {
            //MOOSTrace("%f OnRxPkt %d messages held for client %s\n",MOOSTime(),q->second.size(),sClient.c_str());

            if(!q->second.empty())
            {
                //copy all the held mail to MsgListTx
                MsgListTx.splice(MsgListTx.begin(),
                		q->second,
                		q->second.begin(),
                		q->second.end());
            }
        }
    }
    
    return true;
}

bool CMOOSDB::OnFetchAllMail(const std::string & sWho,MOOSMSG_LIST & MsgListTx)
{
	MOOSMSG_LIST_STRING_MAP::iterator q = m_HeldMailMap.find(sWho);
	if(q!=m_HeldMailMap.end())
	{
		if(!q->second.empty())
		{
            MsgListTx.splice(MsgListTx.begin(),
            		q->second,
            		q->second.begin(),
            		q->second.end());
		}
	}
	return true;
}

/** This functions decides what needs to be done on a message by message basis */
bool CMOOSDB::ProcessMsg(CMOOSMsg &MsgRx,MOOSMSG_LIST & MsgListTx)
{
    
    
    switch(MsgRx.m_cMsgType)
    {
    case MOOS_NOTIFY:    //NOTIFICATION
        return OnNotify(MsgRx);
        break;
    case MOOS_WILDCARD_UNREGISTER:
    case MOOS_UNREGISTER:
        return OnUnRegister(MsgRx);
        break;
    case MOOS_REGISTER:    //REGISTRATION
    case MOOS_WILDCARD_REGISTER:
        return OnRegister(MsgRx);
        break;
    case MOOS_NULL_MSG:
        break;    
    case MOOS_COMMAND:  //COMMAND
        break;
    case MOOS_SERVER_REQUEST:
        return DoServerRequest(MsgRx,MsgListTx);
        break;
    }
    
    return true;
}


/** called when the in focus client is telling us something
has changed. Ie this is a notify packet */
bool CMOOSDB::OnNotify(CMOOSMsg &Msg)
{
    double dfTimeNow = HPMOOSTime();
    
    CMOOSDBVar & rVar  = GetOrMakeVar(Msg);
    
    if(rVar.m_nWrittenTo==0)
    {
        rVar.m_cDataType=Msg.m_cDataType;


        //look to see if any existing wildcards make us want to subscribe
		//to this new message
		HASH_MAP_TYPE<std::string, std::set<MOOS::MsgFilter> >::const_iterator g;
		for (g = m_ClientFilters.begin(); g != m_ClientFilters.end(); ++g)
		{
			//for every client
			std::set<MOOS::MsgFilter>::const_iterator h;
			for (h = g->second.begin(); h != g->second.end(); ++h)
			{
				//for every filter
				if (h->Matches(Msg))
				{
					//add the filter owner (client *g) as a subscriber
					rVar.AddSubscriber(g->first, h->period());
					if(!m_bQuiet)
					{
                        std::cout<<"+ subs of \""<<g->first<<"\" to \""
                                <<Msg.GetKey()<<"\" via wildcard \""<<h->as_string()
                                <<"\""<<std::endl;
					}
				}
			}
		}

    }
    
    if(rVar.m_cDataType==Msg.m_cDataType)
    {
        
        rVar.m_dfWrittenTime = dfTimeNow;
        
        rVar.m_dfTime        = Msg.m_dfTime;
        
        rVar.m_sWhoChangedMe = Msg.m_sSrc;
        
        rVar.m_sSrcAux       = Msg.m_sSrcAux; // Added by mikerb 5-29-12
        
        if(Msg.m_sOriginatingCommunity.empty())
        {
            //we are the server in the originating community
            rVar.m_sOriginatingCommunity = m_sCommunityName;
            Msg.m_sOriginatingCommunity = m_sCommunityName;
        }
        else
        {
            //this message came from another community
            rVar.m_sOriginatingCommunity = Msg.m_sOriginatingCommunity;
        }
        
        switch(rVar.m_cDataType)
        {
        case MOOS_DOUBLE:
            rVar.m_dfVal = Msg.m_dfVal;
            break;
        case MOOS_STRING:
		case MOOS_BINARY_STRING:
            rVar.m_sVal = Msg.m_sVal;
            break;
        }
        
        //record sSrc as a writer of this data
        rVar.m_Writers.insert(Msg.m_sSrc);
        
        //increment the number of times we have written to this variable
        rVar.m_nWrittenTo++;
        
        //how often is it being written?
        double dfDT = (dfTimeNow-rVar.m_Stats.m_dfLastStatsTime);
        int nWrites  = rVar.m_nWrittenTo-rVar.m_Stats.m_nLastStatsWrites;
        if(dfDT>0.5)
        {
            //this looks a little hookey - the numbers are arbitrary to give sensible
            //looking frequencies when timing is coarse
            if(dfDT>10.0)
            {
                //MIN
                rVar.m_dfWriteFreq = 0.0;
            }
            else
            {
                //IIR FILTER COOEFFICENT
                double dfAlpha = 0.5;
                double df = dfDT/nWrites;

                rVar.m_dfWriteFreq = dfAlpha*rVar.m_dfWriteFreq + (1.0-dfAlpha)/(df);
            }
            rVar.m_Stats.m_nLastStatsWrites = rVar.m_nWrittenTo;
            rVar.m_Stats.m_dfLastStatsTime = dfTimeNow;
        }
        
        //now comes the intersting part...
        //which clients have asked to be informed
        //of changes in this variable?
        REGISTER_INFO_MAP::iterator p;
        
        
        for(p = rVar.m_Subscribers.begin();p!=rVar.m_Subscribers.end();++p)
        {
            
            CMOOSRegisterInfo & rInfo = p->second;
            //has enough time expired since the last time we
            //sent notification for the variable?
            if(rInfo.Expired(dfTimeNow))
            {
                
                string  & sClient = p->second.m_sClientName;
                
                //the Msg we were passed has all the information we require already
                Msg.m_cMsgType = MOOS_NOTIFY;
                
                
                AddMessageToClientBox(sClient,Msg);
                

                //finally we remember when we sent this to the client in question
                rInfo.SetLastTimeSent(dfTimeNow);
            }
        }
    }
    else
    {
        MOOSTrace("Attempting to update var \"%s\" which is type %c with data type %c by client %s (=%s)\n",
            Msg.m_sKey.c_str(),
            rVar.m_cDataType,
            Msg.m_cDataType,
            Msg.m_sSrc.c_str(),
            Msg.GetAsString(20,4).c_str());

    }
    
    
    
    return true;
}


/** we now want to store some message in anoth cleints message box, when they next call
in they shall be informed of the change by stuffing this msg into a return packet */
bool    CMOOSDB::AddMessageToClientBox(const string &sClient,CMOOSMsg & Msg)
{
    MOOSMSG_LIST_STRING_MAP::iterator q = m_HeldMailMap.find(sClient);
    
    if(q==m_HeldMailMap.end())
    {
        //there is no mail waiting to be sent to this client
        //should only happen at start up...
        MOOSMSG_LIST NewList;
        m_HeldMailMap[sClient] = NewList;
        
        q = m_HeldMailMap.find(sClient);
        
        assert(q!=m_HeldMailMap.end());
    }
    
    //q->second is now a reference to a list of messages that will be
    //sent to sClient the next time it calls into the database...   
    q->second.push_back(Msg);
    
    return true;
}


/** Called when a msg containing a unregistration (desubscribe) 
request is received */
bool CMOOSDB::OnUnRegister(CMOOSMsg &Msg)
{
	if(Msg.IsType(MOOS_UNREGISTER))
	{
		//what are we looking to un register for?
		bool bAlreadyThere = VariableExists(Msg.m_sKey);
		if(bAlreadyThere)
		{
			CMOOSDBVar & rVar  = GetOrMakeVar(Msg);
			rVar.RemoveSubscriber(Msg.m_sSrc);
		}
	}
	else if (Msg.IsType(MOOS_WILDCARD_UNREGISTER))
	{
		//here we parse out the filter
		std::string app_pattern = "";
		std::string var_pattern = "";
		double period = 0.0;


		MOOSValFromString(app_pattern,Msg.GetString(),"AppPattern");
		MOOSValFromString(var_pattern,Msg.GetString(),"VarPattern");
		MOOS::MsgFilter F(app_pattern,var_pattern,period);

		DBVAR_MAP::iterator q;
		for(q = m_VarMap.begin();q!=m_VarMap.end();++q)
		{
			CMOOSMsg M;
			Var2Msg(q->second,M);
			if(F.Matches(M))
			{
				M.m_cMsgType = MOOS_UNREGISTER;
				M.m_cDataType = MOOS_STRING;
				M.m_sSrc = Msg.GetSource();
				M.m_sKey = q->first;
				OnUnRegister(M);//smart...
			}
		}
	}
    
    return true;
}


/** Called when a msg containing a registration (subscription) 
request is received */
bool CMOOSDB::OnRegister(CMOOSMsg &Msg)
{
    
    //what are we looking to register for?
	if(Msg.IsType(MOOS_REGISTER))
	{

		//if the variable already exists then post a notification message
		//to the client
		bool bAlreadyThere = VariableExists(Msg.m_sKey);

		CMOOSDBVar & rVar  = GetOrMakeVar(Msg);

        //PMN drops this check to allow notification
        //periods to be changed dynamically 21/12/17
//		if(rVar.HasSubscriber(Msg.m_sSrc))
//			return true;

		if(!rVar.AddSubscriber(Msg.m_sSrc,Msg.m_dfVal))
			return false;

        double dfActualPeriod;
        if(!rVar.GetUpdatePeriod(Msg.m_sSrc,dfActualPeriod)){
            return false;
        }
        std::string detail =MOOSFormat("%s@%.1f",rVar.m_sName.c_str(),dfActualPeriod);
        m_EventLogger.AddEvent("register",
                               Msg.m_sSrc,
                               detail);

		if(bAlreadyThere && rVar.m_nWrittenTo!=0)
		{
			//when the client registered the variable already existed...
			//better tell them
			CMOOSMsg ReplyMsg;
			Var2Msg(rVar,ReplyMsg);

			ReplyMsg.m_cMsgType = MOOS_NOTIFY;

			AddMessageToClientBox(Msg.m_sSrc,ReplyMsg);

        	rVar.m_Subscribers[Msg.m_sSrc].SetLastTimeSent(MOOS::Time());

		}
	}
	else if(Msg.IsType(MOOS_WILDCARD_REGISTER))
	{
		//here we parse out the filter
		std::string app_pattern = "";
		std::string var_pattern = "";
		double period = 0.0;


		MOOSValFromString(app_pattern,Msg.GetString(),"AppPattern");
		MOOSValFromString(var_pattern,Msg.GetString(),"VarPattern");
		MOOSValFromString(period,Msg.GetString(),"Interval");
		MOOS::MsgFilter F(app_pattern,var_pattern,period);

		//store this filter we will need it later when new
		//as yet undiscovered variables are written
		m_ClientFilters[Msg.GetSource()].insert(F);


        m_EventLogger.AddEvent("wildcard",Msg.m_sSrc,Msg.GetString());


		//now iterate over all existing variables and see if they match
		//if the do simply register for them...
		DBVAR_MAP::iterator q;
		for(q = m_VarMap.begin();q!=m_VarMap.end();++q)
		{
			CMOOSMsg M;
			Var2Msg(q->second,M);
			if(F.Matches(M))
			{
				M.m_cMsgType = MOOS_REGISTER;
				M.m_cDataType = MOOS_DOUBLE;
				M.m_dfVal = period;
				M.m_sSrc = Msg.GetSource();

				if(!m_bQuiet)
				{
                    std::cout<<MOOS::ConsoleColours::yellow()
                            <<"+ subs of \""
                            <<Msg.GetSource()<<"\" to variables matching \""
                            <<var_pattern<<":"<<app_pattern<<"\""
                            <<MOOS::ConsoleColours::reset()<<std::endl;
				}

				OnRegister(M);//smart...
			}
		}



	}
    
    return true;
}


/** return a reference to a DB variable is it already exists
and if not make one and then return a reference to it.
@param Msg      Msg.m_sKey contains name of variable
*/
CMOOSDBVar & CMOOSDB::GetOrMakeVar(CMOOSMsg &Msg)
{    
    
    //look up this variable name
    DBVAR_MAP::iterator p = m_VarMap.find(Msg.m_sKey);
    

    if(p==m_VarMap.end())
    {
        //we need to make a new variable here for this key
        //as we don't know about it!
        
        CMOOSDBVar NewVar(Msg.m_sKey);
        
        switch(Msg.m_cMsgType)
        {
        case MOOS_REGISTER:
        	//interesting case is when this method is being used to
        	//register for a variable that has not been written to. Here
        	//we simply make the variable but don't commit to its data type
        	NewVar.m_cDataType = MOOS_NOT_SET;
        	break;
        case MOOS_NOTIFY:
        	//we are making a new variable
    		//new variable will have data type of request message
        	NewVar.m_cDataType = Msg.m_cDataType;
        	break;
        default:
        	///nothing to do  - I wonder what this is?
        	break;
        }



        //index our new creation
        m_VarMap[Msg.m_sKey] = NewVar;
        
        //check we can get it back ok!!
        p = m_VarMap.find(Msg.m_sKey);
        
        assert(p!=m_VarMap.end());
        
#ifdef DB_VERBOSE
        
        MOOSTrace("\nMaking new variable: \n");
        MOOSTrace("    Name=\"%s\"\n",NewVar.m_sName.c_str());
        MOOSTrace("    Src =\"%s\"\n",Msg.m_sSrc.c_str());
        MOOSTrace("    Type =\"%c\"\n",NewVar.m_cDataType);
#endif

        m_EventLogger.AddEvent("create",Msg.GetSource(),Msg.GetName());

    }
    

    //ok we know what you are talking about
    CMOOSDBVar & rVar = p->second;
    
    //return the reference
    return rVar;
}


bool CMOOSDB::OnConnect(string &sClient)
{
    m_EventLogger.AddEvent("connect",sClient,"client connects");

    //notify ourselves....
    CMOOSMsg DBC(MOOS_NOTIFY,"DB_EVENT",MOOSFormat("connected=%s",sClient.c_str()));
    DBC.m_sOriginatingCommunity = m_sCommunityName;
    DBC.m_sSrc = m_sDBName;
    OnNotify(DBC);


    return true;
}

bool CMOOSDB::OnDisconnect(string &sClient)
{
    //for all variables remove subscriptions to sClient
    if(!m_bQuiet)
    {
        std::cout<<MOOS::ConsoleColours::yellow()<<sClient<<" is leaving...           ";
    }
    
    DBVAR_MAP::iterator p;
    
    for(p=m_VarMap.begin();p!=m_VarMap.end();++p)
    {
        CMOOSDBVar  & rVar = p->second;
        
        rVar.RemoveSubscriber(sClient);
    }
    if(m_ClientFilters.find(sClient)!=m_ClientFilters.end())
    {
    	m_ClientFilters[sClient].clear();
    }
    
    m_HeldMailMap.erase(sClient);
    
    if(!m_bQuiet)
        std::cout<<MOOS::ConsoleColours::Green()<<"[OK]\n"<<MOOS::ConsoleColours::reset();

    m_EventLogger.AddEvent("disconnect",sClient,"client disconnects");


    //notify ourselves....
    CMOOSMsg DBC(MOOS_NOTIFY,"DB_EVENT",MOOSFormat("disconnected=%s",sClient.c_str()));
    DBC.m_sOriginatingCommunity = m_sCommunityName;
    DBC.m_sSrc = m_sDBName;
    OnNotify(DBC);

    return true;
}

bool CMOOSDB::DoServerRequest(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    //explictly requesting the server to do something...
    
    if(Msg.m_sKey=="ALL")
    {
        return OnServerAllRequested(Msg,MsgTxList);
    }
    else if(Msg.m_sKey.find("PROC_SUMMARY")!=string::npos)
    {
        return OnProcessSummaryRequested(Msg,MsgTxList);
    }
    else if (Msg.m_sKey.find("VAR_SUMMARY") != string::npos) 
    {
        return OnVarSummaryRequested(Msg, MsgTxList);
    }
    else if(Msg.m_sKey.find("DB_CLEAR")!=string::npos)
    {
        return OnClearRequested(Msg,MsgTxList);
    }
    
    
    
    return false;
}

void CMOOSDB::UpdateSummaryVar()
{

    std::stringstream ss;
    DBVAR_MAP::iterator p;

    for(p=m_VarMap.begin();p!=m_VarMap.end();++p)
    {
        ss<<std::left<<std::setw(20);
        ss<<p->first<<" ";

        ss<<std::left<<std::setw(20);
        ss<<MOOS::TimeToDate(p->second.m_dfWrittenTime,false,true)<<" ";

        ss<<std::left<<std::setw(20);
        if(p->second.m_sWhoChangedMe.empty())
        {
            ss<<"(write pending)"<<" ";
        }
        else
        {
            ss<<p->second.m_sWhoChangedMe<<" ";
        }

        //write frequency
        ss << std::fixed << std::setw( 4 ) << std::setprecision( 1 ) << p->second.m_dfWriteFreq<< "Hz ";

        ss<<std::left<<std::setw(2);
        ss<<p->second.m_cDataType<<" ";

        switch(p->second.m_cDataType)
        {
            case MOOS_DOUBLE:
                ss<<p->second.m_dfVal;
                break;
            case MOOS_STRING:
            {
                unsigned int s = p->second.m_sVal.size();
                if(s>25)
                    ss<<(p->second.m_sVal.substr(0,22)+"...");
                else
                    ss<<p->second.m_sVal;

                break;
            }
            case MOOS_BINARY_STRING:
            {
                unsigned int s = p->second.m_sVal.size();
                std::string bss;
                if(s<1024)
                    bss = MOOSFormat("*binary* %-4d B",s);
                else if(s<1024*1024)
                    bss = MOOSFormat("*binary* %.3f KB",s/(1024.0));
                else
                    bss = MOOSFormat("*binary* %.3f MB",s/(1024.0*1024.0));

                ss<<bss;
                break;
            }
        }


        ss<<"\n";

    }

    CMOOSMsg DBC(MOOS_NOTIFY,"DB_VARSUMMARY",ss.str());
    DBC.m_sOriginatingCommunity = m_sCommunityName;
    DBC.m_sSrc = m_sDBName;
    OnNotify(DBC);

}

bool CMOOSDB::OnProcessSummaryRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    DBVAR_MAP::iterator p;
    STRING_LIST::iterator q;
    STRING_LIST Clients;
    
    m_pCommServer->GetClientNames(Clients);

    for(q=Clients.begin();q!=Clients.end();++q)
    {
        string sWho = *q;
        
        string sPublished= "PUBLISHED=";
        string sSubscribed = "SUBSCRIBED=";
        
        for(p=m_VarMap.begin();p!=m_VarMap.end();++p)
        {
            CMOOSDBVar  & rVar = p->second;
            
            if(rVar.m_Writers.find(sWho)!=rVar.m_Writers.end())
            {
                if(!sPublished.empty())
                {
                    sPublished+=",";
                }
                sPublished+=rVar.m_sName;
                
            }
            
            if(rVar.m_Subscribers.find(sWho)!=rVar.m_Subscribers.end())
            {
                if(!sSubscribed.empty())
                {
                    sSubscribed+=",";
                }
                sSubscribed+=rVar.m_sName;
            }
        }
        
        
        CMOOSMsg MsgReply;
        
        MsgReply.m_nID        = Msg.m_nID;
        
        MsgReply.m_cMsgType   = MOOS_NOTIFY;
        MsgReply.m_cDataType  = MOOS_STRING;
        MsgReply.m_dfTime     = MOOSTime()-m_dfStartTime; //for display
        MsgReply.m_sSrc       = m_sDBName;
        MsgReply.m_sKey       = "PROC_SUMMARY";
        MsgReply.m_sVal       = sWho+":"+sSubscribed+","+sPublished;
        MsgReply.m_dfVal      = -1;
        
        
        MsgTxList.push_front(MsgReply);
    }
    
    return true;
    
}

bool CMOOSDB::OnServerAllRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    
    DBVAR_MAP::iterator p;
    
    for(p=m_VarMap.begin();p!=m_VarMap.end();++p)
    {
        CMOOSDBVar  & rVar = p->second;
        
        CMOOSMsg MsgVar;
        
        MsgVar.m_nID        = Msg.m_nID;
        
        MsgVar.m_cMsgType   = MOOS_NOTIFY;
        MsgVar.m_cDataType  = rVar.m_cDataType;
        MsgVar.m_dfTime     = rVar.m_dfWrittenTime-m_dfStartTime; //for display
        MsgVar.m_sSrc       = rVar.m_sWhoChangedMe;
        MsgVar.m_sKey       = rVar.m_sName;
        MsgVar.m_sVal       = rVar.m_sVal;
		MsgVar.m_sSrcAux    = rVar.m_sSrcAux;
        MsgVar.m_dfVal      = rVar.m_dfVal;
        MsgVar.m_dfVal2     = rVar.m_dfWriteFreq;
        MsgVar.m_sOriginatingCommunity = rVar.m_sOriginatingCommunity;
        
        if(MsgVar.m_dfTime<0) 
        {
            MsgVar.m_dfTime =-1;
        }
        
        MsgTxList.push_front(MsgVar);
        
        
    }
    
    
    return true;
}

//Suggested addition by MIT users 2006 - shorter version of OnProcessSummary
bool CMOOSDB::OnVarSummaryRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    std::string TheVars;
    DBVAR_MAP::iterator p;
    for(p = m_VarMap.begin(); p != m_VarMap.end(); ++p) 
    {
        //look to a comma
        if(p!=m_VarMap.begin())
            TheVars += ",";

        TheVars += p->first;
    }
    
    CMOOSMsg Reply;

    //so the client knows the query result correspondences
    Reply.m_nID = Msg.m_nID; 
    Reply.m_cMsgType = MOOS_NOTIFY;
    Reply.m_cDataType = MOOS_STRING;
    Reply.m_dfTime = MOOSTime();
    Reply.m_sSrc = m_sDBName;
    Reply.m_sKey = "VAR_SUMMARY";
    Reply.m_sVal = TheVars;
    Reply.m_dfVal = -1;

    MsgTxList.push_front(Reply);

    return true;
}





bool CMOOSDB::OnClearRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
	MOOS::DeliberatelyNotUsed(Msg);
	MOOS::DeliberatelyNotUsed(MsgTxList);

    MOOSTrace("Clear Down Requested:\n");
    
    MOOSTrace("    Resetting %d variables...",m_VarMap.size());
    
    DBVAR_MAP::iterator p;
    
    for(p = m_VarMap.begin();p!=m_VarMap.end();++p)
    {
        CMOOSDBVar & rVar = p->second;
        rVar.Reset();
    }
    MOOSTrace("done\n");
    
    
    MOOSTrace("    Removing %d existing notification queues...",m_HeldMailMap.size());
    MOOSMSG_LIST_STRING_MAP::iterator q;
    
    for(q = m_HeldMailMap.begin();q!=m_HeldMailMap.end();++q)
    {
        MOOSMSG_LIST & rList = q->second;
        rList.clear();
    }
    MOOSTrace("done\n");
    
    //MOOSTrace("    resetting DB start Time...done\n");
    LogStartTime();
    
    
    return true;
    
}


void CMOOSDB::LogStartTime()
{
    m_dfStartTime = MOOSTime();
    
    return;
  
    
}



bool CMOOSDB::VariableExists(const string &sVar)
{
    DBVAR_MAP::iterator p=m_VarMap.find(sVar);
    
    return (!m_VarMap.empty()) && (p!=m_VarMap.end());
}

void CMOOSDB::Var2Msg(CMOOSDBVar &Var, CMOOSMsg &Msg)
{
    Msg.m_dfTime = Var.m_dfTime;
    Msg.m_cDataType = Var.m_cDataType;
    Msg.m_sSrc = Var.m_sWhoChangedMe;
    Msg.m_sSrcAux = Var.m_sSrcAux;   // Added by mikerb 5-29-12
    Msg.m_sKey = Var.m_sName;
    Msg.m_sOriginatingCommunity = Var.m_sOriginatingCommunity;
    switch(Var.m_cDataType)
    {
    case MOOS_DOUBLE:
        Msg.m_dfVal = Var.m_dfVal;
        break;
    case MOOS_STRING:
	case MOOS_BINARY_STRING:
        Msg.m_sVal = Var.m_sVal;
        break;
    }
}


