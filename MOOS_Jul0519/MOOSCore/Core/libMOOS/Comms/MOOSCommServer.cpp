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
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/



// MOOSCommServer.cpp: implementation of the CMOOSCommServer class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
	#pragma warning(disable:4018) // signed/unsigned comparison
	#pragma warning(disable:4389) // signed/unsigned operation
	#pragma warning(disable:4127) // conditional expression is constant
#endif

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Comms/MOOSCommServer.h"
#include "MOOS/libMOOS/Comms/MOOSCommPkt.h"
#include "MOOS/libMOOS/Utils/MOOSException.h"
#include "MOOS/libMOOS/Comms/XPCTcpSocket.h"
#include "MOOS/libMOOS/Utils/ThreadPriority.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"

#include <iostream>
#include <stdexcept>
#include <cmath>

using namespace std;

#ifndef _WIN32
    #include <sys/signal.h>
#endif

#define TOLERABLE_SILENCE 5.0
#define TOLERABLE_TRANSIT_TIME 0.015;
#define DEFAULT_SERVER_SOCKET_RECEIVE_BUFFER_SIZE_KB 128
#define DEFAULT_SERVER_SOCKET_SEND_BUFFER_SIZE_KB 128

MOOS::ThreadPrint gMOOSCommsServerTheadPrinter(std::cerr);

#ifdef _WIN32
#define INVALID_SOCKET_SELECT WSAEINVAL
#else
#define INVALID_SOCKET_SELECT EBADF
#endif

const double kHeartBeatPrintPeriod = 1.0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


bool ServerListenLoopProc(void * pParameter)
{

    CMOOSCommServer* pMe =     (CMOOSCommServer*)pParameter;

    return pMe->ListenLoop();
}

bool  ServerLoopProc( void * pParameter)
{

    CMOOSCommServer* pMe =     (CMOOSCommServer*)pParameter;

    return pMe->ServerLoop();
}

bool TimerLoopProc( void * pParameter)
{

    CMOOSCommServer* pMe =     (CMOOSCommServer*)pParameter;

    return pMe->TimerLoop();
}

bool CMOOSCommServer::StartThreads()
{
    m_bQuit = false;

    if(!m_ListenThread.Initialise(ServerListenLoopProc, this))
        return false;
    if(!m_ServerThread.Initialise(ServerLoopProc, this))
        return false;
    if(!m_TimerThread.Initialise(TimerLoopProc, this))
        return false;

    if(!m_ListenThread.Start())
        return false;
    if(!m_ServerThread.Start())
        return false;
    if(!m_TimerThread.Start())
        return false;

    return true;

}

CMOOSCommServer::CMOOSCommServer()
{
    m_nMaxSocketFD = 0;
    m_pfnRxCallBack = NULL;
    m_pfnDisconnectCallBack = NULL;
    m_pfnConnectCallBack = NULL;
	m_pfnFetchAllMailCallBack = NULL;
    m_sCommunityName = "#1";
    m_bQuiet  = false;
	m_bDisableNameLookUp = true;
	m_bQuit = false;
    m_bPrintHeartBeat = false;

	m_bBoostIOThreads= false;

	m_dfClientTimeout = TOLERABLE_SILENCE;
	m_dfCommsLatencyConcern = TOLERABLE_TRANSIT_TIME;
}

CMOOSCommServer::~CMOOSCommServer()
{
    Stop();
}

bool CMOOSCommServer::Stop()
{
    if(m_TimerThread.IsThreadRunning())
        m_TimerThread.Stop();


    if(m_ListenThread.IsThreadRunning())
        m_ListenThread.Stop();


    if(m_ServerThread.IsThreadRunning())
        m_ServerThread.Stop();

    if(m_pListenSocket!=NULL)
    {
        m_pListenSocket->vCloseSocket();
        delete m_pListenSocket;
        m_pListenSocket = NULL;
    }

    SOCKETLIST::iterator q;
    for(q = m_ClientSocketList.begin();q!=m_ClientSocketList.end();++q)
    {
        XPCTcpSocket* pSocket = *q;

        pSocket->vCloseSocket();
        delete pSocket;
    }

    m_ClientSocketList.clear();
    m_Socket2ClientMap.clear();
    m_AsynchronousClientSet.clear();
    m_ClientTimingVector.clear();

    return true;

}
void CMOOSCommServer::SetCommandLineParameters(int argc,  char * argv[])
{
	m_CommandLineParser.Open(argc,argv);
}

void CMOOSCommServer::SetWarningLatencyMS(double dfPeriod)
{
	m_dfCommsLatencyConcern = dfPeriod/1000.0;
}


bool CMOOSCommServer::Run(long lPort, const string & sCommunityName,bool bDisableNameLookUp,unsigned int nAuditPort)
{

    m_sCommunityName = sCommunityName;

    m_lListenPort = lPort;
	
	m_bDisableNameLookUp = bDisableNameLookUp;

	m_nAuditPort = nAuditPort;



	if(m_CommandLineParser.IsAvailable())
	{

        m_bPrintHeartBeat = m_CommandLineParser.GetFlag("--print_heart_beat");

		//here we look to parse latency
		//--latency=y:10
		std::string sLatency = "0";
		m_CommandLineParser.GetVariable("--response",sLatency);
		std::vector<std::string> sLL = MOOS::StringListToVector(sLatency);
		for(std::vector<std::string>::iterator q = sLL.begin(); q!=sLL.end();++q)
		{
			try
			{
				std::string sNum=*q;
				std::string sClient = "*";
				if(sNum.find(":")!=std::string::npos)
				{
					sClient = MOOSChomp(sNum,":");
				}
				if(!MOOSIsNumeric(sNum))
					throw std::runtime_error("error processing response "+ *q + " expected form [client_name:]time_ms\n");

				double dfT = MOOS::StringToDouble(sNum);

				//we push specialisms to the front and wildcards to the back
				if(sClient.find_first_of("*?")!=std::string::npos)
				{
					//this is not a wildcard
					m_ClientTimingVector.push_front(std::make_pair(sClient,dfT));
				}
				else
				{
					//this is a wildcard
					m_ClientTimingVector.push_back(std::make_pair(sClient,dfT));
				}



			}
			catch(const std::runtime_error & e)
			{
				std::cerr<<e.what()<<std::endl;
				continue;
			}

		}

	}


    if(!m_bQuiet)
    	DoBanner();

    m_nTotalActions = 0;
    SocketsInit();
    StartThreads();

    return true;
}


bool CMOOSCommServer::TimerLoop()
{
    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
	//optionally inhibit all MOOSTrace..    
    if(m_bQuiet)
        InhibitMOOSTraceInThisThread(true);
    
    int nPeriod = 3000;

    SOCKETLIST::iterator p,q;

    while(!m_TimerThread.IsQuitRequested())
    {
        MOOSPause(nPeriod);

        double dfTimeNow = MOOSTime();

        m_SocketListLock.Lock();

        p = m_ClientSocketList.begin();

        while(p!=m_ClientSocketList.end())
        {

            double dfLastCalled = (*p)->GetReadTime();
            q = p;
            ++q;
            if(dfTimeNow-dfLastCalled>m_dfClientTimeout)
            {
                MOOSTrace("its been %f seconds since my last confession:\n",dfTimeNow-dfLastCalled);
                MOOSTrace("\tTime Now %f\n\tLastReadTime %f\n",dfTimeNow,dfLastCalled );
                if(OnAbsentClient(*p))
                {
                    m_ClientSocketList.erase(p);
                }
            }
            p=q;
        }

        m_SocketListLock.UnLock();
    }

    return true;

}

bool CMOOSCommServer::SetClientTimeout(double dfTimeoutPeriod)
{
	if(dfTimeoutPeriod<0)
		return false;

	m_dfClientTimeout = dfTimeoutPeriod;
	return true;
}


bool  CMOOSCommServer::OnAbsentClient(XPCTcpSocket* pClient)
{
    MOOSTrace("\n------------ABSENT CLIENT---------\n");

    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    string sWho;
    p = m_Socket2ClientMap.find(pClient->iGetSocketFd());

    if(p!=m_Socket2ClientMap.end())
    {
        sWho = p->second;

        MOOSTrace("Client \"%s\" is being disconnected - where are you?.\n",p->second.c_str());

        m_Socket2ClientMap.erase(p);
    }

    GetMaxSocketFD();

    pClient->vCloseSocket();

    delete pClient;

    if(m_pfnDisconnectCallBack!=NULL)
    {
        MOOSTrace("Invoking user OnDisconnect callback...\n");
		
		if(m_bQuiet)
			InhibitMOOSTraceInThisThread(false);
        
		(*m_pfnDisconnectCallBack)(sWho,m_pDisconnectCallBackParam);
		
		if(m_bQuiet)
			InhibitMOOSTraceInThisThread(true);
    }

    MOOSTrace("--------------------------------\n");

    return true;
}

bool CMOOSCommServer::ListenLoop()
{
    
    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
    
    //optionally inhibit all MOOSTrace..    
    if(m_bQuiet)
        InhibitMOOSTraceInThisThread(true);
    

    
    m_pListenSocket = new XPCTcpSocket(m_lListenPort);

    try
    {
		//PMN removes this after noticing it allows multiple
		//servers to run simultaneously on Win32
#ifndef _WIN32

        //linger L;L.l_linger=0;L.l_onoff=0;
        //m_pListenSocket->vSetLinger( L);

        m_pListenSocket->vSetReuseAddr(1);
#endif
        if(m_bDisableNagle){
            if(!m_bQuiet){
                gMOOSCommsServerTheadPrinter.SimplyPrintTimeAndMessage("disabling nagle");
            }
            m_pListenSocket->vSetNoDelay(1);
        }

        m_pListenSocket->vBindSocket();
    }
    catch(XPCException& e)
    {
    #if _WIN32
        e;
    #endif

        MOOSTrace("Error binding to listen socket - Is there another CommServer Running?\n");
        MOOSTrace("This Server Is Quitting\n");

        m_bQuit = true;

        delete m_pListenSocket;

        m_pListenSocket = NULL;

        return false;
    }

    while(!m_ListenThread.IsQuitRequested())
    {

        try
        {
            char sClientName[255];

            m_pListenSocket->vListen(50);



            //sit here waiting for action and occasionally
            //checking we are not being told to quit
            for(;;)
            {
                //break;
                struct timeval timeout;
                fd_set fdset;
                timeout.tv_sec    = 1;
                timeout.tv_usec = 0;
                FD_ZERO(&fdset);
                FD_SET(m_pListenSocket->iGetSocketFd(), &fdset);

                if(select(m_pListenSocket->iGetSocketFd() + 1,
                    &fdset,
                    NULL,
                    NULL,
                    &timeout)!=0)
                {
                    //break from this while loop-  there is action on the socket
                    break;
                }
                else
                {
                    if(m_ListenThread.IsQuitRequested())
                        return true;
                }
            }



            XPCTcpSocket * pNewSocket = NULL;

			if(!m_bDisableNameLookUp)
			{
				pNewSocket = m_pListenSocket->Accept(sClientName);
			}
			else
			{
				pNewSocket = m_pListenSocket->Accept(); 
				sClientName[0]='\0'; 
			}
			
			try
			{
				pNewSocket->vSetRecieveBuf(m_nReceiveBufferSizeKB*1024);
				pNewSocket->vSetSendBuf(m_nSendBufferSizeKB*1024);

			}
			catch(  XPCException & e)
			{
				std::cerr<<"there was trouble configuring socket buffers: "<<e.sGetException()<<"\n";
			}

            m_SocketListLock.Lock();

            if(OnNewClient(pNewSocket,sClientName))
            {
                //store new socket
                m_ClientSocketList.push_front(pNewSocket);
                pNewSocket->SetReadTime(MOOSTime());

                GetMaxSocketFD();
            }

            m_SocketListLock.UnLock();
        }
        catch(XPCException & e)
        {
            MOOSTrace("Exception Thrown in listen loop: %s\n",e.sGetException());
        }

    }


    return true;
}


bool CMOOSCommServer::ServerLoop()
{

    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
    //optionally inhibit all MOOSTrace..    
    if(m_bQuiet)
        InhibitMOOSTraceInThisThread(true);
    
    struct timeval timeout;        // The timeout value for the select system call
    fd_set fdset;                // Set of "watched" file descriptors

    if(m_bBoostIOThreads)
    	MOOS::BoostThisThread();

    double last_heart_beat = MOOS::Time();

    while(!m_ServerThread.IsQuitRequested())
    {

        if(m_ClientSocketList.empty())
        {
            MOOSPause(100);
            continue;
        }

        // The socket file descriptor set is cleared and the socket file
        // descriptor contained within tcpSocket is added to the file
        // descriptor set.
        FD_ZERO(&fdset);

        SOCKETLIST::iterator p,q;

        m_SocketListLock.Lock();

        //rotate list..
        if(!m_ClientSocketList.empty())
        {
            m_ClientSocketList.push_front(m_ClientSocketList.back());
            m_ClientSocketList.pop_back();

            for(p = m_ClientSocketList.begin();p!=m_ClientSocketList.end();++p)
            {
                FD_SET((*p)->iGetSocketFd(), &fdset);
            }
        }
        m_SocketListLock.UnLock();

        // The select system call is set to timeout after 1 seconds with no data existing
        // on the socket. This has to be here, within the loop as Linux actually writes over
        // the timeout structure on completion of select (no that was a hard bug to find)
        timeout.tv_sec    = 1;
        timeout.tv_usec = 0;



        // A select is setup to return when data is available on the socket
        // for reading.  If data is not available after 1000 useconds, select
        // returns with a value of 0.  If data is available on the socket,
        // the select returns and data can be retrieved off the socket.
        int iSelectRet = select(m_nMaxSocketFD + 1,
            &fdset,
            NULL,
            NULL,
            &timeout);

        // If select returns a -1, then it failed and the thread exits.
        switch(iSelectRet)
        {
        case -1:
            if(XPCSocket::iGetLastError()==INVALID_SOCKET_SELECT)
            {
                //this can be caused by absenteeism between set up of fdset and select
                //prefer to catch and tolerate than block other threads for duration
                //of select and processing - added by PMN in Jan 2008 to address a
                //race condition which took a long time to show up...
                break;
            }
            else
            {
                return false;
            }

        case 0:
            //timeout...nothing to read
            break;

        default:
            //something to read:
            m_SocketListLock.Lock();
            for(p = m_ClientSocketList.begin();p!=m_ClientSocketList.end();++p)
            {
                m_pFocusSocket = *p;

                if (FD_ISSET(m_pFocusSocket->iGetSocketFd(), &fdset) != 0)
                {
                    //something to do read:
                    if(!ProcessClient())
                    {
                        //client disconnected!
                        OnClientDisconnect();
                        m_ClientSocketList.erase(p);
                        break;
                    }
                }
            }
            m_SocketListLock.UnLock();
            break;
        }

        //zero socket set..
        FD_ZERO(&fdset);

        if(m_bPrintHeartBeat && MOOS::Time()-last_heart_beat>kHeartBeatPrintPeriod){
            last_heart_beat = MOOS::Time();
            std::cerr<<"DB::ServerLoop ticks at "<< last_heart_beat<<"\n";
        }

    }

    return 0;
}


bool CMOOSCommServer::ProcessClient()
{
    bool bResult = true;

    try
    {
        m_pFocusSocket->SetReadTime(MOOSTime());

        //now we act on that packet
        //by way of the user supplied called back
        if(m_pfnRxCallBack!=NULL)
        {

            CMOOSCommPkt PktRx,PktTx;
            MOOSMSG_LIST MsgLstRx,MsgLstTx;

            //read input
            ReadPkt(m_pFocusSocket,PktRx);

            //convert to list of messages
            PktRx.Serialize(MsgLstRx,false);

            std::string sWho = m_Socket2ClientMap[m_pFocusSocket->iGetSocketFd()];
            //let owner figure out what to do !
            //this is a user supplied call back

			if(m_bQuiet)
				InhibitMOOSTraceInThisThread(false);

            if(!(*m_pfnRxCallBack)(sWho,MsgLstRx,MsgLstTx,m_pRxCallBackParam))
            {
                //client call back failed!!
                MOOSTrace(" CMOOSCommServer::ProcessClient()  pfnCallback failed\n");
            }
			
			if(m_bQuiet)
				InhibitMOOSTraceInThisThread(true);

			
            //we must send something back... just to keep the link alive
            //PMN changes this in 2007 as part of the new timing scheme
            //every packet will no begin with a NULL message the double val
            //of which will be the current time on the BD's machine
            //if( 1 || MsgLstTx.size()==0)
            {
                //add a default packet so client doesn't block
                CMOOSMsg NullMsg;
                NullMsg.m_dfVal = MOOSLocalTime();
                MsgLstTx.push_front(NullMsg);
            }

            //stuff reply mesage into a packet
            PktTx.Serialize(MsgLstTx,true);

            //send packet
            SendPkt(m_pFocusSocket,PktTx);

        }
    }
    catch(CMOOSException & e)
    {
        MOOSTrace("ProcessClient() Exception: %s\n", e.m_sReason);
        bResult = false;
    }

    return bResult;

}

bool CMOOSCommServer::OnNewClient(XPCTcpSocket * pNewClient,char * sName)
{

	MOOS::DeliberatelyNotUsed(sName);

	//now this is simply a nicety. It *helps* but cannot solve a corner case
	//in which a client disconnects (is destroyed) and a femto second later
	//starts up again. The DB can discover the closure at the same time
	//as handling the reconnection....not much we can do as there is no way
	//not make this synchronous
	MOOSPause(100);

	if(!m_bQuiet)
	    std::cout<<"\n------------"<<MOOS::ConsoleColours::Green()<<"CONNECT"<<MOOS::ConsoleColours::reset()<<"-------------\n";


    if(HandShake(pNewClient))
    {
        if(m_pfnConnectCallBack!=NULL)
        {
            std::string sWho  = GetClientName(pNewClient);
            if(!(*m_pfnConnectCallBack)(sWho,m_pConnectCallBackParam))
            {
                if(!m_bQuiet)
                {
                    std::cerr<<"user defined connect callback returns false\n";
                }
            }
        }

        if(!m_bQuiet)
        {
            std::cout<<"  Handshaking   :  "<<MOOS::ConsoleColours::green()<<"OK\n"<<MOOS::ConsoleColours::reset();

            string sName = GetClientName(pNewClient);

            if(!sName.empty())
            {
                std::cout<<"  Client's name :  "<<MOOS::ConsoleColours::green()<<sName<<MOOS::ConsoleColours::reset()<<"\n";
            }
            if(m_AsynchronousClientSet.find(sName)!=m_AsynchronousClientSet.end())
            {
                std::cout<<"  Type          :  "<<MOOS::ConsoleColours::Yellow()<<"Asynchronous"<<MOOS::ConsoleColours::reset()<<"\n";
            }
            else
            {
                std::cout<<"  Type          :  "<<MOOS::ConsoleColours::green()<<"Synchronous"<<MOOS::ConsoleColours::reset()<<"\n";
            }

            if(m_bBoostIOThreads)
            {
                std::cout<<"  Priority      :  "<<MOOS::ConsoleColours::Yellow()<<"raised"<<MOOS::ConsoleColours::reset()<<"\n";
            }
            else
            {
                std::cout<<"  Priority      :  "<<MOOS::ConsoleColours::green()<<"normal"<<MOOS::ConsoleColours::reset()<<"\n";
            }
        }
    }
    else
    {
        if(!m_bQuiet)
        {
            std::cerr<<"  Handshaking   :  "<<MOOS::ConsoleColours::Red()<<"FAIL\n"<<MOOS::ConsoleColours::reset()<<"\n";
            std::cerr<<MOOS::ConsoleColours::Red()<<"Handshaking failed - client is spurned\n"<<MOOS::ConsoleColours::reset();
        }

        pNewClient->vCloseSocket();
        delete pNewClient;

        if(!m_bQuiet)
            MOOSTrace("--------------------------------\n");

        return false;
    }

    if(!m_bQuiet)
        std::cout<<"  Total Clients :  "<<MOOS::ConsoleColours::green()<<m_Socket2ClientMap.size()<<MOOS::ConsoleColours::reset()<<"\n";


    if(!m_bQuiet)
        MOOSTrace("--------------------------------\n");



    return true;
}



bool CMOOSCommServer::OnClientDisconnect()
{


    if(!m_bQuiet)
        std::cout<<"\n----------"<<MOOS::ConsoleColours::Yellow()<<"DISCONNECT"<<MOOS::ConsoleColours::reset()<<"------------\n";


    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    string sWho;
    p = m_Socket2ClientMap.find(m_pFocusSocket->iGetSocketFd());

    if(p!=m_Socket2ClientMap.end())
    {
        sWho = p->second;

        //MOOSTrace("Client \"%s\" has disconnected.\n",p->second.c_str());

        m_Socket2ClientMap.erase(p);
        m_AsynchronousClientSet.erase(sWho);
    }


    GetMaxSocketFD();

    m_pFocusSocket->vCloseSocket();

    delete m_pFocusSocket;

    if(m_pfnDisconnectCallBack!=NULL)
    {
        //MOOSTrace("Invoking user OnDisconnect callback...\n");
		if(m_bQuiet)
			InhibitMOOSTraceInThisThread(false);

        (*m_pfnDisconnectCallBack)(sWho,m_pDisconnectCallBackParam);

		if(m_bQuiet)
			InhibitMOOSTraceInThisThread(true);

	}

    if(!m_bQuiet)
        MOOSTrace("--------------------------------\n");


    return true;
}

//void CMOOSCommServer::SetOnRxCallBack(bool (__cdecl *pfn)(MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx, void * pParam), void * pParam)
void CMOOSCommServer::SetOnRxCallBack(bool ( *pfn)(const std::string & ,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx, void * pParam), void * pParam)
{
    //address of function to invoke (static)
    m_pfnRxCallBack=pfn;

    //store the address of the object invoking the callback -> needed for scope
    //resolution when callback is invoked
    m_pRxCallBackParam = pParam;
}

//void CMOOSCommServer::SetOnDisconnectCallBack(bool (__cdecl *pfn)(string & MsgListRx, void * pParam), void * pParam)
void CMOOSCommServer::SetOnDisconnectCallBack(bool (*pfn)(string & , void * pParam), void * pParam)
{
    //address of function to invoke (static)
    m_pfnDisconnectCallBack=pfn;

    //store the address of the object invoking the callback -> needed for scope
    //resolution when callback is invoked
    m_pDisconnectCallBackParam = pParam;
}

void CMOOSCommServer::SetOnConnectCallBack(bool (*pfn)(string & , void * pParam), void * pParam)
{
    //address of function to invoke (static)
    m_pfnConnectCallBack=pfn;

    //store the address of the object invoking the callback -> needed for scope
    //resolution when callback is invoked
    m_pConnectCallBackParam = pParam;
}



void CMOOSCommServer::SetOnFetchAllMailCallBack(bool (*pfn)(const std::string  & sClient,MOOSMSG_LIST & MsgListTx,void * pParam),void * pParam)
{
    //address of function to invoke (static)
	m_pfnFetchAllMailCallBack = pfn;

	//store the parameter to pass with the invocation
	m_pFetchAllMailCallBackParam = pParam;

}

bool CMOOSCommServer::IsUniqueName(string &sClientName)
{
    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    for(p = m_Socket2ClientMap.begin();p!=m_Socket2ClientMap.end();++p)
    {
        if(p->second==sClientName)
        {
            return false;
        }
    }

    return true;
}


//here we can check that the client is speaking the correct wire protocol
//we begin by reading a string and checking it is what we are expecting
//note we are only reading a few bytes so this lets us catch the case where
//an old client that doesn't send a string simp;y sends a COmmPkt first
//chances of a comm packet spelling out a protocol name are pretty damn slim.....
bool CheckProtocol(XPCTcpSocket *pNewClient)
{
    char sProtocol[MOOS_PROTOCOL_STRING_BUFFER_SIZE+1] = {};
    int nRead = pNewClient->iRecieveMessage(sProtocol, MOOS_PROTOCOL_STRING_BUFFER_SIZE );
    if (nRead <=0)
    {
        return MOOSFail("Client disconnected during wire protocol transmission.\n"
                        "Make sure the client and MOOSDB are linking against a "
                        "MOOSLIB which uses the same protocol.\n");

    }
    if (!MOOSStrCmp(sProtocol, MOOS_PROTOCOL_STRING))
    {
        //this is bad - wrong flavour of comms - perhaps client needs to be recompiled...
        return MOOSFail("Incompatible wire protocol between DB and Client:\n  "
                        "Expecting protocol named \"%s\".\n  Client is using a protocol"
                        " called  \"%s\"\n\n  Make sure the client and MOOSDB"
                        " are linking against a MOOSLIB which uses the same"
                        " protocol \n",MOOS_PROTOCOL_STRING,sProtocol);
    }

    return true;
}


bool CMOOSCommServer::HandShake(XPCTcpSocket *pNewClient)
{
    CMOOSMsg Msg;

    double dfSkew = 0;

    try
    {
		
		if(!CheckProtocol(pNewClient))
		{
			throw CMOOSException("protocol error");
		}	
		
        if(ReadMsg(pNewClient,Msg,5))
        {
            double dfClientTime = Msg.m_dfTime;

            dfSkew = MOOSTime()-dfClientTime;

            if(IsUniqueName(Msg.m_sVal))
            {
                m_Socket2ClientMap[pNewClient->iGetSocketFd()] = Msg.m_sVal;
                //std::cerr<<"CMOOSCommServer::HandShake added "<<Msg.m_sVal<<" to m_Socket2ClientMap \n";
                if(MOOSStrCmp(Msg.m_sKey,"asynchronous"))
                {
                	m_AsynchronousClientSet.insert(Msg.m_sVal);
                }

            }
            else
            {
                PoisonClient(pNewClient,
							 MOOSFormat("A client of this name (\"%s\") already exists",
										Msg.m_sVal.c_str())
				);

                return false;
            }
        }
        else
        {
            PoisonClient(pNewClient,"Failed to read receive client's name");
            return false;
        }

        //send a message back to the client saying welcome
        CMOOSMsg MsgW(MOOS_WELCOME,"",dfSkew);

        //we are a V10 DB we can support AysncComms
        MsgW.m_sVal = "asynchronous";
        std::string sAux;
        MOOSAddValToString(sAux,"hostname",GetLocalIPAddress());

        MsgW.m_sSrcAux = sAux;
        MsgW.m_sOriginatingCommunity = m_sCommunityName;
        SendMsg(pNewClient,MsgW);

        return true;
    }
    catch (CMOOSException & e)
    {
        MOOSTrace("\nException caught [%s]\n",e.m_sReason);
        return false;
    }
}

void CMOOSCommServer::PoisonClient(XPCTcpSocket *pSocket, const std::string & sReason)
{
    //kill the client...
    CMOOSMsg MsgK(MOOS_POISON,"",sReason);
    SendMsg(pSocket,MsgK);
}

string CMOOSCommServer::GetClientName(XPCTcpSocket *pSocket)
{
    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    p = m_Socket2ClientMap.find(pSocket->iGetSocketFd());

    if(p!=m_Socket2ClientMap.end())
    {
        return p->second;
    }
    else
    {
        MOOSTrace("CMOOSCommServer::GetClientName() failed!\n");
        return "";
    }

}


bool CMOOSCommServer::SupportsAsynchronousClients()
{
	return false;
}

void CMOOSCommServer::DoBanner()
{
    if(m_bQuiet)
        return;

    std::cout<<"------------------- MOOSDB V10 -------------------\n";

    std::cout<<"  Hosting  community                "<<
    		MOOS::ConsoleColours::Green()<<"\""<<m_sCommunityName<<"\"\n"<<MOOS::ConsoleColours::reset();

    std::cout<<"  Name look up is                   ";
    if(!m_bDisableNameLookUp)
    {
    	std::cout<<MOOS::ConsoleColours::Green()<<"on\n"<<MOOS::ConsoleColours::reset();
    }
    else
    {
    	std::cout<<MOOS::ConsoleColours::red()<<"off\n"<<MOOS::ConsoleColours::reset();
    }

    std::cout<<"  Asynchronous support is           ";
    if(SupportsAsynchronousClients())
    {
    	std::cout<<MOOS::ConsoleColours::Green()<<"on\n"<<MOOS::ConsoleColours::reset();
    }
    else
    {
    	std::cout<<MOOS::ConsoleColours::red()<<"off\n"<<MOOS::ConsoleColours::reset();
    }

    std::cout<<"  Connect to this server on port    ";
    std::cout<<MOOS::ConsoleColours::green()<<m_lListenPort<<MOOS::ConsoleColours::reset()<<"\n";

    std::cout<<"--------------------------------------------------\n";

}

int CMOOSCommServer::GetMaxSocketFD()
{
    SOCKETLIST::iterator p;

    m_nMaxSocketFD = 0;
    for(p=m_ClientSocketList.begin();p!=m_ClientSocketList.end();++p)
    {
        m_nMaxSocketFD = m_nMaxSocketFD > (*p)->iGetSocketFd()
            ? m_nMaxSocketFD :
        (*p)->iGetSocketFd();
    }

    return m_nMaxSocketFD;

}


bool CMOOSCommServer::GetClientNames(STRING_LIST &sList)
{
    sList.clear();

    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    for(p = m_Socket2ClientMap.begin();p!=m_Socket2ClientMap.end();++p)
    {
        sList.push_front(p->second);
    }
    return true;
}


bool CMOOSCommServer::GetTimingStatisticSummary(std::string & sSummary)
{
    return m_Auditor.GetTimingStatisticSummary(sSummary);
}
