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
// MOOSCommServer.cpp: implementation of the CMOOSCommServer class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif

#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSGlobalHelper.h"
#include "MOOSCommServer.h"
#include "MOOSCommPkt.h"
#include "MOOSException.h"
#include "XPCTcpSocket.h"
#include <iostream>

using namespace std;

#ifdef _WIN32
#define INVALID_SOCKET_SELECT WSAEINVAL
#else
#define INVALID_SOCKET_SELECT EBADF
#endif


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
    m_sCommunityName = "!£";
    m_bQuiet  = false;
}

CMOOSCommServer::~CMOOSCommServer()
{

}


bool CMOOSCommServer::Run(long lPort, const string & sCommunityName)
{

    m_sCommunityName = sCommunityName;

    m_lListenPort = lPort;

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

    double dfTimeOut = 4.0;

    SOCKETLIST::iterator p,q;

    while(!m_bQuit)
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
            if(dfTimeNow-dfLastCalled>dfTimeOut)
            {
                MOOSTrace("its been %f seconds sinc my last confession:\n",dfTimeNow-dfLastCalled);
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
        (*m_pfnDisconnectCallBack)(sWho,m_pDisconnectCallBackParam);
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
        m_pListenSocket->vSetReuseAddr(1);
#endif
        m_pListenSocket->vBindSocket();
    }
    catch(XPCException e)
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

    while(1)
    {

        try
        {
            char sClientName[200];

            m_pListenSocket->vListen();

            XPCTcpSocket * pNewSocket = m_pListenSocket->Accept(sClientName);

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
        catch(XPCException e)
        {
            MOOSTrace("Exception Thrown in listen loop: %s\n",e.sGetException());
        }

    }

    delete m_pListenSocket;
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



    while(!m_bQuit)
    {

        if(m_ClientSocketList.empty())
        {
            MOOSPause(1);
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

            for(p = m_ClientSocketList.begin();p!=m_ClientSocketList.end();p++)
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
            for(p = m_ClientSocketList.begin();p!=m_ClientSocketList.end();p++)
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
            if(!(*m_pfnRxCallBack)(sWho,MsgLstRx,MsgLstTx,m_pRxCallBackParam))
            {
                //client call back failed!!
                MOOSTrace(" CMOOSCommServer::ProcessClient()  pfnCallback failed\n");
            }

            //we must send something back... just to keep the link alive
            //PMN changes this in 2007 as part of the new timing scheme
            //every packet will no begin with a NULL message the double val
            //of which will be the current time on the BD's machine
            if( 1 || MsgLstTx.size()==0)
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
    catch(CMOOSException e)
    {
        MOOSTrace("ProcessClient() Exception: %s\n", e.m_sReason);
        bResult = false;
    }

    return bResult;

}

bool CMOOSCommServer::OnNewClient(XPCTcpSocket * pNewClient,char * sName)
{
    MOOSTrace("\n------------CONNECT-------------\n");


    MOOSTrace("New client connected from machine \"%s\"\n",sName);
    MOOSTrace("Handshaking....");



    if(HandShake(pNewClient))
    {
        MOOSTrace("done\n");

        string sName = GetClientName(pNewClient);

        if(!sName.empty())
        {
            MOOSTrace("clients name is \"%s\"\n",sName.c_str());
        }
    }
    else
    {
        MOOSTrace("Handshaking failed - client is spurned\n");
        pNewClient->vCloseSocket();
        delete pNewClient;
        MOOSTrace("--------------------------------\n");
        return false;
    }

    MOOSTrace("There are now %d clients connected.\n",m_ClientSocketList.size()+1);

    MOOSTrace("--------------------------------\n");



    return true;
}

bool CMOOSCommServer::OnClientDisconnect()
{


    MOOSTrace("\n------------DISCONNECT-------------\n");

    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    string sWho;
    p = m_Socket2ClientMap.find(m_pFocusSocket->iGetSocketFd());

    if(p!=m_Socket2ClientMap.end())
    {
        sWho = p->second;

        MOOSTrace("Client \"%s\" has disconnected.\n",p->second.c_str());

        m_Socket2ClientMap.erase(p);
    }

    GetMaxSocketFD();

    m_pFocusSocket->vCloseSocket();

    delete m_pFocusSocket;

    if(m_pfnDisconnectCallBack!=NULL)
    {
        MOOSTrace("Invoking user OnDisconnect callback...\n");
        (*m_pfnDisconnectCallBack)(sWho,m_pDisconnectCallBackParam);
    }

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
void CMOOSCommServer::SetOnDisconnectCallBack(bool (*pfn)(string & MsgListRx, void * pParam), void * pParam)
{
    //address of function to invoke (static)
    m_pfnDisconnectCallBack=pfn;

    //store the address of the object invoking the callback -> needed for scope
    //resolution when callback is invoked
    m_pDisconnectCallBackParam = pParam;
}

bool CMOOSCommServer::IsUniqueName(string &sClientName)
{
    SOCKETFD_2_CLIENT_NAME_MAP::iterator p;

    for(p = m_Socket2ClientMap.begin();p!=m_Socket2ClientMap.end();p++)
    {
        if(p->second==sClientName)
        {
            return false;
        }
    }

    return true;
}

bool CMOOSCommServer::HandShake(XPCTcpSocket *pNewClient)
{
    CMOOSMsg Msg;

    double dfSkew = 0;

    try
    {
        if(ReadMsg(pNewClient,Msg,5))
        {
            double dfClientTime = Msg.m_dfTime;

            dfSkew = MOOSTime()-dfClientTime;

            if(IsUniqueName(Msg.m_sVal))
            {
                m_Socket2ClientMap[pNewClient->iGetSocketFd()] = Msg.m_sVal;
            }
            else
            {
                PoisonClient(pNewClient,"A client of this name already exists");

                return false;
            }
        }
        else
        {
            PoisonClient(pNewClient,"Failed to read receive clients name");
            return false;
        }

        //send a message back to the client saying welcome
        CMOOSMsg MsgW(MOOS_WELCOME,"",dfSkew);
        SendMsg(pNewClient,MsgW);

        return true;
    }
    catch (CMOOSException e)
    {
        MOOSTrace("\nException caught [%s]\n",e.m_sReason);
        return false;
    }
}

void CMOOSCommServer::PoisonClient(XPCTcpSocket *pSocket, const char *sReason)
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

void CMOOSCommServer::DoBanner()
{
    MOOSTrace("***************************************************\n");
    MOOSTrace("*       This is MOOS Server for Community \"%s\"      \n",m_sCommunityName.c_str());
    MOOSTrace("*       c. P Newman 2001                           \n");
    MOOSTrace("*                                                  \n");
    MOOSTrace("*       Binding on %d                              \n",m_lListenPort);
#ifdef _WIN32
    MOOSTrace("*       built on %s\n",__TIMESTAMP__);
#endif
    MOOSTrace("*                                                  \n");
    MOOSTrace("*       This machine is %s endian                 \n",IsLittleEndian()?"Little":"Big");
    MOOSTrace("***************************************************\n");

}

int CMOOSCommServer::GetMaxSocketFD()
{
    SOCKETLIST::iterator p;

    m_nMaxSocketFD = 0;
    for(p=m_ClientSocketList.begin();p!=m_ClientSocketList.end();p++)
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

    for(p = m_Socket2ClientMap.begin();p!=m_Socket2ClientMap.end();p++)
    {
        sList.push_front(p->second);
    }

    return true;
}
