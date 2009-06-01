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
// MOOSCommClient.h: interface for the CMOOSCommClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(MOOSCommClientH)
#define MOOSCommClientH

#include <MOOSGenLib/MOOSLock.h>
#include <MOOSGenLib/MOOSThread.h>
#include <iostream>
#include <iomanip>
#include "MOOSCommObject.h"
#include <set>
#include <string>
#include <memory>


#define OUTBOX_PENDING_LIMIT 1000
#define INBOX_PENDING_LIMIT 1000
#define CLIENT_DEFAULT_FUNDAMENTAL_FREQ 5
#define CLIENT_MAX_FUNDAMENTAL_FREQ 200

#ifndef UNUSED_PARAMETER
    #ifdef _WIN32
        #define UNUSED_PARAMETER(a) a
    #else
        #define UNUSED_PARAMETER(a) 
    #endif
#endif

class XPCTcpSocket;

namespace MOOS
{
  class CMOOSSkewFilter;
}

//extern std::auto_ptr<std::ofstream> SkewLog;

/** This class is the most important component of MOOS as seen from the eyes
of a component developer */
class CMOOSCommClient  :public CMOOSCommObject
{
public:

    ///default constructor
    CMOOSCommClient();
    
    ///default destructor
    virtual ~CMOOSCommClient();
    

    /** notify the MOOS community that something has changed (string)*/
    bool Notify(const std::string &sVar, const std::string & sVal, double dfTime=-1);

    /** notify the MOOS community that something has changed (double)*/
    bool Notify(const std::string & sVar,double dfVal, double dfTime=-1);
    
    /** Register for notification in changes of named variable
    @param sVar name of variable of interest
    @param dfInterval minimum time between notifications*/
    bool Register(const std::string & sVar,double dfInterval);

    /** UnRegister for notification in changes of named variable
    @param sVar name of variable of interest*/
    bool UnRegister(const std::string & sVar);

    /** returns true if this obecjt is connected to the server */
    bool IsConnected();

    /** Called by a user of CMOOSCommClient to retrieve mail
    @param MsgList a list of messages into which the newly received message will be placed
    @return true if there is new mail */
    bool Fetch(MOOSMSG_LIST  & MsgList);

    /** place a single message in the out box and return immediately. Completion of this method
    does not infer transmission. However transmission will occur at the next available oppurtunity.
    Inpractice the apparent speed of message transmission will be very fast indeed. This model howver prevents
    wayward user software bring down the MOOSComms by way of denial of service. (ie hogging the network)
    @param Msg reference to CMOOSMsg which user whishes to send*/
    bool Post(CMOOSMsg  & Msg);

    /** internal method which runs in a seperate thread and manges the input and output
    of messages from thser server. DO NOT CALL THIS METHOD.*/
    bool ClientLoop();

    /** called by the above to do the client mail box shuffling **/
    virtual bool DoClientWork();

    /** Run the MOOSCommClient Object. This call is non blocking and begins managing process IO
    with the MOOSComms protocol
    @param sServer Name of machine on which server resides eg LOCALHOST or guru.mit.edu
    @param lPort   number of port on which server is listening for new connections eg 9000
    @param sMyName std::string name by which this MOOS process will be known - eg "MotionController" or "DepthSensor"
    @param nFundamentalFrequency the basic tick frequency of the comms loop. Default value of 5 implies mail will be
    retrieved and sent from the server at 5Hz
    */
    bool Run(const char * sServer,long lPort, const char * sMyName, unsigned int nFundamentalFreq=5);

    /** set the user supplied OnConnect call back. This callback , when set, will be invoked
    when a connection to the server is made. It is a good plan to register for notification of variables
    in this callback
    @param pfn pointer to static function of type bool Fn(void * pParam)
    @param pCallerParam parameter passed to callback function when invoked*/
    void SetOnConnectCallBack(bool (*pfn)(void * pParamCaller), void * pCallerParam);


    /** set the user supplied OnConnect call back. This callback , when set, will be invoked
    when a connection to the server is lost.
    @param pfn pointer to static function of type bool Fn(void * pParam)
    @param pCallerParam parameter passed to callback function when invoked*/
    void SetOnDisconnectCallBack(bool (*pfn)(void * pParamCaller), void * pCallerParam);

    /** set the user supplied OnMail callback. This usually will not be set. Users are expected to use FetchMail from
     their own thread and not use this call back. This function is experimental as of release 7.2 and coders are
     not encouraged to use it*/
    void SetOnMailCallBack(bool (*pfn)(void * pParamCaller), void * pCallerParam);
    
    /** return true if a mail callback is installed */
    bool HasMailCallBack();
    
    /** Directly and asynhrounously make a request to the server (use this very rarely if ever. Its not meant for public consumption)
    @param sWhat string specifying what request to make - ALL, DB_CLEAR, PROCESS_SUMMARY or VAR_SUMMARY
    @param MsgList List of messages returned by server
    @param dfTimeOut TimeOut
    @param bContinuouslyClearBox true if all other message returned with query are to be discarded.*/
    bool ServerRequest(const std::string &sWhat, MOOSMSG_LIST & MsgList, double dfTimeOut = 2.0, bool bContinuouslyClearBox = true);



    /**a static helper function that lets a user browse a mail list 
    the message is removed if bremove is true*/
    static bool PeekMail(MOOSMSG_LIST &Mail, const std::string &sKey, CMOOSMsg &Msg,bool bErase = false, bool bFindYoungest = false);
    
    
    /**a static helper function that lets a user browse a mail list as for PeekMail. 
     true is returned if mail is found and it is not more that 5 seconds old*/
    static bool PeekAndCheckMail(MOOSMSG_LIST &Mail, const std::string &sKey, CMOOSMsg &Msg,bool bErase = false, bool bFindYoungest = false);
      
    /** Have a peek at mail box and remove a particular message, by default all other messages
    are removed. Note this is quite different from ::PeekMail*/
    bool Peek(MOOSMSG_LIST &List, int nIDRequired, bool bClearBox = true);


    /** return a string of the host machines's IP adress*/
    static std::string GetLocalIPAddress();
    
    /** describe this client in a string */
    std::string GetDescription();
    
    /** call with "true" if you want thsi client to fake its own outgoing name. This is rarely used but very useful when it is*/
    bool FakeSource(bool bFake);

    /** make the client shut down */
    bool Close(bool bNice =true);

    /** return the list of messages names published*/
    std::set<std::string> GetPublished(){return m_Published;};

    /** return the list of messages registered*/
    std::set<std::string> GetRegistered(){return m_Registered;};
    
    /** used to control how verbose the connection process is */
    void SetQuiet(bool bQ){m_bQuiet = bQ;};

    /** used to control whether local clock skew (used by MOOSTime())  is se via the server at the other
     end of this connection */
    void DoLocalTimeCorrection(bool b){m_bDoLocalTimeCorrection = b;};
    
    /** used to control debug printing */
    void SetVerboseDebug(bool bT){m_bVerboseDebug = bT;};
    
    bool SetCommsTick(int nCommsTick);
    


protected:
    bool ClearResources();
    
    int m_nNextMsgID;
    
    /** send library info to stdout */
    void DoBanner();
    
    /** called when connection to server is closed */
    bool OnCloseConnection();
    
    /** true if we are connected to the server */
    bool m_bConnected;
    
    /** true if we want to be able to fake sources of messages (used by playback)*/
    bool m_bFakeSource;
    
    /** performs a handshake with the server when a new connection is made. Within this
    function this class tells the server its name*/
    bool HandShake();
    
    /** The number of pending unsent messages that can be tolerated*/
    unsigned int m_nOutPendingLimit;
    
    /** The number of unread incoming  messages that can be tolerated*/
    unsigned int m_nInPendingLimit;
    
    /** name of MOOS process
    @see Init*/
    std::string m_sMyName;
    
    /** Mutex around Outgoing mail box 
    @see CMOOSLock
    */
    CMOOSLock m_OutLock;
    /** Mutex around incoming mail box 
    @see CMOOSLock
    */
    CMOOSLock m_InLock;

    /** Connect to the server process using info supplied to Init
    @see Init*/
    bool ConnectToServer();
    
    /** called internally to start IO management thread 
    @see ClientLoop*/
    bool StartThreads();
    

    /** pointer to socket connected to server */
    XPCTcpSocket* m_pSocket;
    
    /** name of the host on which the server process lives
    @see Init*/
    std::string m_sDBHost;
    
    /** port number on which server process is listening for new connections
    @see Init*/
    long m_lPort;
    
    /** IO thread will continue so long as this flag is false */
    bool  m_bQuit;
    
    /** true if mail present (saves using a semaphore to open an empty box) */
    bool  m_bMailPresent;

    bool UpdateMOOSSkew(double dfRQTime, double dfTXTime,double dfRXTime);
    
    /*thread to handle communications with a server object*/
    CMOOSThread m_ClientThread;
    
    /** List of messages that a pending to be sent
    @see Post*/
    MOOSMSG_LIST m_OutBox;
    
    /** List of message that have been received and are ready for reading by user
    @see Fetch*/
    MOOSMSG_LIST m_InBox;
    
    /** parameter that user wants passed to him/her with connect callback*/
    void * m_pConnectCallBackParam;
    
    /** the user supplied OnConnect callback */
    bool (*m_pfnConnectCallBack)( void * pConnectParam);
    
    
    /** parameter that user wants passed to him/her with disconnect callback*/
    void * m_pDisconnectCallBackParam;
    
    /** the user supplied OnDisConnect callback */
    bool (*m_pfnDisconnectCallBack)( void * pParam);
    
    /** parameter that user wants passed to mail callback */
    void * m_pMailCallBackParam;
    
    /** the user supplied OnMailCallBack*/
    bool (*m_pfnMailCallBack)(void* pParam);
    
    
    /** funcdamental frequency with which comms with server occurs
    @see Run
    */
    unsigned int m_nFundamentalFreq;

    /** a set of strings of the resources (messages names/keys) that have been registered for */
    std::set<std::string> m_Registered;

    /** the set of messages names/keys that have been sent */
    std::set<std::string> m_Published;

    /** controls how verbose connectionn is*/
    bool m_bQuiet;
    
    /** controls verbose debugging printing */
    bool m_bVerboseDebug;

    /** controls whether skew is set */
    bool m_bDoLocalTimeCorrection;
    
    /** Skew filter keeps track of clock skew with server */
    std::auto_ptr< MOOS::CMOOSSkewFilter > m_pSkewFilter;

};

#endif // !defined(MOOSCommClientH)
