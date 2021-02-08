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
// MOOSCommClient.h: interface for the CMOOSCommClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(MOOSCommClientH)
#define MOOSCommClientH


#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <string>
#include <memory>


#include "MOOS/libMOOS/Utils/MOOSLock.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"
#include "MOOS/libMOOS/Utils/Macros.h"
#include "MOOS/libMOOS/Comms/MOOSCommObject.h"
#include "MOOS/libMOOS/Comms/ActiveMailQueue.h"
#include "MOOS/libMOOS/Comms/ClientCommsStatus.h"
#include "MOOS/libMOOS/Comms/EndToEndAudit.h"




#define OUTBOX_PENDING_LIMIT 2048
#define INBOX_PENDING_LIMIT 2048
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
    bool Notify(const std::string &sVar, const std::string & sVal, const std::string & sSrcAux, double dfTime=-1);
    bool Notify(const std::string &sVar, const char * sVal,double dfTime=-1);
    bool Notify(const std::string &sVar, const char * sVal,const std::string & sSrcAux, double dfTime=-1);


    /** notify the MOOS community that something has changed (double)*/
    bool Notify(const std::string & sVar,double dfVal, double dfTime=-1);
    bool Notify(const std::string & sVar,double dfVal, const std::string & sSrcAux,double dfTime=-1);


	/** notify the MOOS community that something has changed binary data*/
    bool Notify(const std::string & sVar,void *  pData, unsigned int nDataSize, double dfTime=-1);
    bool Notify(const std::string & sVar,void *  pData, unsigned int nDataSize, const std::string & sSrcAux,double dfTime=-1);
    bool Notify(const std::string & sVar,const std::vector<unsigned char>& vData,double dfTime=-1);
    bool Notify(const std::string & sVar,const std::vector<unsigned char>& vData, const std::string & sSrcAux,double dfTime=-1);

	
    /** Register for notification in changes of named variable
    @param sVar name of variable of interest
    @param dfInterval minimum time between notifications*/
    bool Register(const std::string & sVar,double dfInterval=0);

    /**
     * Wild card registration
     * @param sVarPattern wildcard pattern for variables eg NAV_*
     * @param sAppPattern wildcard pattern for variables eg GPS_*
     * @param dfInterval minimim time between notifications
     * @return true on success
     */
    bool Register(const std::string & sVarPattern,const std::string & sAppPattern, double dfInterval);


    /** UnRegister for notification in changes of named variable
    @param sVar name of variable of interest*/
    bool UnRegister(const std::string & sVar);

    /** Wildcard unregister */
    bool UnRegister(const std::string &sVarPattern, const std::string & sAppPattern);


    /** returns true if this obecjt is connected to the server */
    bool IsConnected();

    /** wait for a connection return false if not connected after nMilliseconds*/
    bool WaitUntilConnected(const unsigned int nMilliseconds);

    /**
     * returns the name with which the client registers with the MOOSDB
     * @return
     */
    std::string GetMOOSName();

    /** Called by a user of CMOOSCommClient to retrieve mail
    @param MsgList a list of messages into which the newly received message will be placed
    @return true if there is new mail */
    bool Fetch(MOOSMSG_LIST  & MsgList);

    /** place a single message in the out box and return immediately. Completion of this method
    does not imply transmission. However transmission will occur at the next available oppurtunity.
    In practice the apparent speed of message transmission will be very fast indeed. This model
    however prevents wayward user software bring down the MOOSComms by way of denial of service.
    (ie hogging the network)
    @param Msg reference to CMOOSMsg which user wishes to send*/
    virtual bool Post(CMOOSMsg  & Msg,bool bKeepMsgSourceName = false);

    /** internal method which runs in a seperate thread and manages the input and output
    of messages from the server. DO NOT CALL THIS METHOD.*/
    virtual bool ClientLoop();

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
    //bool Run(const char * sServer,long lPort, const char * sMyName, unsigned int nFundamentalFreq=5);
    bool Run(const std::string & sServer, int Port, const std::string & sMyName, unsigned int nFundamentalFrequency=5);


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

    
    /** return true if a mail callback is installed */
    bool HasMailCallBack();
    
    /** Directly and asynchronously make a request to the server (use this very rarely if ever. It's not
    meant for public consumption)
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
    
    /** describe this client in a string */
    std::string GetDescription();
    
    /** call with "true" if you want this client to fake its own outgoing name. This is rarely used but very useful when it is */
    bool FakeSource(bool bFake);

    /** make the client shut down */
    virtual bool Close(bool bNice =true);

    /** return the list of messages names published*/
    std::set<std::string> GetPublished(){return m_Published;};
    
    /** return true if we are registered for named variable */
    bool IsRegisteredFor(const std::string & sVariable);

    /** return the list of messages registered*/
    std::set<std::string> GetRegistered(){return m_Registered;};
    
    /** return true if client is running */
    virtual bool IsRunning();

    /** return true if this client is Asynchronous (so can have data pushed to it at any time)*/
    virtual bool IsAsynchronous();

    /** how much incoming mail is pending?*/
    unsigned int GetNumberOfUnreadMessages();

    /** how much outgoing mail is pending?*/
    unsigned int GetNumberOfUnsentMessages();

    /** get total number of bytes sent*/
    uint64_t GetNumBytesSent();

    /** get total number of bytes received*/
    uint64_t GetNumBytesReceived();

    /** get total number of messages received*/
    uint64_t GetNumMsgsReceived();

    /** get total number of messages sent*/
    uint64_t GetNumMsgsSent();

    /** get db host name*/
    std::string GetDBHostname();

    /** get port of DB */
    int GetDBHostPort();

    /** get client name */
    std::string GetClientName();

    /** Get db host name as seen by DB. Returns empty string if
    not connected*/
    std::string GetDBHostNameAsSeenByDB() const;

    /** set / get scale factor which determines how to encourgage message bunching
     * at high timewarps
     * returns false if outside 0 - 1.0
     * if timewarp was 100 and dfSF is 0.1 then 10ms delay is
     * invoked in comms loop
     */
    bool SetCommsControlTimeWarpScaleFactor(double dfSF);
    double GetCommsControlTimeWarpScaleFactor();

    /** used to control how verbose the connection process is */
    void SetQuiet(bool bQ){m_bQuiet = bQ;};

    /** used to control whether local clock skew (used by MOOSTime())  is se via the server at the other
     end of this connection */
    void DoLocalTimeCorrection(bool b){m_bDoLocalTimeCorrection = b;};
    
    /** used to control debug printing */
    void SetVerboseDebug(bool bT){m_bVerboseDebug = bT;};
    
    /** Set the number of ms between loops of the Comms thread (not relevent if teh cleint is Asynchoronous)*/
    bool SetCommsTick(int nCommsTick);

    bool ExpectOutboxOverflow(unsigned int outbox_pending_size);

    /**
     * return name of community the client is attached to
     * @return name of community the client is attached to
     */
    std::string GetCommunityName();

    /**
     * Flush the comms client (force it to talk to the Server now).
     * It is rare that you will need this...
     * @return true on success;
     */
    virtual bool Flush();

    /** set the user supplied OnMail callback. This is an internal function - do not call it yourself
     * if you want rapid response use V10 */
    void SetOnMailCallBack(bool (*pfn)(void * pParamCaller), void * pCallerParam);



    /**
	 * Register a custom call back for a particular message. This call back will be called from its own thread.
	 * @param sQueueName
	 * @param pfn  pointer to your function should be type bool func(CMOOSMsg &M, void *pParam)
	 * @param pYourParam a void * pointer to the thing we want passed as pParam above
	 * @return true on success
	 */
    virtual bool AddActiveQueue(const std::string & sQueueName,
    				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
    				void * pYourParam );

    /**
     * same as above only with a wildcard...
     */
    bool AddWildcardActiveQueue(const std::string & sQueueName,
    				const std::string & sPattern,
    				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
    				void * pYourParam );



    /**
	 * Register a custom call back for a particular message. This call back will be called from its own thread.
	 * @param sQueueName
	 * @param Instance of class on which to invoke member function
	 * @param member function of class bool func(CMOOSMsg &M)
	 * @return true on success
	 */
    template <class T>
    bool AddActiveQueue(const std::string & sQueueName,
    		T* Instance,
    		bool (T::*memfunc)(CMOOSMsg &)  );

    /**
     * same as above only with a wildcard...
     */
    template <class T>
    bool AddWildcardActiveQueue(const std::string & sQueueName,
    				const std::string & sPattern,
    	    		T* Instance,
    	    		bool (T::*memfunc)(CMOOSMsg &)  );



    /**
	 * stop a message routing to a Active Queue
	 * @param sQueueName name of Active Queue
	 * @param sMsgName name of message to route to this queue
	 * @return true on success
	 */
    bool RemoveMessageRouteToActiveQueue(const std::string & sQueueName,
   		const std::string & sMsgName);


    /**
	 * make a message route to a Active Queue (which will call a custom callback)
	 * @param sQueueName name of Active Queue
	 * @param sMsgName name of message to route to this queue
	 * @return true on success
	 */
     bool AddMessageRouteToActiveQueue(const std::string & sQueueName,
    		const std::string & sMsgName);


    /**
     * Register a custom callback and create the active queue as needed.
	 * @param sQueueName the queue name
	 * @param sMsgName name of message to route to this queue
	 * @param pfn  pointer to your function should be type bool func(CMOOSMsg &M, void *pParam)
	 * @param pYourParam a void * pointer to the thing we want passed as pParam above
	 * @return true on success
     *
     */
    bool AddMessageRouteToActiveQueue(const std::string & sQueueName,
    				const std::string & sMsgName,
    				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
    				void * pYourParam );

    DEPRECATED(bool AddMessageCallBack(const std::string & sQueueName,
    				const std::string & sMsgName,
    				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
    				void * pYourParam ));


    /**
	 * Register a custom callback and create the active queue as needed.
	 * @param sQueueName the queue name
	 * @param sMsgName name of message to route to this queue
	  *@param Instance of class on which to invoke member function
	 * @param member function of class bool func(CMOOSMsg &M)
	 * @return true on success
	 */
    template <class T>
    bool AddMessageRouteToActiveQueue(const std::string & sQueueName,
    				const std::string & sMsgName,
    				T* Instance,
    				bool (T::*memfunc)(CMOOSMsg &) );


    /**
     * remove the named active queue
     * @param sCallbackName
     * @return
     */
    bool RemoveActiveQueue(const std::string & sQueueName);


    /**
     * Does this named active queue exist?
     * @param sCallbackName
     * @return
     */
    bool HasActiveQueue(const std::string & sQueueName);

    /** Print all active queues*/
    void PrintMessageToActiveQueueRouting();



    /** enable or disable comms status monitoring across the community*/
    void EnableCommsStatusMonitoring(bool bEnable);

    /** query the comms status of some other client*/
    bool GetClientCommsStatus(const std::string & sClient, MOOS::ClientCommsStatus & TheStatus);

    /** get all client statuses */
    void GetClientCommsStatuses(std::list<MOOS::ClientCommsStatus> & Statuses);


    /** internal function used to collect client status sumamries - do not use */
    bool ProcessClientCommsStatusSummary(CMOOSMsg & M);





protected:
    bool ClearResources();
    
    int m_nNextMsgID;
    
    /** send library info to stdout */
    virtual void DoBanner();
    
    /** called when connection to server is closed */
    virtual bool OnCloseConnection();
    
    /** get total number of Message Packets recieved*/
    uint64_t GetNumPktsReceived();

    /** true if we are connected to the server */
    bool m_bConnected;
    /** time of the last successful connection */
    double m_dfLastConnectionTime;
    
    /** true if we want to be able to fake sources of messages (used by playback)*/
    bool m_bFakeSource;
    
    /** true if w are monitoring all clients' comms status*/
    bool m_bMonitorClientCommsStatus;

    /** performs a handshake with the server when a new connection is made. Within this
    function this class tells the server its name*/
    bool HandShake();
    
    /**returns the key used when handshaking */
    virtual std::string HandShakeKey();

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

    /** Mutex around incoming DoClientWork method
    @see CMOOSLock
    */
    CMOOSLock m_WorkLock;


    /** Connect to the server process using info supplied to Init
    @see Init*/
    bool ConnectToServer();
    
    /** called internally to start IO management thread 
    @see ClientLoop*/
    virtual bool StartThreads();
    

    /** pointer to socket connected to server */
    XPCTcpSocket* m_pSocket;
    
    /** name of the host on which the server process lives
     * as see by the machine the client is running
    @see Init*/
    std::string m_sDBHost;


    /** name of the machine running the DB as seen by the DB
    returned by GetDBHostNameAsSeenByDB and set up during
    hand shaking*/
    std::string m_sDBHostAsSeenByDB;
    
    /** port number on which server process is listening for new connections
    @see Init*/
    long m_lPort;
    
    /** community name hosted by connected MOOSDB */
    std::string m_sCommunityName;

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
    
    
    /** fundamental frequency with which comms with server occurs
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
    MOOS::ScopedPtr< MOOS::CMOOSSkewFilter > m_pSkewFilter;
    
    /**
     * list of names of active mail queues for each message name
     */
    std::map<std::string,std::set<std::string>  > Msg2ActiveQueueName_;

    /**
     * mapping a queue name to a pointer to a queue
     */
    std::map<std::string,MOOS::ActiveMailQueue*> ActiveQueueMap_;

    /**
     * a list of <queuename,pattern> pairs
     */
    std::map< std::string, std::string > WildcardQueuePatterns_;

    /** the set of messages names/keys that have been received and
     * because of that need not be shown again to the wildcard queues. Note
     * that this may be a larger set than m_Registered because of wildcard
     * subscriptions  */
    std::set<std::string> WildcardCheckSet_;

    /*
     * a mutex protecting  ActiveQueues_
     */
    CMOOSLock ActiveQueuesLock_;

    /*
     * an inernal helper function which sorts some mail into
     * active queues (if any have been installed)
     */
    bool DispatchInBoxToActiveThreads();

    /*
     * a counter for total bytes received
     */
    uint64_t m_nBytesReceived;

    /*
     * a counter for total bytes received.
     */
    uint64_t m_nBytesSent;


    /*
     * a counter for total bytes received.
     */
    uint64_t m_nPktsReceived;

     /*
     * a counter for total message received.
     */
    uint64_t m_nMsgsReceived;


    /*
    * a counter for total message received.
    */
    uint64_t m_nMsgsSent;


    /**
     * internal variable describing if mail should be
     * posted to front or back of mailbox
     */
    bool m_bPostNewestToFront;

    /** true if after handshaking DB announces its ability to support aysnc comms*/
    bool m_bDBIsAsynchronous;


    /** true if we expect Comms to overflow and want older (unsent) messages to be replaced by new ones */
    bool m_bExpectMailBoxOverFlow;

    //how much to delay outgoing mail thread as a proportion oof timewarp
    double m_dfOutGoingDelayTimeWarpScaleFactor;


    /** turn on comms status monitoring of all clients */
    bool ControlClientCommsStatusMonitoring(bool bEnable);


    //used for monitoring status of all clients in network
    std::map<std::string , MOOS::ClientCommsStatus> m_ClientStatuses;

    //used to protect status monitoring variable m_ClientStatuses
    CMOOSLock m_ClientStatusLock;


    //some tools for recurrnent subscription management
    CMOOSLock RecurrentSubscriptionLock;
    bool AddRecurrentSubscription(const std::string &sVar, double dfPeriod);
    bool RemoveRecurrentSubscription(const std::string & sVar);
    bool ApplyRecurrentSubscriptions();

private:
    std::map< std::string, double > m_RecurrentSubscriptions;

    MOOS::EndToEndAudit end_to_end_auditor_;


};

#include "MOOS/libMOOS/Comms/MOOSCommClient.hxx"


#endif // !defined(MOOSCommClientH)
