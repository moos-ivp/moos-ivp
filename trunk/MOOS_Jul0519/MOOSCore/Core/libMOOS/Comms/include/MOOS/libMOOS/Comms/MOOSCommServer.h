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
// MOOSCommServer.h: interface for the CMOOSCommServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSCOMMSERVER_H__2FDF870F_F998_4D3C_AD18_FCC2C5C12DDA__INCLUDED_)
#define AFX_MOOSCOMMSERVER_H__2FDF870F_F998_4D3C_AD18_FCC2C5C12DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _WIN32
    #include <winsock2.h>
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
#else
    #include <sys/time.h>
    #include <pthread.h>
#endif

#include "MOOS/libMOOS/Comms/MOOSCommObject.h"
#include "MOOS/libMOOS/Comms/MOOSCommPkt.h"
#include "MOOS/libMOOS/Utils/MOOSLock.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Comms/ServerAudit.h"


class XPCTcpSocket;
#include <list>
#include <map>
#include <set>

/** This class is the MOOS Comms Server. It lies at the heart of the communications
architecture and typically is of no interest to the component developer. It maintains a list of all
the connected clients and their names. It simultaneously listens on all sockets for calling clients
and then calls a user supplied call back to handle the request. This class is only used by the
CMOOSDB application*/
class CMOOSCommServer  : public CMOOSCommObject
{
public:
    typedef std::list<XPCTcpSocket*> SOCKETLIST;
    typedef std::map<int,std::string > SOCKETFD_2_CLIENT_NAME_MAP;
    typedef std::list<std::string > STRING_LIST;

    bool GetClientNames(STRING_LIST & sList);

    /** Set the recieve message call back handler. The callback will be called whenever
    a client sends one or more messages to the server. The supplied call back must be of the form
    static bool MyCallBack(MOOSMSG_LIST & RxLst,MOOSMSG_LIST & TxLst, void * pParam).
    @param sClient    Name of client at the end of the socket sending this Pkt
    @param RxLst    contains the incoming messages.
    @param TxLst    passed to the handler as a recepticle for all the message that    should be sent back to the client in response to the incoming messages.
    @param pParam      user suplied parameter to be passed to callback function
    */
    void SetOnRxCallBack(bool (*pfn)(const std::string  & sClient,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx,void * pParam),void * pParam);

    /** Set the disconnect message call back handler.  The supplied call back must be of the form
    static bool MyCallBack(std::string  & sClient,, void * pParam).
    @param sClient   Name of client at the end of the socket sending this Pkt
    @param pParam      user suplied parameter to be passed to callback function
    */
    void SetOnDisconnectCallBack(bool (*pfn)(std::string  & sClient,void * pParam),void * pParam);


    /** Set the connect  call back handler.  The supplied call back must be of the form
    static bool MyCallBack(std::string  & sClient,, void * pParam).
    @param sClient   Name of client just connected
    @param pParam      user suplied parameter to be passed to callback function
    */
    void SetOnConnectCallBack(bool (*pfn)(std::string  & sClient,void * pParam),void * pParam);



    /**
	* Set up the callback which is can be to collect all
	* pending mail in named client
	* @param pfn
	* @param pParam
	*/
    void SetOnFetchAllMailCallBack(bool (*pfn)(const std::string  & sClient,MOOSMSG_LIST & MsgListTx,void * pParam),void * pParam);

    /** This function is the listen loop called from one of the two server threads. It is responsible
    for accepting a coonection and creating a new client socket.    */
    virtual bool ListenLoop();

    /** This function is the server loop called from one of the two server threads. It listens to all presently connected
    sockets and when a call is received invokes thse user supplied callback */
    virtual bool ServerLoop();

    /** This function is the timer loop called from one of the three
    server threads. It makes sure all clients speak occasionally*/
    virtual bool TimerLoop();

    /**
     * specify how long the sever will tolerate silence from a client
     * @param dfTimeoutPeriod positive float in seconds
     * @return true on success
     */
    bool  SetClientTimeout(double dfTimeoutPeriod);


    /** specify a threshold above which the DB will print a warning if the time between
     * the time stamp in a message and it being printed is exceeded
     * @param dfPeriod
     * @return
     */
    void SetWarningLatencyMS(double dfPeriod);


    /** Initialise the server. This is a non blocking call and launches the MOOS Comms server threads.
    @param lPort port number to listen on
    */
    virtual bool Run(long lPort,const std::string  & sCommunityName, bool bDisableNameLookUp = false, unsigned int nAuditPort = 9020);
    
    /*stop the server*/
    virtual bool Stop();

    virtual void SetCommandLineParameters(int argc,  char * argv[]);
    
    /** used to control how verbose the server is. Setting to true turns off all Tracing */
    void SetQuiet(bool bQ){m_bQuiet = bQ;};

    /** return true if server is running */
    bool IsRunning(){return m_bQuit==false;};

    /**fill in a string which tells us all about client timing statistics.
     * @param sSummary has format clientname=a:b:c:d,.....
     * @param a recent latency in ms
     * @param b max latency in ms
     * @param c min latency in ms
     * @param d moving average latency
     */
    bool GetTimingStatisticSummary(std::string & sSummary);


    /// default constructor
    CMOOSCommServer();

    /// default destructor
    virtual ~CMOOSCommServer();

protected:
    /** a simple mutex to guard access to m_ClientSocketList
    @see m_ClientSocketList */
    CMOOSLock m_SocketListLock;

    /** figures out what the largest socket FD of all connected sockets. (needed by select)*/
    int GetMaxSocketFD();

    /** prints class information banner to stdout*/
    virtual void DoBanner();

    /** return true if Aynschronous Clients are supported */
    virtual bool SupportsAsynchronousClients();

    /** Get the name of the client on the remote end of pSocket*/
    std::string  GetClientName(XPCTcpSocket* pSocket);

    /** Send a Poisoned mesasge to the client on the end of pSocket. This may cause the client
    comms thrad to die */
    void PoisonClient(XPCTcpSocket* pSocket,const std::string & sReason);

    /** Perform handshaling with client just after a connection has been accepted */
    bool HandShake(XPCTcpSocket* pNewSocket);

    /** returns true if a server has no connection to the named client
    @param sClientName reference to client name std::string
    */
    bool IsUniqueName(std::string  & sClientName);

    /// internal count of the number of calls processed
    int m_nTotalActions;

    /// called when a client disconnects or and error occurs
    virtual bool OnClientDisconnect();

    ///called when a client goes quiet...
    virtual bool OnAbsentClient(XPCTcpSocket* pClient);

    /** a thread to listen for new connections */
    CMOOSThread m_ListenThread;

    /** a thread to handle existing connections */
    CMOOSThread m_ServerThread;

    /** a thread to notice if clients appear to have fallen silent */
    CMOOSThread m_TimerThread;

    /** user supplied OnRx callback
    @see SetOnRxCallBack */
    bool (*m_pfnRxCallBack)(const std::string  & sClient,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx,void * pCaller);

    /** place holder for the address of the object passed back to the user during an Rx callback
    @see SetOnRxCallBack */
    void * m_pRxCallBackParam;


    /** user supplied OnDisconnect callback
    @see SetOnDisconnectCallBack */
    bool (*m_pfnDisconnectCallBack)(std::string  & sClient,void * pParam);

    /** place holder for the address of the object passed back to the user during a Disconnect callback
    @see SetOnDisconnectCallBack */
    void * m_pDisconnectCallBackParam;


    /** user supplied OnConnect callback
    @see SetOnConnectCallBack */
    bool (*m_pfnConnectCallBack)(std::string  & sClient,void * pParam);

    /** place holder for the address of the object passed back to the user during a Connect callback
    @see SetOnConnectCallBack */
    void * m_pConnectCallBackParam;



    /** user supplied FetchAllMail callback which is called after a message has caused a call back
	@see SetOnFetchAllMailCallBack */
	bool (*m_pfnFetchAllMailCallBack)(const std::string  & sClient,MOOSMSG_LIST & MsgListTx,void * pCaller);

	/** place holder for the address of the object passed back to the user during an FetchAllMail callback
	@see SetOnFetchAllMailCallBack */
    void * m_pFetchAllMailCallBackParam;



    /** Listen socket (bound to port address supplied in constructor) */
    XPCTcpSocket * m_pListenSocket;

    /** pointer to the socket which server is currently processing call from */
    XPCTcpSocket * m_pFocusSocket;

    /** list of all currently connected sockets */
    SOCKETLIST    m_ClientSocketList;


    /** map of socket file descriptors to the std::string  name of the client process at the other end*/
    SOCKETFD_2_CLIENT_NAME_MAP m_Socket2ClientMap;

    /** map of socket file descriptors to the std::string  name of the client if that client supports
     * asynchronous reception of data*/
    std::set<std::string> m_AsynchronousClientSet;

    /** Called when a new client connects. Performs handshaking and adds new socket to m_ClientSocketList
    @param pNewClient pointer to the new socket created in ListenLoop;
    @see ListenLoop*/
    virtual bool    OnNewClient(XPCTcpSocket * pNewClient,char * sName);

    /** called from Server loop this function handles all the processing for the current client call. It inturn
    invokes the user supplied callback function */
    virtual bool    ProcessClient();


    /**
     * remove a socket from list of sockets that have open connections
     * @param pSocket
     * @return
     */
    bool RemoveSocketFromClientSocketList(XPCTcpSocket * pSocket);

    /// port listen socket is bound to
    long    m_lListenPort;

    /// threads continue while this flag is false
    bool    m_bQuit;

    /// largest FD of all connected sockets
    int        m_nMaxSocketFD;

    /// called from init to start the listen and server threads up
    bool    StartThreads();

	/// flag to say don't bother with name look up
	bool m_bDisableNameLookUp;

    /// name of community being served
    std::string  m_sCommunityName;
    
    //something to help with CommandLine parseing
    MOOS::CommandLineParser m_CommandLineParser;
    
    //details on timing info that can be sent to asynchronous clients
	std::list< std::pair< std::string, double >  > m_ClientTimingVector;
    
    ///how quiet are we
    bool m_bQuiet;

    //how long will we tolerate a cleint not talking to us?
    double m_dfClientTimeout;

    //what threshold in transit time from client to DB worries us
    //and will cause us to issue a warning
    double m_dfCommsLatencyConcern;

    //what port will we broadcast stats on by default
    unsigned int m_nAuditPort;

    MOOS::ServerAudit m_Auditor;

    //print heartbeat;
    bool m_bPrintHeartBeat;




};

#endif // !defined(AFX_MOOSCOMMSERVER_H__2FDF870F_F998_4D3C_AD18_FCC2C5C12DDA__INCLUDED_)
